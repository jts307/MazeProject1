/* 
 * priority_queue.c - CS50 'queue_priority' module (modified from bag.c)
 * 
 * see priority_queue.h for more information.
 *
 * we_free, CS50, Winter 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct pq_node {
  void *item;		      // pointer to data for this item
  double priority;	      // lower priority gets extracted first
  struct pq_node *next;       // link to next node
} pq_node_t;

/**************** global types ****************/
typedef struct priority_queue {
  struct pq_node *front;    // item with lowest priority in the queue
} priority_queue_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see priority_queue.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static pq_node_t *pq_node_new(void *item, double priority);

/**************** priority_queue_new() ****************/
priority_queue_t* priority_queue_new(void)
{
  priority_queue_t *pq = count_malloc(sizeof(priority_queue_t));

  if (pq == NULL) {
      return NULL; // error allocating priority queue
  } else {
      // initialize contents of priority queue structure
      pq->front = NULL;
      return pq;
  }
}

/**************** priority_queue_insert() ****************/
void priority_queue_insert(priority_queue_t *pq, void *item, double priority)
{
  if (pq != NULL && item != NULL) {
    // allocate a new node to be added to the queue
    pq_node_t *new = pq_node_new(item, priority);
    if (new != NULL) {
      // find first node that has less priority than new node
      pq_node_t *front = pq->front;
		
      // if queue is empty then add item and return
      if (pq->front  == NULL) {
        pq->front = new;
	return;
      }
      // checking if new node has less priority than front
      if (priority < front->priority) {
	new->next = front;
	pq->front = new;
	return;
      }
      // checking rest of nodes
      while ((front->next != NULL) && (priority > front->next->priority)) {
        front = front->next;
      } 
      // insert before node that is found, unless node is not found
      // then its placed at end of list
      new->next = front->next;
      front->next = new;
    }
  }

#ifdef MEMTEST
  count_report(stdout, "After priority_queue_insert");
#endif
}

/**************** priority_queue_extract() ****************/
void* priority_queue_extract(priority_queue_t *pq)
{
  if (pq == NULL) {
      return NULL; 
  } else if (pq->front == NULL) {
      return NULL; // priority queue is empty
  } else {
      pq_node_t *out = pq->front;    // the node to take out
      void *item = out->item;	   // the item to return
      pq->front = out->next;	   // remove node from linked list
      count_free(out);
      return item;
  }
}

/**************** priority_queue_view() ****************/
void* priority_queue_view(priority_queue_t *pq)
{
  if (pq == NULL) {
      return NULL; 
  } else if (pq->front == NULL) {
      return NULL; // priority queue is empty
  } else {
      return pq->front->item;  // return item at front of queue
  }
}

/**************** priority_queue_print() ****************/
void priority_queue_print(priority_queue_t *pq, FILE *fp, void (*itemprint)(FILE *fp, void *item))
{
  if (fp != NULL) {
    if (pq != NULL) {
      fputc('{', fp);
      for (pq_node_t *node = pq->front; node != NULL; node = node->next) {
        // print this node's item
	// using passed function
        if (itemprint != NULL) {  
          fputc(' ', fp);
          (*itemprint)(fp, node->item); 
        }
      }
      fputs(" }\n", fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** priority_queue_delete() ****************/
void priority_queue_delete(priority_queue_t *pq, void (*itemdelete)(void *item))
{
  if (pq != NULL) {
    for (pq_node_t *node = pq->front; node != NULL; ) {
      // delete node's item 
      if (itemdelete != NULL) {		   
        (*itemdelete)(node->item);	    
      }
      pq_node_t *next = node->next;	    // remember next node
      count_free(node);			    // free the current node
      node = next;			    // and move on to next node
    }
    count_free(pq);
  }

#ifdef MEMTEST
  count_report(stdout, "End of priority_queue_delete");
#endif
}

/**************** pq_node_new ****************/
/* Allocate and initialize a pq_node 
 * Input:
 * 	-item: a void pointer to data 
 * 	-priority: determines place in priority queue, 
 * 	           lower gets put nearer to front.
 * Output:
 * 	Returns a node that contains the passed item and priority.
 */
static pq_node_t* pq_node_new(void *item, double priority)
{
  pq_node_t *node = count_malloc(sizeof(pq_node_t));

  if (node == NULL) {
      // error allocating memory for node; return error
      return NULL;
  } else {
      // initialize node with info
      node->item = item;
      node->priority = priority;
      node->next = NULL;
      return node;
  }
}
