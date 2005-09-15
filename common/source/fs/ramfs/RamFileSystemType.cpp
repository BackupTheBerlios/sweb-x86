// Projectname: SWEB
// Simple operating system for educational purposes

#include "fs/ramfs/RamFileSystemType.h"
#include "fs/ramfs/RamFsSuperblock.h"

//----------------------------------------------------------------------
RamFileSystemType::RamFileSystemType()
{
  fs_name_ = "ramfs";
  fs_flags_ = 0;
}

//----------------------------------------------------------------------
RamFileSystemType::~RamFileSystemType()
{
}

//----------------------------------------------------------------------
Superblock *RamFileSystemType::readSuper(Superblock *superblock, void*) const
{
  return superblock;
}

//----------------------------------------------------------------------
Superblock *RamFileSystemType::createSuper(Dentry *root) const
{
  Superblock *super = new RamFsSuperblock(root);
  return super;
}
