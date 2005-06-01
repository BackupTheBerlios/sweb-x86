
//
// CVS Log Info for $RCSfile: RamFsSuperblock.h,v $
//
// $Id: RamFsSuperblock.h,v 1.1 2005/06/01 09:20:36 davrieb Exp $
// $Log: Superblock.h,v $
// Revision 1.1  2005/05/10 16:42:32  davrieb
// add first attempt to write a virtual file system
//
//

#ifndef RamFsSuperblock_h___
#define RamFsSuperblock_h___

#include "fs/Superblock.h"

#define MAGIC_NUMBER_EXT2 0xEF53

// The falg of the super block.
#define MS_RDONLY 1

class FileSystemType;
class Iattr;
class Statfs;
class WaitQueue;
class File;

//-----------------------------------------------------------------------------
/**
 * RamFsSuperblock
 *
 * The first block of the virtual-file-system. It contains for instance the
 * configuration of the file system. Sotre information concerning a mounted
 * filesystem. For disk-based filesystems, this object usually corresponds
 * to a filesystem control block stored on disk.
 */
class RamFsSuperblock : Superblock
{
public:

  typedef List<Inode> IList;

protected:

  IList inode_list_;

public:

  RamFsSuperblock();

  virtual ~RamFsSuperblock();

  /// This method is called to read a specific inode from a mounted
  /// file-system. It is only called from get_new_inode.
  virtual void read_inode(Inode* inode);

  /// This method is called to write a specific inode to a mounted file-system,
  /// and gets called on inodes which have been marked dirty with
  /// mark_inode_dirty.
  virtual void write_inode(Inode* inode);

  /// This method is called whenever the reference count on an inode reaches 0,
  /// and it is found that the link count (i_nlink= is also zero. It si
  /// presumed that the file-system will deal with this situation be
  /// invalidating the inode in the file-system and freeing up any resourses
  /// used.
  virtual void delete_inode(Inode* inode);

  /// This method is needed to implement statfs(2) system call
  virtual int32 statfs(Superblock*, Statfs*, int32) { return 0; }

  /// Optional method, call when VFS clears the inode. This is needed (at
  /// least) by file-system which attaches kmalloced data to the inode
  /// sturcture, as particularly might be the case for file-systems using the
  /// generic_ip field in class Inode.
  virtual void clear_inode(Inode* inode) {}
};
//-----------------------------------------------------------------------------

#endif // Superblock_h___
