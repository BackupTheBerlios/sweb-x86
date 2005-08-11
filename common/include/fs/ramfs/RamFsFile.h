// Projectname: SWEB
// Simple operating system for educational purposes
//
// Copyright (C) 2004 Maria Mauerhofer
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
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * CVS Log Info for $RCSfile: RamFsFile.h,v $
 *
 * $Id: RamFsFile.h,v 1.3 2005/08/11 16:35:59 qiangchen Exp $
 * $Log: RamFsFile.h,v $
 * Revision 1.2  2005/08/02 18:27:15  lythien
 * write RamFsFile
 *
 */


#ifndef RamFsFile_h___
#define RamFsFile_h___

#include "fs/File.h"


/**
 * class RamFsFile provides
 *
 */
class RamFsFile: public File
{
 public:


  /**
   * default constructor for class RamFsFile
   */
  RamFsFile(Inode* inode, Dentry* dentry);


  /**
   * destructor for class RamFsFile
   */
  virtual ~RamFsFile();

  //----------------------------------------------------------------------
  /// Getter method for the filename.
  ///
  /// @return is the file's name
  virtual char *getName() const;

  //----------------------------------------------------------------------
  /// Getter Method for the dentry.
  ///
  /// @return is the dentry associated to the File.
  virtual Dentry *getDentry() const;

  //----------------------------------------------------------------------
  /// Sets the file position relative to the start of the file, the  end of 
  /// the file or the current file position.
  ///
  /// @param offset is the offset to set.
  /// @param origin is the on off SEEK_SET, SEEK_CUR and SEEK_END.
  /// @returns the offset from the start off the file or -1 on failure.
  // l_off_t llSeek(l_off_t offset, uint8 origin)

  //----------------------------------------------------------------------
  /// reads from the file
  ///
  /// @param buffer is the buffer where the data is written to
  /// @param count is the number of bytes to read.
  /// @param offset is the offset to read from counted from the start of the file.
  virtual int32 read(int32 *buffer, size_t count, l_off_t offset);

  //----------------------------------------------------------------------
  /// write to the file
  ///
  /// @param buffer is the buffer where the data is read from
  /// @param count is the number of bytes to write.
  /// @param offset is the offset to write from counted from the start of the file.
  virtual int32 write(int32 *buffer, size_t count, l_off_t offset);

  //----------------------------------------------------------------------
  /// Open the file
  ///
  /// @param inode is the inode the read the file from.
  virtual int32 open(uint32 flag);

  //-----------------------------------------------------------------------
  /// Close the file
  ///
  /// @param inode is close, the superblock has the information, that this
  /// inode is not use anymore.
  virtual int32 close();

  //----------------------------------------------------------------------
  /// Flush all off the file's write operations. The File will be written to disk.
  /// 
  /// @return is the error code of the flush operation.
  virtual int32 flush();
};


#endif // RamFsFile_h___


