#ifndef __CACHE_H__
#define __CACHE_H__

#include <pthread.h>

struct line {
    char *uri;
    int size;
    void *buf;
    struct line *next;
};

typedef struct line Line;

typedef struct {
    void *buf;
    int size;
} Result;

typedef struct {
    int total_size;
    pthread_rwlock_t rwlock;
    Line *head;
} Cache;

Cache *cache_init(int size);
Result *visit_line(Cache *cache, char *uri);
void add_line(Cache *cache, void *buf, int size, char *uri);
void remove_line(Cache *cache);

#endif