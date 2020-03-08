/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}


istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string row;
   while(getline(is, row)){
      string key, value;
      int val;
      size_t kh, kt, vh;//key head , key tail , value head
      kh = row.find("\"");
      kt = row.find("\"", kh + 1);
      vh = row.find(":");
      if(kh != string::npos && kt != string::npos && vh != string::npos){
         key = key.assign(row, kh + 1, kt - kh - 1);
         value = value.assign(row, vh + 1);
         val = stoi(value);  // string to int
         DBJsonElem tmp(key, val);
         j._obj.push_back(tmp);
      }
   }
   j.read = true;
   is.clear();
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{\n";
   if(j._obj.size() == 0);
   else{
      for(long unsigned int i = 0;i < j._obj.size() - 1;i++)
         os << "  " << j._obj.at(i) << "," << endl;
      os << "  " << j._obj.at(j._obj.size() - 1) << endl;
   }
   os << "}\n";
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for(long unsigned int i = 0;i < _obj.size();i++){
      if(elm.key() == _obj.at(i).key()){
         //cout << "Wrong Input : identical key!" << endl;
         return false;
      }
   }
   _obj.push_back(elm); 
   return true;
} 

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   double Ave = 0;
   if(_obj.size() == 0) return NAN;
   Ave = (double)this -> sum() / _obj.size();  //size will not be 0
   return Ave;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if(_obj.size() == 0){
      idx = _obj.size();
      return INT_MIN;
   }
   for(long unsigned int i = 0;i < _obj.size();i++){
      if(_obj.at(i).value() > maxN){
         maxN = _obj.at(i).value();
         idx = i;
      }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if(_obj.size() == 0){
      idx = _obj.size();
      return INT_MAX;
   }
   for(long unsigned int i = 0;i < _obj.size();i++){
      if(_obj.at(i).value() < minN){
         minN = _obj.at(i).value();
         idx = i;
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(_obj.size() == 0) return 0;
   int s = 0;
   for(long unsigned int i = 0;i < _obj.size();i++)
      s += _obj.at(i).value();
   return s;
}
