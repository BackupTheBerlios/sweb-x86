#include "arch_bd_virtual_device.h"

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

BDVirtualDevice::BDVirtualDevice(BDDriver * driver, uint32 offset, uint32 num_blocks, uint32 block_size, char *name, bool writable)
{
    kprintfd("BDVirtualDevice::ctor:entered");
    offset_       = offset;
    num_blocks_   = num_blocks;
    block_size_   = block_size;
    writable_     = writable;
    driver_       = driver;
    
    kprintfd("BDVirtualDevice::ctor:calling string functions\n");
    name_         = new char[ strlen( name ) * sizeof( char ) + 1 ];
    strncpy( name_, name, strlen(name) );
    name_[ strlen(name) ] = '\0';
    kprintfd("BDVirtualDevice::ctor:string functions done\n");
    
    dev_number_   = 0;
};

    
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////    
void BDVirtualDevice::addRequest(BDRequest * command) 
{
  uint32 res = 5;
  uint32 start_block = 0;
  
  switch( command->getCmd() )
  {
    case BDRequest::BD_GET_BLK_SIZE:
      res = block_size_;
      command->setStatus( BDRequest::BD_DONE );
      break;
    case BDRequest::BD_GET_NUM_BLOCKS: 
      res = num_blocks_;
      command->setStatus( BDRequest::BD_DONE );
      break;
    case BDRequest::BD_READ: 
    case BDRequest::BD_WRITE:
      start_block = command->getStartBlock();
      command->setStartBlock( start_block + offset_ );
   default:
      res = driver_->addRequest( command );
      break;
  }

  command->setResult( res );
  return;
};

