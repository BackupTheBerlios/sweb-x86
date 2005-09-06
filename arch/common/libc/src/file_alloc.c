
// Projectname: SWEB
// Simple operating system for educational purposes
//
// Copyright (C) 2005  Andreas Niederl
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
 * CVS Log Info for $RCSfile: file_alloc.c,v $
 *
 * $Id: file_alloc.c,v 1.1 2005/09/06 17:19:25 aniederl Exp $
 * $Log$
 */


#include <unistd.h>
#include <sys/syscall.h>


//----------------------------------------------------------------------
/**
 * Creates a new hard link to an existing file.
 * If new_path exists it will not be overwritten.
 *
 * @return 0 on success, -1 otherwise and errno is set appropriately
 *
 */
//int link(const char *old_path, const char *new_path)
__syscall_2(int, link, const char *, old_path, const char *, new_path)


//----------------------------------------------------------------------
/**
 * Deletes a name from the filesystem.
 * If that name was the last link to a file and no processes have the file
 * open the file is deleted.
 * If a process has the file opened when the last link to it is deleted, the
 * file will be deleted after the last file descriptor referring to it is
 * closed.
 *
 * @param path the pathname to delete from the filesystem
 * @return 0 on success, -1 otherwise and errno is set appropriately
 *
 */
//int unlink(const char *path)
__syscall_1(int, unlink, const char *, path)


//----------------------------------------------------------------------
/**
 * Closes the given file descriptor, so that it no longer refers to any file
 * and may be reused.
 * Any locks held on the file it was associated with, and owned by the process,
 * are removed.
 * If file_descriptor is the last copy of a particular file descriptor the
 * the resources associated with it are freed; if the descriptor was the last
 * reference to a file which has been removed using unlink() the file is
 * deleted.
 *
 * @param file_descriptor the file descriptor which should be closed
 * @return 0 on success, -1 otherwise and errno is set appropriately
 *
 */
__syscall_1(int, close, int, file_descriptor)


//----------------------------------------------------------------------
/**
 * Defines the open syscall which will be used in the open function.
 *
 */
__syscall_3(int, __syscall_open, const char *, path, mode_t, mode)


//----------------------------------------------------------------------
/**
 * Converts a pathname into a file descriptor which can be used in subsequent
 * read and write operations.
 * If successfull, the lowest file descriptor not currently open for the
 * process will be returned.
 *
 * Possible values for the flags parameter are:
 *  - O_RDONLY  Open for reading only
 *  - O_WRONLY  Open for writing only
 *  - O_RDWR    Open for reading and writing
 *
 * which can be bitwise combined (or'd) with:
 *  - O_APPEND  Open in append mode
 *  - O_CREAT   Create file if it does not exist
 *  - O_TRUNC   Truncate file to 0 length if it already exists and is a
 *              regular file
 *
 * The mode argument specifies the permissions to use in case a new file is
 * created. The effective permissions are (mode & ~umask).
 *
 * @param path A pathname pointing to the file to open
 * @param flags Flags to specify how the file is opened
 * @param mode Access permissions on file creation, ignored otherwise
 * @return A valid file descriptor or -1 if an error occured
 *
 */
int open(const char *path, int flags, ...)
{
  // taken from the uClibc open()
  mode_t mode;

  if(flags & O_CREAT)
  {
    va_list ap;

    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  return __syscall_open(file, flags, mode);
}


//----------------------------------------------------------------------
/**
 * Creates a pipe.
 * A pair of file descriptors pointing to a pipe inode is created and placed
 * in an array pointed to by the given array parameter.
 * The first element in the array is for reading and the second for writing.
 *
 * @param file_descriptor_array An array into which the two file descriptors\
 for the new pipe will be written
 * @return 0 on success, -1 otherwise and errno is set appropriately
 *
 */
__syscall_1(int, pipe, int, file_descriptor_array[2])


//----------------------------------------------------------------------
/**
 * Creates a copy of the given file descriptor using the lowest-numbered
 * unused descriptor.
 * The copy shares locks, file position pointers and flags with the original
 * file descriptor but not the close-on-exec flag.
 *
 * @param file_descriptor the descriptor to copy
 * @return the new descriptor or -1 if an error occured (errno is as usually\
 set appropriately)
 *
 */
__syscall_1(int, dup, int, file_descriptor)


//----------------------------------------------------------------------
/**
 * Creates a copy of the given file descriptor using the given new descriptor
 * and closing it first if necessary.
 * The copy shares locks, file position pointers and flags with the original
 * file descriptor but not the close-on-exec flag.
 *
 * @param old_file_descriptor the descriptor to copy
 * @param new_file_descriptor the descriptor which should be the copy
 * @return the new descriptor or -1 if an error occured (errno is as usually\
 set appropriately)
 *
 */
__syscall_2(int, dup2, int, old_file_descriptor, int, new_file_descriptor)


//----------------------------------------------------------------------
/**
 * Renames a file, moving it between directories if required.
 *
 * @param old_path The old pathname
 * @param new_path The new pathname
 * @return 0 on success, -1 otherwise and errno is set appropriately
 *
 */
__syscall_2(int, rename, const char *, old_path, const char *, new_path)
