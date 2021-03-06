/**
 * @file RamFSSuperblock.cpp
 */

#include "fs/FileDescriptor.h"
#include "fs/ramfs/RamFSSuperblock.h"
#include "fs/ramfs/RamFSInode.h"
#include "fs/ramfs/RamFSFile.h"
#include "fs/Dentry.h"
#include "fs_global.h"
#include "assert.h"

#include "console/kprintf.h"
#include "console/debug.h"
#define ROOT_NAME "/"


RamFSSuperblock::RamFSSuperblock ( Dentry* s_root, uint32 s_dev ) : Superblock ( s_root, s_dev )
{
  Dentry *root_dentry = new Dentry ( ROOT_NAME );

  if ( s_root )
  {
    // MOUNT
    mounted_over_ = s_root;
  }
  else
  {
    // ROOT
    mounted_over_ = root_dentry;
  }
  s_root_ = root_dentry;

  // create the inode for the root_dentry
  Inode *root_inode = ( Inode* ) ( new RamFSInode ( this, I_DIR ) );
  int32 root_init = root_inode->mknod ( root_dentry );
  assert ( root_init == 0 );

  // add the root_inode in the list
  all_inodes_.pushBack ( root_inode );
}


RamFSSuperblock::~RamFSSuperblock()
{
  assert ( dirty_inodes_.empty() == true );

  uint32 num = s_files_.getLength();
  for ( uint32 counter = 0; counter < num; counter++ )
  {
    FileDescriptor *fd = s_files_.at ( 0 );
    File* file = fd->getFile();
    s_files_.remove ( fd );

    if ( file )
    {
      delete file;
    }
    delete fd;
  }

  assert ( s_files_.empty() == true );

  num = all_inodes_.getLength();
  for ( uint32 counter = 0; counter < num; counter++ )
  {
    Inode* inode = all_inodes_.at ( 0 );
    all_inodes_.remove ( inode );
    Dentry* dentry = inode->getDentry();
    delete dentry;

    delete inode;
  }

  assert ( all_inodes_.empty() == true );
}


Inode* RamFSSuperblock::createInode ( Dentry* dentry, uint32 type )
{
  Inode *inode = ( Inode* ) ( new RamFSInode ( this, type ) );
  assert ( inode );
  if ( type == I_DIR )
  {
    debug ( RAMFS,"createInode: I_DIR\n" );
    int32 inode_init = inode->mknod ( dentry );
    assert ( inode_init == 0 );
  }
  else if ( type == I_FILE )
  {
    debug ( RAMFS,"createInode: I_FILE\n" );
    int32 inode_init = inode->mkfile ( dentry );
    assert ( inode_init == 0 );
  }

  all_inodes_.pushBack ( inode );
  return inode;
}


int32 RamFSSuperblock::readInode ( Inode* inode )
{
  assert ( inode );

  if ( !all_inodes_.included ( inode ) )
  {
    all_inodes_.pushBack ( inode );
  }
  return 0;
}


void RamFSSuperblock::writeInode ( Inode* inode )
{
  assert ( inode );

  if ( !all_inodes_.included ( inode ) )
  {
    all_inodes_.pushBack ( inode );
  }
}


void RamFSSuperblock::delete_inode ( Inode* inode )
{
  all_inodes_.remove ( inode );
}


int32 RamFSSuperblock::createFd ( Inode* inode, uint32 flag )
{
  assert ( inode );

  File* file = inode->link ( flag );
  FileDescriptor* fd = new FileDescriptor ( file );
  s_files_.pushBack ( fd );
  global_fd.pushBack ( fd );

  if ( !used_inodes_.included ( inode ) )
  {
    used_inodes_.pushBack ( inode );
  }

  return ( fd->getFd() );
}


int32 RamFSSuperblock::removeFd ( Inode* inode, FileDescriptor* fd )
{
  assert ( inode );
  assert ( fd );

  s_files_.remove ( fd );
  //global_fd.remove(fd);

  File* file = fd->getFile();
  int32 tmp = inode->unlink ( file );

  debug ( RAMFS,"remove the fd num: %d\n", fd->getFd() );
  if ( inode->getNumOpenedFile() == 0 )
  {
    used_inodes_.remove ( inode );
  }
  delete fd;

  return tmp;
}
