#include <stdio.h>
#include <assert.h>

#include "avl.h"

#define NUM_TEST_ELEMS 16

int main(int argc, char** argv) {

  struct avl* t = avl_create();

  // Initialize the tree.
  for (int i = 0; i < NUM_TEST_ELEMS; i++) {
    avl_insert(i, t);
  }

  // Test containment.
  for (int i = 0; i < NUM_TEST_ELEMS; i++) {
    assert(avl_contains(i, t) == 1);
  }

  avl_print(t);

}
