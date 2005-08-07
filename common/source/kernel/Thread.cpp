//----------------------------------------------------------------------
//  $Id: Thread.cpp,v 1.15 2005/08/07 16:47:25 btittelbach Exp $
//----------------------------------------------------------------------
//
//  $Log: Thread.cpp,v $
//  Revision 1.14  2005/07/21 19:08:41  btittelbach
//  Jö schön, Threads u. Userprozesse werden ordnungsgemäß beendet
//  Threads können schlafen, Mutex benutzt das jetzt auch
//  Jetzt muß nur der Mutex auch überall verwendet werden
//
//  Revision 1.13  2005/07/12 21:05:39  btittelbach
//  Lustiges Spielen mit UserProgramm Terminierung
//
//  Revision 1.12  2005/07/12 17:53:13  btittelbach
//  Bochs SerialConsole ist jetzt lesbar
//
//  Revision 1.11  2005/07/05 20:22:56  btittelbach
//  some changes
//
//  Revision 1.10  2005/07/05 17:29:48  btittelbach
//  new kprintf(d) Policy:
//  [Class::]Function: before start of debug message
//  Function can be abbreviated "ctor" if Constructor
//  use kprintfd where possible
//
//  Revision 1.9  2005/05/31 17:29:16  nomenquis
//  userspace
//
//  Revision 1.8  2005/05/25 08:27:49  nomenquis
//  cr3 remapping finally really works now
//
//  Revision 1.7  2005/05/19 15:43:43  btittelbach
//  Ans�tze f�r eine UserSpace Verwaltung
//
//  Revision 1.6  2005/05/16 20:37:51  nomenquis
//  added ArchMemory for page table manip
//
//  Revision 1.5  2005/05/10 08:53:50  nelles
//  stack trace experimenting
//
//  Revision 1.4  2005/04/27 08:58:16  nomenquis
//  locks work!
//  w00t !
//
//  Revision 1.3  2005/04/26 15:58:45  nomenquis
//  threads, scheduler, happy day
//
//  Revision 1.2  2005/04/23 22:20:30  btittelbach
//  just stuff
//
//  Revision 1.1  2005/04/23 21:27:12  nomenquis
//  commit for bernhard
//
//----------------------------------------------------------------------

#include "kernel/Thread.h"
#include "ArchCommon.h"
#include "console/kprintf.h"
#include "ArchThreads.h"
#include "Scheduler.h"
#include "Loader.h"

static void ThreadStartHack()
{
  currentThread->Run();
  kprintfd("ThreadStartHack: Panic, thread returned\r\n");
  //FIXXME: we propably should clean up the memory here, or change this Hack entireley
  Scheduler::instance()->removeCurrentThread();
  for(;;);
}

Thread::Thread()
{
  kprintfd("Thread::Thread: Thread ctor, this is %x, stack is %x, sizeof stack is %x\r\n", this,stack_, sizeof(stack_));
  ArchCommon::bzero((pointer)stack_,sizeof(stack_),1);

  //kprintfd("Thread::Thread: After bzero\n");
  ArchThreads::createThreadInfosKernelThread(kernel_arch_thread_info_,(pointer)&ThreadStartHack,getStackStartPointer());
  user_arch_thread_info_=0;
  switch_to_userspace_ = 0;
  state_=Running;
  loader_ = 0;
}

Thread::~Thread()
{
  if (loader_)
  {
    kprintfd("Thread::~Thread: cleaning up UserspaceAddressSpace (freeing Pages)\n");
    loader_->cleanupUserspaceAddressSpace();
    delete loader_;
  }
  kprintfd("Thread::~Thread: freeing ThreadInfos\n");
  ArchThreads::cleanupThreadInfos(user_arch_thread_info_); //yes that's safe
  ArchThreads::cleanupThreadInfos(kernel_arch_thread_info_);
}

//if the Thread we want to kill, is the currentThread, we better not return
void Thread::kill()
{
  switch_to_userspace_ = false;
  state_=ToBeDestroyed;
  kprintfd("Thread::kill: Preparing currentThread (%x) for destruction\n",currentThread);
  if (currentThread == this)
    Scheduler::instance()->yield();
}

pointer Thread::getStackStartPointer()
{
  pointer stack = (pointer)stack_;
  stack += sizeof(stack_) - sizeof(uint32);
  return stack;
}
