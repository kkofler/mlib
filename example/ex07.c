#if 1 // Non macro version.

#include "m-dict.h"
DICT_DEF2(m32, unsigned int, M_DEFAULT_OPLIST, char, M_DEFAULT_OPLIST)

int main(void) {
  dict_m32_t h;
  dict_m32_init(h);                         // h is init.
  dict_m32_set_at (h, 5, 10);               // h[5] = 10
  char *k = dict_m32_get(h, 10);            // k == NULL
  int is_missing = (k != NULL);             // true
  assert (is_missing);
  dict_m32_remove(h, 5);                    // h is now empty
  dict_it_m32_t it;                         // iterate over all dictionnary
  for (dict_m32_it (it, h) ; !dict_m32_end_p (it); dict_m32_next(it)) {
    dict_pair_m32_t *item = dict_m32_ref(it);
    (*item)->value = 1;                        // Set its value to 1
  }
  dict_m32_clear(h);                        // h is cleared
  return 0;
}

#else // Use of M_LET & M_FOR macros (otherwise equivalent)

#include "m-dict.h"
DICT_DEF2(m32, unsigned int, M_DEFAULT_OPLIST, char, M_DEFAULT_OPLIST)
#define M32_OPLIST DICT_OPLIST(m32, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST)
int main(void) {
  M_LET(h, M32_OPLIST) {                      // h is init
    dict_m32_set_at (h, 5, 10);               // h[5] = 10
    char *k = dict_m32_get(h, 10);            // k == NULL
    int is_missing = (k != NULL);             // true
    assert (is_missing);
    dict_m32_remove(h, 5);                    // h is now empty
    for M_EACH(item, h, M32_OPLIST) {         // traverse each item
        (*item)->value = 1;                      // Set its value to 1
      }
  }                                           // h is cleared
  return 0;
}

#endif

     
