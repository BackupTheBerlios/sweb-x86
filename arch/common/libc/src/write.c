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
 * CVS Log Info for $RCSfile: write.c,v $
 *
 * $Id: write.c,v 1.1 2005/09/06 17:19:25 aniederl Exp $
 * $Log$
 */


#include <unistd.h>


//----------------------------------------------------------------------
/**
 * Writes to a file descriptor.
 * Up to count bytes from the provided buffer are written to the given file.
 *
 * If the given file is capable of seeking, the write will start at the file
 * position associated with the descriptor. This offset is incremented by
 * the number of bytes actually written.
 *
 * @param file_descriptor file descriptor referencing the file to write
 * @param buffer the buffer where the write data will be placed
 * @param count the number of bytes to write
 * @param offset the absolute offset where the write operation starts
 * @return the number of bytes written on success, 0 if count is zero or\
 nothing was written, and -1 if an error occured
 *
 */
__syscall_3(ssize_t, write, int, file_descriptor, const void *, buffer,
            size_t, count)
