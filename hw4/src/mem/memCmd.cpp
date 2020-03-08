/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   int n = 0;
   int size = 0;
   size_t arrsize = 0;
   size_t num = 0;
   bool newarr = false;
   bool firstime = true;
   for(size_t i = 0;i < options.size();i++){
      if(myStrNCmp("-Array", options.at(i), 2) == 0){
         if(newarr) return CmdExec::errorOption(CMD_OPT_EXTRA, options.at(i));
         if(i == options.size() - 1) return CmdExec::errorOption(CMD_OPT_MISSING, "");
         if(!myStr2Int(options.at(i + 1), size)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
         if(size <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
         arrsize = (size_t)size;
         newarr = true;
         i++;
      }
      else{
         if(firstime == true){
            if(!myStr2Int(options[i], n)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
            if(n <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
            num = (size_t)n;
            firstime = false;
         }
         else return CmdExec::errorOption(CMD_OPT_EXTRA, options.at(i));
      }
   }
   if(firstime == true) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
   // Use try-catch to catch the bad_alloc exception
   try{
      if(newarr == false)
         mtest.newObjs(num);
      else
         mtest.newArrs(num ,arrsize);
   }
   catch(bad_alloc e){
      return CMD_EXEC_ERROR;
   }
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   int oid = 0;
   int rid = 0;
   size_t objId = -1;
   size_t numRandId = 0;
   bool doarr = false;
   bool firstime = true;
    for(size_t i = 0;i < options.size();i++){
      if(myStrNCmp("-Array", options.at(i), 2) == 0){
         if(doarr) return CmdExec::errorOption(CMD_OPT_EXTRA, options.at(i));
         doarr = true;
      }
      else{
         if(firstime == true){
            if(myStrNCmp("-Index", options.at(i), 2) == 0){
               if(i == options.size() - 1) return CmdExec::errorOption(CMD_OPT_MISSING, "");
               if(!myStr2Int(options.at(i + 1), oid)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
               if(oid < 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
               objId = (size_t)oid;
               firstime = false;
               i++;
            }
            else if(myStrNCmp("-Random", options.at(i), 2) == 0){
               if(i == options.size() - 1) return CmdExec::errorOption(CMD_OPT_MISSING, "");
               if(!myStr2Int(options.at(i + 1), rid)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
               if(rid <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
               numRandId = (size_t)rid;
               firstime = false;
               i++;
            }
            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.at(i));
         }
         else return CmdExec::errorOption(CMD_OPT_EXTRA, options.at(i));
      }
   }
   if(firstime == true) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   if(doarr == false){
      if(objId != -1){
         if(objId >= mtest.getObjListSize()){
         cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(objId));
         }
         mtest.deleteObj(objId);
      }
      else{
         if(mtest.getObjListSize() == 0){
            cerr << "Size of object list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
         }
         int s = mtest.getObjListSize();
         for(size_t i = 0;i < numRandId;i++){
            mtest.deleteObj(rnGen(s));
         }
      }
      //random 剩pushfront debug reset... recyclelist numelm...
   }
   else{
      if(objId != -1){
         if(objId >= mtest.getArrListSize()){
         cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(objId));
         }
         mtest.deleteArr(objId);
      }
      else{
         if(mtest.getArrListSize() == 0){
            cerr << "Size of array list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, "");
         }
         int s = mtest.getArrListSize();
         for(size_t i = 0;i < numRandId;i++){
            mtest.deleteArr(rnGen(s));
         }
      }
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();
   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


