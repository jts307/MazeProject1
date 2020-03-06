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
 * we_free, CS50, Winter 2020
 */

#ifndef __PRIORITY_QUEUE_H
#define __PRIORITY_QUEUE_H

/**************** global types ****************/
typedef struct priority_queue priority_queue_t;

/**************** functions ****************/

/* Create a new (empty) priority queue; return NULL if error. 
*  Input: Nothing
*  Output: priority queue structure
*/
priority_queue_t* priority_queue_new(void)

/* Add new item to the queue with a priority where lower priority are put 
*  closer to the front of the queue; a NULL queue is ignored; a NULL item is ignored. 
*  Input:
*     -pq: the priority queue structure in question, must have memory already allocated for it
*     -item: pointer to an item to be stored
*     -priority: number that determines order in queue, 
*                     lower numbered item get put closer to front
*  Output:
*     -returns nothing
*/
void priority_queue_insert(priority_queue_t *pq, void *item, int priority);

/* Returns the item in the front of the queue, i.e. the item with the lowest priority; 
*  return NULL if bag is NULL or empty. Removes item from queue upon inspection.
*  Input: 
*      -pq: the priority queue structure in question, must have memory already allocated for it
*  Output: Described in purpose of function above
*/
void* priority_queue_extract(priority_queue_t *pq);

/* Returns the item in the front of the queue, i.e. the item with the lowest priority; 
*  return NULL if bag is NULL or empty. 
*  Input: 
*      -pq: the priority queue structure in question, must have memory already allocated for it
*  Output: Described in purpose of function above
*/
void* priority_queue_view(priority_queue_t *pq);


/* Print all items within a priority queue to a file stream.
 * Input: 
 *     -pq: the priority queue structure in question, must have memory already allocated for it
 *     -fp: pointer to file stream that queue will be printed to.
 *     -itemprint: function that prints an item to a file stream
 * Output:
 *     -a file with the priority queue printed to it, in the format: { item1 item2 item3... }
 * If fp==NULL; nothing is printed. If bag==NULL, (null) is printed. 
 * If itemprint==NULL, nothing is printed for each item.
 */
void priority_queue_print(priority_queue_t *pq, FILE *fp, void (*itemprint)(FILE *fp, void *item));

/* Delete the priority queue, i.e. free any memory it is using; ignores NULL priority queue.
 * Input:
 *      -pq: the priority queue structure in question
 *      -itemdelete: a function that will delete each item (may be NULL).
 * Output:
 *      -returns nothing
 */
void priority_queue_delete(priority_queue_t *pq, void (*itemdelete)(void *item));

#endif // __PRIORITY_QUEUE_H
