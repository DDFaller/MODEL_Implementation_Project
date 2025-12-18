#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stddef.h>

typedef struct {
    double *buf;
    size_t cap;
    size_t used;
} Arena;

double *arena_alloc(Arena *a, size_t n);
void arena_reset(Arena *a);
size_t arena_mark(Arena *a);
void arena_restore(Arena *a, size_t mark);

#endif // ARENA_ALLOCATOR_H