/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <stdlib.h>
#include <time.h>
#include <math.h> //

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
#define bits 64
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  genDFSList();
  srand(time(NULL));
  int FecGsameTime = 0;
  int sim_time = 0;
  int timeLim = pow(log(_dfsList.size()), 2); 
  while(FecGsameTime < timeLim){
    vector<string> patterns;
    for(int i = 0;i < bits;++i){
      string pattern = "";
      for(int j = 0;j < _piList.size();++j){
        unsigned long x = rand();
        unsigned long y = rand();
        size_t num = (x << 32 | y);
        bitset<bits> b(num);
        pattern += to_string(b[i]);
      }
      patterns.push_back(pattern);
    }
    sim_time += bits;
    simulate(patterns);
    int bef = _FecGrps.size();
    genFec();
    genLog(patterns, bits);
    int aft = _FecGrps.size();
    if(bef == aft) ++FecGsameTime;
  }
  cout << sim_time << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  genDFSList();
  vector<string> patterns;
  string pattern;
  bool err = false;
  int cnt = 0;
  int sim_time = 0;
  while(patternFile >> pattern){
    if(pattern.size() != _piList.size()){
      cerr << "Error: Pattern(" << pattern << ") length(" << pattern.size() << ") does not match the number of inputs(" << _piList.size() << ") in a circuit!!" << endl;
      err = true;
      break;
    }
    for(int i = 0;i < pattern.size();++i){
      if(pattern[i] != '0' && pattern[i] != '1'){
        cerr << "Error: Pattern(" << pattern << ") contains a non-0/1 character('" << pattern[i] << "')." << endl;
        err = true;
        break;
      }
    }
    if(err) break;
    ++cnt;
    patterns.push_back(pattern);
    if(cnt % bits == 0){
      simulate(patterns);
      genFec();
      genLog(patterns, bits);
      sim_time += bits;
      patterns.clear();
    }
  }
  if(cnt % bits != 0 && !err){
    simulate(patterns);
    genFec();
    genLog(patterns, cnt % bits);
    sim_time += cnt % bits;
  }
  cout << sim_time << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::simulate(vector<string>& patterns){
  GateList::iterator it;
  vector<size_t> inputs;
  for(int i = 0;i < _piList.size();++i){
    size_t input = 0;
    for(int j = 0;j < patterns.size();++j){
      input |= (((size_t)patterns.at(j)[i] - '0') << j);
    }
    inputs.push_back(input);
    _piList.at(i) -> _simValue = input;
  }
  for(it = _dfsList.begin();it != _dfsList.end();++it){
    if((*it) -> isAig() || (*it) -> _type == PO_GATE)
      (*it) -> simulate();
  }
}

void
CirMgr::genLog(vector<string>& patterns, int num){
  if (_simLog == 0) return;
  vector<bitset<bits>> outputs;
  for(int i = 0;i < _poList.size();++i){
    bitset<bits> output(_poList.at(i) -> _simValue);
    outputs.push_back(output);
  }
  for(int i = 0;i < num;++i){
    *_simLog << patterns.at(i) << ' ';
    for(int j = 0;j < _poList.size();++j){
      *_simLog << outputs.at(j)[i];
    }
    *_simLog << '\n';
  }
}

void 
CirMgr::initialFec(){
  FecGrp* fecgrp = new FecGrp(0);
  _FecGrps.push_back(fecgrp);
  _FecGrps.at(0) -> _fecList[0] = pair<CirGate*, bool>(_const, 0);
  _const -> _fecgrp = fecgrp;
  GateList::iterator it;
  for(it = _dfsList.begin();it != _dfsList.end();++it){
    if((*it) -> isAig()){
      _FecGrps.at(0) -> _fecList[(*it) -> _varID] = pair<CirGate*, bool>(*it, 0);
      (*it) -> _fecgrp = fecgrp;
    }
  }
}

void 
CirMgr::genFec(){
  if(_initFec == false){
    initialFec();
    _initFec = true;
  } 
  int size = _FecGrps.size();
  for(int i = 0;i < size;++i){
    FecGrp* g = _FecGrps[i];

    map<size_t, FecGrp*> newFecGrps;
    Feciterator iter = g -> _fecList.begin();

    for(;iter != g -> _fecList.end();){
      // cout << "gate: " << iter -> first << " simvalue: " << iter -> second.first -> _simValue << endl;
      if(iter -> second.first -> _simValue != g -> _fecList.begin() -> second.first -> _simValue){
        if(iter -> second.first -> _simValue == ~(g -> _fecList.begin() -> second.first -> _simValue)){
          iter -> second.second = 1;
          ++iter;
        } 
        else{
          map<size_t, FecGrp*>::iterator found = newFecGrps.find(iter -> second.first -> _simValue);
          map<size_t, FecGrp*>::iterator foundinv = newFecGrps.find(~(iter -> second.first -> _simValue));
          if(found == newFecGrps.end() && foundinv == newFecGrps.end()){
            FecGrp* fecgrp = new FecGrp(_FecGrps.size());
            fecgrp -> _fecList[iter -> first] = pair<CirGate*, bool>(iter -> second.first, 0);
            newFecGrps.insert(pair<size_t, FecGrp*>(iter -> second.first -> _simValue, fecgrp));
            iter -> second.first -> _fecgrp = fecgrp;
            // cout << "new fecgrp: " << fecgrp -> _grpName << endl;
            // cout << "move: " << iter -> first << " (" << iter -> second.first -> _simValue << ") to fecgrp: " << fecgrp -> _grpName << endl;
          }
          else{
            bool inv = (found != newFecGrps.end())? 0: 1;
            (inv == 0)? found -> second -> _fecList[iter -> first] = pair<CirGate*, bool>(iter -> second.first, inv)
                      : foundinv -> second -> _fecList[iter -> first] = pair<CirGate*, bool>(iter -> second.first, inv);
            (inv == 0)? iter -> second.first -> _fecgrp = found -> second
                      : iter -> second.first -> _fecgrp = foundinv -> second;
            // cout << "only move: " << iter -> first << " (" << iter -> second.first -> _simValue << ") to fecgrp: " << found -> second -> _grpName << endl;
          }
          iter = g -> _fecList.erase(iter);
        }
      }
      else ++iter;
    }
    for(map<size_t, FecGrp*>::iterator nfg = newFecGrps.begin();nfg != newFecGrps.end();++nfg){
      if(nfg -> second -> _fecList.size() == 1){
        nfg -> second -> _fecList.begin() -> second.first -> _fecgrp = 0;
        delete nfg -> second;
      }
      else 
        _FecGrps.push_back(nfg -> second);
    }
  }
  vector<FecGrp*>::iterator del;
  for(del = _FecGrps.begin();del != _FecGrps.end();){
    if((*del) -> _fecList.size() == 1){
      (*del) -> _fecList.begin() -> second.first -> _fecgrp = 0;
      FecGrp* tmp = (*del);
      _FecGrps.erase(del);
      delete tmp;
    }
    else ++del;
  }
}
