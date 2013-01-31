/* a simple implementation of linked lists */
#ifndef _LIST_H
#define _LIST_H

struct list {
    char data;
    struct list *next;
};

typedef struct list LIST;

/* create a new list
 *
 * Pre:
 * Post: returns a pointer to a valid linked-list
 */
struct list *list_new(char data);

/* append to a list
 *
 * Pre: none. if node == NULL this is equivalent to list_new
 * Post: returns a pointer to a valid linked-list
 */
struct list *list_append(struct list *node, char data);

/* go to the next node in a list
 *
 * Pre: node != NULL
 * Post: returns a pointer to a valid linked-list
 */
struct list *list_next(struct list *node);

/* get the length of a list
 *
 * Pre: node != NULL
 * Post: returns a pointer to a valid linked-list
 */
unsigned int list_length(struct list *node);

/* free the memory used by a list
 *
 * Pre: node != NULL
 * Post: returns a pointer to a valid linked-list
 */
void list_free(struct list *node);

/* reverse the list

   pre: none. if node == NULL it will return NULL
   post: returns a pointer to a valid (reverted) linked-list
*/
struct list* list_reverse(struct list* node);
#endif
