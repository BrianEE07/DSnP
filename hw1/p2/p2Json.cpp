/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>//I/O
#include <stdio.h>//getline
#include <string>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
fstream file;

bool Json::read(const string& jsonFile)
{
   // TODO
   file.open(jsonFile, ios::in);
   if(!file){
      return false;
   }
   else{
      string row;
      while(getline(file, row)){
         string key, value;
         int val;
         size_t kh, kt, vh;//key head , key tail , value head
            kh = row.find("\"");
            kt = row.find("\"", kh + 1);
            vh = row.find(":");
            if(kh != string::npos && kt != string::npos && vh != string::npos){
               key = key.assign(row, kh + 1, kt - kh - 1);
               value = value.assign(row, vh + 1);
               val = stoi(value); //string to int
               JsonElem tmp(key, val);
               _obj.push_back(tmp);
            }
      }
      file.close();
      return true;
   }
}
void Json::print() const{
   cout << "{\n";
   if(_obj.size() == 0);
   else{
      for(long unsigned int i = 0;i < _obj.size() - 1;i++)
         cout << "  " << _obj.at(i) << "," << endl;
      cout << "  " << _obj.at(_obj.size() - 1) << endl;
   }
   cout << "}\n";
}
int Json::sum() const{
   int Sum = 0;
   for(long unsigned int i = 0;i < _obj.size();i++)
      Sum += _obj.at(i).getvalue();
   return Sum;
}
double Json::ave() const{
   double Ave = 0;
   Ave = (double)this -> sum() / _obj.size();//size will not be 0
   return Ave;
}
JsonElem Json::max() const{
   JsonElem Max = _obj.at(0);
   for(long unsigned int i = 1;i < _obj.size();i++){
      if(_obj.at(i).getvalue() > Max.getvalue())
         Max = _obj.at(i);
   }
   return Max;
}
JsonElem Json::min() const{
   JsonElem Min = _obj.at(0);
   for(long unsigned int i = 1;i < _obj.size();i++){
      if(_obj.at(i).getvalue() < Min.getvalue())
         Min = _obj.at(i);
   }
   return Min;
}
void Json::add(){
   string key;
   int value;
   cin >> key >> value;
   int chk = 0;
   for(long unsigned int i = 0;i < _obj.size();i++){//identical key is rejected!
      if(key == _obj.at(i).getkey()){
         cout << "Wrong Input : identical key!" << endl;
         chk = 1;
         break;
      }
   }
   if(cin.fail()) cout << "Wrong Input : not a number!" << endl;//when cin >> value is not a number
   else if(chk == 1);//identical key is rejected!
   else{
      JsonElem Add(key, value);
      _obj.push_back(Add);
   } 
} 
int Json::getsize() const{
   return _obj.size();
}
ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}
int JsonElem::getvalue() const{
   return _value;
}
string JsonElem::getkey() const{
   return _key;
}