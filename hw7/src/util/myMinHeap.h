/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>
#include <iostream>//

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0){ if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear(){ _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const{ return _data[i];}   
   Data& operator [] (size_t i){ return _data[i];}

   size_t size() const{ return _data.size();}

   // TODO
   const Data& min() const{ return _data.at(0);}
   void insert(const Data& d){
      int t = _data.size();
      _data.push_back(d);
      while (t > 0) {
         int p = (t - 1) / 2;
         if (_data.at(p) < d || _data.at(p).getLoad() == d.getLoad()) break; 
         _data.at(t) = _data.at(p);
         t = p;
      }
      _data.at(t) = d;
   }
   void delMin(){
      int p = 0, t = 2 * p + 1;
      int n = _data.size() - 1;
      while (t <= n) {
         if (t < n){
            if (_data.at(t + 1) < _data.at(t)) ++t;
         }
         if (_data.at(n) < _data.at(t)) break;
         _data.at(p) = _data.at(t);
         p = t; 
         t = 2 * p + 1;
      } 
      _data.at(p) = _data.at(n);
      _data.pop_back();
   }
   void delData(size_t i){
      int p = i, t = 2 * p + 1;
      int n = _data.size() - 1;
      while (t <= n) {
         if (t < n){
            if (_data.at(t + 1) < _data.at(t)) ++t;
         }
         if (_data.at(n) < _data.at(t)) break;
         _data.at(p) = _data.at(t);
         p = t; 
         t = 2 * p + 1;
      } 
      t = (p - 1) / 2;
      while(t >= 0){
         if(_data.at(t) < _data.at(n) || _data.at(t).getLoad() == _data.at(n).getLoad()) break;
         _data.at(p) = _data.at(t);
         p = t;
         t = (t - 1) / 2;
      }
      _data.at(p) = _data.at(n);
      _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
