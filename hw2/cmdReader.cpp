/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY :{/* TODO */ 
                              if(_readBufPtr == _readBuf) mybeep();
                              else{
                                 moveBufPtr(_readBufPtr - 1); 
                                 deleteChar(); 
                              }}
                              break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */ moveBufPtr(_readBufPtr + 1); break;
         case ARROW_LEFT_KEY : /* TODO */ moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        :{/* TODO */
                              int mod = (_readBufPtr - _readBuf) % TAB_POSITION;
                              insertChar(' ', TAB_POSITION - mod);
                              }
                              break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)          //done
{
   // TODO...
   int i = ptr - _readBufPtr;
   if(ptr >= _readBuf && ptr <= _readBufEnd){
      while(i > 0){                                
         int arrn = _readBufPtr - _readBuf;           //array number
         cout << _readBuf[arrn];
         _readBufPtr++;
         i--;
      }
      while(i < 0){                                
         cout << "\b";
         _readBufPtr--;
         i++;
      }
   }
   else{
      mybeep();
      return false;
   }
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   if(_readBufPtr == _readBufEnd){
      mybeep();
      return false;
   }
   else{
      int arrn = _readBufPtr - _readBuf;     //array number
      int arrl = _readBufEnd - _readBuf;     //array length 
      for(int i = arrn;i < arrl - 1;i++) _readBuf[i] = _readBuf[i + 1];    //move foreward 1 character and store to buffer
      _readBufEnd--;
      int nwarrl = _readBufEnd - _readBuf;                        //new array length
      _readBuf[nwarrl] = ' ';                                     //new arrl(end) so need not to write [arrl - 1]
      for(int i = arrn;i <= nwarrl;i++) cout << _readBuf[i];
      for(int i = arrn;i <= nwarrl;i++) cout << '\b';             //cursor go back to _readBufPtr 
      _readBuf[nwarrl] = '\0';
      return true;
   }
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);
   for(int i = 0;i < repeat;i++){
      cout << ch;
      _readBufEnd++;
   }
   int count = 0;
   int arrn = _readBufPtr - _readBuf;     //array number
   int arrl = _readBufEnd - _readBuf;     //array length
   for(int i = arrn;i < arrl - repeat;i++){
      cout << _readBuf[i];  
      count++;
   }
   while(count != 0){
      cout << "\b";
      count--;
   }
   for(int i = arrl - 1;i >= arrn + repeat;i--){     //store new data
      _readBuf[i] = _readBuf[i - repeat];
   } 
   for(int i = 0;i < repeat;i++){            //store new data
      _readBuf[arrn + i] = ch;
      _readBufPtr++;
   } 
   _readBuf[arrl] = '\0';
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   int arrn = _readBufPtr - _readBuf;              //array number
   int arrl = _readBufEnd - _readBuf;              //array length
   for(int i = 0;i <= arrl;i++) _readBuf[i] = '\0';
   for(int i = arrn;i > 0;i--) cout << '\b';
   for(int i = 0;i <= arrl;i++) cout << ' ';
   for(int i = arrl;i >= 0;i--) cout << '\b';
   _readBufPtr = _readBufEnd = _readBuf;
   *_readBufEnd = 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
      if(index < _historyIdx){
         if(index < 0) index = 0; 
         int dis1 = _historyIdx - index;              //distance
         if(_historyIdx == 0) mybeep();               //top  //note 在原本line 按上會存入暫存 回原行會顯示 在歷史案enter後會洗掉 在歷史案enter會存入歷史在原行案enter會存原行到歷史              
         else{                                               //回原行改掉岸上會重新存，在歷史改掉岸上不變       
            if(_historyIdx == (int)_history.size() && _tempCmdStored == false){     //store tmp
               _history.push_back(_readBuf);
               _tempCmdStored = true;
            } 
            _historyIdx -= dis1;                      
            retrieveHistory();
         }
         _historyIdx = index;
      }
      else if(index > _historyIdx){
         if(index >= (int)_history.size()){
            if(_tempCmdStored == false) index = (int)_history.size();
            else index = (int)_history.size() - 1 ;
         }
         int dis2 = index - _historyIdx;              //distance
         if(_historyIdx == (int)_history.size()) mybeep();               //bottom
         //else if(_historyIdx == ((int)_history.size() - 1) && _tempCmdStored == true) mybeep();      //bottom
         else{
            _historyIdx += dis2;
            retrieveHistory();
            if(_historyIdx == (int)_history.size() - 1){           //important (to forget temp when go back to last line)
               _tempCmdStored = false;
               _history.pop_back();
            }
         }
         _historyIdx = index;
      }
      else;
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   int arrl = _readBufEnd - _readBuf;
   //moveBufPtr(_readBuf);
   for(int i = 0;i < arrl;i++){
      if(_readBuf[0] == ' '){
         for(int j = 0;j < arrl - 1;j++){
            //deleteChar();
            _readBuf[j] = _readBuf[j + 1];
         }
         _readBuf[arrl - 1] = '\0';
         _readBufEnd--;
      }
      else break;
   }
   //moveBufPtr(_readBufEnd);
   int nwarrl = _readBufEnd - _readBuf;
   for(int i = nwarrl - 1;i >= 0;i--){
      if(_readBuf[i] == ' '){          
         //moveBufPtr(_readBufPtr - 1); 
         //deleteChar(); 
         _readBuf[i] = '\0';
      }
      else break;
   }
   if(_readBuf[0] != '\0'){
      if(_tempCmdStored == true){
         _history.pop_back();
         _history.push_back(_readBuf); 
      }
      else _history.push_back(_readBuf); 
      _historyIdx = (int)_history.size();
   }
   else{
      if(_tempCmdStored == true) _history.pop_back();
      _historyIdx = (int)_history.size();
   }     
   _tempCmdStored = false;
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
