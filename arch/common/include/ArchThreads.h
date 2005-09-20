//----------------------------------------------------------------------
//  $Id: ArchThreads.h,v 1.9 2005/09/20 17:44:44 lythien Exp $
//----------------------------------------------------------------------
//
//  $Log: ArchThreads.h,v $
//  Revision 1.8  2005/08/26 13:58:24  nomenquis
//  finally even the syscall handler does that it is supposed to do
//
//  Revision 1.7  2005/07/21 19:08:39  btittelbach
//  Jö schön, Threads u. Userprozesse werden ordnungsgemäß beendet
//  Threads können schlafen, Mutex benutzt das jetzt auch
//  Jetzt muß nur der Mutex auch überall verwendet werden
//
//  Revision 1.6  2005/06/14 18:22:37  btittelbach
//  RaceCondition anfälliges LoadOnDemand implementiert,
//  sollte optimalerweise nicht im InterruptKontext laufen
//
//  Revision 1.5  2005/05/31 17:29:16  nomenquis
//  userspace
//
//  Revision 1.4  2005/05/25 08:27:48  nomenquis
//  cr3 remapping finally really works now
//
//  Revision 1.3  2005/04/27 08:58:16  nomenquis
//  locks work!
//  w00t !
//
//  Revision 1.2  2005/04/26 15:58:45  nomenquis
//  threads, scheduler, happy day
//
//  Revision 1.1  2005/04/24 16:58:03  nomenquis
//  ultra hack threading
//
//----------------------------------------------------------------------


#ifndef _ARCH_THREADS_H_
#define _ARCH_THREADS_H_

#include "types.h"

/** @class ArchThreadsInfo
 *
 * this is where the thread info for task switching is stored
 *
 */
class ArchThreadInfo;

/** @class Thread
 *
 * this is where the thread itself will reside
 *
 */
class Thread;

/** @ArchThreadInfo
 *
 *
 *
 */
extern ArchThreadInfo *currentThreadInfo;

/** @Thread
 *
 *
 *
 */
extern Thread *currentThread;

/** @class ArchThreads
 *
 *
 *
 */
class ArchThreads
{
public:

/** @initialise
 *
 *
 *
 */
  static void initialise();

/** @switchToThreadOnIret
 *
 * @param thread
 *
 */
  static void switchToThreadOnIret(Thread *thread);

/** @cleanupThreadInfo
 *
 * @param info
 *
 */
  static void cleanupThreadInfos(ArchThreadInfo *&info);

/** @createThreadInfosKernelThread
 *
 * @param info
 * @param start_function
 * @param stack
 */
  static void createThreadInfosKernelThread(ArchThreadInfo *&info, pointer start_function, pointer stack);

/** @createThreadInfoUserspaceThread
 *
 * @param info
 * @param start_function
 * @param user_stack
 * @param kernel_stack
 */
  static void createThreadInfosUserspaceThread(ArchThreadInfo *&info, pointer start_function, pointer user_stack, pointer kernel_stack);

/** @yield
 *
 *
 *
 */
  static void yield();

/** @setPageDirectory
 *
 * @param thread
 * @param page_dir_physical_page
 */
  static void setPageDirectory(Thread *thread, uint32 page_dir_physical_page);

/** @getPageDirectory
 *
 * @param thread
 *
 */
  uint32 getPageDirectory(Thread *thread);

/** @testSetLock
 *
 * @param lock
 * @param new_value
 */
  static uint32 testSetLock(uint32 &lock, uint32 new_value);

/** @printThreadRegisters
 *
 * @param thread
 * @param userspace_register
 *
 */
  static void printThreadRegisters(Thread *thread, uint32 userspace_registers);
};

#endif
