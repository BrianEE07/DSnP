/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

CirGate* CirMgr::_const = new CirGate(0, -1, CONST_GATE);

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo() + 1
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName){
   ifstream file;
   file.open(fileName, ios::in);
   if(!file){
      cerr << "cannot open file" << endl;
      return false;
   }
   if(!readHeader(file)) return false;
   if(!readInput(file)) return false;
   if(!readOutput(file)) return false;
   if(!readAig(file)) return false;
   if(!readSymbol(file)) return false;
   file.close();
   _gateList.insert(pair<unsigned, CirGate*>(0, _const));
   connect();
   // use lineNo
   lineNo = 0;
   colNo = 0;
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary(){
   size_t i, o, a, t = 0;
   i = _piList.size();
   o = _poList.size();
   a = 0;
   map<unsigned, CirGate*>::iterator it = _gateList.begin();
   for(;it != _gateList.end();++it){
      if(it -> second -> isAig()) ++a;
   }
   t = i + o + a;
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << right << setw(12) << i << endl;
   cout << "  PO" << right << setw(12) << o << endl;
   cout << "  AIG" << right << setw(11) << a << endl;
   cout << "------------------" << endl;
   cout << "  Total" << right << setw(9) << t << endl;
}

void
CirMgr::printNetlist(){
   genDFSList();
   GateList:: iterator it;
   int idx = 0;
   cout << endl;
   for(it = _dfsList.begin();it != _dfsList.end();++it){
      if((*it) -> _type != UNDEF_GATE){
         cout << "[" << idx << "] ";
         cout << left << setw(4) << (*it) -> getTypeStr() << (*it) -> _varID;
         for(int j = 0;j < (*it) -> _faninList.size();++j){
            cout << " ";
            cout << (((*it) -> _faninList.at(j) -> _type == UNDEF_GATE) ? "*" : "");
            cout << (((*it) -> _inv[j]) ? "!" : "") << (*it) -> _faninList.at(j) -> _varID;
         }
         ((*it) -> _symbol == "") ? cout << endl : cout << " (" << (*it) -> _symbol << ")" << endl;
         ++idx;
      }
   }
}

void
CirMgr::printPIs(){
   cout << "PIs of the circuit:";
   GateList:: iterator it;
   for(it = _piList.begin();it != _piList.end();++it) cout << " " << (*it) -> _varID;
   cout << endl;
}

void
CirMgr::printPOs(){
   cout << "POs of the circuit:";
   GateList:: iterator it;
   for(it = _poList.begin();it != _poList.end();++it) cout << " " << (*it) -> _varID;
   cout << endl;
}

void
CirMgr::printFloatGates(){
   // already in ascending order because use map
   map<unsigned, CirGate*>::iterator iter;
   int count = 0;
   for(iter = _gateList.begin();iter != _gateList.end();++iter){
      if(iter -> second -> _type == PO_GATE || iter -> second -> isAig()){
         for(int i = 0;i < iter -> second ->_faninList.size();++i){
            if(iter -> second -> _faninList.at(i) -> _type == UNDEF_GATE){
               ++count;
               if(count == 1) cout << "Gates with floating fanin(s):";
               cout << " " << iter -> first;
               break; 
            }
         }
      }
   }
   if(count != 0) cout << endl;
   count = 0;
   for(iter = _gateList.begin();iter != _gateList.end();++iter){
      if(iter -> second -> _type == PI_GATE || iter -> second -> isAig()){
         if(iter -> second -> _fanoutList.size() == 0){
            ++count;
            if(count == 1) cout << "Gates defined but not used  :";
            cout << " " << iter -> first;
         }
      }
   }
   cout << endl;
}

void
CirMgr::printFECPairs() const{
   if(!_FecGrps.empty()){
      vector<FecGrp*> print = _FecGrps;
      sort(print.begin(), print.end(), CirMgr::compare);
      for(int i = 0;i < print.size();++i){
         FecGrp* g = print.at(i);
         cout << "[" << i << "]";
         Feciterator it;
         for(it = g -> _fecList.begin();it != g -> _fecList.end();++it){
            (it -> second.second == g -> _fecList.begin() -> second.second)
            ? cout << " " << it -> first
            : cout << " !" << it -> first; 
         }
         cout << endl;
      }
   }
}

void
CirMgr::writeAag(ostream& outfile){
   genDFSList();
   GateList::iterator it;
   unsigned A = 0;
   for(int i = 0;i < _dfsList.size();++i){
      if(_dfsList.at(i) -> isAig()) A++;
   }
   outfile << "aag";
   for(int i = 0;i < 4;i++) outfile << " " << miloa[i];
   outfile << " " << A;
   outfile << '\n';
   for(it = _piList.begin();it != _piList.end();++it) outfile << (*it) -> _varID * 2 << '\n';
   for(it = _poList.begin();it != _poList.end();++it) outfile << (*it) -> _faninList.at(0) -> _varID * 2 + (((*it) -> _inv[0] == 1) ? 1 : 0) << '\n';
   for(it = _dfsList.begin();it != _dfsList.end();++it){
      if((*it) -> isAig()){
         outfile << (*it) -> _varID * 2;
         outfile << " " << (*it) -> _faninList.at(0) -> _varID * 2 + (((*it) -> _inv[0] == 1) ? 1 : 0);
         outfile << " " << (*it) -> _faninList.at(1) -> _varID * 2 + (((*it) -> _inv[1] == 1) ? 1 : 0); 
         outfile << '\n';
      }
   }
   for(int i = 0;i < _piList.size();++i) 
      if(_piList.at(i) -> _symbol != "")
         outfile << "i" << i << " " << _piList.at(i) -> _symbol << '\n';
   for(int i = 0;i < _poList.size();++i) 
      if(_poList.at(i) -> _symbol != "")
         outfile << "o" << i << " " << _poList.at(i) -> _symbol << '\n';
   outfile << "c" << '\n' << "AAG output by Yung-Wei Fan." << '\n';
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const{
   
}


//private helper functions
bool 
CirMgr::readHeader(ifstream& fi){
   // todo colNo
   fi.getline(buf, 1024);
   string line(buf);
   if(buf[0] == '\0'){
      errMsg = "aag";
      return parseError(MISSING_IDENTIFIER);
   }
   if(buf[0] == ' ') return parseError(EXTRA_SPACE);
   if(buf[0] == '\t'){
      errInt = (int)buf[0];
      return parseError(ILLEGAL_WSPACE);
   }
   size_t idx = line.find_first_of(" ");
   string idt = line.substr(0, idx);
   if(idt != "aag"){
      errMsg = idt;
      return parseError(ILLEGAL_IDENTIFIER);
   }
   string tmp[5];
   string name[5] = {"variables", "PIs", "latches", "POs", "AIGs"};
   if(idx == string::npos){
         errMsg = "number of " + name[0];
         return parseError(MISSING_NUM);
   }
   for(int i = 0;i < 5;i++){
      if(idx == line.size() - 1){
         errMsg = "number of " + name[i];
         return parseError(MISSING_NUM);
      }
      ++idx;
      if(line.at(idx) == ' ') return parseError(EXTRA_SPACE);
      if(line.at(idx) == '\t'){
         errInt = (int)line.at(idx);
         return parseError(ILLEGAL_WSPACE);
      }
      size_t idx_end = line.find_first_of(" ", idx);
      tmp[i] = line.substr(idx, idx_end - idx);
      stringstream ss(tmp[i]);
      unsigned num;
      if(!(ss >> num) || tmp[i] != to_string(num)){
         errMsg = "number of " + name[i] + "(" + tmp[i] + ")";
         return parseError(ILLEGAL_NUM);
      }
      if(idx_end == string::npos && i != 4){
         errMsg = "number of " + name[i + 1];
         return parseError(MISSING_NUM);
      }
      if(i == 4 && idx_end != string::npos) return parseError(MISSING_NEWLINE);
      idx = idx_end;
      miloa[i] = num;
   }
   if(miloa[0] < miloa[1] + miloa[2] + miloa[4]){
      errMsg = "Number of " + name[0];
      errInt = miloa[0];
      return parseError(NUM_TOO_SMALL);
   }
   if(miloa[2] != 0){
      errMsg = name[2];
      return parseError(ILLEGAL_NUM);
   }
   lineNo = 1;
   return true;
}
bool
CirMgr::readInput(ifstream& fi){
   unsigned gateID;
   for(int i = 0;i < miloa[1];++i){
      fi.getline(buf, 1024);
      string line(buf);
      if(buf[0] == '\0'){
         errMsg = "PI literal ID";
         return parseError(MISSING_NUM);
      }
      if(buf[0] == ' ') return parseError(EXTRA_SPACE);
      if(buf[0] == '\t'){
         errInt = (int)buf[0];
         return parseError(ILLEGAL_WSPACE);
      }
      size_t idx = line.find_first_of(" ");
      line = line.substr(0, idx);
      stringstream ss(line);
      unsigned inLtr;
      if(!(ss >> inLtr) || line != to_string(inLtr)){
         errMsg = "PI literal ID(" + line + ")";
         return parseError(ILLEGAL_NUM);
      }
      if(idx != string::npos) return parseError(MISSING_NEWLINE);
      if(inLtr < 2){
         errInt = inLtr;
         return parseError(REDEF_CONST);
      }
      if(inLtr > miloa[0] * 2 + 1){
         errInt = inLtr;
         return parseError(MAX_LIT_ID);
      }
      if(inLtr % 2 != 0){
         errMsg = "PI";
         errInt = inLtr;
         return parseError(CANNOT_INVERTED);
      }
      map<unsigned, CirGate*>::iterator it;
      it = _gateList.find(inLtr / 2);
      if(it != _gateList.end()){
         errGate = it -> second;
         errInt = inLtr;
         return parseError(REDEF_GATE);
      }
      gateID = inLtr / 2;
      CirGate* pi = new CirPiGate(gateID, lineNo);
      _piList.push_back(pi);
      _gateList.insert(pair<unsigned, CirGate*>(gateID, pi));
      lineNo = i + 2;
   }
   return true;
}
bool
CirMgr::readOutput(ifstream& fi){
   unsigned gateID;
   for(int i = 0;i < miloa[3];++i){
      fi.getline(buf, 1024);
      string line(buf);
      if(buf[0] == '\0'){
         errMsg = "PO literal ID";
         return parseError(MISSING_NUM);
      }
      if(buf[0] == ' ') return parseError(EXTRA_SPACE);
      if(buf[0] == '\t'){
         errInt = (int)buf[0];
         return parseError(ILLEGAL_WSPACE);
      }
      size_t idx = line.find_first_of(" ");
      line = line.substr(0, idx);
      stringstream ss(line);
      unsigned outLtr;
      if(!(ss >> outLtr) || line != to_string(outLtr)){
         errMsg = "PO literal ID(" + line + ")";
         return parseError(ILLEGAL_NUM);
      }
      if(idx != string::npos) return parseError(MISSING_NEWLINE);
      if(outLtr > miloa[0] * 2 + 1){
         errInt = outLtr;
         return parseError(MAX_LIT_ID);
      }
      gateID = miloa[0] + i + 1;
      CirGate* po = new CirPoGate(gateID, lineNo, outLtr % 2);
      unsigned in = (outLtr % 2 == 0) ? outLtr / 2 : (outLtr - 1) / 2;
      po -> _faninList.push_back((CirGate*)(size_t)in);
      _poList.push_back(po);
      _gateList.insert(pair<unsigned, CirGate*>(gateID, po));
      lineNo = i + miloa[1] + 2;
   }
   return true;
}
bool
CirMgr::readAig(ifstream& fi){
   unsigned gateID;
   for(int i = 0;i < miloa[4];i++){
      fi.getline(buf, 1024);
      string line(buf);
      if(buf[0] == '\0'){
         errMsg = "AIG gate literal ID";
         return parseError(MISSING_NUM);
      }
      if(buf[0] == ' ') return parseError(EXTRA_SPACE);
      if(buf[0] == '\t'){
         errInt = (int)buf[0];
         return parseError(ILLEGAL_WSPACE);
      }
      string tmp[3];
      size_t idx = 0;
      unsigned content[3];
      for(int j = 0;j < 3;++j){
         idx = line.find_first_of(" ");
         tmp[j] = line.substr(0, idx);
         if(j != 2 && idx != string::npos) line = line.substr(idx + 1);
         stringstream ss(tmp[j]);
         if(!(ss >> content[j]) || tmp[j] != to_string(content[j])){
            if(j == 0) errMsg = "AIG gate literal ID(" + tmp[j] + ")";
            else errMsg = "AIG input literal ID(" + tmp[j] + ")";
            return parseError(ILLEGAL_NUM);
         }
         if(j == 0){
            if(content[j] < 2){
               errInt = content[j];
               return parseError(REDEF_CONST);
            }
            if(content[j] > miloa[0] * 2 + 1){
               errInt = content[j];
               return parseError(MAX_LIT_ID);
            }
            if(content[j] % 2 != 0){
               errMsg = "AIG gate";
               errInt = content[j];
               return parseError(CANNOT_INVERTED);
            }
            map<unsigned, CirGate*>::iterator it;
            it = _gateList.find(content[0] / 2);
            if(it != _gateList.end()){
               errGate = it -> second;
               errInt = content[0];
               return parseError(REDEF_GATE);
            }
         }
         else{
            if(content[j] > miloa[0] * 2 + 1){
               errInt = content[j];
               return parseError(MAX_LIT_ID);
            }
         }
         if(j != 2 && idx == string::npos) return parseError(MISSING_SPACE);
         if(j != 2 && line.size() == 0){
            errMsg = "AIG input literal ID";
            return parseError(MISSING_NUM);
         }
         if(j != 2 && line.at(0) == ' ') return parseError(EXTRA_SPACE);
         if(j != 2 && line.at(0) == '\t'){
            errInt = (int)line.at(0);
            return parseError(ILLEGAL_WSPACE);
         }
         if(j == 2 && idx != string::npos) return parseError(MISSING_NEWLINE);
      }
      gateID = content[0] / 2;
      CirGate* aig = new CirAigGate(gateID, lineNo, content[1] % 2, content[2] % 2);
      unsigned in0 = (content[1] % 2 == 0) ? content[1] / 2 : (content[1] - 1) / 2;
      unsigned in1 = (content[2] % 2 == 0) ? content[2] / 2 : (content[2] - 1) / 2;
      aig -> _faninList.push_back((CirGate*)(size_t)in0);
      aig -> _faninList.push_back((CirGate*)(size_t)in1);
      _gateList.insert(pair<unsigned, CirGate*>(gateID, aig));
      lineNo =  i + miloa[1] + miloa[3] + 2;
   }
   return true;
}
bool
CirMgr::readSymbol(ifstream& fi){
   string line;
   int count = 0;
   while(getline(fi, line)){
      if(line.size() == 0){
         errMsg = line;
         return parseError(ILLEGAL_SYMBOL_TYPE);
      }
      if(line.at(0) == ' ') return parseError(EXTRA_SPACE);
      if(line.at(0) == '\t'){
         errInt = (int)line.at(0);
         return parseError(ILLEGAL_WSPACE);
      }
      if(line == "c") break;
      if(line.at(0) != 'i' && line.at(0) != 'o'){
         errMsg = line.at(0);
         return parseError(ILLEGAL_SYMBOL_TYPE);
      }
      if(line.size() == 1){
         errMsg = "symbol index";
         return parseError(MISSING_NUM);
      }
      if(line.at(1) == ' ') return parseError(EXTRA_SPACE);
      if(line.at(1) == '\t'){
         errInt = (int)line.at(1);
         return parseError(ILLEGAL_WSPACE);
      }
      size_t idx = line.find_first_of(" ");
      string indexS = line.substr(1, idx - 1);
      unsigned index;
      stringstream ss(indexS);
      if(!(ss >> index) || indexS != to_string(index)){
         errMsg = "symbol index(" + indexS + ")";
         return parseError(ILLEGAL_NUM);
      }
      if(line.at(0) == 'i' && index >= miloa[1]){
         errMsg = "PI";
         errInt = index;
         return parseError(NUM_TOO_BIG);
      }
      if(line.at(0) == 'o' && index >= miloa[3]){
         errMsg = "PO";
         errInt = index;
         return parseError(NUM_TOO_BIG);
      }
      if(idx == string::npos){
         errMsg = "symbolic name";
         return parseError(MISSING_IDENTIFIER);
      }
      if(idx == line.size() - 1){
         errMsg = "symbolic name";
         return parseError(MISSING_IDENTIFIER);
      }
      string symbName = line.substr(idx + 1);
      if(line.at(0) == 'i'){
         if(_piList.at(index) -> _symbol != ""){
            errMsg = "i";
            errInt = index;
            return parseError(REDEF_SYMBOLIC_NAME);
         }
         _piList.at(index) -> _symbol = symbName;
      }
      if(line.at(0) == 'o'){
         if(_poList.at(index) -> _symbol != ""){
            errMsg = "o";
            errInt = index;
            return parseError(REDEF_SYMBOLIC_NAME);
         }
         _poList.at(index) -> _symbol = symbName;
      }
      // undone:isprint() == false
      lineNo = count + miloa[1] + miloa[3] + miloa[4] + 2;
      count++;
   }
   return true;
}
void 
CirMgr::connect(){
   map<unsigned, CirGate*>::iterator it;
   for(it = _gateList.begin();it !=_gateList.end();it++){
      if(it -> second -> _type == PI_GATE) continue;
      else if(it -> second -> _type == CONST_GATE) continue;
      else if(it -> second -> _type == UNDEF_GATE) continue;
      // PO & AIG
      else{ 
         for(int i = 0;i < it -> second -> _faninList.size();++i){
            unsigned faninNum = (unsigned)(size_t)(void*)(it -> second -> _faninList.at(i));
            map<unsigned, CirGate*>::iterator iter;
            iter = _gateList.find(faninNum);
            if(iter != _gateList.end() && iter -> second -> _type != UNDEF_GATE)
            {
               it -> second -> _faninList.at(i) = iter -> second;
               iter -> second -> _fanoutList.push_back(it -> second);
            }
            else
            {
               if(iter == _gateList.end()){
                  CirGate* floating = new CirGate(faninNum, -1);
                  _gateList.insert(pair<unsigned, CirGate*>(faninNum, floating));
               }
               it -> second -> _faninList.at(i) = _gateList.find(faninNum) -> second;
               _gateList.find(faninNum) -> second -> _fanoutList.push_back(it -> second);
            }
         }
      }
   }
}

void 
CirMgr::genDFSList(){
   CirGate::setGlobalRef();
   _dfsList.clear();
   GateList::iterator it;
   for(it = _poList.begin();it != _poList.end();++it){
      (*it) -> dfsTraversal(_dfsList);
   }
}

void
CirMgr::reset(){
   map<unsigned, CirGate*>::iterator it;
   for(it = _gateList.begin();it != _gateList.end();++it){
      if(it -> second -> _type != CONST_GATE) delete (it -> second);
   }
   _const -> _fecgrp = 0;
}

bool
CirMgr::compare(FecGrp* g1, FecGrp* g2){
   unsigned a = g1 -> _fecList.begin() -> first;
   unsigned b = g2 -> _fecList.begin() -> first;
   return a < b;
}