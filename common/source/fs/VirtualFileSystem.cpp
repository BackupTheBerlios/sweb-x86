
//
// CVS Log Info for $RCSfile: VirtualFileSystem.cpp,v $
//
// $Id: VirtualFileSystem.cpp,v 1.1 2005/05/10 16:42:30 davrieb Exp $
// $Log$
//

#include "fs/VirtualFileSystem.h"
#include "util/string.h"
#include "util/assert.h"


FileSystemType::FileSystemType(const char* fs_name) :
    fs_name_(fs_name)
{
  assert(fs_name != 0);
}

FileSystemType::~FileSystemType()
{
}

const char* FileSystemType::getFSName() const
{
  return fs_name_;
}

void FileSystemType::setFSName(const char* fs_name)
{
    fs_name_ = fs_name;
}

int32 FileSystemType::getFSFlags() const
{
  return fs_flags_;
}

void FileSystemType::setFSFlags(int32 fs_flags)
{
  fs_flags_ = fs_flags;
}

const FileSystemType::ReadSuper FileSystemType::getReadSuperFunction() const
{
  return read_super_;
}

void FileSystemType::setReadSuperFunction(FileSystemType::ReadSuper read_super)
{
  read_super_ = read_super;
}

int32 VirtualFileSystem::registerFileSystem(FileSystemType *file_system_type)
{
  if (file_system_type == 0)
  {
    return -1;
  }

  if (file_system_type->getFSName() == 0)
  {
    return -2;
  }

  uint32 fst_counter = 0;

  while ((fst_counter < MAX_FILE_SYSTEM_TYPES))
  {
    if (file_system_types_ + fst_counter)
    {
      if (strcmp(file_system_types_[fst_counter].getFSName(), file_system_type->getFSName()))
      {
        return -1;
      }
      ++fst_counter;
    }

    // TODO
    // make array dynamic
    // enter new entry in the rigth way
//    (file_system_types_ + fst_counter) = file_system_type;
  }

  return 0;
}


int32 VirtualFileSystem::unregisterFileSystem(FileSystemType *file_system_type)
{

  return 0;
}

