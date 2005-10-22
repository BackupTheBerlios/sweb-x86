//----------------------------------------------------------------------
//   $Id: Scheduler.h,v 1.14 2005/10/22 14:00:31 btittelbach Exp $
//----------------------------------------------------------------------
//
//  $Log: Scheduler.h,v $
//  Revision 1.13  2005/09/26 13:56:55  btittelbach
//  +doxyfication
//  +SchedulerClass upgrade
//
//  Revision 1.12  2005/09/13 22:15:51  btittelbach
//  small BugFix: Scheduler really works now
//
//  Revision 1.11  2005/09/13 21:24:42  btittelbach
//  Scheduler without Memory Allocation in critical context (at least in Theory)
//
//  Revision 1.9  2005/09/07 00:33:52  btittelbach
//  +More Bugfixes
//  +Character Queue (FiFoDRBOSS) from irq with Synchronisation that actually works
//
//  Revision 1.8  2005/09/05 23:01:24  btittelbach
//  Keyboard Input Handler
//  + several Bugfixes
//
//  Revision 1.7  2005/08/07 16:47:24  btittelbach
//  More nice synchronisation Experiments..
//  RaceCondition/kprintf_nosleep related ?/infinite memory write loop Error still not found
//  kprintfd doesn't use a buffer anymore, as output_bochs blocks anyhow, should propably use some arch-specific interface instead
//
//  Revision 1.6  2005/07/21 19:08:41  btittelbach
//  Jö schön, Threads u. Userprozesse werden ordnungsgemäß beendet
//  Threads können schlafen, Mutex benutzt das jetzt auch
//  Jetzt muß nur der Mutex auch überall verwendet werden
//
//  Revision 1.5  2005/07/05 20:22:56  btittelbach
//  some changes
//
//  Revision 1.4  2005/06/14 18:51:47  btittelbach
//  afterthought page fault handling
//
//  Revision 1.3  2005/05/31 17:25:56  btittelbach
//  Scheduler mit Listen geschmückt
//
//  Revision 1.2  2005/05/25 08:27:48  nomenquis
//  cr3 remapping finally really works now
//
//  Revision 1.1  2005/04/26 15:58:45  nomenquis
//  threads, scheduler, happy day
//
//----------------------------------------------------------------------


#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "types.h"
#include "List.h"
#include "SpinLock.h"


#define MAX_THREADS 20

class Thread;
class ArchThreadInfo;

extern ArchThreadInfo *currentThreadInfo;
extern Thread *currentThread;


//-----------------------------------------------------------
/// Scheduler Class
///
/// This is a singelton class, it is instantiated in startup() and must be accessed via Scheduler::instance()->....
/// The Scheduler knows about all running and sleeping threads and decides which thread to run next
/// 
class Scheduler
{
public:

//-----------------------------------------------------------
/// Singelton Class Instance Access Method
/// @return Pointer to Scheduler
  static Scheduler *instance();

//-----------------------------------------------------------
/// createScheduler is called by startup() and does exatly what it's name implies.
  static void createScheduler();

//-----------------------------------------------------------
/// adds a new Thread and prepares to run it
/// this is the method that should be used to start a new Thread
///
/// @param *thread Pointer to the instance of a Class derived from Thread that contains the Thread to be started
  void addNewThread(Thread *thread);

//-----------------------------------------------------------
/// removes the currently active Thread from the scheduling list
/// this method has actually no use right now and should propably not be used
/// if you want to keep a thread from being scheduled use sleep() instead
/// if you want to remove a thread permanently use Thread::kill() instead
  void removeCurrentThread();
//-----------------------------------------------------------
/// puts the currentThread to sleep and keeps it from being scheduled
  void sleep();
//-----------------------------------------------------------
/// wakes up a sleeping thread
///
/// @param *thread_to_wake, Pointer to the Thread that will be woken up
  void wake(Thread *thread_to_wake);
//-----------------------------------------------------------
/// forces a task switch without waiting for the next timer interrupt
///
  void yield();
//-----------------------------------------------------------
/// prints a List of all Threads using kprintfd
///
  void printThreadList();
//-----------------------------------------------------------
/// compares all threads in the scheduler's list to the one given
/// since the scheduler knows about all existing threads, this is
/// a good way to see if a *thread is valid
/// @param *thread Pointer to the Thread's instance we want to check its existance bevore accessing it
/// @return true if *thread exists, fales otherwise
  bool checkThreadExists(Thread* thread);

//-----------------------------------------------------------
/// NEVER EVER EVER CALL THIS METHOD OUTSIDE OF AN INTERRUPT CONTEXT 
/// this is the methode that decides which threads will be scheduled next
/// it is called by either the timer interrupt handler or the yield interrupt handler
/// and changes the global variables currentThread and currentThreadInfo
/// @return 1 if the InterruptHandler should switch to Usercontext or 0 if we can stay in Kernelcontext
  uint32 schedule();

protected:
friend class IdleThread;
//-----------------------------------------------------------
/// this method is periodically called by the idle-Thread
/// it removes and deletes Threads in state ToBeDestroyed
///
  void cleanupDeadThreads();

friend class Mutex;
//-----------------------------------------------------------
/// this a a protected Method and should be called from Mutex onlx
/// it is somewhat of a hack, we need to release the Spinlock,
/// after we set the ThreadState Sleeping, but before we yield away
/// also we must not be interrupted and we want to avoid disabling Interrupts
/// (even though it would be possible in this case, as we don't allocate memory)
///
/// @param &lock The SpinLock we want to release
  void sleepAndRelease(SpinLock &lock);


private:

  Scheduler();

//-----------------------------------------------------------
/// Scheduler internal lock abstraction method
/// locks the thread-list against concurrent access by prohibiting a thread switch
  void lockScheduling();  //not as severe as stopping Interrupts
//-----------------------------------------------------------
/// Scheduler internal lock abstraction method
/// unlocks the thread-list
  void unlockScheduling();
//-----------------------------------------------------------
/// Scheduler internal lock abstraction method
/// tests the thread-list-lock without setting it,
/// use this _only_ in InterruptHandler-Context
/// @return true if lock is set, false otherwise
  bool testLock();

  static Scheduler *instance_;

  List<Thread*> threads_;

  static void startThreadHack();

  bool kill_old_;

  uint32 block_scheduling_;

};
#endif
