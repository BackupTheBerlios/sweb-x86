// Projectname: SWEB
// Simple operating system for educational purposes

#ifndef FileSystemType_h___
#define FileSystemType_h___

#include "types.h"

class Superblock;
class Dentry;

/// File system flag indicating if the system in question requires an device.
#define FS_REQUIRES_DEV   0x0001 // located on a physical disk device
#define FS_NOMOUNT        0x0010 // Filesystem has no mount point

/// The maximal number of file system types.
#define MAX_FILE_SYSTEM_TYPES 16

/**
 * FileSystemType
 * 
 * FileSystemType is used to register the file system to the vfs.
 * It also reads the superblock from the block device.
 *
 */
class FileSystemType
{

  protected:

    char* fs_name_;

    int32 fs_flags_;

  public:

    FileSystemType();

    virtual ~FileSystemType();

    FileSystemType const &operator =(FileSystemType const &instance)
    {
      fs_name_ = instance.fs_name_;
      fs_flags_ = instance.fs_flags_;
      return (*this);
    }

    const char* getFSName() const;

    void setFSName(const char* fs_name);

    int32 getFSFlags() const;

    void setFSFlags(int32 fs_flags);

    /// Reads the superblock from the device.
    ///
    /// @return is a pointer to the resulting superblock.
    /// @param superblock is the superblock to fill with data.
    /// @param data is the data given to the mount system call.
    virtual Superblock *readSuper(Superblock *superblock, void *data) const;

    /// Creates an Superblock object for the actual file system type.
    ///
    /// @return a pointer to the Superblock object
    virtual Superblock *createSuper(Dentry *root) const;

};

#endif // FileSystemType_h___


