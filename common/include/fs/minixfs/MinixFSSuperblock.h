// Projectname: SWEB
// Simple operating system for educational purposes

#ifndef MinixFSSuperblock_h___
#define MinixFSSuperblock_h___

#include "fs/PointList.h"
#include "fs/Superblock.h"
#include "MinixStorageManager.h"

class Inode;
class Superblock;

//-----------------------------------------------------------------------------
/**
 * MinixFSSuperblock
 *
 */
class MinixFSSuperblock : public Superblock
{
  public:

  /// constructor
    MinixFSSuperblock(Dentry* s_root);

  /// destructor
    virtual ~MinixFSSuperblock();

  /// create a new Inode of the superblock, mknod with dentry, add in the list.
    virtual Inode* createInode(Dentry* dentry, uint32 type);

  /// remove the corresponding file descriptor.
    virtual int32 removeFd(Inode* inode, FileDescriptor* fd);

  /// This method is called to read a specific inode from a mounted
  /// file-system.
    virtual int32 readInode(Inode* inode);

  /// This method is called to write a specific inode to a mounted file-system,
  /// and gets called on inodes which have been marked dirty.
    virtual void write_inode(Inode* inode);

  /// This method is called whenever the reference count on an inode reaches 0,
  /// and it is found that the link count (i_nlink= is also zero. It si
  /// presumed that the file-system will deal with this situation be
  /// invalidating the inode in the file-system and freeing up any resourses
  /// used.
    virtual void delete_inode(Inode* inode);

  /// create a file with the given flag and  a file descriptor with the given
  /// inode.
    virtual int32 createFd(Inode* inode, uint32 flag);

  private:
    int32 initInodes();
};
//-----------------------------------------------------------------------------

#endif // MinixFSSuperblock_h___
