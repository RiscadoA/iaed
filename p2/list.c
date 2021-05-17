/*
 * File: 		list.c
 * Author: 		Ricardo Antunes
 * Description: Doubly linked list implementation used by the filesystem.
 */

#include "adt.h"

#include <malloc.h>

/* Describes a doubly linked list link. */
struct link {
	struct file* file;		/* File this link points to */
	struct link* prev;	/* Previous link, may be NULL */
	struct link* next;	/* Next link, may be NULL */
};

/* Describes a doubly linked list. */
struct list {
	struct link* first;
	struct link* last;
};

/*
 * Creates a new doubly linked list and returns a pointer to it. If the
 * allocation fails, NULL is returned.  
 */
struct list* list_create(void) {
	/* calloc initializes list->first and list->last to NULL (0) */
	return calloc(1, sizeof(struct list));
}

/* Destroys a doubly linked list, freeing all memory associated with it. */
void list_destroy(struct list* list) {
	struct link* link;
	
	/* Free all links */
	while(list->first != NULL) {
		link = list->first;
		list->first = link->next;
		free(link);
	}

	free(list);
}

/*
 * Inserts a new file into a doubly linked list. If the allocation fails, NULL
 * is returned and the list remains unchanged. Otherwise, a pointer to the new
 * link which points to the file is returned.
 */
struct link* list_insert(struct list* list, struct file* file) {
	struct link* link = malloc(sizeof(struct link));
	
	if (link != NULL) {
		link->next = NULL;
		link->prev = list->last;
		if (list->last != NULL)
			list->last->next = link;
		list->last = link;
	}

	return link;
}

/* Removes a link from a doubly linked list. */
void list_remove(struct list* list, struct link* link) {
	if (link->prev != NULL)
		link->prev->next = link->next;
	if (link->next != NULL)
		link->next->prev = link->prev;
	if (list->first == link)
		list->first = link->next;
	if (list->last == link)
		list->last = link->prev;
	free(list);
}

/*
 * Traverses a doubly linked list. fn(ptr, file) is called for each file present
 * in the list. If fn(ptr, file) returns a non-NULL value, the traversal ends
 * early and that value is returned. Otherwise, NULL is returned.
 */
void* list_traverse(struct list* list, void* ptr, traverse_fn fn) {
	struct link* link;
	void* ret;

	for (link = list->first; link != NULL; link = link->next)
		if ((ret = fn(ptr, link->file)) != NULL)
			return ret;

	return NULL;
}

/* Returns the first file in a list. */
struct file* list_first(struct list* list) {
	return list->first == NULL ? NULL : list->first->file;
}
