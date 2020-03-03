/* 
 * priority_queue.h - header file for CS50 'priority_queue' module (modified from bag.h)
 * 
 * A ‘priority_que’ is a collection of (item, integer) pairs. It starts 
 * out empty and grows as (item, integer) pairs are added to it one at 
 * a time. Each item is given a priority in the queue based off of the 
 * passed integer, lower integers have lower priority. Items are 
 * extracted, i.e. removed from the queue and returned to a caller, 
 * one at a time and the item with the current lowest priority in the 
 * queue is always extracted. 
 *
 * Jacob Werzinsky, CS50, Winter 2020
 */

#ifndef __PRIORITY_QUEUE_H
#define __PRIORITY_QUEUE_H

/**************** global types ****************/
typedef struct priority_queue priority_queue_t;

/**************** functions ****************/

/* Create a new (empty) priority queue; return NULL if error. */
priority_queue_t* priority_queue_new(void)

/* Add new item to the queue with a priority; a NULL queue is ignored; a NULL item is ignored. */
void priority_queue_insert(priority_queue_t *pq, void *item, int priority);

/* Return any data item from the bag; return NULL if bag is NULL or empty. */
void* priority_queue_extract(priority_queue_t *pq);

/* */
void* priority_queue_view(priority_queue_t *pq);


/* Print the whole bag; provide the output file and func to print each item.
 * If fp==NULL; do nothing. If bag==NULL, print (null). 
 * If itemprint==NULL, print nothing for each item.
 */
void priority_queue_print(priority_queue_t *pq, FILE *fp, void (*itemprint)(FILE *fp, void *item));

/* Delete the whole bag; ignore NULL bag.
 * Provide a function that will delete each item (may be NULL).
 */
void priority_queue_delete(priority_queue_t *pq, void (*itemdelete)(void *item));

#endif // __PRIORITY_QUEUE_H
