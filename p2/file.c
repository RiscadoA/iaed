/*
 * File: 		file.c
 * Author: 		Ricardo Antunes
 * Description: Filesystem implementation.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "adt.h"

/* Describes a file. */
struct file {
	char* value;				/* File value, may be NULL */
	char* component;			/* File path component */

	struct file* parent;		/* Parent file */
	struct avl* avl_children;	/* Children sorted lexicographically */
	struct list* l_children; 	/* Children sorted by creation time */
	struct link* l_self;		/* The link where this file is (may be NULL) */
};

/* Allocates a new file and fills it with default data. */
struct file* file_alloc(const char* comp) {
	struct file* file;
	
	if ((file = calloc(1, sizeof(struct file))) == NULL)
		return NULL;
	if ((file->component = malloc(strlen(comp) + 1)) == NULL) {
		free(file);
		return NULL;
	}
	if ((file->l_children = list_create()) == NULL) {
		free(file);
		free(file->component);
		return NULL;
	}

	strcpy(file->component, comp);

	return file;
}

/* Frees the memory associated with a file. */
void file_free(struct file* file) {
	avl_destroy(file->avl_children);
	list_destroy(file->l_children);
	if (file->value != NULL)
		free(file->value);
	free(file->component);
	free(file);
}

/* Creates a filesystem root. Returns NULL if the memory allocation failed. */
struct file* file_create_root(void) {
	return file_alloc("");
}

/*
 * Creates a new file on a path with a NULL value. If a file already exists,
 * the old file is returned unchanged. Returns NULL if the memory allocation
 * failed.
 */
struct file* file_create(char* path, struct file* root) {
	struct file* file;
	struct avl* avl;
	const char* comp;

	for (comp = strtok(path, "/"); comp != NULL; comp = strtok(NULL, "/")) {
		file = avl_find(root->avl_children, comp);
		if (file != NULL)
			root = file;
		else {
			file = file_alloc(comp);
			if (file == NULL)
				return NULL; /* Allocation failed */
			file->parent = root;
			
			if ((file->l_self = list_insert(root->l_children, file)) == NULL) {
				file_free(file);
				return NULL; /* Allocation failed */
			}

			if ((avl = avl_insert(root->avl_children, file)) == NULL) {
				list_remove(root->l_children, file->l_self);
				file_free(file);
				return NULL; /* Allocation failed */
			}

			root->avl_children = avl;
			root = file;
		}
	}

	return root;
}

/*
 * Destroys a file and its children, removing it from the tree and freeing the
 * memory associated with it.
 */
void file_destroy(struct file* file) {
	struct file* child;
	struct file* parent = file->parent;

	if (file == NULL)
		return;

	/* Destroy children first */
	while ((child = list_first(file->l_children)) != NULL)
		file_destroy(child);

	/* Remove file from its parent */
	if (parent != NULL) {
		parent->avl_children = avl_remove(parent->avl_children, file);
		list_remove(parent->l_children, file->l_self);
	}

	/* Free memory */
	file_free(file);
}

/* Calls file_destroy(child) for every child in file. */
void file_destroy_children(struct file* file) {
	struct file* child;

	/* Destroy children */
	while ((child = list_first(file->l_children)) != NULL)
		file_destroy(child);
}

/*
 * Tries to find a file from its path. Returns a pointer to the file, and, if no
 * file was found, NULL is returned.
 */
struct file* file_find(struct file* root, char* path) {
	const char* comp;

	for (comp = strtok(path, "/"); comp != NULL; comp = strtok(NULL, "/")) {
		root = avl_find(root->avl_children, comp);
		if (root == NULL)
			return NULL;
	}

	return root;
}

/*
 * Auxiliar function for searching files by value. If the file is not found,
 * NULL is returned. Otherwise, a pointer to the file is returned.
 */
void* file_search_aux(void* value, struct file* root) {
	if (root->value != NULL && strcmp(value, root->value) == 0)
		return root;
	return list_traverse(root->l_children, value, &file_search_aux);
}

/*
 * Searches a file by value. If the file is not found, NULL is returned.
 * Otherwise, a pointer to the file is returned.
 */
struct file* file_search(struct file* root, char* value) {
	return list_traverse(root->l_children, value, &file_search_aux);
}

/*
 * Sets an existing file's value or adds a new file with that value on the
 * specified path. Returns a pointer to the file whose value was changed. If
 * a memory allocation fails, NULL is returned. 
 */
struct file* file_set(char* path, char* value, struct file* root) {
	struct file* file = file_create(path, root);

	if (file != NULL) {
		file->value = realloc(file->value, strlen(value) + 1);
		strcpy(file->value, value);
	}

	return file;
}

/* Prints a file's path recursivily */
void file_print_path(struct file* root) {
	if (root->parent == NULL)
		return;
	file_print_path(root->parent);
	printf("/%s", root->component);
}

/* Auxiliar function which prints each path and value */
void* file_print_aux(void* unused, struct file* file) {
	/*
	 * Supress unused parameter warning, must be done to avoid having to create
	 * another function pointer type and traversal function.
	 */
	(void)unused;
		
	if (file->value != NULL) {
		file_print_path(file);
		putchar(' ');
		puts(file->value);
	}
	list_traverse(file->l_children, NULL, &file_print_aux);
	return NULL;
}

/*
 * Prints all paths and values beneath the root file passed sorted by creation
 * time.
 */
void file_print(struct file* root) {
	list_traverse(root->l_children, NULL, &file_print_aux);
}

/* Auxiliar function which prints each file traversed */
void* file_list_aux(void* unused, struct file* file) {
	/*
	 * Supress unused parameter warning, must be done to avoid having to create
	 * another function pointer type and traversal function.
	 */
	(void)unused;

	puts(file->component);
	return NULL;
}

/*
 * Prints all paths immediately beneath the root file passed sorted
 * lexicographicaly.
 */
void file_list(struct file* root) {
	/* The AVL is traversed in order = lexicographically */
	avl_traverse(root->avl_children, NULL, &file_list_aux);
}

/* Returns a file's value. */
const char* file_value(struct file* file) {
	return file->value;
}

/* Returns a file's path component. */
const char* file_component(struct file* file) {
	return file->component;
}