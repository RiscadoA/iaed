/*
 * File: 		adt.h
 * Author: 		Ricardo Antunes
 * Description: Abstract data type function prototypes are declared here.
 */

#ifndef TADS_H
#define TADS_H

struct file;
struct avl;
struct list;
struct link;

/*
 * Function pointer type passed to traversal functions. If the function returns
 * a non NULL value, the traversal is ended early.
 */
typedef void*(*traverse_fn)(void*, struct file*);

/* File ADT function prototypes. */

struct file* file_create(char* path, struct file* root);
void file_destroy(struct file* file);
struct file* file_set(char* path, char* value, struct file* root);

struct file* file_find(struct file* root, char* path);
struct file* file_search(struct file* root, char* value);
void file_print(struct file* root);
void file_list(struct file* root);

const char* file_value(struct file* file);
const char* file_component(struct file* file);
int file_time(struct file* file);
struct file* file_parent(struct file* file);

/* AVL tree ADT function prototypes. */

struct avl* avl_insert(struct avl* avl, struct file* file);
struct avl* avl_remove(struct avl* avl, struct file* file);
void avl_destroy(struct avl* avl);
struct file* avl_find(struct avl* avl, const char* key);
void* avl_traverse(struct avl* avl, void* ptr, traverse_fn fn);

/* Doubly linked list ADT function prototypes. */

struct list* list_create(void);
void list_destroy(struct list* list);
struct link* list_insert(struct list* list, struct file* file);
void list_remove(struct list* list, struct link* link);
void* list_traverse(struct list* list, void* ptr, traverse_fn fn);
struct file* list_first(struct list* list);

#endif