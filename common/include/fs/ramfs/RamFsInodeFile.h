// Projectname: SWEB
// Simple operating system for educational purposes
//
// Copyright (C) 2005  Chen Qiang
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef RamFsInodeFile_h___
#define RamFsInodeFile_h___

#include "fs/Inode.h"

class FileLock;
class VMArea;
class WaitQueue;
class Swmaphore;
class Dentry;
class BufferHead;
class Semaphore;
class File;
class Page;

// three possible inode state bits:
#define I_DIRTY 1 // Dirty inodes are on the per-super-block s_dirty_ list, and
// will be written next time a sync is requested.
#define I_LOCK 2 // Inodes are locked while they are being created, read oder
// written.
#define I_FREEING 4 // An inode is has this state when the reference count and
// link count have both reached zero. This seems to be only
// used by fat file-system.

// The per-inode flags:
#define MS_NODEV 2 // If this inode is a device special file, it cannot be
// opend.

//-------------------------------------------------------------------------
/**
 * RamFsInode
 *
 * All information needed by the filesystem to handle a file is included in a
 * data class called an inode. A filename is a casually assigned label that
 * can be changed, but the inode is unique to the file and remains the same
 * as long as the file exists.
 *
 * The path is through the inode hash table. Each inode is hashed (to an 8 bit
 * number) based on the address of the file-system's superblock and the inode
 * number. Inodes with the same hash value are then chained together in a
 * doubly linked list.
 */
class RamFsInodeFile : public Inode
{
protected:
  int32* data_;

public:

  RamFsInodeFile(Superblock *super_block) : Inode(super_block, I_FILE) {}

  virtual ~RamFsInodeFile();

  //--------------------------------------------------------------------------
  // This methode are only meaningful on directory inodes.
  //--------------------------------------------------------------------------
  /// create is called when the VFS wants to create a file with the given name
  /// (in the dentry) in the given directory. The VFS will have already checked
  /// that the name doesn't exist, and the dentry passed will be a negative
  /// dentry meanging that the inode pointer will be NULL.If create successful,
  /// get a new empty inode from the cache with get_empty_inode, fill in the
  /// fields and insert it into the hash table with insert_inode_hash, mark it
  /// dirty, and instantiate it into the Dcache with d_instantiate.
  virtual int32 create(Inode *inode, Dentry *dentry) { return 0; }

  /// lookup should check if that name (given by the Dentry) exists in the
  /// directory (given by the inode) and should update the Dentry using d_add
  /// if it does. This involves finding and loading the inode. If the lookup
  /// failed to find anything, this is indicated by returning a negative
  /// Dentry, with an inode pointer of NULL.
  virtual Dentry* lookup(Inode *inode, Dentry *dentry) {return((Dentry*)0);}

  /// The link method should make a hard link from the name refered to by the
  /// src_dentry to the name referred to by the dst_denty, which is in the
  /// directory refered to by the Inode.
  virtual int32 link(Dentry *src_dentry, Inode *inode, Dentry *dst_dentry)
  {return 0;}

  /// This should remove the name refered to by the Dentry from the directory
  /// referred to by the inode. It should d_delete the Dentry on success.
  virtual int32 unlink(Inode *inode, Dentry *dentry) {return 0;}

  /// This should create a symbolic link in the given directory with the given
  /// name having the given value. It should d_instantiate the new inode into
  /// the dentry on success.
  virtual int32 symlink(Inode *inode, Dentry *dentry, const char *link_name)
  {return 0;}

  /// Create a directory with the given parent and name.
  virtual int32 mkdir(Inode *inode, Dentry *dentry) {return 0;}

  /// Remove the named directory (if empty) and d_delete the dentry.
  virtual int32 rmdir(Inode *inode, Dentry *dentry) {return 0;}

  /// Create a device special file with the given parent, name and device
  /// number. Then d_instantiate the new inode into the dentry.
  virtual int32 mknod(Inode *inode, Dentry *dentry, int32 dev_num) {return 0;}

  /// The old_inode and old_entry refer to a directory and name that exist.
  /// rename should rename the object to have the parent and name given by the
  /// the new_inode and new_dentry.
  virtual int32 rename(Inode *old_inode, Dentry *old_dentry,
    Inode *new_inode, Dentry *new_dentry) { return 0; }
  //--------------------------------------------------------------------------

  /// The symbolic link referred to by the dentry is read and the value is
  /// copied into the user buffer (with copy_to_user) with a maximum length
  /// given by the intege.
  virtual int32 readlink(Dentry *dentry, char*, int32 max_length) {return 0;}

  /// If the directory (parent dentry) have a directory and a name within that
  /// directory (child dentry) then the obvious result of following the name
  /// from the directory would arrive at the child dentry. If an inode requires
  /// some other, non-obvious, result - s do symbolic links - the inode should
  /// provide a follow_link method to return the appropriate new dentry.
  /// @prt_dentry the parent dentry
  /// @chd_dentry the child dentry
  /// @lookup_flags a number of LOOKUP flags
  virtual Dentry* follow_link(Dentry *prt_dentry, Dentry *chd_dentry,
      uint32 lookup_flags = 0) {return((Dentry*)0);}

  /// This method is used to find the device block that holds a given block of
  /// a file. get_block should initialise the b_dev and b_blocknr field of the
  /// buffer_head, and should possibly modify the b_state flags.
  /// @inode the file that the block hold
  /// @block_number the file offset divided by file-system block size
  /// @b_state the buffer state flags.
  virtual int32 get_block(Inode *inode, int64 block_number,
      BufferHead *buffer_head, int32 b_state) {return 0;}

  /// It is needed for memory mapping of files, for using the send file system
  /// call.
  virtual int32 read_page(File *, Page *) { return 0; }
  virtual int32 wirte_page(File *, Page *) { return 0; }
  virtual int32 flush_page(Inode *, Page *, uint64) { return 0; }

  int32 readData(int32 offset, int32 size, int32 *buffer);
};


#endif // RamFsInodeFile_h___

