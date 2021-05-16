/*
 * File: 		tad.h
 * Author: 		Ricardo Antunes
 * Description: Abstract data type function prototypes are declared here.
 */

#ifndef TADS_H
#define TADS_H

struct file;
struct avl;
struct list;

/*
 *	Function pointer type passed to traversal functions. If the function returns
 *	a non NULL value, the traversal is ended early.
 */
typedef void*(*traverse_fn)(void*, struct file* file);

/* File TAD function prototypes. */

struct file* file_create(const char* path, struct file* root);
void file_destroy(struct file* file);
struct file* file_find(const char* path, struct file* root);
void file_set(const char* path, char* value, struct file* root);
const char* file_value(struct file* file);
const char* file_component(struct file* file);
void file_print(struct file* root);
void file_list(struct file* root);

/* AVL tree TAD function prototypes. */

struct avl* avl_insert(struct avl* avl, struct file* file);
struct avl* avl_remove(struct avl* avl, struct file* file);
void* avl_traverse(struct avl* avl, void* ptr, traverse_fn* fn);

/* List TAD function prototypes. */

struct list* list_insert(struct list* list, struct file* file);
struct list* list_remove(struct list* list, struct file* file);
struct file* list_first(struct list* list);
void* list_traverse(struct list* list, void* ptr, traverse_fn* fn);

#endif