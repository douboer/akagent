#include "ring.h"

unsigned int __akfs_ring_get(akfs_ring_t *ring ,unsigned char *data ,unsigned int len)
{
    unsigned int l;
    unsigned char *buffer = (unsigned char *)ring + ring->offset;

    len = min(len, ring->in - ring->out);

    l = min(len ,ring->size - (ring->out & (ring->size - 1)));
    printf("__akfs_ring_get l %u\n" ,l);
    memcpy(data ,buffer + (ring->out & (ring->size - 1)) ,l);

    memcpy(data + l ,buffer,len - l);

    ring->out += len;

    return len;
}
