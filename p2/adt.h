/*
 * File: 		adt.h
 * Author: 		Ricardo Antunes
 * Description: Abstract data type function prototypes are declared here.
 */

#ifndef ADT_H
#define ADT_H

struct fs;
struct file;
struct avl;
struct table;
struct list;
struct link;

/*
 * Function pointer type passed to traversal functions. If the function returns
 * a non NULL value, the traversal is ended early.
 */
typedef void*(*traverse_fn)(void*, struct file*);

/* Filesystem and file ADT function prototypes. */

struct fs* filesystem_create(void);
void filesystem_destroy(struct fs* fs);

struct file* file_create(struct fs* fs, char* path);
void file_delete(struct fs* fs, struct file* file);
struct file* file_set(struct fs* fs, char* path, char* value);

struct file* file_find(struct fs* fs, char* path);
struct file* file_search(struct fs* fs, char* value);
void file_print_path(struct file* file);
void file_print(struct fs* fs);
void file_list(struct file* file);

const char* file_value(struct file* file);
const char* file_component(struct file* file);
struct file* file_parent(struct file* file);
int file_time(struct file* file);
int file_height(struct file* file);

/* AVL tree ADT function prototypes. */

struct avl* avl_insert(struct avl* avl, struct file* file);
struct avl* avl_remove(struct avl* avl, struct file* file);
void avl_destroy(struct avl* avl);
struct file* avl_find(struct avl* avl, const char* key);
void* avl_traverse(struct avl* avl, void* ptr, traverse_fn fn);

/* Hash table function prototypes. */

struct table* table_create(void);
void table_destroy(struct table* table);
int table_insert(struct table* table, struct file* file);
void table_remove(struct table* table, struct file* file);
struct file* table_search(struct table* table, const char* value);

/* Doubly linked list ADT function prototypes. */

struct list* list_create(void);
void list_destroy(struct list* list);
struct link* list_insert(struct list* list, struct file* file);
void list_remove(struct list* list, struct link* link);
void* list_traverse(struct list* list, void* ptr, traverse_fn fn);
struct link* list_find(struct list* list, struct file* file);
struct file* list_first(struct list* list);

#endif