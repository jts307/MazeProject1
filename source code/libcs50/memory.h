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

#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdio.h>
#include <stdlib.h>

/**************** assertp **************************/
/* If pointer p is NULL, print error message to stderr and die,
 * otherwise, return p unchanged.  Works nicely as a pass-through:
 *   thingp = assertp(malloc(sizeof(thing)), "thing");
 * We assume:
 *   caller provides a message string suitable for printf.
 * We return:
 *   the value of p
 * We exit if p=NULL, after printing message to stderr.
 */
void *assertp(void *p, const char *message);

/**************** count_malloc_assert() ****************/
/* Like malloc() but track the number of successful allocations,
 * and, if response is NULL, print error and die.
 * Unlike malloc(), it takes a 'message' parameter.
 * We assume:
 *   caller provides a message string suitable for printf.
 * We return:
 *   the pointer produced by malloc.
 * We exit if any error, after printing to stderr.
 */
void *count_malloc_assert(const size_t size, const char *message);

/**************** count_malloc() ****************/
/* Just like malloc() but track the number of successful allocations
 * Caller provides:
 *   the size of desired allocation, as in malloc().
 * We return
 *   pointer to allocated space, or NULL if failure.
 * We track the number of calls - see count_net().
 */
void *count_malloc(const size_t size);

/**************** count_calloc_assert() ****************/
/* Just like calloc() but track the number of successful allocations
 * and, if response is NULL, print error and die.
 * Unlike calloc(), it takes a 'message' parameter.
 * We assume:
 *   caller provides a message string suitable for printf.
 * We return:
 *   the pointer produced by calloc.
 * We exit if any error, after printing to stderr.
 */
void *count_calloc_assert(const size_t nmemb, const size_t size, 
                          const char *message);

/**************** count_calloc() ****************/
/* Just like calloc() but track the number of successful allocations.
 * Caller provides:
 *   the number and size of desired allocation, as in calloc().
 * We return
 *   pointer to allocated space, or NULL if failure.
 * We track the number of calls - see count_net().
 */
void *count_calloc(const size_t nmemb, const size_t size);

/**************** count_free() ****************/
/* Just like free() but track the number of calls.
 * We assume:
 *   caller provides pointer to space produced by count_malloc or count_calloc.
 * We track the number of calls - see count_net().
 */
void count_free(void *ptr);

/**************** count_report() ****************/
/* Print a report of the current malloc/free counts.
 * We assume:
 *   caller provides a FILE open for writing, and message suitable for printf.
 * We format and print a report to that FILE, indicating the number of calls
 * to count_malloc/calloc and of calls to count_free, and the net difference.
 */
void count_report(FILE *fp, const char *message);

/**************** count_net() ****************/
/* Return the current net malloc-free counts.
 * We assume:
 *   caller has been using count_malloc/calloc and count_free. 
 * We return:
 *   returns positive if there are unfreed allocations,
 *   returns negative if there were more free's than alloc's (!),
 *   returns zero if they balance.
 */
int count_net(void);

#endif // __MEMORY_H
