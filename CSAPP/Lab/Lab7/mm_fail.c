/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define WSIZE 4
#define DSIZE 8
#define ALIGNMENT 8
#define OVERHEAD 24
#define CHUNKSIZE (1 << 12)

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Pack a size and allocated into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(size_t *)(p))
#define PUT(p, val) (*(size_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define PREVP(bp) ((char *)(bp))
#define SUCCP(bp) ((char *)(bp) + DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
#define GET_PREV(bp) (*((char **)(PREVP(bp))))
#define GET_SUCC(bp) (*((char **)(SUCCP(bp))))
#define PUT_PREV(bp, val) (GET_PREV(bp) = val)
#define PUT_SUCC(bp, val) (GET_SUCC(bp) = val)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

static void *heap_listp;

static void f() {
    int a = rand();
    int b = a * 3 + 1; 
    printf("%d\n", b);
    return;
}

static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    char *prev, *succ;

    if (prev_alloc && next_alloc) {
        return bp;
    } else if (prev_alloc && !next_alloc) {
        // coalesce with the next
        prev = GET_PREV(NEXT_BLKP(bp));
        succ = GET_SUCC(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        PUT_SUCC(bp, succ);
        PUT_PREV(bp, prev);
        PUT_SUCC(prev, bp);
        if (succ) PUT_PREV(succ, bp);
        return bp;
    } else if (!prev_alloc && next_alloc) {
        // coalesce with the prev
        prev = GET_PREV(PREV_BLKP(bp));
        succ = GET_SUCC(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        return PREV_BLKP(bp);
    } else {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        // sort by address
        PUT_SUCC(PREV_BLKP(bp), GET_SUCC(NEXT_BLKP(bp)));
        if (GET_SUCC(NEXT_BLKP(bp)))
            PUT_PREV(GET_SUCC(NEXT_BLKP(bp)), PREV_BLKP(bp));
        return (PREV_BLKP(bp));
    }
}

static void *extend_heap(size_t words) {
    char *bp, *prev, *succ;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((int)(bp = mem_sbrk(size)) == -1) {
        return NULL;
    }

    PUT(HDRP(bp), PACK(size, 0)); // free block header
    PUT(FTRP(bp), PACK(size, 0)); // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1)); // new epilogue header
    /// @attention: coalesce first
    char *new_bp = coalesce((void *)bp);
    if (GET_SIZE(HDRP(new_bp)) != GET_SIZE(HDRP(bp))) return new_bp;

    // succ
    PUT_SUCC(bp, NULL);
    // prev
    prev = heap_listp;
    while ((succ = GET_SUCC(prev)) != NULL) {
        prev = succ;
    }
    if (prev != bp) {
        /// @attention: coalseced new-extended block doesn't have to update
        PUT_SUCC(prev, bp);
        PUT_PREV(bp, prev);
    }

    return bp;
}

static void *find_fit(size_t asize) {
    char *bp = GET_SUCC(heap_listp);
    if (bp == NULL) return NULL;
    size_t alloc, size;

    alloc = GET_ALLOC(HDRP(bp));
    size = GET_SIZE(HDRP(bp));
    while (!(size == 0 && alloc == 1)) {
        if (!alloc && (size == asize || size >= asize + OVERHEAD)) {
            return bp;
        }
        bp = GET_SUCC(bp);
        if (bp == NULL) return NULL;
        alloc = GET_ALLOC(HDRP(bp));
        size = GET_SIZE(HDRP(bp));
    }
    return NULL;
}

static void place(void *bp, size_t asize) {
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    if (size > asize) {
        PUT(HDRP(NEXT_BLKP(bp)), PACK(size - asize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size - asize, 0));
        PUT_PREV(NEXT_BLKP(bp), GET_PREV(bp));
        PUT_SUCC(NEXT_BLKP(bp), GET_SUCC(bp));

        PUT_SUCC(GET_PREV(bp), NEXT_BLKP(bp));
        if (GET_SUCC(bp) != NULL)
            PUT_PREV(GET_SUCC(bp), NEXT_BLKP(bp));
    } else {
        PUT_SUCC(GET_PREV(bp), GET_SUCC(bp));
        if (GET_SUCC(bp) != NULL)
            PUT_PREV(GET_SUCC(bp), GET_PREV(bp));
    }
    
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    char *bp;

    if ((heap_listp = mem_sbrk(4 * WSIZE + 2 * DSIZE)) == NULL) return -1;
    PUT(heap_listp, 0); // alignment padding
    PUT(heap_listp + WSIZE, PACK(OVERHEAD, 1)); // prologue header
    PUT(heap_listp + DSIZE * 3, PACK(OVERHEAD, 1)); // prologue footer
    PUT(heap_listp + WSIZE + DSIZE * 3, PACK(0, 1)); // epilogue header
    heap_listp += DSIZE;
    PUT_PREV(heap_listp, NULL);
    PUT_SUCC(heap_listp, NULL);

    if ((bp = extend_heap(CHUNKSIZE / WSIZE)) == NULL)
        return -1;

    PUT_SUCC(heap_listp, bp);
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize; // adjusted block size
    size_t extendsize; // amount to extend heap if no fit
    char *bp;

    if (size <= 0) return NULL;

    if (size <= DSIZE)
        asize = DSIZE + OVERHEAD;
    else
        asize = DSIZE * ((size + OVERHEAD + DSIZE - 1) / DSIZE);
    
    // Search the free list for a fit
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        // if (GET_ALLOC(HDRP(GET_SUCC(heap_listp)))) {
        //     f();
        //     printf("error! bp = %x\n", bp);
        // }
        return bp + 2 * DSIZE;
    }

    // No fit found. Get more memory and place the block
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    /// @atention: after extension, we should `find_fit' again
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        // if (GET_ALLOC(HDRP(GET_SUCC(heap_listp)))) {
        //     printf("error! bp = %x\n", bp);
        // }
        return bp + 2 * DSIZE;
    }
    return NULL;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    /// @attention: ptr must be subtracted by 2 * DSIZE
    ptr = ptr - 2 * DSIZE;

    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    
    // coalesce first
    char *new_ptr = coalesce(ptr);

    if (GET_SIZE(HDRP(new_ptr)) != GET_SIZE(HDRP(ptr))) return;

    /// @attention: sort by address
    char *prev = heap_listp;
    char *succ = GET_SUCC(heap_listp);
    while (succ != NULL) {
        if (prev < (char *)ptr && succ > (char *)ptr) {
            PUT_SUCC(prev, ptr);
            PUT_PREV(succ, ptr);
            PUT_PREV(ptr, prev);
            PUT_SUCC(ptr, succ);
            break;
        }
        prev = succ;
        succ = GET_SUCC(prev);
    }
    if (succ == NULL && prev != ptr) {
        PUT_SUCC(prev, ptr);
        PUT_PREV(ptr, prev);
        PUT_SUCC(ptr, NULL);
    }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = GET_SIZE(HDRP(oldptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}