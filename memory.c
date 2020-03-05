/*
 * memory - count_malloc and related functions 
 * 
 * 1. Replacements for malloc(), calloc(), and free(),
 *    that count the number of calls to each,
 *    so you can print reports about the current balance of memory.
 * 
 * 2. Variants that 'assert' the result is non-NULL;
 *    if NULL occurs, kick out an error and die.
 *
 * David Kotz, April 2016, 2017, 2019
 * Updated by Temi Prioleau, January 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

/**************** file-local global variables ****************/
// track malloc and free across *all* calls within this program.
static int nmalloc = 0;    // number of successful malloc calls
static int nfree = 0;    // number of free calls
static int nfreenull = 0;  // number of free(NULL) calls


/**************** assertp ****************/
/* see memory.h for description */
void *
assertp(void *p, const char *message)
{
  if (p == NULL) {
    fprintf(stderr, "MALLOC FAILED: %s\n", message);
    exit (99);
  }
  return p;
}

/**************** count_malloc_assert() ****************/
/* see memory.h for description */
void *
count_malloc_assert(const size_t size, const char *message)
{
  void *ptr = assertp(malloc(size), message);
  nmalloc++;
  return ptr;
}


/**************** count_malloc() ****************/
/* see memory.h for description */
void *
count_malloc(const size_t size)
{
  void *ptr = malloc(size);
  if (ptr != NULL) {
    nmalloc++;
  }
  return ptr;
}

/**************** count_calloc_assert() ****************/
/* see memory.h for description */
void *
count_calloc_assert(const size_t nmemb, const size_t size, const char *message)
{
  void *ptr = assertp(calloc(nmemb, size), message);
  nmalloc++;
  return ptr;
}

/**************** count_calloc() ****************/
/* see memory.h for description */
void *
count_calloc(const size_t nmemb, const size_t size)
{
  void *ptr = calloc(nmemb, size);
  if (ptr != NULL) {
    nmalloc++;
  }
  return ptr;
}

/**************** count_free() ****************/
/* see memory.h for description */
void 
count_free(void *ptr)
{
  if (ptr != NULL) {
    free(ptr);
    nfree++;
  } else {
    // it's an error to call free(NULL)!
    nfreenull++;
  }
}

/**************** count_report() ****************/
/* see memory.h for description */
void 
count_report(FILE *fp, const char *message)
{
  fprintf(fp, "%s: %d malloc, %d free, %d free(NULL), %d net\n", 
          message, nmalloc, nfree, nfreenull, nmalloc - nfree - nfreenull);
}

/**************** count_net() ****************/
/* see memory.h for description */
int
count_net(void)
{
  return nmalloc - nfree - nfreenull;
}
