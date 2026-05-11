/**
 * @brief Minimal syscalls stubs for newlib-nano.
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

int __io_putchar(int ch) __attribute__((weak));
int __io_getchar(void) __attribute__((weak));

int _write(int file, char *ptr, int len) {
	(void) file;
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		__io_putchar(*ptr++);
	}
	return len;
}

int _read(int file, char *ptr, int len) {
	(void) file;
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		*ptr++ = (char) __io_getchar();
	}
	return len;
}

int _close(int file) {
	(void) file;
	return -1;
}

int _fstat(int file, struct stat *st) {
	(void) file;
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file) {
	(void) file;
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	(void) file;
	(void) ptr;
	(void) dir;
	return 0;
}

int _getpid(void) {
	return 1;
}

int _kill(int pid, int sig) {
	(void) pid;
	(void) sig;
	errno = EINVAL;
	return -1;
}

void _exit(int status) {
	(void) status;
	while (1) {}
}
