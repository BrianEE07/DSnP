/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"
#include <bitset> // 

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
  friend class CirMgr;
public:
  CirGate(){}
  CirGate(unsigned g, unsigned l, GateType t = UNDEF_GATE, bool inv0 = 0, bool inv1 = 0) : _varID(g), _lineN(l), _type(t), _ref(0), _simValue(0) {_inv[0] = inv0; _inv[1] = inv1;}
  virtual ~CirGate() {}

  // Basic access methods
  string getTypeStr() const;
  unsigned getLineNo() const { return _lineN; }
  virtual bool isAig() const { 
    if(_type == AIG_GATE) return true;
    return false; 
  }

  // Printing functions
  void reportGate() const;
  void reportFanin(int level) const;
  void reportFanout(int level) const;

  // DFS traversal
  void dfsTraversal(GateList&);
  bool isGlobalRef(){ return (_ref == _globalRef);}
  void setToGlobalRef(){ _ref = _globalRef;}
  static void setGlobalRef(){ _globalRef++;}

  static unsigned long long _globalRef;

  // simulate
  bool simulate();

private:

protected:
  unsigned _varID;
  int _lineN;
  GateList _faninList;
  GateList _fanoutList;
  GateType _type;
  bool _inv[2];
  bool _toDel = true;
  string _symbol = "";
  unsigned _ref;
  
  size_t _simValue;
  FecGrp* _fecgrp;
  Var _var;
};

class CirPiGate : public CirGate
{
public:
  CirPiGate(unsigned g, unsigned l, GateType t = PI_GATE) : CirGate(g, l, t){}
private:
};

class CirPoGate : public CirGate
{
public:
  CirPoGate(unsigned g, unsigned l, bool inv, GateType t = PO_GATE) : CirGate(g, l, t, inv){}
private:
};

class CirAigGate : public CirGate
{
public:
  CirAigGate(unsigned g, unsigned l, bool inv0, bool inv1, GateType t = AIG_GATE) : CirGate(g, l, t, inv0, inv1){}
private:
};
#endif // CIR_GATE_H
