/* Basic tests for SYSV shared memory functions.
   Copyright (C) 2016 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

static void do_prepare (void);
#define PREPARE(argc, argv)	do_prepare ()
static int do_test (void);
#define TEST_FUNCTION           do_test ()

/* This defines the `main' function and some more.  */
#include <test-skeleton.c>

/* These are for the temporary file we generate.  */
static char *name;
static int shmid;

static void
remove_shm (void)
{
  /* Enforce message queue removal in case o early test failure.  
     Ignore error since the shm maybe already being removed.  */
  shmctl (shmid, IPC_RMID, 0);
}

static void
do_prepare (void)
{
  int fd = create_temp_file ("tst-sysvshm.", &name);
  if (fd == -1)
    FAIL_EXIT1 ("cannot create temporary file (errno=%d)", errno);
}

#define CHECK_EQ(v, k) \
  if ((v) != (k)) \
    FAIL_EXIT1("%d != %d", v, k)

/* It is not an extensive test, but rather a functional one aimed to check
   correct parameter passing on kernel.  */

#define SHM_MODE 0666

static int
do_test (void)
{
  atexit (remove_shm);

  key_t key = ftok (name, 'G');
  if (key == -1)
    FAIL_EXIT1 ("ftok failed");

  long int pgsz = sysconf (_SC_PAGESIZE);
  if (pgsz == -1)
    FAIL_EXIT1 ("sysconf (_SC_PAGESIZE) failed (errno = %d)", errno);

  shmid = shmget(key, pgsz, IPC_CREAT | IPC_EXCL | SHM_MODE);
  if (shmid == -1)
    {
      if (errno == ENOSYS)
	FAIL_UNSUPPORTED ("shmget not supported");
      FAIL_EXIT1 ("shmget failed (errno=%d)", errno);
    }

  /* Get shared memory kernel information and do some sanitiy checks.  */
  struct shmid_ds shminfo;
  if (shmctl (shmid, IPC_STAT, &shminfo) == -1)
    FAIL_EXIT1 ("shmctl with IPC_STAT failed (errno=%d)", errno);

  if (shminfo.shm_perm.__key != key)
    FAIL_EXIT1 ("shmid_ds::shm_perm::key (%d) != %d",
		(int) shminfo.shm_perm.__key, (int) key);
  if (shminfo.shm_perm.mode != SHM_MODE)
    FAIL_EXIT1 ("shmid_ds::shm_perm::mode (%o) != %o",
		shminfo.shm_perm.mode, SHM_MODE);
  if (shminfo.shm_segsz != pgsz)
    FAIL_EXIT1 ("shmid_ds::shm_segsz (%lu) != %lu",
		(long unsigned) shminfo.shm_segsz, pgsz);

  /* Attach on shared memory and realize some operations.  */
  int *shmem = shmat (shmid, NULL, 0);
  if (shmem == (void*) -1)
    FAIL_EXIT1 ("shmem failed (errno=%d)", errno);

  shmem[0]   = 0x55555555;
  shmem[32]  = 0x44444444;
  shmem[64]  = 0x33333333;
  shmem[128] = 0x22222222;
  
  if (shmdt (shmem) == -1)
    FAIL_EXIT1 ("shmem failed (errno=%d)", errno);

  shmem = shmat (shmid, NULL, SHM_RDONLY);
  if (shmem == (void*) -1)
    FAIL_EXIT1 ("shmem failed (errno=%d)", errno);

  CHECK_EQ (shmem[0],   0x55555555);
  CHECK_EQ (shmem[32],  0x44444444);
  CHECK_EQ (shmem[64],  0x33333333);
  CHECK_EQ (shmem[128], 0x22222222);
  
  if (shmdt (shmem) == -1)
    FAIL_EXIT1 ("shmem failed (errno=%d)", errno);

  /* Finally free up the semnaphore resource.  */
  if (shmctl (shmid, IPC_RMID, 0) == -1)
    FAIL_EXIT1 ("semctl failed (errno=%d)", errno);

  return 0;
}