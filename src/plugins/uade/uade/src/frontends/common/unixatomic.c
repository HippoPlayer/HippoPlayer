#include <uade/unixatomic.h>
#include <uade/sysincludes.h>

#include <errno.h>
#include <stdint.h>
#include <assert.h>

#ifdef _WIN32
#include <Winsock2.h>
#define UREAD(fd, target, len) recv(fd, target, len, 0)
#define UWRITE(fd, target, len) send(fd, target, len, 0)
#define UCLOSE(fd) closesocket(fd)
#else
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define UREAD(fd, target, len) read(fd, target, len)
#define UWRITE(fd, target, len) write(fd, target, len)
#define UCLOSE(fd) close(fd)
#endif
int uade_atomic_close(int fd)
{
  while (1) {
    if (UCLOSE(fd) < 0) {
      if (errno == EINTR)
	continue;
      return -1;
    }
    break;
  }
  return 0;
}

int uade_atomic_read(int fd, const void *buf, size_t count)
{
  char *b = (char *) buf;
  int bytes_read = 0;
  int ret;
  while (bytes_read < count) {
    ret = (int)UREAD(fd, &b[bytes_read], count - bytes_read);
    if (ret < 0) {
      if (errno == EINTR)
        continue;
      if (errno == EAGAIN) {
	fd_set s;
	FD_ZERO(&s);
	FD_SET(fd, &s);
	if (select(fd + 1, &s, NULL, NULL, NULL) == 0)
	  fprintf(stderr, "atomic_read: very strange. infinite select() returned 0. report this!\n");
	continue;
      }
      return -1;
    } else if (ret == 0) {
      return 0;
    }
    bytes_read += ret;
  }
  return bytes_read;
}

int uade_atomic_write(int fd, const void *buf, size_t count)
{
  char *b = (char *) buf;
  int bytes_written = 0;
  int ret;
  while (bytes_written < count) {
    ret = (int)UWRITE(fd, &b[bytes_written], count - bytes_written);
    if (ret < 0) {
      if (errno == EINTR)
        continue;
      if (errno == EAGAIN) {
	fd_set s;
	FD_ZERO(&s);
	FD_SET(fd, &s);
	if (select(fd + 1, NULL, &s, NULL, NULL) == 0)
	  fprintf(stderr, "atomic_write: very strange. infinite select() returned 0. report this!\n");
	continue;
      }
      return -1;
    }
    bytes_written += ret;
  }
  return bytes_written;
}

