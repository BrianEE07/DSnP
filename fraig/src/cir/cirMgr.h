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
#include <map> //
#include <string>
#include <fstream>
#include <iostream>
#include <bitset> // 
#include <sstream> //

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {reset();} 

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

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary();
   void printNetlist();
   void printPIs();
   void printPOs();
   void printFloatGates();
   void printFECPairs() const;
   void writeAag(ostream&);
   void writeGate(ostream&, CirGate*) const; 

private:
   ofstream           *_simLog;
   FecList _FecGrps;
   bool _initFec = false;

   bool readHeader(ifstream&);
   bool readInput(ifstream&);
   bool readOutput(ifstream&);
   bool readAig(ifstream&);
   bool readSymbol(ifstream&);
   void connect();
   void optReconnect(CirGate*&, int, bool);
   void optDelete(CirGate*&, int, bool);
   unsigned long long hashFunction(size_t, size_t, bool , bool);
   void merge(CirGate*&, CirGate*&);
   void simulate(vector<string>&);
   void genLog(vector<string>&, int);
   void initialFec();
   void genFec();
   static bool compare(FecGrp*, FecGrp*);
   void genProofModel(SatSolver&);
   bool satProve(SatSolver&, Feciterator , Feciterator);
   bool getSatAssignment(SatSolver& s, string&);
   void mergeFraig(FecGrp*);
   void genDFSList();
   void reset();
   unsigned miloa[5];
   GateList _piList;
   GateList _poList;
   GateList _dfsList;
   map<unsigned, CirGate*> _gateList;
   static CirGate* _const;

};


class FecGrp{
   friend class CirMgr;
   friend class CirGate;
public:
   FecGrp() {}
   FecGrp(unsigned n): _grpName(n) {}
   ~FecGrp() {}

private:
   unsigned _grpName;
   map<unsigned, pair<CirGate*, bool>> _fecList;
};
#endif // CIR_MGR_H
