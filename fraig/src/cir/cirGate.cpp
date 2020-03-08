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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/

unsigned long long CirGate::_globalRef = 0;
int space = 0;
IdList rpt;

void
CirGate::dfsTraversal(GateList& dfsList){
   GateList::iterator it;
   for(it = _faninList.begin();it != _faninList.end();++it){
      if(!(*it) -> isGlobalRef()){
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

// Printing functions
void
CirGate::reportGate() const{
   string str(getTypeStr());
   bitset<sizeof(size_t) * 8> b(_simValue);
   Feciterator it;
   str = str + "(" + to_string(_varID) + ")" + ((_symbol == "") ? "" : "\"" + _symbol + "\"") + ", line " + to_string(_lineN + 1);
   cout << "================================================================================" << endl;
   cout << "= " << str << endl;
   cout << "= FECs:";
   if(_fecgrp != 0){
      if(_fecgrp -> _fecList.size() != 0){
         for(it = _fecgrp -> _fecList.begin();it != _fecgrp -> _fecList.end();++it){
            if(it -> first != _varID){
               (it -> second.second == _fecgrp -> _fecList[_varID].second)? cout << " " << it -> first: cout << " !" << it -> first;   
            }
         }
      } 
   }
   cout << endl;
   cout << "= Value: ";
   for(int i = sizeof(size_t) - 1;i >= 0;--i){
      for(int j = 8 - 1;j >= 0;--j){
         cout << b[sizeof(size_t) * i + j];
      }
      if(i != 0) cout << "_";
      else cout << endl;
   }
   cout << "================================================================================" << endl;
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

// simulate
bool
CirGate::simulate(){
   if(_type == PO_GATE){
      _simValue = (_inv[0] == 1)? ~_faninList.at(0) -> _simValue: _faninList.at(0) -> _simValue;
   }
   // AIG_GATE
   else{
      size_t a = (_inv[0] == 1)? ~_faninList.at(0) -> _simValue: _faninList.at(0) -> _simValue;
      size_t b = (_inv[1] == 1)? ~_faninList.at(1) -> _simValue: _faninList.at(1) -> _simValue;
      _simValue = a & b;
   }
   return true;
}
