/*
 * A definition for an AVL tree implementation.
 */

#ifndef __AVL_H
#define __AVL_H

/*
 * Structure used to represent an AVL tree.
 */
struct avl;

/*
 * Creates a new, empty binary search tree.
 */
struct avl* avl_create();

/*
 * Free the memory associated with an AVL tree.
 */
void avl_free(struct avl* t);

/*
 * Returns 1 if the given binary search tree is empty or 0 otherwise.
 */
int avl_isempty(struct avl* t);

/*
 * Inserts a given value into an existing binary search tree
 */
void avl_insert(int val, struct avl* t);

/*
 * Removes a given value from an existing binary search tree.
 */
void avl_remove(int val, struct avl* t);

/*
 * Determines whether an AVL tree contains a given value.
 */
int avl_contains(int val, struct avl* t);

/*
 * Prints an AVL tree to stdout.
 */
void avl_print(struct avl* t);


#endif
