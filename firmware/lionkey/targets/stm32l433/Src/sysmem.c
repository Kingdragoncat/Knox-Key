/**
 * @brief _sbrk implementation for newlib heap management.
 */

#include <errno.h>

extern int end; /* Set by linker */

void *_sbrk(int incr) {
	static unsigned char *heap_end;
	unsigned char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = (unsigned char *) &end;
	}
	prev_heap_end = heap_end;

	heap_end += incr;

	return (void *) prev_heap_end;
}
