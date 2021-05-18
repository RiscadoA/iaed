/*
 * File: 		file.c
 * Author: 		Ricardo Antunes
 * Description: Filesystem implementation.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "adt.h"

/* Describes a filesystem. */
struct fs {
	struct file* root;			/* Root file ('/') */
	struct table* value_table;	/* Hash table used to search by value */
	int time;					/* Current time (number of files inserted) */
};

/* Describes a file. */
struct file {
	char* value;				/* File value, may be NULL */
	char* component;			/* File path component */
	int time;					/* File creation time */
	int height;					/* File height in the tree */

	struct file* parent;		/* Parent file */
	struct avl* avl_children;	/* Children sorted lexicographically */
	struct list* l_children; 	/* Children sorted by creation time */
	struct link* l_self;		/* The link where this file is (may be NULL) */
};

/* Allocates a new file and fills it with default data. */
static struct file* file_alloc(const char* comp, int time) {
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
	file->time = time;

	return file;
}

/* Frees the memory associated with a file. */
static void file_free(struct file* file) {
	avl_destroy(file->avl_children);
	list_destroy(file->l_children);
	if (file->value != NULL)
		free(file->value);
	free(file->component);
	free(file);
}

/* Creates a filesystem. Returns NULL if the memory allocation failed. */
struct fs* filesystem_create(void) {
	struct fs* fs = calloc(1, sizeof(struct fs));
	
	fs->root = file_alloc("", 0);
	if (fs->root == NULL) {
		free(fs);
		return NULL;
	}

	fs->value_table = table_create();
	if (fs->value_table == NULL) {
		file_free(fs->root);
		free(fs);
		return NULL;
	}

	return fs;
}

/* Deletes a filesystem and frees all memory associated with it. */
void filesystem_destroy(struct fs* fs) {
	file_delete(fs, fs->root);
	table_destroy(fs->value_table);
	free(fs);
}

/*
 * Creates a new file on a path with a NULL value. If a file already exists,
 * the old file is returned unchanged. Returns NULL if the memory allocation
 * failed.
 */
struct file* file_create(struct fs* fs, char* path) {
	struct file* file, * root = fs->root;
	struct avl* avl;
	const char* comp;

	for (comp = strtok(path, "/"); comp != NULL; comp = strtok(NULL, "/")) {
		file = avl_find(root->avl_children, comp);
		if (file != NULL)
			root = file;
		else {
			file = file_alloc(comp, ++fs->time);
			if (file == NULL)
				return NULL; /* Allocation failed */
			
			if ((file->l_self = list_insert(root->l_children, file)) == NULL) {
				file_free(file);
				return NULL; /* Allocation failed */
			}

			if ((avl = avl_insert(root->avl_children, file)) == NULL) {
				list_remove(root->l_children, file->l_self);
				file_free(file);
				return NULL; /* Allocation failed */
			}

			file->parent = root;
			file->height = root->height + 1;
			root->avl_children = avl;
			root = file;
		}
	}

	return root;
}

/*
 * Delete a file and its children, removing it from the tree and freeing the
 * memory associated with it.
 */
void file_delete(struct fs* fs, struct file* file) {
	struct file* child, * parent;

	if (file == NULL) {
		/* Delete every non-root file */
		while ((child = list_first(fs->root->l_children)) != NULL)
			file_delete(fs, child);
	}
	else {
		/* Delete children first */
		while ((child = list_first(file->l_children)) != NULL)
			file_delete(fs, child);
		parent = file->parent;

		/* Remove file from its parent */
		if (parent != NULL) {
			parent->avl_children = avl_remove(parent->avl_children, file);
			list_remove(parent->l_children, file->l_self);
		}

		table_remove(fs->value_table, file);

		/* Free memory */
		file_free(file);
	}
}

/*
 * Tries to find a file from its path. Returns a pointer to the file, and, if no
 * file was found, NULL is returned.
 */
struct file* file_find(struct fs* fs, char* path) {
	struct file* file = fs->root;
	const char* comp;

	for (comp = strtok(path, "/"); comp != NULL; comp = strtok(NULL, "/")) {
		file = avl_find(file->avl_children, comp);
		if (file == NULL)
			return NULL;
	}

	return file;
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
struct file* file_search(struct fs* fs, char* value) {
	return table_search(fs->value_table, value);
}

/*
 * Sets an existing file's value or adds a new file with that value on the
 * specified path. Returns a pointer to the file whose value was changed. If
 * a memory allocation fails, NULL is returned. 
 */
struct file* file_set(struct fs* fs, char* path, char* value) {
	struct file* file = file_create(fs, path);

	if (file != NULL) {
		table_remove(fs->value_table, file);

		if ((file->value = realloc(file->value, strlen(value) + 1)) == NULL)
			return NULL; /* Allocation failed */
		strcpy(file->value, value);

		if (!table_insert(fs->value_table, file))
			return NULL; /* Allocation failed */
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
void file_print(struct fs* fs) {
	list_traverse(fs->root->l_children, NULL, &file_print_aux);
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
 * Prints all paths immediately beneath the file passed sorted lexicographicaly.
 */
void file_list(struct file* file) {
	/* The AVL is traversed in order = lexicographically */
	avl_traverse(file->avl_children, NULL, &file_list_aux);
}

/* Returns a file's value. May be NULL. */
const char* file_value(struct file* file) {
	return file->value;
}

/* Returns a file's path component. */
const char* file_component(struct file* file) {
	return file->component;
}

/* Returns a file's parent. May be NULL. */
struct file* file_parent(struct file* file) {
	if (file == NULL)
		return NULL;
	return file->parent;
}

/* Returns a file's creation time. */
int file_time(struct file* file) {
	return file->time;
}

/* Returns a file's height on the filesystem. */
int file_height(struct file* file) {
	return file->height;
}