/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

#include <iostream>
using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* p = 0):
      _data(d), _left(l), _right(r), _parent(p){}

   T _data;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
   BSTreeNode<T>* _parent;
};


template <class T>
class BSTree
{
public:
   // TODO: design your own class!!
   BSTree() : _size(0){
      _root = new BSTreeNode<T>(T());
      _root -> _left = _root -> _right = _root -> _parent = _root; // _root is a Dummy Node.
   }
   ~BSTree() { clear(); delete _root; }
   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n = 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node -> _data; }
      T& operator * () { return _node -> _data; }
      iterator& operator ++ () { 
         if(this -> isDum()) {}
         else if(this -> isRDum() == true){
            while(this -> isRChild()){
               _node = _node -> _parent;
               if(this -> isDum() == true) break;  
            }
            if(this -> isDum() == false) 
               _node = _node -> _parent; 
         }
         else{
            _node = _node -> _right;
            while(this -> isLDum() == false) _node = _node -> _left;
         }
         return *(this);
      }
      iterator operator ++ (int) { 
         iterator tmp(_node);
         ++(*this);
         return tmp; 
      }
      iterator& operator -- () { 
         if(this -> isDum()) _node = _node -> _parent;
         else if(this -> isLDum() == true){
            while(this -> isLChild()){
               _node = _node -> _parent;
               if(this -> isDum() == true) break;  
            }
            if(this -> isDum() == false) 
               _node = _node -> _parent;
         }
         else{
            _node = _node -> _left;
            while(this -> isRDum() == false) _node = _node -> _right;
         }
         return *(this); 
      }
      iterator operator -- (int) { 
         iterator tmp(_node);
         --(*this);
         return tmp; 
      }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return *(this); 
      }

      bool operator != (const iterator& i) const { 
         return _node != i._node;
      }
      bool operator == (const iterator& i) const {
         return _node == i._node; 
      }
      
   private:
      BSTreeNode<T>* _node;
      // private member function
      inline bool isDum(){
         if(_node -> _left == _node) return true;
         return false;
      }
      inline bool isLDum(){
         if(_node -> _left -> _left == _node -> _left) return true;
         return false;
      }
      inline bool isRDum(){
         if(_node -> _right -> _left == _node -> _right) return true;
         return false;
      }
      inline bool isPDum(){
         if(_node -> _parent -> _left == _node -> _parent) return true;
         return false;
      }
      inline bool isLeaf(){
         if(isLDum() && isRDum()) return true;
         return false;
      }
      inline bool isLChild(){
         if(_node == _node -> _parent -> _left) return true;
         return false;
      }
      inline bool isRChild(){
         if(_node == _node -> _parent -> _right) return true;
         return false;
      }
   };
   // _root -> _parent is dummy
   // dummy -> _left is dummy 
   // dummy -> _right is _root
   // dummy -> _parent is last(biggest) element(!empty) or dummy(empty)
   // leaf -> _left & _right are both dummy
   iterator begin() const {
      if(empty()) return end();
      iterator tmp(_root);
      while(tmp.isLDum() == false) tmp._node = tmp._node -> _left;
      return tmp; 
   }
   iterator end() const {     // dummy
      iterator tmp(_root -> _parent);
      return tmp; 
   }
   bool empty() const { 
      if(_root -> _parent == _root) return true;
      return false; 
   }
   size_t size() const {  
      return _size;
   }

   void insert(const T& x) {
      if(empty()){
         BSTreeNode<T>* n = new BSTreeNode<T>(x);
         toDum(n -> _left);
         toDum(n -> _right);
         toDum(n -> _parent);
         _root -> _parent = n;
         _root -> _right = n;
         _root = n;
      }
      else if(x > end()._node -> _parent -> _data){  // bigger then ever
         BSTreeNode<T>* n = new BSTreeNode<T>(x);
         toDum(n -> _left);
         toDum(n -> _right);
         end()._node -> _parent -> _right = n;
         n -> _parent = end()._node -> _parent;
         end()._node -> _parent = n;
      }
      else{ 
         insert(_root, x);
      }
      ++_size; 
   }
   void pop_front() {      
      if(empty()) return;
      erase(begin());
   }
   void pop_back() {
      if(empty()) return;
      iterator tmp(end()); --tmp;
      erase(tmp);
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(empty()) return false;
      if(pos.isLeaf()) delLeaf(pos);   // leaf case
      else if(pos.isLDum() || pos.isRDum()) delOneChild(pos);     // one child case
      else delTwoChild(pos);           // two child case
      --_size;
      return true; 
   }
   bool erase(const T& x) { 
      iterator era = find(x);
      if(era == end()) return false;
      erase(era); // _size decrease 1
      return true; 
   }

   iterator find(const T& x) { 
      iterator it(_root);
      return find(x, it);
   }

   void clear() { 
      while(_size > 0) pop_front();
   }  // delete all nodes except for the dummy node

   void sort() const { 
   }
   void print() const {
   }
private:
   BSTreeNode<T>*  _root;     // = dummy node if tree is empty
   size_t _size;
   void toDum(BSTreeNode<T>* &n) const {
       n = _root -> _parent;
   }
   void insert(BSTreeNode<T>* node, const T& x){
      if(x <= node -> _data){
         if(node -> _left -> _left == node -> _left){
            BSTreeNode<T>* n = new BSTreeNode<T>(x);
            toDum(n -> _left);
            toDum(n -> _right);
            node -> _left = n;
            n -> _parent = node;
         }
         else insert(node -> _left, x);
      }
      else{
         if(node -> _right -> _left == node -> _right){
            BSTreeNode<T>* n = new BSTreeNode<T>(x);
            toDum(n -> _left);
            toDum(n -> _right);
            node -> _right = n;
            n -> _parent = node;
         }
         else insert(node -> _right, x);
      }
   }
   void delLeaf(iterator pos){
      iterator smaller(pos); --smaller;
      if(pos.isLChild()){
         toDum(pos._node -> _parent -> _left);
      } 
      else{
         toDum(pos._node -> _parent -> _right);
         if(pos._node == _root -> _parent -> _parent){
            _root -> _parent -> _parent = smaller._node;// biggest is deleted
         }
      }
      if(pos._node == _root) {
         toDum(_root);
      }
      delete pos._node;
   }
   void delOneChild(iterator pos){
      iterator smaller(pos); --smaller;
      if(pos.isLDum() == false){      // has left child
         if(pos.isLChild()){
            pos._node -> _parent -> _left = pos._node -> _left;
            pos._node -> _left -> _parent = pos._node -> _parent;
         }
         else{
            pos._node -> _parent -> _right = pos._node -> _left;
            pos._node -> _left -> _parent = pos._node -> _parent;
            if(pos._node == _root -> _parent -> _parent){
               _root -> _parent -> _parent = smaller._node;// biggest is deleted
            }
         }
         if(pos._node == _root){
            _root = pos._node -> _left;
         }
      }
      else{    // has right child
         if(pos.isLChild()){
            pos._node -> _parent -> _left = pos._node -> _right;
            pos._node -> _right -> _parent = pos._node -> _parent;
         } 
         else{
            pos._node -> _parent -> _right = pos._node -> _right;
            pos._node -> _right -> _parent = pos._node -> _parent;
         }
         if(pos._node == _root){
            _root = pos._node -> _right;
            
         }
      }
      delete pos._node;
   }
   void delTwoChild(iterator pos){
      iterator smaller(pos); --smaller;
      iterator bigger(pos); ++bigger;
      *pos = *bigger; // two child ------>  bigger won’t be dummy
      // delete bigger node
      // bigger node can be either Leaf or OneChild case
      if(bigger.isLeaf()) delLeaf(bigger);
      else delOneChild(bigger);
   }
   iterator find(const T& x, iterator& pos){
      if(x == *pos) return pos;
      else if(x < *pos){
         pos._node = pos._node -> _left;
         if(pos.isDum() == false)
            return find(x, pos);
         else
            return end();
      }
      else{ // x > *it
         pos._node = pos._node -> _right;
         if(pos.isDum() == false)
            return find(x, pos);
         else
            return end();
      }
   }
};

#endif // BST_H
