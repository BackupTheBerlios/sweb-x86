/**
 * @file Condition.cpp
 */

#include "Condition.h"
#include "Scheduler.h"
#include "assert.h"
#include "console/kprintf.h"
#include "ArchInterrupts.h"
#include "console/debug.h"

Condition::Condition(Mutex *lock)
{
  sleepers_=new List<Thread *>();
  lock_=lock;
}

Condition::~Condition()
{
  delete sleepers_;
}

void Condition::wait()
{
  // list is protected, because we assume, the lock is being held
  assert(lock_->isHeldBy(currentThread));
  assert(ArchInterrupts::testIFSet());
  sleepers_->pushBack(currentThread);
  //<-- an interrupt and signal could happen here or during "sleep()"  ! problem: Thread* gets deleted before thread goes to sleep -> no wakeup call possible on next signal
  debug(CONDITION, "Condition::wait: Thread %x  %d:%s wating on Condition %x\n",currentThread,currentThread->getPID(),currentThread->getName(),this);
  Scheduler::instance()->sleepAndRelease(*lock_);
  lock_->acquire();
}

void Condition::signal()
{
  if (! lock_->isHeldBy(currentThread))
    return;
  assert(ArchInterrupts::testIFSet());
  Thread *thread=0;
  if (!sleepers_->empty())
  {
    thread = sleepers_->front();
    if (thread->state_ == Sleeping)
    {
      //Solution to above Problem: Wake and Remove from List only Threads which are actually sleeping
      Scheduler::instance()->wake(thread);
      sleepers_->popFront();
    }
  }
  if (thread)
    debug(CONDITION,"Condition::signal: Thread %x  %d:%s being signaled for Condition %x\n",thread,thread->getPID(),thread->getName(),this);
}

void Condition::broadcast()
{
  if (! lock_->isHeldBy(currentThread))
    return;
  assert(ArchInterrupts::testIFSet());
  Thread *thread;
  List<Thread*> tmp_threads;
  while (!sleepers_->empty())
  {
    thread = sleepers_->front();
    sleepers_->popFront();
    if (thread->state_ == Sleeping)
      Scheduler::instance()->wake(thread);
    else
      tmp_threads.pushBack(thread);
    debug(CONDITION,"Condition::broadcast: Thread %x  %d:%s being signaled for Condition %x\n",thread,thread->getPID(),thread->getName(),this);
  }
  while (!tmp_threads.empty())
  {
    sleepers_->pushBack(tmp_threads.front());
    tmp_threads.popFront();
  }
}
