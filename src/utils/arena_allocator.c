/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include "arena_allocator.h"

double *arena_alloc(Arena *a, size_t n) {
    if (a->used + n > a->cap) return NULL;
    double *p = a->buf + a->used;
    a->used += n;
    return p;
}

void arena_reset(Arena *a) {
    a->used = 0;
}

size_t arena_mark(Arena *a) {
    return a->used;
}

void arena_restore(Arena *a, size_t mark) {
    a->used = mark;
}
