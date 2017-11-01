#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "avl.h"

struct avl_node {
  int val;
  struct avl_node* left;
  struct avl_node* right;
  int height;
};

struct avl {
  struct avl_node* root;
};


struct avl* avl_create() {
  struct avl* t = malloc(sizeof(struct avl));
  assert(t);
  t->root = NULL;
  return t;
}


void avl_free(struct avl* t) {
  assert(t);
  while (!avl_isempty(t)) {
    avl_remove(t->root->val, t);
  }
}


int avl_isempty(struct avl* t) {
  assert(t);
  return t->root == NULL;
}


/*
 * Helper function to generate a single AVL node containing a given value.
 */
struct avl_node* _avl_node_create(int val) {
  struct avl_node* n = malloc(sizeof(struct avl_node));
  assert(n);
  n->val = val;
  n->left = n->right = NULL;
  n->height = 0;
  return n;
}


/*
 * Helper function to get a node's height, even if the node is NULL (in which
 * case the height is -1).
 */
int _avl_get_height(struct avl_node* n) {
  if (n == NULL) {
    return -1;
  }
  return n->height;
}


/*
 * Helper function to update the height of a node, based on the height of
 * its children.
 */
void _avl_update_height(struct avl_node* n) {
  int lh = _avl_get_height(n->left);
  int rh = _avl_get_height(n->right);

  // Height is 1 more than the maximum child height.
  if (lh > rh) {
    n->height = lh + 1;
  } else {
    n->height = rh + 1;
  }
}


/*
 * This function performs a left rotation on the subtree rooted at a given
 * node.  The rotation is centered around the node's right child.  The new
 * subtree root (the rotation's center) is returned.
 */
struct avl_node* _avl_rotate_left(struct avl_node* n) {
  struct avl_node* center = n->right;

  // The center's left child and n "trade places" in the tree.
  n->right = center->left;
  center->left = n;

  _avl_update_height(n);
  _avl_update_height(center);

  return center;
}


/*
 * This function performs a right rotation on the subtree rooted at a given
 * node.  The rotation is centered around the node's right child.  The new
 * subtree root (the rotation's center) is returned.
 */
struct avl_node* _avl_rotate_right(struct avl_node* n) {
  struct avl_node* center = n->left;

  // The center's right child and n "trade places" in the tree.
  n->left = center->right;
  center->right = n;

  _avl_update_height(n);
  _avl_update_height(center);

  return center;
}


/*
 * Helper function to compute a node's balance factor, which is the signed
 * difference in heights between its right subtree and left subtree.  A
 * negative balance factor means the node is left-heavy, a positive balance
 * factor means the node is right-heavy, and a zero balance factor means the
 * node is height-balanced.
 */
int _avl_balance_factor(struct avl_node* n) {
  return _avl_get_height(n->right) - _avl_get_height(n->left);
}


struct avl_node* _avl_balance(struct avl_node* n) {

  int bf = _avl_balance_factor(n);
  if (bf < -1) {

    /*
     * Here, n is height-imbalanced on the left.  We need a double rotation if
     * n’s left child is right-heavy.
     */
    if (_avl_balance_factor(n->left) > 0) {
      n->left = _avl_rotate_left(n->left);
    }
    return _avl_rotate_right(n);

  } else if (bf > 1) {

    /*
     * Here, n is height-imbalanced on the right.  We need a double rotation if
     * n’s right child is left-heavy.
     */
    if (_avl_balance_factor(n->right) < 0) {
      n->right = _avl_rotate_right(n->right);
    }
    return _avl_rotate_left(n);

  } else {

    // Just update the height if we didn’t rotate.
    _avl_update_height(n);
    return n;

  }
}


/*
 * Helper function to insert a given value into a subtree of a AVL rooted at
 * a given node.  Operates recursively by determining into which subtree (left
 * or right) under the given node the value should be inserted and performing
 * the insertion on that subtree.
 *
 * Returns the root of the given subtree, modified to contain a new node with
 * the specified value.
 */
struct avl_node* _avl_subtree_insert(int val, struct avl_node* n) {

  if (n == NULL) {

    /*
     * If n is NULL, we know we've reached a place to insert val, so we
     * create a new node holding val and return it.
     */
    return _avl_node_create(val);

  } else if (val < n->val) {

    /*
     * If val is less than the value at n, we insert val in n's left subtree
     * (somewhere) and update n->left to point to the modified subtree (with
     * val inserted).
     */
    n->left = _avl_subtree_insert(val, n->left);

  } else {

    /*
     * If val is greater than or equal to the value at n, we insert val in n's
     * right subtree (somewhere) and update n->right to point to the modified
     * subtree (with val inserted).
     */
    n->right = _avl_subtree_insert(val, n->right);

  }

  /*
   * For the else if and else conditions, the subtree rooted at n has already
   * been modified (by setting n->left or n->right above), so we can just
   * return n here, after balancing n's subtree.
   */
  return _avl_balance(n);

}


void avl_insert(int val, struct avl* t) {

  assert(t);

  /*
   * We insert val by using our subtree insertion function starting with the
   * subtree rooted at t->root (i.e. the whole tree).
   */
  t->root = _avl_subtree_insert(val, t->root);

}


/*
 * Helper function to return the minimum value in a subtree of a AVL.
 */
int _avl_subtree_min_val(struct avl_node* n) {
  /*
   * The minimum value in any subtree is just the leftmost value.  Keep going
   * left till we get there.
   */
  while (n->left != NULL) {
    n = n->left;
  }
  return n->val;
}


/*
 * Helper function to remove a given value from a subtree of a AVL rooted at
 * a specified node.  Operates recursively by figuring out whether val is in
 * the left or the right subtree of the specified node and performing the
 * remove operation on that subtree.
 *
 * Returns the potentially new root of the given subtree, modified to have
 * the specified value removed.
 */
struct avl_node* _avl_subtree_remove(int val, struct avl_node* n) {

  if (n == NULL) {

    /*
     * If n is NULL, that means we've reached a leaf node without finding
     * the value we wanted to remove.  The tree won't be modified.
     */
    return NULL;

  } else if (val < n->val) {

    /*
     * If val is less than n, remove val from n's left subtree and update
     * n->left to point to the modified subtree (with val removed).  Return
     * n, whose subtree itself has now been modified, after balancing that
     * subtree.
     */
    n->left = _avl_subtree_remove(val, n->left);
    return _avl_balance(n);

  } else if (val > n->val) {

    /*
     * If val is greater than n, remove val from n's right subtree and update
     * n->right to point to the modified subtree (with val removed).  Return
     * n, whose subtree itself has now been modified, after balancing that
     * subtree.
     */
    n->right = _avl_subtree_remove(val, n->right);
    return _avl_balance(n);

  } else {

    /*
     * If we've reached this point, we've found a node with value val.  We
     * need to remove this node from the tree, and the way we do that will
     * differ based on whether the node has 0, 1, or 2 children.
     */
    if (n->left != NULL && n->right != NULL) {

      /*
       * If n has 2 children, we replace the value at n with the value at n's
       * in-order successor node, which is the minimum value in n's right
       * subtree.  Then we recursively remove n's in-order successor node from
       * the tree (specifically from n's right subtree).  Balance n before
       * returning it
       */
      n->val = _avl_subtree_min_val(n->right);
      n->right = _avl_subtree_remove(n->val, n->right);
      return _avl_balance(n);

    } else if (n->left != NULL) {

      /*
       * If n has only a left child, we simply delete n by freeing it and
       * returning the left child node so that it becomes the new child of
       * n's parent via the recursion.  We don't need to balance the left
       * child before returning, since it should already be balanced.
       */
      struct avl_node* left_child = n->left;
      free(n);
      return left_child;

    } else if (n->right != NULL) {

      /*
       * If n has only a right child, we simply delete n by freeing it and
       * returning the right child node so that it becomes the new child of
       * n's parent via the recursion.  We don't need to balance the right
       * child before returning, since it should already be balanced.
       */
      struct avl_node* right_child = n->right;
      free(n);
      return right_child;

    } else {

      /*
       * Otherwise, n has no children, and we can simply free it and return
       * NULL so that n's parent will lose n as a child via the recursion.
       */
      free(n);
      return NULL;

    }

  }

}


void avl_remove(int val, struct avl* t) {

  assert(t);

  /*
   * We remove val by using our subtree removal function starting with the
   * subtree rooted at t->root (i.e. the whole tree).
   */
  t->root = _avl_subtree_remove(val, t->root);

}


int avl_contains(int val, struct avl* t) {

  assert(t);

  // Iteratively search for val in t.
  struct avl_node* cur = t->root;
  while (cur != NULL) {

    if (val == cur->val) {

      // We found the value we're looking for in cur.
      return 1;

    } else if (val < cur->val) {

      /*
       * The value we're looking for is less than the value at cur, so we
       * branch left.
       */
      cur = cur->left;

    } else {

      /*
       * The value we're looking for is greater than or equal to the value at
       * cur, so we branch right.
       */
       cur = cur->right;

    }

  }

  /*
   * If we make it to a leaf node (i.e. cur is NULL), we didn't find what we
   * were looking for.
   */
  return 0;

}


void _avl_subtree_print(struct avl_node* n, int level) {
  if (n == NULL) {
    return;
  }
  for (int i = 0; i < level; i++) {
    printf("  ");
  }
  printf("%d\n", n->val);
  _avl_subtree_print(n->left, level + 1);
  _avl_subtree_print(n->right, level + 1);
}


void avl_print(struct avl* t){
  assert(t);
  if (t->root == NULL) {
    printf("EMPTY\n");
  } else {
    _avl_subtree_print(t->root, 0);
  }
}
