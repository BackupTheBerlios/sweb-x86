//----------------------------------------------------------------------
//   $Id: Console.h,v 1.1 2005/04/22 17:21:40 nomenquis Exp $
//----------------------------------------------------------------------
//
//  $Log: $
//----------------------------------------------------------------------


#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"

class Console
{
public:
    
  virtual uint32 getNumRows() const=0;
  virtual uint32 getNumColumns() const=0;

  virtual void clear()=0;
  
  virtual uint32 setCharacter(uint32 const&column, uint32 const &row, uint8 const &character)=0;

  // geeee, the most brilliant names in history, ever
  virtual uint32 setAsCurrent()=0;
  virtual uint32 unsetAsCurrent()=0;


protected:
  
};



#endif
