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

#ifndef Lock_h___
#define Lock_h___

/**
 * Lock
 *
 */
class Lock
{
 protected:

  int lock_;

  int babble_;

  char *owner_;

  int oline_;

 public:

  Lock();

  virtual ~Lock() {}
};


#endif // Lock_h___


