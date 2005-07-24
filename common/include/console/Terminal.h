//----------------------------------------------------------------------
//  $Id: Terminal.h,v 1.2 2005/07/24 17:02:59 nomenquis Exp $
//----------------------------------------------------------------------
//
//  $Log: Terminal.h,v $
//  Revision 1.1  2005/04/23 15:58:32  nomenquis
//  lots of new stuff
//
//----------------------------------------------------------------------



#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "types.h"
#include "Console.h"

class Console;
  
class Terminal
{
friend class Console;
public:

  Terminal(Console *console, uint32 num_columns, uint32 num_rows);
  
  void write(char character);
  void writeString(char const *string);
  void writeBuffer(char const *buffer, size_t len);

  void setForegroundColor(Console::FOREGROUNDCOLORS const &color);
  void setBackgroundColor(Console::BACKGROUNDCOLORS const &color);

protected:
  
  void setAsActiveTerminal();
  void unSetAsActiveTerminal();

private:
  
 
  void writeInternal(char character);

  void clearScreen();
  void fullRedraw();
  uint32 getNumRows() const;
  uint32 getNumColumns() const;

  uint32 setCharacter(uint32 row,uint32 column, uint8 character);


  void scrollUp();

  Console *console_;
  uint32 num_columns_;
  uint32 num_rows_;
  uint32 len_;
  uint8 *characters_;
  uint8 *character_states_;

  uint32 current_column_;
  uint8 current_state_;

  uint8 active_;
  
  Mutex mutex_;
};


#endif
