/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep(){
   genDFSList();
   GateList::iterator it;
   IdList::iterator it_id;
   map<unsigned, CirGate*>::iterator iter;
   for(it = _dfsList.begin();it != _dfsList.end();++it){
      (*it) -> _toDel = false;
   }
   for(iter = _gateList.begin();iter != _gateList.end();){
      CirGate* tmp = 0;
      if(iter -> second -> _toDel == true){
         if(iter -> second -> isAig() || iter -> second -> _type == UNDEF_GATE){
            tmp = iter -> second;
            // cout << "remove from gateList : " << iter -> second -> getTypeStr() << "(" << iter -> first << ")" << endl;
            iter = _gateList.erase(iter);
            cout << "Sweeping: " << tmp -> getTypeStr() << "(" << tmp -> _varID << ")" << " removed..." << endl;
            delete tmp;
         }
         else{
            iter -> second -> _fanoutList.clear();
            ++iter;
         }
      }
      else{
         if(iter -> second -> _fanoutList.size() != 0){
            GateList::iterator i = iter -> second -> _fanoutList.begin();
            for(;i != iter -> second -> _fanoutList.end();){
               if((*i) -> _toDel == true){ 
                  i = iter -> second -> _fanoutList.erase(i);
               }
               else ++i;
            }
         }
         ++iter;
      }
   }
   for(iter = _gateList.begin();iter != _gateList.end();++iter){
      iter -> second -> _toDel = true;
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void 
CirMgr::optimize(){
  GateList::iterator it;
  genDFSList();
  for(it = _dfsList.begin();it != _dfsList.end();++it){
    if((*it) -> isAig()){
        int del = 0;
        for(int i = 0;i < (*it) -> _faninList.size();++i){
          assert(i <= 1);
          // case1:fanin has const 0
          if((*it) -> _faninList.at(i) -> _type == CONST_GATE && (*it) -> _inv[i] == 0){
              optReconnect(*it, i, 0);
              optDelete(*it, i, 0);
              del = 1;
              break;
          }
          // case2:fanin has const 1
          else if((*it) -> _faninList.at(i) -> _type == CONST_GATE && (*it) -> _inv[i] == 1){
              optReconnect(*it, !i, 0);
              optDelete(*it, !i, 0);
              del = 1;
              break;
          }
          // nothing
          else continue;
        }
        if(del == 1) continue;
        // case3:identical fanins
        if((*it) -> _faninList.at(0) == (*it) -> _faninList.at(1) && (*it) -> _inv[0] == (*it) -> _inv[1]){
          optReconnect(*it, 0, 0);
          optDelete(*it, 0, 0);
        } 
        // case4:inverted fanins
        else if((*it) -> _faninList.at(0) == (*it) -> _faninList.at(1) && (*it) -> _inv[0] != (*it) -> _inv[1]){
          optReconnect(*it, 0, 1);
          optDelete(*it, 0, 1);
        }
        // nothing
        else continue;
    }
  }
  // not used undef gates
  map<unsigned, CirGate*>::iterator iter;
  for(iter = _gateList.begin();iter != _gateList.end();){
    if(iter -> second -> _type == UNDEF_GATE && iter -> second -> _fanoutList.size() == 0){
        CirGate* tmp = iter -> second;
        iter = _gateList.erase(iter);
        delete tmp;
    }
    else ++iter;
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void
CirMgr::optReconnect(CirGate*& g, int i, bool iscnst){
  for(int j = 0;j < g -> _fanoutList.size();++j){
    for(int k = 0;k < g -> _fanoutList.at(j) -> _faninList.size();++k)
        if(g -> _fanoutList.at(j) -> _faninList.at(k) == g){
          if(!iscnst){
              if(g -> _fanoutList.at(j) -> _inv[k] == 0)
                g -> _fanoutList.at(j) -> _inv[k] = g -> _inv[i];
              else
                g -> _fanoutList.at(j) -> _inv[k] = !(g -> _inv[i]);
          }
          g -> _fanoutList.at(j) -> _faninList.at(k) = (iscnst)? _const: g -> _faninList.at(i);
        }
  }
  for(int j = 0;j < g -> _faninList.size();++j){
    for(int k = 0;k < g -> _faninList.at(j) -> _fanoutList.size();++k)
        if(g -> _faninList.at(j) -> _fanoutList.at(k) == g)
          g -> _faninList.at(j) -> _fanoutList.erase(g -> _faninList.at(j) -> _fanoutList.begin() + k);
  }
  
  for(int l = 0;l < g -> _fanoutList.size();++l)
    (iscnst)? _const -> _fanoutList.push_back(g -> _fanoutList.at(l)): g -> _faninList.at(i) -> _fanoutList.push_back(g -> _fanoutList.at(l));
}

void
CirMgr::optDelete(CirGate*& g, int i, bool iscnst){
  map<unsigned, CirGate*>::iterator iter;
  CirGate* tmp;
  iter = _gateList.find(g -> _varID);
  tmp = iter -> second;
  _gateList.erase(iter);
  cout << "Simplifying: " << ((iscnst)? 0:g -> _faninList.at(i) -> _varID) << " merging ";
  cout << ((g -> _inv[i] && !iscnst)? "!": "");
  cout << g ->_varID << "..." << endl;
  delete tmp;
}