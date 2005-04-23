//----------------------------------------------------------------------
//   $Id: TextConsole.h,v 1.2 2005/04/23 15:58:32 nomenquis Exp $
//----------------------------------------------------------------------
//
//  $Log: TextConsole.h,v $
//  Revision 1.1  2005/04/22 17:21:40  nomenquis
//  added TONS of stuff, changed ZILLIONS of things
//
//----------------------------------------------------------------------

#ifndef _TEXTCONSOLE_H_
#define _TEXTCONSOLE_H_

#include "Console.h"

class TextConsole : public Console
{
public:
  
  TextConsole();

  
  virtual uint32 setAsCurrent();
  virtual uint32 unsetAsCurrent();

private:
  
  virtual void consoleClearScreen();
  virtual uint32 consoleSetCharacter(uint32 const &row, uint32 const&column, uint8 const &character, uint8 const &state);
  virtual uint32 consoleGetNumRows() const;
  virtual uint32 consoleGetNumColumns() const;

};





#endif
