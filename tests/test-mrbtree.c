/*
 * Copyright (c) 2017, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <gmp.h> // For testing purpose only.
#include "m-string.h"
#include "m-rbtree.h"

static bool uint_in_str(unsigned int *u, FILE *f)
{
  int n = fscanf(f, "%u", u);
  return n == 1;
}

static void uint_out_str(FILE *f, unsigned int u)
{
  fprintf(f, "%u", u);
}
static void uint_get_str(string_t str, unsigned int u, bool append)
{
  (append ? string_cat_printf : string_printf) (str, "%u", u);
}


START_COVERAGE
RBTREE_DEF(uint, unsigned int, M_OPLIST_CAT((IN_STR(uint_in_str M_IPTR), OUT_STR(uint_out_str), GET_STR(uint_get_str)) , M_DEFAULT_OPLIST) )
RBTREE_DEF(float, float)
END_COVERAGE

#define UINT_OPLIST RBTREE_OPLIST(uint)
#define FLOAT_OP RBTREE_OPLIST(float)

static void test_uint(void)
{
  rbtree_uint_t tree, tree2;
  rbtree_uint_init(tree);
  assert (rbtree_uint_empty_p(tree));
  rbtree_uint_init_set(tree2, tree);
  assert (rbtree_uint_empty_p(tree2));
  rbtree_uint_clear(tree);

  const unsigned int max = 1001;
  for(unsigned int num = 1; num < max; num++) {
    rbtree_uint_init(tree);
    for(unsigned int i = 0; i < num; i++)
      rbtree_uint_push(tree, i);
    assert(rbtree_uint_size(tree) == num);
    assert(rbtree_uint_empty_p(tree) == false);
    unsigned int count = 0;
    for M_EACH(item, tree, UINT_OPLIST) {
        assert (count == *item);
        count ++;
      }
    assert (*rbtree_uint_min(tree) == 0);
    assert (*rbtree_uint_max(tree) == num-1);
    assert (count == num);
    rbtree_uint_set(tree2, tree);
    unsigned int *ptr = rbtree_uint_get(tree2, num/2);
    assert (ptr != NULL);
    assert (*ptr == num/2);
    rbtree_uint_clear(tree);

    bool b = rbtree_uint_pop(NULL, tree2, num+1);
    assert (b == false);
    for(unsigned int i = 0; i < num; i+=3) {
      b = rbtree_uint_pop(NULL, tree2, i);
      assert(b);
      ptr = rbtree_uint_get(tree2, i);
      assert (ptr == NULL);
    }
    for(unsigned int i = 1; i < num; i+=3) {
      b = rbtree_uint_pop(NULL, tree2, i);
      assert(b);
      ptr = rbtree_uint_get(tree2, i);
      assert (ptr == NULL);
    }
  }

  rbtree_uint_init (tree);
  assert(rbtree_uint_empty_p(tree));
  rbtree_uint_clean(tree2);
  rbtree_uint_push(tree2, 17421);
  assert(rbtree_uint_size(tree2) == 1);
  rbtree_uint_swap(tree, tree2);
  assert(rbtree_uint_empty_p(tree2));
  assert(rbtree_uint_size(tree) == 1);
  unsigned int *ptr = rbtree_uint_get(tree, 17421);
  assert(ptr != NULL && *ptr == 17421);
  
  rbtree_uint_clear(tree);
  rbtree_uint_clear(tree2);
}

static void test_float(void)
{
  M_LET(tree, FLOAT_OP) {
    for(float f = -17.42; f < 17.42; f+=0.01)
      rbtree_float_push (tree, f);
    bool p = false;
    float g;
    for M_EACH(item, tree, FLOAT_OP) {
        if (p) {
          assert (g < *item);
        }
        g = *item;
        p = true;
      }
  }
}

static void test_io(void)
{
  M_LET(str, STRING_OPLIST)
  M_LET(tree1, tree2, UINT_OPLIST) {
    // Empty one
    FILE *f = fopen ("a.dat", "wt");
    if (!f) abort();
    rbtree_uint_out_str(f, tree1);
    fclose (f);

    f = fopen ("a.dat", "rt");
    if (!f) abort();
    bool b = rbtree_uint_in_str (tree2, f);
    assert (b == true);
    assert (rbtree_uint_equal_p (tree1, tree2));
    fclose(f);

    rbtree_uint_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[]"));

    // Fill in data
    for(unsigned int i = 0 ; i < 10; i++)
      rbtree_uint_push(tree1, i);

    f = fopen ("a.dat", "wt");
    if (!f) abort();
    rbtree_uint_out_str(f, tree1);
    fclose (f);

    f = fopen ("a.dat", "rt");
    if (!f) abort();
    b = rbtree_uint_in_str (tree2, f);
    assert (b == true);
    assert (rbtree_uint_equal_p (tree1, tree2));
    fclose(f);

    rbtree_uint_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[0,1,2,3,4,5,6,7,8,9]"));
  }
  
}

int main(void)
{
  test_uint();
  test_float();
  test_io();
  exit(0);
}
