//----------------------------------------------------------------------
//   $Id: Scheduler.h,v 1.1 2005/04/26 15:58:45 nomenquis Exp $
//----------------------------------------------------------------------
//
//  $Log: $
//----------------------------------------------------------------------


#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "types.h"


#define MAX_THREADS 20

class Thread;
class ArchThreadInfo;
  
extern ArchThreadInfo *currentThreadInfo;
extern Thread *currentThread;

 
class Scheduler
{
public:
  
  static Scheduler *instance();
  static void createScheduler();

  void addNewThread(Thread *thread);


  void yield();

  // NEVER EVER EVER CALL THIS ONE OUTSIDE OF AN INTERRUPT CONTEXT //
  void schedule(uint32 from_interrupt=false);

private:

  Scheduler();

  static Scheduler *instance_;

  Thread* threads_[MAX_THREADS];
  
  static void startThreadHack();

};









#endif
