#ifndef __ALWAYSKNOW_USERSPACE_RING_H__
#define __ALWAYSKNOW_USERSPACE_RING_H__

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct akfs_ring_s{
    unsigned int offset;
    unsigned int size;
    unsigned int in;
    unsigned int out;
}akfs_ring_t;

#define min(x, y) ({                        \
        typeof(x) _min1 = (x);          \
        typeof(y) _min2 = (y);          \
        (void) (&_min1 == &_min2);      \
        _min1 < _min2 ? _min1 : _min2; })

unsigned int __akfs_ring_get(akfs_ring_t *ring ,unsigned char *data ,unsigned int len);

#endif
