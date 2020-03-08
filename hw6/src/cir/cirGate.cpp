/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h
/**************************************/
/*   class CirGate member functions   */
/**************************************/

unsigned CirGate::_globalRef = 0;
int space = 0;
IdList rpt;

void
CirGate::dfsTraversal(GateList& dfsList){
   GateList::iterator it;
   for(it = _faninList.begin();it != _faninList.end();++it){
      if(!(*it) -> isGlobalRef() && (*it) -> _type != UNDEF_GATE){
         (*it) -> setToGlobalRef();
         (*it) -> dfsTraversal(dfsList);
      }
   }
  dfsList.push_back(this);
}


string 
CirGate::getTypeStr() const{ 
  switch(_type){
    case UNDEF_GATE:
      return "UNDEF";
    case PI_GATE:
      return "PI";
    case PO_GATE:
      return "PO";
    case AIG_GATE:
      return "AIG";
    case CONST_GATE:
      return "CONST";
    default:
      return "";
  }
}


void
CirGate::reportGate() const{
  string str(getTypeStr());
  str = str + "(" + to_string(_varID) + ")" + ((_symbol == "") ? "" : "\"" + _symbol + "\"") + ", line " + to_string(_lineN + 1);
  cout << "==================================================" << endl;
  cout << "= " << str << right << setw(50 - str.size() - 2) << "=" << endl;
  cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const{
  assert (level >= 0);
  bool repeat = false;
  cout << getTypeStr() << " " << _varID;
  if(level == 0 || _faninList.size() == 0){
    cout << endl;
    return;
  }
  for(int i = 0;i < rpt.size();++i){
    if(_varID == rpt.at(i)) {
      repeat = true;
      break;
    }
  }
  if(repeat) cout << " (*)" << endl;
  else{
    rpt.push_back(_varID);
    cout << endl;
    --level;
    ++space;
    for(int i = 0;i < _faninList.size();++i){
      for(int j = 0;j < space * 2;++j) cout << " ";
      cout << ((_inv[i] == 1) ? "!" : "");
      _faninList.at(i) -> reportFanin(level);
    }
    --space;
  }
  if(space == 0) rpt.clear();
}

void
CirGate::reportFanout(int level) const{
  assert (level >= 0);
  bool repeat = false;
  cout << getTypeStr() << " " << _varID;
  if(level == 0 || _fanoutList.size() == 0){
    cout << endl;
    return;
  }
  for(int i = 0;i < rpt.size();++i){
    if(_varID == rpt.at(i)) {
      repeat = true;
      break;
    }
  }
  if(repeat) cout << " (*)" << endl;
  else{
    rpt.push_back(_varID);
    cout << endl;
    --level;
    ++space;
    for(int i = 0;i < _fanoutList.size();++i){
      for(int j = 0;j < space * 2;++j) cout << " ";
      bool inv = 0;
      for(int k = 0;k < _fanoutList.at(i) -> _faninList.size();++k){
        if(_fanoutList.at(i) -> _faninList.at(k) == this){
          inv = (_fanoutList.at(i) -> _inv[k]);
          break;
        }
      }
      cout << ((inv == 1) ? "!" : "");
      _fanoutList.at(i) -> reportFanout(level);
    }
    --space;
  }
  if(space == 0) rpt.clear();
}

