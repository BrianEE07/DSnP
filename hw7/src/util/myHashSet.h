/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>
#include <iostream> //

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(const HashSet<Data>* p, vector<Data>* b = 0, size_t n = 0, int r = 0): _parent(p), _bucket(b), _nodeIdx(n), _rowNum(r) {}
      iterator(vector<Data>* b = 0, size_t n = 0, int r = 0): _bucket(b), _nodeIdx(n), _rowNum(r) {}
      iterator(const iterator& i): _parent(i._parent), _bucket(i._bucket), _nodeIdx(i._nodeIdx), _rowNum(i._rowNum) {}
      ~iterator() {}
      const Data& operator * () const { 
         assert(_bucket -> size() > _nodeIdx);
         return _bucket -> at(_nodeIdx);
      }
      iterator& operator ++ () { 
         if(_bucket -> size() > _nodeIdx + 1){
            ++_nodeIdx;
            return *this;
         }
         _nodeIdx = 0;
         while(_rowNum != _parent -> numBuckets()){
            ++_bucket;
            ++_rowNum;
            if(!(_bucket -> empty())) return *this;
         }
         return *this;
      }
      iterator operator ++ (int) { 
         iterator tmp = *this;
         if(_bucket -> size() > _nodeIdx + 1){
            ++_nodeIdx;
            return tmp;
         }
         _nodeIdx = 0;
         while(_rowNum != _parent -> numBuckets()){
            ++_bucket;
            ++_rowNum;
            if(!(_bucket -> empty())) return tmp;
         }
         return tmp;
      }
      iterator& operator -- () {
         if(_nodeIdx != 0){
            --_nodeIdx;
            return *this;
         }
         while(_rowNum != 0){
            --_bucket;
            --_rowNum;
            if(!(_bucket -> empty())){
               _nodeIdx = _bucket -> size() - 1;
               return *this;
            }
         }
         return *this;
      }
      iterator operator -- (int) {
         iterator tmp = *this;
         if(_nodeIdx != 0){
            --_nodeIdx;
            return tmp;
         }
         while(_rowNum != 0){
            --_bucket;
            --_rowNum;
            if(!(_bucket -> empty())){
               _nodeIdx = _bucket -> size() - 1;
               return tmp;
            }
         }
         return tmp;
      }
      bool operator == (const iterator& i) const { return (i._bucket == _bucket && i._nodeIdx == _nodeIdx);}
      bool operator != (const iterator& i) const { return (i._bucket != _bucket || i._nodeIdx != _nodeIdx); }
      iterator& operator = (const iterator& i){
         _bucket = i._bucket;
         _nodeIdx = i._nodeIdx;
         return *this;
      }
   private:
      const HashSet<Data>* _parent;
      vector<Data>* _bucket;
      size_t _nodeIdx;
      int _rowNum;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      iterator tmp(this, _buckets);
      if(tmp._bucket -> empty()) ++tmp;
      return tmp; 
   }
   // Pass the end
   iterator end() const {
      iterator tmp(this, _buckets + _numBuckets, 0, _numBuckets);
      return tmp; 
   }
   // return true if no valid data
   bool empty() const { 
      return begin() == end(); }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
      iterator it(begin());
      for(;it != end();++it) ++s;
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      size_t bn = bucketNum(d);
      for(size_t i = 0;i < (_buckets + bn) -> size();++i){
         if(d == (_buckets + bn) -> at(i)) return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t bn = bucketNum(d);
      for(size_t i = 0;i < (_buckets + bn) -> size();++i){
         if(d == (_buckets + bn) -> at(i)){
            d = (_buckets + bn) -> at(i);
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      size_t bn = bucketNum(d);
      for(size_t i = 0;i < (_buckets + bn) -> size();++i){
         if(d == (_buckets + bn) -> at(i)){
            (_buckets + bn) -> at(i) = d;
            return true;
         }
      }
      insert(d);
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false if d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if(check(d)) return false;
      (_buckets + bucketNum(d)) -> push_back(d);
      return true; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t bn = bucketNum(d);
      for(size_t i = 0;i < (_buckets + bn) -> size();++i){
         if(d == (_buckets + bn) -> at(i)){
            if(i == (_buckets + bn) -> size() - 1) (_buckets + bn) -> pop_back();
            else{ 
               (_buckets + bn) -> at(i) = (_buckets + bn) -> back();
               (_buckets + bn) -> pop_back();
            }
            return true;
         }
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;
   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); 
   }
};

#endif // MY_HASH_SET_H
