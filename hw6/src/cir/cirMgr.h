/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> //

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
  CirMgr(){}
  ~CirMgr(){ reset();}

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate* getGate(unsigned gid){ 
    map<unsigned, CirGate*>::iterator it;
    it = _gateList.find(gid);
    if(it == _gateList.end()) return 0;
    return it -> second;
  }

  // Member functions about circuit construction
  bool readCircuit(const string&);

  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist();
  void printPIs();
  void printPOs();
  void printFloatGates();
  void writeAag(ostream&);

private:
  bool readHeader(ifstream&);
  bool readInput(ifstream&);
  bool readOutput(ifstream&);
  bool readAig(ifstream&);
  bool readSymbol(ifstream&);
  void connect();
  void genDFSList();
  void reset();
  unsigned miloa[5];
  GateList _piList;
  GateList _poList;
  GateList _dfsList;
  IdList _gateIDswithFloat;
  map<unsigned, CirGate*> _gateList;
  static CirGate* _const;
};

#endif // CIR_MGR_H
