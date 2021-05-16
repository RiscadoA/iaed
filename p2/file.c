/*
 * File: 		file.c
 * Author: 		Ricardo Antunes
 * Description: Filesystem implementation.
 */

#include <malloc.h>
#include <string.h>

#include "tad.h"

/* Describes a file. */
struct file {
	char* value;				/* File value, may be NULL */
	char* component;			/* File path component */
	struct file* parent;		/* Parent file */
	struct avl* lex_children;	/* Children sorted lexicographically */
	struct list* time_children; /* Children sorted by creation time */
};

/*
 * Creates a new file on a path with a NULL value. If a file already exists,
 * the old file is returned unchanged.
*/
struct file* file_create(const char* path, struct file* root) {
	struct file* file;
	int clen;

	/* Get first component length */
	for (clen = 0; path[clen] != '\0' && path[clen] != '/'; clen++);
	if (path[clen] == '\0')
		if (strncmp(root->component, path, clen) == 0)
			return root;
		else {
			file = (struct file*)malloc(sizeof(struct file));
			file->value = NULL;
			file->component = (char*)malloc((clen + 1) * sizeof(char));
			strncpy(file->component, path, clen);
			file->component[clen] = '\0';
			file->parent = root;
			file->lex_children = NULL;
			file->time_children = NULL;
			if (root != NULL) {
				avl_insert(root->lex_children, file);
				list_insert(root->time_children, file);
			}
			return file_create(path + clen + 1, file);
		}

	return list_traverse(root->time_children, path + clen + 1, &file_find);
}

/*
 * Destroys a file and its children, removing it from the tree and freeing the
 * memory associated with it.
 */
void file_destroy(struct file* file) {
	struct file* child;

	/* Destroy children first */
	for (child = list_first(file->time_children); child != NULL;)
		file_destroy(child);

	/* Remove file from its parent */
	if (file->parent != NULL) {
		avl_remove(file->parent->lex_children, file);
		list_remove(file->parent->time_children, file);
	}

	/* Free memory */
	if (file->value)
		free(file->value);
	free(file->component);
	free(file);
}

/*
 * Tries to find a file from its path. Returns a pointer to the file, and, if no
 * file was found, NULL is returned.
 */
struct file* file_find(const char* path, struct file* root) {
	int clen;

	/* Get first component length */
	for (clen = 0; path[clen] != '\0' && path[clen] != '/'; clen++);
	if (path[clen] == '\0') /* Recursion end */
		return strncmp(root->component, path, clen) == 0 ? root : NULL;

	return list_traverse(root->time_children, path + clen + 1, &file_find);
}

/*
 * Sets an existing file's value or adds a new file with that value on the
 * specified path. 
 */
void file_set(const char* path, char* value, struct file* root) {
	struct file* file = file_create(path, root);

	if (file->value != NULL)
		free(file->value);
	file->value = value;
}

/* Returns a file's value. */
const char* file_value(struct file* file) {
	return file->value;
}

/* Returns a file's path component. */
const char* file_component(struct file* file) {
	return file->component;
}

/* Prints a file's path recursivily */
void file_print_path(struct file* root) {
	if (root->parent == NULL)
		return;
	file_print_path(root->parent);
	printf("/%s", root->component);
}

/* Auxiliar function which prints each path and value */
void* file_print_aux(void* _, struct file* file) {
	file_print_path(file);
	putchar(' ');
	puts(file->value);
	list_traverse(file->time_children, NULL, &file_print_aux);
	return NULL;
}

/*
 * Prints all paths and values beneath the root file passed sorted by creation
 * time.
 */
void file_print(struct file* root) {
	list_traverse(root->time_children, NULL, &file_print_aux);
}

/* Auxiliar function which prints each file traversed */
void* file_list_aux(void* _, struct file* file) {
	puts(file->component);
	return NULL;
}

/*
 * Prints all paths immediately beneath the root file passed sorted
 * lexicographicaly.
 */
void file_list(struct file* root) {
	avl_traverse(root->lex_children, NULL, &file_list_aux);
}
