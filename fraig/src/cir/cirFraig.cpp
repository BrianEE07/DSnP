/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
#define bits 64
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash(){
  MyHash hashmap;
  GateList::iterator it;
  MyHash::iterator iter;
  genDFSList();
  for(it = _dfsList.begin();it != _dfsList.end();++it){
    if((*it) -> isAig()){
      unsigned long long key;
      key = hashFunction((size_t)(*it) -> _faninList.at(0), (size_t)(*it) -> _faninList.at(1), (*it) -> _inv[0], (*it) -> _inv[1]);
      iter = hashmap.find(key);
      if(iter != hashmap.end()){
        cout << "Strashing: " << iter -> second -> _varID << " merging " << (*it) -> _varID << "..." << endl;
        merge(*it, iter -> second);
        CirGate* tmp = *it;
        _gateList.erase((*it) -> _varID);
        delete tmp;
      }
      else 
        hashmap.insert(pair<unsigned long long, CirGate*>(key, *it));
    }
  }
  genDFSList();
  hashmap.clear();
}

void 
CirMgr::printFEC() const{
  cout << "Total #FEC Group = " << _FecGrps.size();
}

void
CirMgr::fraig(){
  SatSolver solver;
  solver.initialize();
  genDFSList();
  genProofModel(solver);
  bool doGen = false;
  while(!_FecGrps.empty()){
    vector<string> patterns;
    
    int n = _dfsList.size();
    int pat_cnt = 0;
    for(int i = 0;i < n;++i){
        if(_dfsList[i] -> isAig()){
        FecGrp* g = _dfsList[i] -> _fecgrp;
        if(g == 0) continue;
        bool doMerge = true;
        Feciterator base = g -> _fecList.begin();
        Feciterator it = ++(g -> _fecList.begin());
        for(;it != g -> _fecList.end();++it){
          bool result = satProve(solver, base, it);
          if(result){
            doMerge = true;
            string pattern;
            if(getSatAssignment(solver, pattern)){
              patterns.push_back(pattern);
              ++pat_cnt;
            }
          }
          if(pat_cnt == bits) break;
        }
        if(doMerge == true){
          cout << "merging..." << endl;
          mergeFraig(g);
          doGen = true;
          cout << "Updating by UNSAT... "; printFEC(); cout << endl;
          break;
        }
        if(pat_cnt == bits){
          cout << "simulating..." << endl;
          simulate(patterns);
          genFec();
          cout << "Updating by SAT... "; printFEC(); cout << endl;
          break;
        }
      }
    }
    if(doGen == true){
      genDFSList();
      doGen = false;
    } 
  }
  //strash();
  _initFec = false;
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
unsigned long long
CirMgr::hashFunction(size_t f1, size_t f2, bool i1, bool i2){
  if(i1 == 1) f1 = 2 * f1 + 0X1;
  if(i2 == 1) f2 = 2 * f2 + 0X1;
  if(f1 > f2) return (f2 << 32) | f1;
  else return (f1 << 32) | f2;
}

void 
CirMgr::merge(CirGate*& src, CirGate*& des){
  for(int i = 0;i < src -> _faninList.size();++i){
    for(int j = 0;j < src -> _faninList.at(i) -> _fanoutList.size();++j)
      if(src -> _faninList.at(i) -> _fanoutList.at(j) == src){
        src -> _faninList.at(i) -> _fanoutList.erase(src -> _faninList.at(i) -> _fanoutList.begin() + j);
      }
  }
  for(int i = 0;i < src -> _fanoutList.size();++i){
    for(int j = 0;j < src -> _fanoutList.at(i) -> _faninList.size();++j)
      if(src -> _fanoutList.at(i) -> _faninList.at(j) == src){
        src -> _fanoutList.at(i) -> _faninList.at(j) = des;
      }
  }
}

void
CirMgr::genProofModel(SatSolver& s)
{
  // take care of const gate
  Var vf = s.newVar();
  _const -> _var = vf;
  Var v = s.newVar();
  s.addAigCNF(vf, v, 0, v, 1);
  for (size_t i = 0, n = _dfsList.size();i < n;++i){
    if(_dfsList[i] -> _type != PO_GATE && _dfsList[i] -> _type != CONST_GATE){
      Var v = s.newVar();
      _dfsList[i] -> _var = v;
    }
    if(_dfsList[i] -> isAig()){
      s.addAigCNF(_dfsList[i] -> _var, 
                  _dfsList[i] -> _faninList[0] -> _var, 
                  _dfsList[i] -> _inv[0], 
                  _dfsList[i] -> _faninList[1] -> _var, 
                  _dfsList[i] -> _inv[1]);
    }
  }
}

bool 
CirMgr::satProve(SatSolver& s, Feciterator f, Feciterator g){
  Var newV = s.newVar();
  s.addXorCNF(newV, f -> second.first -> _var, 
                    f -> second.second, 
                    g -> second.first -> _var, 
                    g -> second.second
                    );
  s.assumeRelease();
  s.assumeProperty(newV, true);
  return s.assumpSolve();
}

bool
CirMgr::getSatAssignment(SatSolver& s, string& pattern){
  for(int i = 0;i < _piList.size();++i){
    int val = s.getValue(_piList[i] -> _var);
    if(val == -1) return false;
    pattern += to_string(val);
  }
  return true;
}

void 
CirMgr::mergeFraig(FecGrp* g){
  Feciterator it;
  Feciterator base = g -> _fecList.begin();
  for(it = ++(g -> _fecList.begin());it != g -> _fecList.end();++it){
    assert(it -> second.first -> isAig());
    for(int i = 0;i < it -> second.first -> _fanoutList.size();++i){
      for(int j = 0;j < it -> second.first -> _fanoutList[i] -> _faninList.size();++j){
        if(it -> second.first -> _fanoutList[i] -> _faninList[j] == it -> second.first){
          it -> second.first -> _fanoutList[i] -> _faninList[j] = base -> second.first;
          it -> second.first -> _fanoutList[i] -> _inv[j] = (it -> second.second == base -> second.second)
          ? it -> second.first -> _fanoutList[i] -> _inv[j]
          : (it -> second.first -> _fanoutList[i] -> _inv[j] ^ 1);
        }
      } 
    }

    for(int i = 0;i < it -> second.first -> _faninList.size();++i){
      for(int j = 0;j < it -> second.first -> _faninList[i] -> _fanoutList.size();++j){
        if(it -> second.first -> _faninList[i] -> _fanoutList[j] == it -> second.first){
          it -> second.first -> _faninList[i] -> _fanoutList.erase(it -> second.first -> _faninList[i] -> _fanoutList.begin() + j);
        }
      }
    }
  }
  for(it = ++(g -> _fecList.begin());it != g -> _fecList.end();){
    assert(it -> second.first -> isAig());
    map<unsigned, CirGate*>::iterator del;
    CirGate* tmp = it -> second.first;
    _gateList.erase(it -> first);
    cout << "Fraig: " << base -> first << " merging ";
    cout << ((it -> second.second != base -> second.second)? "!": "") << it -> first << "..." << endl;
    it -> second.first -> _fecgrp = 0;
    it = g -> _fecList.erase(it);
    delete tmp;
  }
  base -> second.first -> _fecgrp = 0;
  vector<FecGrp*>::iterator fg;
  for(fg = _FecGrps.begin();fg != _FecGrps.end();++fg){
    if((*fg) == g){
      FecGrp* tmp = (*fg);
      _FecGrps.erase(fg);
      delete tmp;
      break;
    }
  }
}
