#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#define NDEBUG

#include "kvec.h"
#include "klist.h"
#include "kbtree.h"
#include "khash.h"

unsigned long g_result;

static unsigned long long
cputime (void)
{
  struct rusage rus;
  getrusage (0, &rus);
  return rus.ru_utime.tv_sec * 1000000ULL + rus.ru_utime.tv_usec;
}

static unsigned int randValue = 0;
static void rand_init(void)
{
  randValue = 0;
}
static unsigned int rand_get(void)
{
  randValue = randValue * 31421U + 6927U;
  return randValue;
}

static void test_function(const char str[], size_t n, void (*func)(size_t))
{
  unsigned long long start, end;
  //  (*func)(n);
  start = cputime();
  (*func)(n);
  end = cputime();
  end = (end - start) / 1000U;
  printf ("%s %Lu ms for n = %lu\n", str, end, n);
}

/********************************************************************************************/

static void test_array(size_t n)
{
  rand_init();
  kvec_t(unsigned int) a1, a2;

  kv_init(a1);
  kv_init(a2);
  
  for(size_t i = 0; i < n; i++) {
    kv_push(unsigned int, a1, rand_get() );
    kv_push(unsigned int, a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += kv_A(a1, i ) * kv_A(a2, i );
  }
  g_result = s;

  kv_destroy(a1);
  kv_destroy(a2);
}

/********************************************************************************************/

#define __int_free(x)
KLIST_INIT(32, unsigned int, __int_free)

static void test_list (size_t n)
{
  rand_init();
  klist_t(32) *a1, *a2;
  a1 = kl_init(32);
  a2 = kl_init(32);

  for(size_t i = 0; i < n; i++) {
    *kl_pushp (32, a1) = rand_get();
    *kl_pushp (32, a2) = rand_get();
  }
  unsigned int s = 0;
  kliter_t(32) *it1, *it2;
  for(it1 = kl_begin(a1), it2 = kl_begin(a2);
      it1 != kl_end(a1);
      it1 = kl_next(it1), it2 = kl_next(it2)) {
    s += kl_val(it1) * kl_val(it2);
  }
  g_result = s;

  kl_destroy(32, a1);
  kl_destroy(32, a2);
}

/********************************************************************************************/

#define my_cmp(a,b) ( (a) < (b) ? -1 : (a) > (b) )

KBTREE_INIT(kTree, unsigned long, my_cmp)

static void test_rbtree(size_t n)
{
  rand_init();
  kbtree_t(kTree) *tree;
  tree = kb_init(kTree, KB_DEFAULT_SIZE/2);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    kb_putp(kTree, tree, &j);
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long j = 0;
    if (kb_getp(kTree, tree, &j) != 0)
      s+= j;
  }
  g_result = s;
  kb_destroy(kTree, tree);
}

/********************************************************************************************/

#define hash_eq(a, b) ((a) == (b))
#define hash_func(a) kh_int_hash_func2(a)

KHASH_INIT(iun, unsigned long, unsigned long, 1, hash_func, hash_eq)

static void
test_dict(unsigned long  n)
{
  rand_init();
  khash_t(iun) *dict = kh_init(iun);
  
  for (size_t i = 0; i < n; i++) {
    int ret;
    khiter_t k = kh_put(iun, dict, rand_get(), &ret);
    kh_value(dict, k) = rand_get();
  }
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    khiter_t k = kh_get(iun, dict, rand_get());
    if (kh_exist(dict, k))
      s += kh_value(dict, k);
  }
  g_result = s;
  
  kh_destroy(iun, dict);
}

/********************************************************************************************/

typedef struct {
  char a[256];
} char_array_t;
static inline void char_init (char_array_t *a) { a->a[0] = 0; }
static inline void char_set (char_array_t *a, const char_array_t b) { strcpy(a->a, b.a); }
static inline bool char_equal_p (const char_array_t a, const char_array_t b) { return strcmp(a.a,b.a)==0; }
static inline size_t char_hash(const char_array_t a) {
  size_t hash = 0;
  const char *s = a.a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

KHASH_INIT(iub, char_array_t, char_array_t, 1, char_hash, char_equal_p)

static void
test_dict_big(unsigned long  n)
{
  rand_init();
  khash_t(iub) *dict = kh_init(iub);

  for (size_t i = 0; i < n; i++) {
    int ret;
    char_array_t s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    khiter_t k = kh_put(iub, dict, s1, &ret);
    char_set(&kh_value(dict, k), s2);
  }
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1.a, "%u", rand_get());
    khiter_t k = kh_get(iub, dict, s1);
    if (kh_exist(dict, k))
      s ++;
  }
  g_result = s;

  kh_destroy(iub, dict);
}

/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]): 0;
  if (n == 1)
    test_function("List   time",10000000, test_list);
  if (n == 2)
    test_function("Array  time", 100000000, test_array);
  if (n == 3)
    test_function("B-tree time", 1000000, test_rbtree);
  if (n == 4)
    test_function("Dict   time", 1000000, test_dict);
  if (n == 6)
    test_function("DictB  time", 1000000, test_dict_big);
}

