// Projectname: SWEB
// Simple operating system for educational purposes

#include "fs/VfsMount.h"


//----------------------------------------------------------------------
VfsMount::VfsMount() :
  mnt_parent_(0),
  mnt_mountpoint_(0),
  mnt_root_(0),
  mnt_sb_(0),
  mnt_flags_(0)
{
}

//----------------------------------------------------------------------
VfsMount::VfsMount(VfsMount* parent, Dentry * mountpoint, Dentry* root,
    Superblock* superblock, int32 flags) :
  mnt_parent_(parent),
  mnt_mountpoint_(mountpoint),
  mnt_root_(root),
  mnt_sb_(superblock),
  mnt_flags_(flags)
{
}

//----------------------------------------------------------------------
VfsMount::~VfsMount()
{
}

//----------------------------------------------------------------------
VfsMount const *VfsMount::getParent() const
{
  return mnt_parent_;
}

//----------------------------------------------------------------------
Dentry const *VfsMount::getMountpoint() const
{
  return mnt_mountpoint_;
}

//----------------------------------------------------------------------
Dentry const *VfsMount::getRoot() const
{
  return mnt_root_;
}

//----------------------------------------------------------------------
Superblock const *VfsMount::getSuperblock() const
{
  return mnt_sb_;
}

//----------------------------------------------------------------------
int32 VfsMount::getFlags() const
{
  return mnt_flags_;
}

