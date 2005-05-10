//----------------------------------------------------------------------
//   $Id: KernelMemoryManager.cpp,v 1.11 2005/05/10 15:27:54 davrieb Exp $
//----------------------------------------------------------------------
//
//  $Log: KernelMemoryManager.cpp,v $
//  Revision 1.10  2005/05/03 18:31:10  btittelbach
//  fix of evil evil MemoryManager Bug
//
//  Revision 1.9  2005/05/03 17:09:25  btittelbach
//  Assert Fix
//
//  Revision 1.8  2005/04/28 14:07:25  btittelbach
//  Merge von size und flag (hack)
//
//  Revision 1.7  2005/04/26 15:58:46  nomenquis
//  threads, scheduler, happy day
//
//  Revision 1.6  2005/04/25 23:09:18  nomenquis
//  fubar 2
//
//  Revision 1.5  2005/04/25 21:15:41  nomenquis
//  lotsa changes
//
//  Revision 1.4  2005/04/23 22:20:30  btittelbach
//  just stuff
//
//  Revision 1.3  2005/04/23 18:13:27  nomenquis
//  added optimised memcpy and bzero
//  These still could be made way faster by using asm and using cache bypassing mov instructions
//
//  Revision 1.2  2005/04/23 17:35:03  nomenquis
//  fixed buggy memory manager
//  (giving out the same memory several times is no good idea)
//
//  Revision 1.1  2005/04/23 15:59:26  btittelbach
//
//  Testing Version vom KMM
//
//  Revision 1.2  2005/04/22 20:18:52  nomenquis
//  compile fixes
//
//  Revision 1.1  2005/04/22 20:13:00  btittelbach
//  Memory Manager v.0001
//
//
//----------------------------------------------------------------------
///
/// KernelMemoryManager
///
/// Management and Allocation of All that is Memory
///
/// @author Bernhard Tittelbach <xro@sbox.tugraz.at>
/// @version 1.0.0
/// 

#include "../../include/mm/KernelMemoryManager.h"
#include "ArchCommon.h"
#include "util/assert.h"

KernelMemoryManager * KernelMemoryManager::instance_ = 0;


void printout(char* text)
{
    uint8 * fb = (uint8*)0xC00B8000;
    static uint32 i=160;

    while (text && *text)
    {
      fb[i++] = *text++;
      fb[i++] = 0x9f;
    }

}
static uint32 fb_start = 0;
static char* fb = (char*)0xC00B8500;

#define print(x)     fb_start += 2; \
    { \
      uint32 divisor; \
      uint32 current; \
      uint32 remainder; \
      current = (uint32)x; \
      divisor = 1000000000; \
      while (divisor > 0) \
      { \
        remainder = current % divisor; \
        current = current / divisor; \
        \
        fb[fb_start++] = (uint8)current + '0' ; \
        fb[fb_start++] = 0x9f ; \
    \
        divisor = divisor / 10; \
        current = remainder; \
      }      \
      uint32 blubba;\
      uint32 asf;\
      for (asf=0;asf<1;++asf)\
        ++blubba;\
    }   
    
uint32 KernelMemoryManager::createMemoryManager(pointer start_address, pointer end_address)
{
  instance_ = new ((void*)start_address) KernelMemoryManager(start_address + sizeof(KernelMemoryManager),end_address);
  return 0;
}

KernelMemoryManager::KernelMemoryManager(pointer start_address, pointer end_address)
{ 
  

  malloc_end_=end_address;
  //memory_free_=end_address-start_address-sizeof(MallocSegment);
  assert (((end_address-start_address-sizeof(MallocSegment)) & 0x80000000) == 0);
  first_=new ((void*) start_address) MallocSegment(0,0,end_address-start_address-sizeof(MallocSegment),false);
  last_=first_;
  //segments_free_=1;
  //segments_used_=0;
  print (4444);
  
}

pointer KernelMemoryManager::allocateMemory(size_t requested_size)
{
  // find next free pointer of neccessary size + sizeof(MallocSegment);
  assert ((requested_size & 0x80000000) == 0);
  MallocSegment *new_pointer = findFreeSegment(requested_size);
  
  if (new_pointer == 0)
    arch_panic((uint8*)"Not enough Memory left\n");
  
  fillSegment(new_pointer,requested_size);

  print(((pointer) new_pointer) + sizeof(MallocSegment))
  print(9877)
  return ((pointer) new_pointer) + sizeof(MallocSegment);

}
  
bool KernelMemoryManager::freeMemory(pointer virtual_address)
{
  //find MallocSegment
  //delete segment
  if (virtual_address == 0 || virtual_address < ((pointer) first_) || virtual_address >= malloc_end_)
    return false;
  
  MallocSegment *m_segment = getSegmentFromAddress(virtual_address);
  if (m_segment->marker_ != 0xdeadbeef)
    return false;
  freeSegment(m_segment);
  return true;
}
  
pointer KernelMemoryManager::reallocateMemory(pointer virtual_address, size_t new_size)
{
  //find MallocSegment
  //check if there is enought free space afterwards
  //if it is -> merge spaces and return same pointer
  //if not -> find large enough space and move memory, return new pointer
  assert ((new_size & 0x80000000) == 0);
  if (new_size == 0)
  {
    freeMemory(virtual_address);
    return 0;
  }
  
  MallocSegment *m_segment = getSegmentFromAddress(virtual_address);
  
  if (new_size == m_segment->getSize())
    return virtual_address;
  
  if (new_size < m_segment->getSize())
  { //downsize segment
    downsizeSegment(m_segment,new_size);
    return virtual_address;
  }
  else
  { //maybe we can solve this the easy way...
    if (m_segment->next_ != 0)
      if (m_segment->next_->getUsed() == false &&
        m_segment->next_->getSize() + m_segment->getSize() >= new_size)
      {
        mergeWithFollowingFreeSegment(m_segment);
        return virtual_address;
      }
      
    //or not.. lets search for larger space
    pointer new_address = allocateMemory(new_size);
    ArchCommon::memcpy(new_address,virtual_address, m_segment->getSize());
    freeSegment(m_segment);
    return new_address;
  }
}




MallocSegment *KernelMemoryManager::getSegmentFromAddress(pointer virtual_address)
{
  MallocSegment *m_segment;
  m_segment = (MallocSegment*) (virtual_address - sizeof(MallocSegment));
  assert(m_segment != 0);
  assert(m_segment->marker_ == 0xdeadbeef);
  return m_segment;
}

MallocSegment *KernelMemoryManager::findFreeSegment(size_t requested_size)
{
  MallocSegment *current=first_;
  while (current != 0)
  {
    assert(current->marker_ == 0xdeadbeef);
    if ((current->getSize() >= requested_size) && (current->getUsed() == false))
      return current;
    
    current = current->next_;
  }
  
  return 0;
}

void KernelMemoryManager::fillSegment(MallocSegment *this_one, size_t requested_size)
{
  assert(this_one != 0);
  assert(this_one->marker_ == 0xdeadbeef);
  assert(this_one->getSize() >= requested_size);

  size_t space_left = this_one->getSize() - requested_size;

  //size stays as it is, if there would be no more space to add a new segment
  this_one->setUsed(true);
  assert(this_one->getUsed() == true);

  //add a free segment after this one, it there's enough space
  if (space_left > sizeof(MallocSegment))
  {
    this_one->setSize(requested_size);
    assert(this_one->getSize() == requested_size);
    assert(this_one->getUsed() == true);
    
    MallocSegment *new_segment = new ((void*) ( ((pointer) this_one)+sizeof(MallocSegment)+requested_size)) MallocSegment(this_one,this_one->next_,space_left-sizeof(MallocSegment),false);
    this_one->next_ = new_segment;
  }
}

void KernelMemoryManager::freeSegment(MallocSegment *this_one)
{
  assert(this_one != 0);
  assert(this_one->marker_ == 0xdeadbeef);

  //mark segment as free
  //if previous segment is free: delete Segment and add space to previous Segmen
  //if next segment is free: delete next Segment and add space to this segment
   
  this_one->setUsed(false);
  assert(this_one->getUsed() == false);
  
  if (this_one->prev_ != 0)
  {
    assert(this_one->prev_->marker_ == 0xdeadbeef);
    if (this_one->prev_->getUsed() == false)
    {
      size_t my_true_size = ((this_one->next_==0)? malloc_end_ - ((pointer) this_one) : ((pointer) this_one->next_) - ((pointer) this_one));

      this_one->prev_->setSize(my_true_size + this_one->prev_->getSize());
      this_one->prev_->next_ = this_one->next_;
      
      MallocSegment *previous_one = this_one->prev_;
      
      this_one = previous_one;
    }
  }
  
  mergeWithFollowingFreeSegment(this_one);
   
  //ease debugging, clear the segment we don't own anymore
  //and the previous MallocSegment Object with it.
  //this is really safe unless someone just grabs memory instead
  //of allocating it with this MemoryManager
  //is something is suddenly zero, we immediatly know there was a 
  //problem with someones pointer
  ArchCommon::bzero(((pointer) this_one) + sizeof(MallocSegment), this_one->getSize(), 0);
}

bool KernelMemoryManager::mergeWithFollowingFreeSegment(MallocSegment *this_one)
{
  assert(this_one != 0);
  assert(this_one->marker_ == 0xdeadbeef);
  
  if (this_one->next_ != 0)
  {
    assert(this_one->next_->marker_ == 0xdeadbeef);
    if (this_one->next_->getUsed() == false)
    {
      MallocSegment *next_one = this_one->next_;
      size_t true_next_size = ((next_one->next_==0)? malloc_end_ - ((pointer) next_one) : ((pointer) next_one->next_) - ((pointer) next_one));

      this_one->setSize(this_one->getSize() + true_next_size);
      this_one->next_ = next_one->next_;

      ArchCommon::bzero((pointer) next_one, sizeof(MallocSegment), 0);
      
      //have to check again, could have changed...
      if (this_one->next_ == 0)
        last_ = this_one;
      
      return true;
    }
  }
  return false;
}

void KernelMemoryManager::downsizeSegment(MallocSegment *this_one, size_t new_size)
{
  assert(this_one != 0);
  assert(this_one->marker_ == 0xdeadbeef);
  assert(this_one->getUsed() >= new_size);

  size_t space_free = this_one->getUsed() - new_size;

  //add a free segment after this one, it there's enough space
  //else, leave it as it is
  if (space_free > sizeof(MallocSegment))
  {
    this_one->setSize(new_size);
    assert(this_one->getSize() == new_size);
    
    MallocSegment *new_segment = new ((void*) ( ((pointer) this_one)+sizeof(MallocSegment)+new_size)) MallocSegment(this_one,this_one->next_,space_free-sizeof(MallocSegment),false);
    this_one->next_ = new_segment;
    //check if we can enlarge...
    mergeWithFollowingFreeSegment(new_segment);
  }
}
