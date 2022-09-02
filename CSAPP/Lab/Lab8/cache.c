#include "cache.h"
#include "csapp.h"

Cache *cache_init(int size) {
    Cache *cache = (Cache*)Malloc(sizeof(Cache));
    cache->total_size = 0;
    cache->head = (Line*)Malloc(sizeof(Line));
    cache->head->uri = NULL;
    cache->head->buf = NULL;
    cache->head->size = size;
    cache->head->next = NULL;
    pthread_rwlock_init(&cache->rwlock, NULL);
    return cache;
}

Result *visit_line(Cache *cache, char *uri) {
    pthread_rwlock_wrlock(&cache->rwlock);
    Line *ptr = cache->head->next, *prev = cache->head;
    while (ptr) {
        if (!strcmp(ptr->uri, uri)) {
            // pthread_rwlock_unlock(&cache->rwlock);
            // pthread_rwlock_wrlock(&cache->rwlock);
            prev->next = ptr->next;
            ptr->next = cache->head->next;
            cache->head->next = ptr;
            Result *res = (Result*)Malloc(sizeof(Result));
            res->buf = Malloc(ptr->size);
            res->size = ptr->size;
            memcpy(res->buf, ptr->buf, ptr->size);
            pthread_rwlock_unlock(&cache->rwlock);
            return res;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    pthread_rwlock_unlock(&cache->rwlock);
    return NULL;
}

void add_line(Cache *cache, void *buf, int size, char *uri) {
    pthread_rwlock_wrlock(&cache->rwlock);
    Line *line = (Line*)Malloc(sizeof(Line));
    line->buf = Malloc(size);
    memcpy(line->buf, buf, size);
    line->size = size;
    line->uri = Malloc(strlen(uri));
    strcpy(line->uri, uri);
    line->next = cache->head->next;
    cache->head->next = line;
    cache->total_size += size;
    pthread_rwlock_unlock(&cache->rwlock);
}

void remove_line(Cache *cache) {
    pthread_rwlock_wrlock(&cache->rwlock);
    Line *line = cache->head->next, *prev = cache->head;
    if (line == NULL) {
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }
    while (line->next) {
        prev = line;
        line = line->next;
    }
    prev->next = line->next;
    cache->total_size -= line->size;
    Free(line->uri);
    Free(line->buf);
    Free(line);
    pthread_rwlock_unlock(&cache->rwlock);
}

Cache *cache_deinit(Cache *cache) {
    Line *ptr = cache->head->next, *prev = cache->head;
    while (ptr) {
        prev = ptr;
        ptr = ptr->next;
        Free(prev->uri);
        Free(prev->buf);
        Free(prev);
    }
    pthread_rwlock_destroy(&cache->rwlock);
    Free(cache);
}