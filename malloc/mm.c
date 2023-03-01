#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.
 * When you hand in, remove the #define DEBUG line. */
//#define DEBUG
#ifdef DEBUG
#define debug(fmt, ...) printf("%s: " fmt "\n", __func__, __VA_ARGS__)
#define msg(...) printf(__VA_ARGS__)
#else
#define debug(fmt, ...)
#define msg(...)
#endif

#define __unused __attribute__((unused))

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* !DRIVER */

typedef int32_t word_t; /* Heap is bascially an array of 4-byte words. */

typedef enum {
  FREE = 0,     /* Block is free */
  USED = 1,     /* Block is used */
  PREVFREE = 2, /* Previous block is free (optimized boundary tags) */
} bt_flags;

#define ALIGNMENT 16
#define ALIGN(size)                                                            \
  (((size) + (ALIGNMENT - 1)) & ~0xF) /* Aligns size to 16                     \
                                       */
#define MAX_SEGRET_LIST 12            /* Maximum number of segretation list */
#define WSIZE 4                       /* Word and header/footer size (bytes) */

static word_t *heap_start; /* Address of the first block */
static word_t *heap_end;   /* Address past last byte of last block */
static word_t *last;       /* Points at last block */

/* Segratation list */

static word_t *segret_lists[MAX_SEGRET_LIST];
#define INDEX_0_MAX_SIZE 32   /* Maximum size of the first segretation list */
#define INDEX_1_MAX_SIZE 64   /* Maximum size of the second segretation list */
#define INDEX_2_MAX_SIZE 128  /* Maximum size of the third segretation list */
#define INDEX_3_MAX_SIZE 256  /* Maximum size of the fourth segretation list */
#define INDEX_4_MAX_SIZE 512  /* Maximum size of the fifth segretation list */
#define INDEX_5_MAX_SIZE 1024 /* Maximum size of the sixth segretation list */
#define INDEX_6_MAX_SIZE                                                       \
  2048 /* Maximum size of the seventh segretation list                         \
        */
#define INDEX_7_MAX_SIZE                                                       \
  4096                        /* Maximum size of the eighth segretation list   \
                               */
#define INDEX_8_MAX_SIZE 8192 /* Maximum size of the ninth segretation list */
#define INDEX_9_MAX_SIZE                                                       \
  16384 /* Maximum size of the tenth segretation list                          \
         */
#define INDEX_10_MAX_SIZE                                                      \
  32768 /* Maximum size of the eleventh segretation list */

/* Allocated block */

/* ------------------------------------------------------------------------------------*/
/*|             |                                                        | |*/
/*|   HEADER    |                       PAYLOAD                          |
 * FOOTER    |*/
/*|    (4)      |                                                        | (4)
 * |*/
/*|             |                                                        | |*/
/* ------------------------------------------------------------------------------------*/

/* Free block */

/* ------------------------------------------------------------------------------------*/
/*|             |                           |                             | |*/
/*|             |     SHORTER_POINTER       |       SHORTER_POINTER       | |*/
/*|   HEADER    |        NEXT_FREE          |          PREV_FREE          |
 * FOOTER    |*/
/*|    (4)      |          (4)              |              (4)            | (4)
 * |*/
/*|             |                           |                             | |*/
/* ------------------------------------------------------------------------------------*/

/* declaration of functions */
static void coalesce(word_t *bt);

/* --=[ boundary tag handling ]=-------------------------------------------- */

static inline word_t bt_size(word_t *bt) {
  return *bt & ~(USED | PREVFREE);
}

static inline int bt_used(word_t *bt) {
  return *bt & USED;
}

static inline int bt_free(word_t *bt) {
  return !(*bt & USED);
}

/* Given boundary tag address calculate it's buddy address. */
static inline word_t *bt_footer(word_t *bt) {
  return (void *)bt + bt_size(bt) - WSIZE;
}

/* Given payload pointer returns an address of boundary tag. */
static inline word_t *bt_fromptr(void *ptr) {
  return (word_t *)ptr - 1;
}

/* Creates boundary tag(s) for given block. */
static inline void bt_make(word_t *bt, size_t size, bt_flags flags) {
  *bt = size | flags;
  *bt_footer(bt) = size | flags;
}

/* Previous block free flag handling for optimized boundary tags. */
static inline bt_flags bt_get_prevfree(word_t *bt) {
  return *bt & PREVFREE;
}

static inline void bt_clr_prevfree(word_t *bt) {
  if (bt)
    *bt &= ~PREVFREE;
}

static inline void bt_set_prevfree(word_t *bt) {
  *bt |= PREVFREE;
}

/* Returns address of payload. */
static inline void *bt_payload(word_t *bt) {
  return bt + 1;
}

/* Returns address of next block or NULL. */
static inline word_t *bt_next(word_t *bt) {
  if (bt == last)
    return NULL;
  word_t *next = ((void *)bt + bt_size(bt));
  if (next > last)
    return NULL;
  return next;
}

/* Returns address of previous block or NULL. */
static inline word_t *bt_prev(word_t *bt) {
  word_t *prev = (void *)bt - bt_size(bt - 1);
  if (prev < heap_start)
    return NULL;
  return prev;
}

/* --=[ segretefree list handling
 * ]=----------------------------------------------- */

static inline unsigned int shorter_pointer(word_t *block) { /* 8 -> 4 */
  return (unsigned long long)block -
         (unsigned long long)heap_start; /* return shorter version of pointer */
}

static inline word_t *longer_pointer(unsigned int block) { /* 4 - > 8 */
  return (word_t *)(block +
                    (unsigned long long)
                      heap_start); /* return longer version of pointer */
}

static inline word_t *get_next(word_t *block) { /* get next block */
  if (*(block + 1) == 0)                        /* if next block is null */
    return NULL;
  return (longer_pointer(*(block + 1)));
}
static word_t *get_prev(word_t *block) { /* get previous block */
  if (*(block + 2) == 0)                 /* if previous block is null */
    return NULL;
  return (longer_pointer(*(block + 2)));
}

static inline int get_index(size_t size) { /* get index of segretation list */
  if (size <= INDEX_0_MAX_SIZE)
    return 0;
  if (size <= INDEX_1_MAX_SIZE)
    return 1;
  if (size <= INDEX_2_MAX_SIZE)
    return 2;
  if (size <= INDEX_3_MAX_SIZE)
    return 3;
  if (size <= INDEX_4_MAX_SIZE)
    return 4;
  if (size <= INDEX_5_MAX_SIZE)
    return 5;
  if (size <= INDEX_6_MAX_SIZE)
    return 6;
  if (size <= INDEX_7_MAX_SIZE)
    return 7;
  if (size <= INDEX_8_MAX_SIZE)
    return 8;
  if (size <= INDEX_9_MAX_SIZE)
    return 9;
  if (size <= INDEX_10_MAX_SIZE)
    return 10;
  else
    return 11;
  return -1;
}

static inline void
segret_list_add(word_t *block) { /* add block to segretation list */
  int size = bt_size(block);
  int index = get_index(size);
  if (segret_lists[index] == NULL) { /* if segretation list is empty */
    segret_lists[index] = block;     /* Our block is now 'head' of list */
    (*((block + 1))) = 0;            /* head->next = NULL */
    (*((block + 2))) = 0;            /* head->prev = NULL */
    return;
  }
  (*((block + 2))) = 0; /* block->prev = NULL*/
  (*((block + 1))) =
    shorter_pointer(segret_lists[index]); /* block->next = head of list */
  (*((segret_lists[index] + 2))) =
    shorter_pointer(block);    /* head->prev = our block */
  segret_lists[index] = block; /* list 'head' = block*/
}

static inline void
segret_list_remove(word_t *block) { /* remove block from segretation list */
  int size = bt_size(block);
  int index = get_index(size);

  word_t *next = get_next(block);
  if (next &&
      segret_lists[index] ==
        block) { /* if next block is not null and our block is head of list */
    (*((next + 2))) = 0;        /* next->prev = NULL */
    segret_lists[index] = next; /* list 'head' = next block */
    return;
  } else if (segret_lists[index] ==
             block) { /* if our block is head of list and next block is null */
    segret_lists[index] = NULL; /* empty list */
    return;
  }
  /* Otherwise */
  word_t *prev = get_prev(block);
  if (!next) {           /* if block is last in free list */
    (*((prev + 1))) = 0; /* prev->next = NULL */
    return;
  }
  (*((prev + 1))) = shorter_pointer(next); /* prev->next = next */
  (*((next + 2))) = shorter_pointer(prev); /* next->prev = prev */
}

/* Find fit in list*/
static word_t *find_block(size_t size) {
  /* First fit */
  int index = get_index(size);
  for (int i = index; i < MAX_SEGRET_LIST;
       i++) {                    /* for each segretation list >= size */
    if (segret_lists[i] == NULL) /* if list is empty */
      continue;
    word_t *block = segret_lists[i]; /* get head of list */
    while (block != NULL) {
      if (bt_size(block) >= size) { /* if block is big enough */
        segret_list_remove(block);  /* remove block from list */
        return block;               /* return block */
      }
      block = get_next(block); /* get next block */
    }
  }
  return NULL; /* no fit found */
}

/* --=[ free_list_handlig ]=-------------------------------------------- */

/* --=[ miscellanous procedures ]=------------------------------------------ */

/* Calculates block size incl. header, footer & payload,
 * and aligns it to block boundary (ALIGNMENT). */
static inline size_t blksz(size_t size) {
  return ALIGN(WSIZE + size + WSIZE); /* align size to ALIGNMENT */
}

static void *morecore(size_t size) {
  void *ptr = mem_sbrk(size);
  if (ptr == (void *)-1)
    return NULL;
  heap_end += size;
  return ptr;
}

int mm_init(void) {
  void *ptr = morecore(ALIGNMENT - WSIZE);
  if (!ptr)
    return -1;
  heap_start = ptr;                           /* init heap start */
  heap_end = ptr + ALIGNMENT - WSIZE;         /* init heap end */
  last = ptr;                                 /* init last block */
  for (int i = 0; i < MAX_SEGRET_LIST; i++) { /* init segretation lists */
    segret_lists[i] = NULL;
  }

  return 0;
}

/* spliting */
static void split(word_t *block,
                  size_t size) { /* split block into two blocks */
  size_t block_size = bt_size(block);
  if (block_size - size < ALIGNMENT ||
      size < ALIGNMENT) {             /* if block is too small to split */
    bt_make(block, block_size, USED); /* don't split*/
    return;
  }

  word_t *new_block = (void *)block + size; /* new block starts after block */
  bt_make(new_block, block_size - size, FREE);
  bt_make(block, size, USED);
  if (last < new_block) { /* if new block is after last block */
    last = new_block;     /* update last block */
    coalesce(last);       /* coalesce last block */
  } else {
    coalesce(new_block); /* coalesce new block */
  }
}

void *malloc(size_t size) {
  if (size == 0)
    return NULL;
  size_t asize = blksz(size);        /* align size to block boundary */
  word_t *block = find_block(asize); /* find block */
  if (!block) {                      /* if no block found */
    block = morecore(asize);         /* get more memory */
    last = block;                    /* update last block */
    bt_make(block, asize, USED);
    if (asize <= 512) { /* if block is small enough */
      word_t *new_free;
      for (int i = 0; i < 10; i++) { /* create 10 new free blocks */
        new_free = morecore(asize);
        bt_make(new_free, asize, FREE);
        segret_list_add(new_free);
      }
      last = new_free;
    }
  } else {                      /* if block found */
    if (bt_size(block) > asize) /* if block is big enough to split */
      split(block, asize);
    else
      bt_make(block, asize, USED);
  }
  return bt_payload(block);
}

/* Coalesce */
static void
coalesce(word_t *block) { /* coalesce block with next and/or previous block */
  word_t *next = bt_next(block);
  word_t *prev = bt_prev(block);
  if (next && prev && bt_free(next) &&
      bt_free(prev)) { /* if next and previous blocks are free */
    segret_list_remove(next);
    segret_list_remove(prev);
    bt_make(prev, bt_size(prev) + bt_size(block) + bt_size(next),
            FREE);                    /* coalesce all three blocks */
    segret_list_add(prev);            /* add coalesced block to list */
  } else if (next && bt_free(next)) { /* if next block is free */
    segret_list_remove(next);
    bt_make(block, bt_size(block) + bt_size(next), FREE);
    segret_list_add(block);           /* add coalesced block to list */
  } else if (prev && bt_free(prev)) { /* if previous block is free */
    segret_list_remove(prev);
    bt_make(prev, bt_size(prev) + bt_size(block), FREE);
    segret_list_add(prev); /* add coalesced block to list */
  } else {                 /* if neighbours aren't free */
    bt_make(block, bt_size(block), FREE);
    segret_list_add(block); /* add block to list */
  }
}

/* --=[ free ]=------------------------------------------------------------- */

void free(void *ptr) {
  if (!ptr)
    return;
  word_t *bt = bt_fromptr(ptr); /* get block from pointer */
  coalesce(bt);                 /* coalesce block */
}

/* --=[ realloc ]=---------------------------------------------------------- */

void *realloc(void *old_ptr, size_t size) {
  /*if ptr is NULL realloc -> malloc*/
  if (!old_ptr)
    return malloc(size);
  /*if size is 0 realloc -> free*/
  if (size == 0) {
    free(old_ptr);
    return NULL;
  }
  /* Otherwise */
  word_t *block = bt_fromptr(old_ptr);
  size_t asize = blksz(size);
  size_t old_size = bt_size(block);
  if (asize == old_size) { /* if old_size is enough */
    return old_ptr;
  }

  if (asize < old_size) { /* if old_size is too big */
    split(block, asize);  /* split block */
    return old_ptr;
  }
  word_t *next = bt_next(block);
  if (next && bt_free(next) &&
      (asize <=
       (old_size + bt_size(next)))) { /* if next block is free and (old_size +
                                         next_size) is big enough */
    segret_list_remove(next);         /* remove next block from list */
    bt_make(block, old_size + bt_size(next), USED);
    split(block, asize); /* try to split block */
    return old_ptr;
  }
  if (last == block) { /* if block is last block */
    /* word_t *new_block =*/morecore(
      ALIGN(asize - old_size)); /* get only (asize - old_size ) more memory */
    bt_make(block, asize, USED);
    return old_ptr;
  }

  word_t *new_block = malloc(size); /* if none of the above, malloc new block */
  memcpy(new_block, old_ptr, old_size); /* copy old block to new block */
  free(old_ptr);                        /* free old block */
  return new_block;
}

/* --=[ calloc ]=----------------------------------------------------------- */

void *calloc(size_t nmemb, size_t size) {
  size_t bytes = nmemb * size;
  void *new_ptr = malloc(bytes);
  if (new_ptr)
    memset(new_ptr, 0, bytes);
  return new_ptr;
}

/* --=[ mm_checkheap ]=----------------------------------------------------- */

void mm_checkheap(int verbose) {
  msg("\n CHECKHEAP\n");
  msg("Heap start: %p Heap end: %p last: %p \n", heap_start, heap_end, last);
}
