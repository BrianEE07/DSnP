/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <iomanip>// fixed , setprecisionADD
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   cout << "Enter command: ";
   string command;
   while (true) {
      cin >> command;
      if(command == "PRINT"){
         json.print();
      }
      else if(command == "SUM"){
         if(json.getsize() == 0) cout << "Error: No element found!!" << endl;
         else
            cout << "The summation of the values is: " << json.sum() << "." << endl;
      }
      else if(command == "AVE"){
         if(json.getsize() == 0) cout << "Error: No element found!!" << endl;
         else
            cout << "The average of the values is: " << fixed << setprecision(1) << json.ave() << "." << endl;
      }
      else if(command == "MAX"){
         if(json.getsize() == 0) cout << "Error: No element found!!" << endl;
         else
            cout << "The maximum element is: { " << json.max() << " }." << endl;
      }
      else if(command == "MIN"){
         if(json.getsize() == 0) cout << "Error: No element found!!" << endl;
         else
            cout << "The minimum element is: { " << json.min() << " }." << endl;
      }
      else if(command =="ADD"){
         json.add();
      }
      else if(command == "EXIT")
         exit(-1);
      else
         cout << "Error: unknown command: \"" << command << "\"" << endl;
      cout << "Enter command: ";
      cin.clear();
      command.clear();
   }
}
