/* 
 * bag.h - header file for CS50 'bag' module
 * 
 * A ‘bag’ is a collection of indistinguishable items; it starts empty, 
 * grows as the caller adds one item at a time, and shrinks as the caller 
 * extracts one item at a time. It could be empty, or could contain hundreds
 * of items. Since items are indistinguishable, the module is free to return
 * any item from the bag. 
 *
 * David Kotz, April 2016, 2017
 * Xia Zhou, July 2017, 2018, January 2019
 * Temi Prioleau, January 2020
 */

#ifndef __BAG_H
#define __BAG_H

/**************** global types ****************/
typedef struct bag bag_t;  // opaque to users of the module, users do not know and cannot access members of this struct

/**************** functions ****************/

/* Create a new (empty) bag; return NULL if error. */
bag_t *bag_new(void);

/* Add new item to the bag; a NULL bag is ignored; a NULL item is ignored. */
void bag_insert(bag_t *bag, void *item);

/* Return any data item from the bag; return NULL if bag is NULL or empty. */
void *bag_extract(bag_t *bag);

/* Print the whole bag; provide the output file and func to print each item.
 * If fp==NULL; do nothing. If bag==NULL, print (null). 
 * If itemprint==NULL, print nothing for each item.
 */
void bag_print(bag_t *bag, FILE *fp, 
	       void (*itemprint)(FILE *fp, void *item));

/* Iterate over the whole bag; call the given function on each item, 
 * passing both the item and an argument. Ignore if NULL bag or NULL itemfunc.
 */
void bag_iterate(bag_t *bag, void *arg,
		 void (*itemfunc)(void *arg, void *item) );

/* Delete the whole bag; ignore NULL bag.
 * Provide a function that will delete each item (may be NULL).
 */
void bag_delete(bag_t *bag, void (*itemdelete)(void *item) );

#endif // __BAG_H
