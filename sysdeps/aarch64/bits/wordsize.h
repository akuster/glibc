/* Copyright (C) 2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifdef __LP64__
# define __WORDSIZE	64
#else
# define __WORDSIZE	32
#endif

/* LP64 and ILP32s ABI uses a 64bit time_t.
   This allows aarch32 and AARCH64 applications
   both access utmp. */
#define __WORDSIZE_TIME64_COMPAT32	1

/* LP64 and ILP32 use the 64bit system call interface. */
#define __SYSCALL_WORDSIZE 64
