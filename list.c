/* a simple implementation of linked lists */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct list *list_new(char data)
{
    struct list *r;

    r = malloc(sizeof(struct list));
    if (!r) {
        fprintf(stderr, "Out of memory. Could not allocate memory in list_new_node.\n");
        exit(EXIT_FAILURE);
    }
    r->data = data;
    r->next = NULL;
    return r;
}

struct list *list_append(struct list *node, char data)
{
    struct list *r;
    /* we actually do appends as prepends for efficiency */
    r = list_new(data);
    r->next = node;
    return r;
}

struct list *list_next(struct list *node)
{
    return node->next;
}

unsigned int list_length(struct list *node)
{
    unsigned int l = 0;
    while (node) {
        l++;
        node = node->next;
    }
    return l;
}

void list_free(struct list *node)
{
    struct list *p, *next;
    p = node;
    while (p) {
        next = p->next;
        free(p);
        p = next;
    }
}

struct list* list_reverse(struct list* node) {
    struct list* p = node;
    struct list* q = NULL;
    while (p) {
        p = node->next;
        node->next = q;
        q = node;
        node = p;
    }
    return q;
}
