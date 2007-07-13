// Projectname: SWEB
// Simple operating system for educational purposes

#include "fs/FileDescriptor.h"
#include "fs/minixfs/MinixFSSuperblock.h"
#include "fs/minixfs/MinixFSInode.h"
#include "fs/minixfs/MinixFSFile.h"
#include "fs/Dentry.h"
#include "fs_global.h"
#include "assert.h"
#include "arch_bd_manager.h"
#include "kmalloc.h"

#include "console/kprintf.h"
#define ROOT_NAME "/"
#define INODE_SIZE 32
#define DENTRY_SIZE 32
#define DENTRY_PER_BLOCK 32
#define MAX_NAME_LENGTH 30

//----------------------------------------------------------------------
MinixFSSuperblock::MinixFSSuperblock(Dentry* s_root, uint32 s_dev) : Superblock(s_root, s_dev)
{
  BDManager::getInstance()->getDeviceByNumber(s_dev)->setBlockSize(MINIX_BLOCK_SIZE);
  //read Superblock data from disc
  Buffer *buffer = new Buffer(MINIX_BLOCK_SIZE);
  BDRequest * bd = new BDRequest(s_dev_, BDRequest::BD_READ, 1, 1, buffer->getBuffer());
  BDManager::getInstance()->getDeviceByNumber(s_dev)->addRequest ( bd );
  uint32 jiffies = 0;
  while( bd->getStatus() == BDRequest::BD_QUEUED && jiffies++ < 50000 );
  if( bd->getStatus() == BDRequest::BD_DONE )
  {
    s_num_inodes_ = buffer->get2Bytes(0);
    s_num_zones_ = buffer->get2Bytes(2);
    s_num_inode_bm_blocks_ = buffer->get2Bytes(4);
    s_num_zone_bm_blocks_ = buffer->get2Bytes(6);
    s_1st_datazone_ = buffer->get2Bytes(8);
    s_log_zone_size_ = buffer->get2Bytes(10);
    s_max_file_size_ = buffer->get4Bytes(12);
    s_magic_ = buffer->get2Bytes(16);
  }
  else
  {
    assert(0);
  }
  kprintfd("s_num_inodes_ : %d\ns_num_zones_ : %d\ns_num_inode_bm_blocks_ : %d\ns_num_zone_bm_blocks_ : %d\ns_1st_datazone_ : %d\ns_log_zone_size_ : %d\ns_max_file_size_ : %d\ns_magic_ : %d\n",s_num_inodes_,s_num_zones_,s_num_inode_bm_blocks_,s_num_zone_bm_blocks_,s_1st_datazone_,s_log_zone_size_,s_max_file_size_,s_magic_);
  
//   for(uint32 i = 0; i < BLOCK_SIZE; i++ )
//     kprintfd( "%2X%c", *(buffer+i), i%8 ? ' ' : '\n' );

  kprintfd("---buffer: %d", buffer);
  delete buffer;
  kprintfd("---buffer: end\n");

  //create Storage Manager
  uint32 bm_size = s_num_inode_bm_blocks_ + s_num_zone_bm_blocks_;
  Buffer *bm_buffer = new Buffer(MINIX_BLOCK_SIZE*bm_size);
  kprintfd("---creating request\n");
  BDRequest * bm_bd = new BDRequest(s_dev_, BDRequest::BD_READ, 3, bm_size, bm_buffer->getBuffer());
  kprintfd("---adding request\n");
  BDManager::getInstance()->getDeviceByNumber(s_dev)->addRequest ( bm_bd );
  jiffies = 0;
  kprintfd("---waiting for request\n");
  while( bm_bd->getStatus() == BDRequest::BD_QUEUED && jiffies++ < 50000 );
  if( bm_bd->getStatus() == BDRequest::BD_DONE )
  {
    kprintfd("---creating Storage Manager\n");
    storage_manager_ = new MinixStorageManager(bm_buffer,
                                               s_num_inode_bm_blocks_,
                                               s_num_zone_bm_blocks_,
                                               s_num_inodes_, s_num_zones_);
    storage_manager_->printBitmap();
  }
  else
  {
    assert(0);
  }
  kprintfd("---bm_buffer: %d", bm_buffer);
  delete bm_buffer;
  kprintfd("---bm_buffer: end\n");

  //call initInodes();
   initInodes();
   if (all_inodes_.at(0))
   {
     int32 root_init = all_inodes_.at(0)->mknod(s_root_);
   }
}

void MinixFSSuperblock::initInodes()
{
  MinixFSInode *root_inode = 0;
  uint32 inodes_start = s_num_inode_bm_blocks_ + s_num_zone_bm_blocks_ + 2;
  Buffer *ibuffer = new Buffer(MINIX_BLOCK_SIZE);
  BDRequest *bd = new BDRequest(s_dev_, BDRequest::BD_READ, inodes_start, 1, ibuffer->getBuffer());
  BDManager::getInstance()->getDeviceByNumber(s_dev_)->addRequest ( bd );
  uint32 jiffies = 0;
  while( bd->getStatus() == BDRequest::BD_QUEUED && jiffies++ < 50000 );
  if( bd->getStatus() == BDRequest::BD_DONE )
  {
    uint16 *i_zones = new uint16[9];
    for(uint32 num_zone = 0; num_zone < 9; num_zone ++)
    {
      i_zones[num_zone] = ibuffer->get2Bytes(14 + (num_zone * 2));
    }
    root_inode = new MinixFSInode( this,
                          ibuffer->get2Bytes(0),
                          ibuffer->get2Bytes(2),
                          ibuffer->get4Bytes(4),
                          ibuffer->get4Bytes(8),
                          ibuffer->getByte(12),
                          ibuffer->getByte(13),
                          i_zones
                                        );
  }
  else
  {
    assert(false);
  }
  Dentry *root_dentry = new Dentry(ROOT_NAME);
  if (s_root_)
  {
    root_dentry->setMountPoint(s_root_);
    mounted_over_ = s_root_; // MOUNT
  }
  else
  {
    mounted_over_ = root_dentry; // ROOT
    s_root_ = root_dentry;
    
  }
  root_dentry->setParent(root_dentry); // NOTE: maybe set to s_root_ for not root mount ?
  root_inode->mknod(root_dentry);
  //TODO read childs from disc
  Buffer *dbuffer = new Buffer(MINIX_ZONE_SIZE);
  for (uint32 zone = 0; root_inode->i_zones_[zone] != 0; zone++)
  {
    BDRequest *bd = new BDRequest(s_dev_, BDRequest::BD_READ, root_inode->i_zones_[zone], MINIX_ZONE_SIZE/MINIX_BLOCK_SIZE, dbuffer->getBuffer());
    BDManager::getInstance()->getDeviceByNumber(s_dev_)->addRequest ( bd );
    uint32 jiffies = 0;
    while( bd->getStatus() == BDRequest::BD_QUEUED && jiffies++ < 50000 );
    if( bd->getStatus() == BDRequest::BD_DONE )
    {
      for(uint32 curr_dentry = 0; curr_dentry < MINIX_BLOCK_SIZE; curr_dentry += DENTRY_SIZE)
      {
        if(dbuffer->get2Bytes(curr_dentry))
        {
          Inode* inode = readInode(dbuffer->get2Bytes(curr_dentry));
          uint32 offset = 2;
          char *name = char[MAX_NAME_LENGTH];
          char ch = 0;
          do
          {
            ch = dbuffer->getByte(curr_dentry + offset);
            name[offset] = ch;
            ++offset;
          } while (ch);
          inode->mknod(new Dentry(name));
        }
      }
    }
    else
    {
      assert(false);
    }
  }
}

Inode* MinixFSSuperblock::readInode(uint16 i_num)
{
  assert(storage_manager_->isSet(i_num));
  
}
//----------------------------------------------------------------------
// void MinixFSSuperblock::initInodes()
// {
//   // starts after the MBR, Superblock, InodeBM and ZoneBM. ends when the Data starts.
//   uint32 inodes_start = s_num_inode_bm_blocks_ + s_num_zone_bm_blocks_ + 2;
//   uint32 num_inode_blocks = s_1st_datazone_ - inodes_start;
//   // read and create the inodes form disc which are marked used in the bitmap
//   uint32 num_used_inodes = storage_manager_->getNumUsedInodes();
//   uint32 inodes_read = 1;
//   uint32 curr_inode = 0;
//   uint32 offset = 0;
//   Buffer *buffer = new Buffer(MINIX_BLOCK_SIZE);
//   kprintfd( "initInodes: inode_start: %d\tnum_inode_blocks: %d\t num_used_inodes: %d\n",inodes_start, num_inode_blocks, num_used_inodes);
//   for (uint32 block = 0; block < num_inode_blocks && inodes_read < num_used_inodes; block++)
//   {
//     BDRequest * bd = new BDRequest(s_dev_, BDRequest::BD_READ, inodes_start + block, 1, buffer->getBuffer());
//     BDManager::getInstance()->getDeviceByNumber(s_dev_)->addRequest ( bd );
//     uint32 jiffies = 0;
//     while( bd->getStatus() == BDRequest::BD_QUEUED && jiffies++ < 50000 );
//     if( bd->getStatus() == BDRequest::BD_DONE )
//     {
//       for(;curr_inode < INODES_PER_BLOCK * (block + 1) && inodes_read < num_used_inodes; curr_inode++)
//       {
//         //kprintfd( "initInodes: curr_inode: %d\t isset: %d\n", curr_inode, storage_manager_->isInodeSet(curr_inode));
//         if(storage_manager_->isInodeSet(curr_inode))
//         {
//           offset = curr_inode * INODE_SIZE - ( block * MINIX_BLOCK_SIZE);
//           uint16 *i_zones = new uint16[9];
//           for(uint32 num_zone = 0; num_zone < 9; num_zone ++)
//           {
//             i_zones[num_zone] = buffer->get2Bytes(offset + 14 + (num_zone * 2));
//           }
//           all_inodes_.pushBack(new MinixFSInode( this,
//                                buffer->get2Bytes(offset),
//                                buffer->get2Bytes(offset + 2),
//                                buffer->get4Bytes(offset + 4),
//                                buffer->get4Bytes(offset + 8),
//                                buffer->getByte(offset + 12),
//                                buffer->getByte(offset + 13),
//                                i_zones
//                                              )
//                               );
//           ++inodes_read;
//         }
//       }
//     }
//     else
//     {
//       assert(0);
//     }
//   }
//   delete buffer;
// }

//----------------------------------------------------------------------
MinixFSSuperblock::~MinixFSSuperblock()
{
  assert(dirty_inodes_.empty() == true);

  uint32 num = s_files_.getLength();
  for(uint32 counter = 0; counter < num; counter++)
  {
    FileDescriptor *fd = s_files_.at(0);
    File* file = fd->getFile();
    s_files_.remove(fd);

    if(file)
    {
      delete file;
    }
    delete fd;
  }

  assert(s_files_.empty() == true);

  num = all_inodes_.getLength();
  for(uint32 counter = 0; counter < num; counter++)
  {
    Inode* inode = all_inodes_.at(0);
    all_inodes_.remove(inode);
    Dentry* dentry = inode->getDentry();
    delete dentry;

    delete inode;
  }

  assert(all_inodes_.empty() == true);
}

//----------------------------------------------------------------------
Inode* MinixFSSuperblock::createInode(Dentry* dentry, uint32 type)
{
  Inode *inode = (Inode*)(new MinixFSInode(this, type));
  if(type == I_DIR)
  {
    int32 inode_init = inode->mknod(dentry);
    assert(inode_init == 0);
  }
  else if(type == I_FILE)
  {
    kprintfd("createInode: I_FILE\n");
    int32 inode_init = inode->mkfile(dentry);
    assert(inode_init == 0);
  }

  all_inodes_.pushBack(inode);
  return inode;
}

//----------------------------------------------------------------------
int32 MinixFSSuperblock::readInode(Inode* inode)
{
  assert(inode);

  if (!all_inodes_.included(inode))
  {
    all_inodes_.pushBack(inode);
  }
  return 0;
}

//----------------------------------------------------------------------
void MinixFSSuperblock::write_inode(Inode* inode)
{
  assert(inode);

  if (!all_inodes_.included(inode))
  {
    all_inodes_.pushBack(inode);
  }
}

//----------------------------------------------------------------------
void MinixFSSuperblock::delete_inode(Inode* inode)
{
  all_inodes_.remove(inode);
  //TODO delete(inode); ?
}

//----------------------------------------------------------------------
int32 MinixFSSuperblock::createFd(Inode* inode, uint32 flag)
{
  assert(inode);

  File* file = inode->link(flag);
  FileDescriptor* fd = new FileDescriptor(file);
  s_files_.pushBack(fd);
  global_fd.pushBack(fd);

  if (!used_inodes_.included(inode))
  {
    used_inodes_.pushBack(inode);
  }

  return(fd->getFd());
}

//----------------------------------------------------------------------
int32 MinixFSSuperblock::removeFd(Inode* inode, FileDescriptor* fd)
{
  assert(inode);
  assert(fd);

  s_files_.remove(fd);
  //global_fd.remove(fd);

  File* file = fd->getFile();
  int32 tmp = inode->unlink(file);

  kprintfd("remove the fd num: %d\n", fd->getFd());
  if(inode->getNumOpenedFile() == 0)
  {
    used_inodes_.remove(inode);
  }
  delete fd;

  return tmp;
}

uint16 MinixFSSuperblock::allocateZone()
{
  return (storage_manager_->acquireZone() + s_1st_datazone_);
}

void MinixFSSuperblock::freeZone(uint16 pointer)
{
  storage_manager_->freeZone(pointer - s_1st_datazone_);
}



