/*
 * File: 		avl.c
 * Author: 		Ricardo Antunes
 * Description: AVL tree implementation used by the filesystem.
 */

#include "adt.h"

#include <string.h>
#include <malloc.h>

/* An AVL tree node. */
struct avl {
	struct file* file;	/* File this node points to */
	struct avl* left;	/* Left (smaller) node, may be NULL */
	struct avl* right;	/* Right (bigger) node, may be NULL */
	int height;			/* Height of the sub-tree rooted on this node */
};

/* Returns the height of a sub-tree of an AVL. */
int avl_height(struct avl* avl) {
	return avl == NULL ? 0 : avl->height;	
}

/* Returns the height of a sub-tree of an AVL. */
int avl_balance_factor(struct avl* avl) {
	return avl == NULL ? 0 : avl_height(avl->left) - avl_height(avl->right);
}

/* Update the height of a sub-tree of an AVL. */
void avl_update_height(struct avl* avl) {
	int h_left = avl_height(avl->left);
	int h_right = avl_height(avl->right);
	avl->height = h_left > h_right ? h_left + 1 : h_right + 1;
}

/* Rotates left an AVL node. */
struct avl* avl_rotate_l(struct avl* avl) {
	struct avl* x = avl->right;
	avl->right = x->left;
	x->left = avl;
	avl_update_height(avl);
	avl_update_height(x);
	return x;
}

/* Rotates left an AVL node. */
struct avl* avl_rotate_r(struct avl* avl) {
	struct avl* x = avl->left;
	avl->left = x->right;
	x->right = avl;
	avl_update_height(avl);
	avl_update_height(x);
	return x;
}

/* Performs a double rotation on an AVL node (left -> right). */
struct avl* avl_rotate_lr(struct avl* avl) {
	if (avl == NULL)
		return NULL;
	avl->left = avl_rotate_l(avl->left);
	return avl_rotate_r(avl);
}

/* Performs a double rotation on an AVL node (right -> left). */
struct avl* avl_rotate_rl(struct avl* avl) {
	if (avl == NULL)
		return NULL;
	avl->right = avl_rotate_r(avl->right);
	return avl_rotate_l(avl);
}

/* Balances an AVL sub-tree and returns a pointer to the new root. */
struct avl* avl_balance(struct avl* avl) {
	int balance_factor;

	if (avl == NULL)
		return NULL;
	balance_factor = avl_balance_factor(avl);

	if (balance_factor > 1) {
		if (avl_balance_factor(avl->left) >= 0)
			avl = avl_rotate_r(avl);
		else
			avl = avl_rotate_lr(avl);
	}
	else if (balance_factor < -1) {
		if (avl_balance_factor(avl->right) <= 0)
			avl = avl_rotate_l(avl);
		else
			avl = avl_rotate_rl(avl);
	}
	else
		avl_update_height(avl);
	
	return avl;
}

/*
 * Inserts a file into an AVL. If the memory allocation fails, the tree is left
 * unchanged and NULL is returned. Otherwise a pointer to the new AVL root is
 * returned.
 */
struct avl* avl_insert(struct avl* avl, struct file* file) {
	struct avl* new;
	int cmp;

	if (avl == NULL) {
		avl = calloc(1, sizeof(struct avl));
		if (avl == NULL) /* Allocation failed */
			return NULL;
		avl->file = file;
		avl->height = 1;
	}
	else {
		if (!(cmp = strcmp(file_component(avl->file), file_component(file))))
			return avl; /* File already in the AVL, don't change anything */
		
		new = avl_insert(cmp > 0 ? avl->right : avl->left, file);
		if (new == NULL)
			return NULL; /* Allocation failed */
		cmp > 0 ? (avl->right = new) : (avl->left = new); /* Update sub-tree */
	}
	
	return avl_balance(avl);
}


/* Removes a file from an AVL. A pointer to the new AVL root is returned. */
struct avl* avl_remove(struct avl* avl, struct file* file) {
	
}

/* Frees all memory associated with an AVL tree. */
void avl_destroy(struct avl* avl) {
	avl_destroy(avl->left);
	avl_destroy(avl->right);
	free(avl);
}

/*
 * Finds a file in the AVL tree with a certain key (file->component) and returns
 * a pointer to it. If no file is found, NULL is returned.
 */
struct file* avl_find(struct avl* avl, const char* key) {
	int cmp;
	
	if (avl == NULL)
		return NULL;

	cmp = strcmp(file_component(avl->file), key);
	if (cmp < 0)
		return avl_find(avl->left, key);
	else if (cmp > 0)
		return avl_find(avl->right, key);
	return avl->file;
}

/*
 * Traverses an AVL (in-order). fn(ptr, file) is called for each file present in
 * the tree. If fn(ptr, file) returns a non-NULL value, the traversal ends early
 * and that value is returned. Otherwise, NULL is returned.
 */
void* avl_traverse(struct avl* avl, void* ptr, traverse_fn fn) {
	void* ret;

	if (avl == NULL)
		return NULL;
	if ((ret = avl_traverse(avl->left, ptr, fn)) != NULL)
		return ret;
	if ((ret = fn(ptr, avl->file)) != NULL)
		return ret;
	return avl_traverse(avl->right, ptr, fn);
}
