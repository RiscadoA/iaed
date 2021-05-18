/*
 * File: 		table.c
 * Author: 		Ricardo Antunes
 * Description: Hash table implementation used by the filesystem.
 */

#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "adt.h"

/*
 * Describes an hash table used to search files by value, following the 
 * order shown in the print command (DFS, sorted by creation time).
 */
struct table {
	struct list* cells[HASH_TABLE_SIZE];
};

/*
 * Data used to search for the file in a list with the value we want and lowest
 * creation time.
 */
struct query_data {
	const char* value;
	struct file* best;
};

/* Gets the hash of a string. */
static int hash(const char* v) {
	int h = 0, a = 127;

	for (; *v != '\0'; ++v)
		h = (a*h + *v) % HASH_TABLE_SIZE;
		
	return h;
}

/*
 * Creates a new hash table and returns a pointer to it. Returns NULL if memory
 * allocation fails.
 */
struct table* table_create(void) {
	return calloc(1, sizeof(struct table));
}

/* Frees all memory associated with a hash table. */
void table_destroy(struct table* table) {
	int i;

	/* Destroy lists. */
	for (i = 0; i < HASH_TABLE_SIZE; ++i)
		if (table->cells[i] != NULL)
			list_destroy(table->cells[i]);
			
	free(table);
}

/*
 * Inserts a file into a hash table. Returns 0 if memory allocation fails,
 * otherwise returns 1.
 */
int table_insert(struct table* table, struct file* file) {
	int h = hash(file_value(file));

	if (table->cells[h] == NULL)
		if ((table->cells[h] = list_create()) == NULL)
			return 0; /* Allocation failed */

	return list_insert(table->cells[h], file) != NULL;
}

/*
 * Removes a file from a hash table. If the file isn't in the table, nothing
 * happens and the table is left unchanged.
 */
void table_remove(struct table* table, struct file* file) {
	int h;

	if (file_value(file) == NULL)
		return;

	h = hash(file_value(file));
	if (table->cells[h] != NULL)
		list_remove(table->cells[h], list_find(table->cells[h], file));
}

/* Returns the better candidate out of the two files. */
static struct file* best_file(struct file* lhs, struct file* rhs) {
	struct file* lhs_p = lhs, * rhs_p = rhs;

	if (lhs == NULL || lhs == rhs)
		return rhs;
	if (rhs == NULL)
		return lhs;

	/* Put files on the same height */
	while (file_height(lhs_p) > file_height(rhs_p))
		lhs_p = file_parent(lhs_p);
	while (file_height(rhs_p) > file_height(lhs_p))
		rhs_p = file_parent(rhs_p);

	/* Go down until the parent is the same but the file is not */
	while (file_parent(lhs_p) != file_parent(rhs_p)) {
		lhs_p = file_parent(lhs_p);
		rhs_p = file_parent(rhs_p);
	}

	/* Compare creation time */
	return file_time(lhs_p) < file_time(rhs_p) ? lhs : rhs;
}

/* Used to traverse a list in the table in order to find a file by value. */
static void* table_list_traverse_aux(void* query_v, struct file* file) {
	struct query_data* query = query_v;

	if (strcmp(file_value(file), query->value) == 0)
		query->best = best_file(query->best, file);

	return NULL; /* Never end the traversal early */ 
}

/* Searchs for a file in the table from its value. */
struct file* table_search(struct table* table, const char* value) {
	struct query_data query;
	int h = hash(value);

	query.value = value;
	query.best = NULL;

	/* Find best file in the list */
	if (table->cells[h] != NULL)
		list_traverse(table->cells[h], &query, &table_list_traverse_aux);

	return query.best;
}