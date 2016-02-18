#include <dirent.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <bits/wordsize.h>

#include <sysdep.h>
#include <sys/syscall.h>
#include <linux/posix_types.h>

struct kernel_dirent64
  {
    uint64_t		d_ino;
    int64_t		d_off;
    unsigned short int	d_reclen;
    unsigned char	d_type;
    char		d_name[256];
  };

static size_t conv (char *buf, size_t nbytes)
{
	char *end = buf + nbytes;
	struct dirent *u = (struct dirent *) buf;
	struct kernel_dirent64 *k = (struct kernel_dirent64 *) buf;
	size_t sizd = offsetof(struct kernel_dirent64, d_name)
		  - offsetof(struct dirent, d_name);

	while ((char*) k < end)
	{
	  struct kernel_dirent64 *nk = (char *) k + k->d_reclen;
	  size_t name_len = k->d_reclen - offsetof(struct kernel_dirent64, d_name);

	  u->d_ino = k->d_ino;
	  u->d_off = k->d_off;
	  u->d_reclen = k->d_reclen - sizd;
	  u->d_type = k->d_type;
	  memmove (u->d_name, k->d_name, name_len);

	  u = (char *) u + u->d_reclen;
	  k = nk;
	}

	return (size_t) u - (size_t) buf;
}

ssize_t
__getdents64 (int fd, char *buf, size_t nbytes)
{

  struct kernel_dirent64 *k;
  int ret;

  ret = INLINE_SYSCALL (getdents64, 3, fd, buf, nbytes);
  if (ret == -1)
	  return ret;

  return conv(buf, ret);
}

strong_alias (__getdents64, __getdents)
