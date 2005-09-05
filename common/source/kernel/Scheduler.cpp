//----------------------------------------------------------------------
//   $Id: Scheduler.cpp,v 1.19 2005/09/05 23:01:24 btittelbach Exp $
//----------------------------------------------------------------------
//
//  $Log: Scheduler.cpp,v $
//  Revision 1.18  2005/08/26 12:01:25  nomenquis
//  pagefaults in userspace now should really really really work
//
//  Revision 1.17  2005/08/11 16:18:02  nomenquis
//  fixed a bug
//
//  Revision 1.16  2005/08/07 16:47:25  btittelbach
//  More nice synchronisation Experiments..
//  RaceCondition/kprintf_nosleep related ?/infinite memory write loop Error still not found
//  kprintfd doesn't use a buffer anymore, as output_bochs blocks anyhow, should propably use some arch-specific interface instead
//
//  Revision 1.15  2005/08/04 20:47:43  btittelbach
//  Where is the Bug, maybe I will see something tomorrow that I didn't see today
//
//  Revision 1.14  2005/07/27 10:04:26  btittelbach
//  kprintf_nosleep and kprintfd_nosleep now works
//  Output happens in dedicated Thread using VERY EVIL Mutex Hack
//
//  Revision 1.13  2005/07/24 17:02:59  nomenquis
//  lots of changes for new console stuff
//
//  Revision 1.12  2005/07/21 19:08:41  btittelbach
//  Jö schön, Threads u. Userprozesse werden ordnungsgemäß beendet
//  Threads können schlafen, Mutex benutzt das jetzt auch
//  Jetzt muß nur der Mutex auch überall verwendet werden
//
//  Revision 1.11  2005/07/12 21:05:38  btittelbach
//  Lustiges Spielen mit UserProgramm Terminierung
//
//  Revision 1.10  2005/07/05 20:22:56  btittelbach
//  some changes
//
//  Revision 1.9  2005/07/05 17:29:48  btittelbach
//  new kprintf(d) Policy:
//  [Class::]Function: before start of debug message
//  Function can be abbreviated "ctor" if Constructor
//  use kprintfd where possible
//
//  Revision 1.8  2005/06/14 18:51:47  btittelbach
//  afterthought page fault handling
//
//  Revision 1.7  2005/05/31 18:13:14  nomenquis
//  fixed compile errors
//
//  Revision 1.6  2005/05/31 17:29:16  nomenquis
//  userspace
//
//  Revision 1.5  2005/05/31 17:25:56  btittelbach
//  Scheduler mit Listen geschmückt
//
//  Revision 1.4  2005/05/25 08:27:49  nomenquis
//  cr3 remapping finally really works now
//
//  Revision 1.3  2005/05/08 21:43:55  nelles
//  changed gcc flags from -g to -g3 -gstabs in order to
//  generate stabs output in object files
//  changed linker script to load stabs in kernel
//  in bss area so GRUB loads them automaticaly with
//  the bss section
//
//  changed StupidThreads in main for testing purposes
//
//  Revision 1.2  2005/04/27 08:58:16  nomenquis
//  locks work!
//  w00t !
//
//  Revision 1.1  2005/04/26 15:58:45  nomenquis
//  threads, scheduler, happy day
//
//----------------------------------------------------------------------


#include "Scheduler.h"
#include "Thread.h"
#include "arch_panic.h"
#include "ArchThreads.h"
#include "console/kprintf.h"
#include "ArchInterrupts.h"

ArchThreadInfo *currentThreadInfo;
Thread *currentThread;



Scheduler *Scheduler::instance_=0;

Scheduler *Scheduler::instance()
{
  return instance_;
}

class IdleThread : public Thread
{
public:
  
  virtual void Run()
  {
    while (1)
    {
//      kprintfd("IdleThread::Run:\n");
      Scheduler::instance()->cleanupDeadThreads();
      Scheduler::instance()->yield();
    }
  }
};

void Scheduler::createScheduler()
{
  instance_ = new Scheduler();
  
  // create idle thread, this one really does not do too much

  Thread *idle = new IdleThread();
  instance_->addNewThread(idle);
}

Scheduler::Scheduler()
{
  kill_me_=0;
  block_scheduling_=0;
}

void Scheduler::addNewThread(Thread *thread)
{
  //new Thread gets scheduled next
  //also gets added to front as not to interfere with remove or xchange

  if (unlikely(ArchThreads::testSetLock(block_scheduling_,1)))
    arch_panic((uint8*) "FATAL ERROR: Scheduler::*: block_scheduling_ was set !! How the Hell did the program flow get here then ?\n");
  kprintf("Scheduler::addNewThread: %x\n",thread);
  threads_.pushFront(thread);
  block_scheduling_=0;
}


//you can't remove the last thread
void Scheduler::removeCurrentThread()
{
  if (unlikely(ArchThreads::testSetLock(block_scheduling_,1)))
    arch_panic((uint8*) "FATAL ERROR: Scheduler::*: block_scheduling_ was set !! How the Hell did the program flow get here then ?\n");

  kprintfd("Scheduler::removeCurrentThread: %x, threads_.size() %d\n",currentThread,threads_.size());
  if (threads_.size() > 1)
  {
    Thread *tmp_thread;
    for (uint32 c=0; c< threads_.size(); ++c)
    {
      tmp_thread = threads_.back();
      threads_.popBack();
      if (tmp_thread == currentThread)      
        break;
      
      threads_.pushFront(tmp_thread);
    }
  }
  block_scheduling_=0;
}

void Scheduler::sleep()
{
  currentThread->state_=Sleeping;
  yield();
}

void Scheduler::wake(Thread* thread_to_wake)
{
  thread_to_wake->state_=Running;
  //DEBUG: Check if thread_to_wake is in List
}

//exchanges the current Thread with a PopUpThread
//note that the popupthread has to remember the original Thread*
//and switch back if he's finished
//~ Thread *Scheduler::xchangeThread(Thread *pop_up_thread)
//~ {
  //~ if (unlikely(ArchThreads::testSetLock(block_scheduling_,1)))
    //~ arch_panic((uint8*) "FATAL ERROR: Scheduler::*: block_scheduling_ was set !! How the Hell did the program flow get here then ?\n");

  //~ Thread *old_thread = threads_.back();
  //~ if (old_thread != currentThread)
    //~ arch_panic((uint8*)"Scheduler::xchangeThread: ERROR: currentThread wasn't where it was supposed to be\n");
  //~ threads_.popBack();
  //~ threads_.pushBack(pop_up_thread);
  //~ currentThread=pop_up_thread;
  
  //~ block_scheduling_=0;
  //~ return old_thread;
//~ }


void Scheduler::startThreadHack()
{
  currentThread->Run();
}

uint32 Scheduler::schedule(uint32 from_interrupt)
{
/*
  static uint32 bochs_sucks = 0;
  if (++bochs_sucks % 10)
  {
    return 0;
  }
 */
  //kprintfd_nosleep("Scheduler::schedule: running\n");  
  //kprintfd_nosleep("Scheduler::schedule: running\n");
  if (ArchThreads::testSetLock(block_scheduling_,1))
  {
    //no scheduling today...
    //keep currentThread as it was
    //and stay in Kernel Kontext
    kprintfd_nosleep("Scheduler::schedule: currently blocked\n");
    return 0;
  }
  
  //kprintfd_nosleep("Scheduler::schedule: currentThread was %x\n",currentThread);
  do 
  {
    currentThread = threads_.front();
    threads_.popFront();

 //   kprintfd_nosleep("Scheduler::schedule: thinking about %x\n",currentThread);
    if (kill_me_ == 0 && currentThread->state_ == ToBeDestroyed)
    {
      kill_me_=currentThread;
      //to be killed Thread gets implicitly removed from list
      continue;
    }
    
    threads_.pushBack(currentThread);
    
  } while (currentThread->state_ != Running);
  kprintfd_nosleep("Scheduler::schedule: new currentThread is %x, switch_userspace:%d\n",currentThread,currentThread->switch_to_userspace_);
  
  uint32 ret = 1;
  
  if ( currentThread->switch_to_userspace_)
    currentThreadInfo =  currentThread->user_arch_thread_info_;
  else
  {
    currentThreadInfo =  currentThread->kernel_arch_thread_info_;
    ret=0;
  }
  /*uint8*foo = 0;
  *foo = 8;*/
  
  block_scheduling_=0;
  return ret;
}

void Scheduler::yield()
{
  if (! ArchInterrupts::testIFSet())
  {
    kprintf("Scheduler::yield: WARNING Interrupts disabled, do you really want to yield ?\n");
    kprintfd("Scheduler::yield: WARNING Interrupts disabled, do you really want to yield ?\n");
    kprintf_nosleep_flush();
  }
  ArchThreads::yield();
}

void Scheduler::cleanupDeadThreads()
{
  //check outside of atmoarity for performance gain,
  // worst case, dead threads are around a while longer
  //then make sure we're atomar (can't really lock list, can I ;->)
  //note: currentThread is always last on list

  if (kill_me_ == 0)
    return;
  
  if (unlikely(ArchThreads::testSetLock(block_scheduling_,1)))
    arch_panic((uint8*) "FATAL ERROR: Scheduler::*: block_scheduling_ was set !! How the Hell did the program flow get here then ?\n");

  kprintfd_nosleep("Scheduler::cleanupDeadThreads: now running\n");
  if (kill_me_)
  {
    if (kill_me_->state_ == ToBeDestroyed)
      delete kill_me_;  
//    else
//      kprintfd("Scheduler::cleanupDeadThreads: ERROR, how did that Thread get to be here\n");
    kill_me_=0;
  }
  kprintfd_nosleep("Scheduler::cleanupDeadThreads: done\n");
  block_scheduling_=0;
}
