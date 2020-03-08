/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

#include <iostream> // for debug

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node++; return (*this); }
      iterator operator ++ (int) { iterator tmp = *this; _node++; return tmp; }
      iterator& operator -- () { _node--; return (*this); }
      iterator operator -- (int) { iterator tmp = *this; _node--; return tmp; }

      iterator operator + (int i) const { iterator tmp = *this; tmp._node += i; return tmp; }
      iterator& operator += (int i) { _node += i; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { if(_node == i._node) return false; return true;}
      bool operator == (const iterator& i) const { if(_node == i._node) return true; return false;}

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if(_size == 0 || _capacity == 0) 
         return end(); 
      iterator tmp(_data); 
      return tmp;
   }
   iterator end() const { 
      if(_capacity == 0) 
         return 0; 
      iterator tmp(_data + _size); 
      return tmp;
   }
   bool empty() const { 
      if(_size == 0) 
         return true;  
      return false;
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) { 
      if (_size == _capacity) 
         expand(); 
      _data[_size++] = x;
      _isSorted = false;
      // cout << _size << "," << _capacity << endl;
   }

   void pop_front() { 
      if(_size == 0) return;
      else if(_size == 1) _size--;
      else{       // _size >= 2
         _data[0] = _data[_size - 1];
         _size--;
         _isSorted = false;
      }
      return;
   }
   void pop_back() { 
      if(_size == 0) return;
      _size--;
      return;
   }

   bool erase(iterator pos) { 
      if(_size == 0) return false;
      else if(_size == 1) _size--;
      else{       // _size >= 2
         *(pos._node) = _data[_size - 1];
         _size--;
         _isSorted = false;
      }
      return true;
   }
   bool erase(const T& x) { 
      iterator era = find(x);
      if(era == end()) return false;
      erase(era);
      return true;
   }

   iterator find(const T& x) { 
      for(size_t i = 0;i < _size;i++){
         if(_data[i] == x){
            iterator ret(_data + i);
            return ret;
         }
      }
      return end(); 
   }

   void clear() { _size = 0;}

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { 
      if(_isSorted == true) return;
      if (!empty()) ::sort(_data, _data + _size);
      _isSorted = true;
   }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void expand(){
      assert(_size == _capacity);
      if(_capacity == 0){
         T* n = new T[1];
         _data = n;
         _capacity = 1;
      }
      else{
         T* n = new T[2 * _capacity];
         T* tmp = _data;
         for(size_t i = 0;i < _size;i++) n[i] = _data[i];
         _data = n;
         delete [] tmp;
         _capacity *= 2;
      }
   }
};

#endif // ARRAY_H
