//----------------------------------------------------------------------
//   $Id: InputThread.h,v 1.1 2005/09/05 23:01:24 btittelbach Exp $
//----------------------------------------------------------------------
//
//  $Log: InputThread.h,v $
//----------------------------------------------------------------------

#ifndef _INPUTTHREAD_H_
#define _INPUTTHREAD_H_

#include "atkbd.h"
#include "kernel/Thread.h"
#include "ipc/FiFo.h"

class InputThread : public Thread
{
  public:

  static void startInputThread();
  
  static InputThread *getInstance() {
    if (instance_ == 0)
      startInputThread();
    return instance_;
  }
  
  InputThread();
  ~InputThread();
  
  virtual void Run();
  
  uint8 getScancode()
  {
    return scancode_input_->get();
  }
  
  void setNumlock(bool on);
  void setCapslock(bool on);
  void setScrolllock(bool on);
  
  
  private:
  
  static InputThread *instance_;
  FiFo<uint8> *scancode_input_;
  uint8 status_lights_;
};

#endif
