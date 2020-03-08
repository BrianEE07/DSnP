/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "dbJson.h"

#include <cmath>// me include

using namespace std;

extern DBJson dbjson;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

void test();

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <jsonfile>
      cerr << "Error: using testdb <jsonfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbjson) {
      cout << "Table is resetting..." << endl;
      dbjson.reset();
   }
   if (!(inf >> dbjson)) {
      cerr << "Error in reading JSON file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print JSON object" << endl;
   cout << "========================" << endl;
   cout << dbjson << endl;

   // TODO
   // Insert what you want to test here by calling DBJson's member functions
   
   test();


   DBJsonElem elm1("IloveDsnp", 12345), elm2("IdontloveDsnp", -32345), elm3("IloveDsnp", 1111);
   if(!dbjson.add(elm1)) cout << "Wrong Input : identical key!   elm1" << endl;
   else cout << "elm1 appended!!!" << endl;
   if(!dbjson.add(elm2)) cout << "Wrong Input : identical key!   elm2" << endl;
   else cout << "elm2 appended!!!" << endl;
   if(!dbjson.add(elm3)) cout << "Wrong Input : identical key!   elm3" << endl;
   else cout << "elm3 appended!!!" << endl;
   cout << dbjson << endl;


   test();


   dbjson.reset();
   cout << dbjson << endl;


   return 0;
}
void test(){
   size_t maxI;
   size_t minI;
   int chkmax = dbjson.max(maxI);
   int chkmin = dbjson.min(minI);
   if(chkmax == (-__INT_MAX__ - 1)) cerr << "Error: The max JSON element is nan." << endl;
   else cout << "The max JSON element is { " << dbjson[maxI] << " }." << endl;
   if(chkmin == __INT_MAX__) cerr << "Error: The min JSON element is nan." << endl;
   else cout << "The min JSON element is { " << dbjson[minI] << " }." << endl;


   float a = dbjson.ave();
   if (isnan(a)) cerr << "Error: The average of the DB is nan." << endl;
   else cout << "The average of the DB is " << a << endl;


   int b = dbjson.sum();
   if (dbjson.empty()) cerr << "Error: The sum of the DB is nan." << endl;
   else cout << "The sum of the DB is " << b << "." << endl;
   return;
}