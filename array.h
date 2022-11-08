#ifndef _TYPED_ARRAY_H_
#define _TYPED_ARRAY_H_

/**
 * Implements a code generated generic array for a certain type.
 * For example, call with MAKE_ARRAY_CODE(int, int_array_).
 * This will result in the following prototypes:
 *
 * typedef struct { } int_array_t;
 * void     int_array_init(int_array_t* list);
 * void     int_array_free(int_array_t* list);
 * void     int_array_push(int_array_t* list, int elt);
 * int      int_array_get (int_array_t* list, unsigned index, int* elt);
 * int      int_array_set (int_array_t* list, unsigned index, int elt);
 * int      int_array_rem (int_array_t* list, unsigned index, int* elt);
 * void     int_array_ins (int_array_t* list, unsigned index, int elt);
 * int      int_array_pop (int_array_t* list, int* elt);
 * int      int_array_peek(int_array_t* list, int* elt);
 * unsigned int_array_size(int_array_t* list);
 *
 */

#include <stdlib.h>

#define COMBINE(a, b) a##b

#define MAKE_ARRAY_HEADER(T, prefix)                                \
  typedef struct {                                                  \
    T*        list;                                                 \
    unsigned  count;                                                \
    unsigned  allocated;                                            \
  } COMBINE(prefix, t);                                             \
  extern                                                            \
  void COMBINE(prefix, init)(COMBINE(prefix, t)* list);             \
  extern                                                            \
  void COMBINE(prefix, push)(COMBINE(prefix, t)* list, T elt);      \
  extern                                                            \
  unsigned COMBINE(prefix, size)(COMBINE(prefix, t)* list);         \
  extern                                                            \
  int COMBINE(prefix, peek)(COMBINE(prefix, t)* list, T* elt);      \
  extern                                                            \
  int COMBINE(prefix, pop)(COMBINE(prefix, t)* list, T* elt);       \
  extern                                                            \
  int COMBINE(prefix, get)(COMBINE(prefix, t)* list, unsigned index, T* elt);\
  extern                                                            \
  int COMBINE(prefix, set)(COMBINE(prefix, t)* list, unsigned index, T elt);\
  extern                                                            \
  int COMBINE(prefix, rem)(COMBINE(prefix, t)* list, unsigned index, T* elt);\
  extern                                                            \
  void COMBINE(prefix, ins)(COMBINE(prefix, t)* list, unsigned index, T elt);\
  extern                                                            \
  void COMBINE(prefix, free)(COMBINE(prefix, t)* list);             \

#define MAKE_ARRAY_CODE(T, prefix)                                  \
  void COMBINE(prefix, init)(COMBINE(prefix, t)* list) {            \
    memset(list, 0, sizeof(*list));                                 \
  }                                                                 \
                                                                    \
  void COMBINE(prefix, free)(COMBINE(prefix, t)* list) {            \
    if (list->list) { free(list->list); }                           \
    memset(list, 0, sizeof(*list));                                 \
  }                                                                 \
                                                                    \
  void COMBINE(prefix, push)(COMBINE(prefix, t)* list, T elt) {     \
    if (list->count >= list->allocated) {                           \
      list->allocated = list->count + 16;                           \
      list->list = realloc(                                         \
        list->list,                                                 \
        sizeof(T) * list->allocated                                 \
      );                                                            \
      if (list->list == NULL) { abort(); }                          \
    }                                                               \
    list->list[ (list->count)++ ] = elt;                            \
  }                                                                 \
                                                                    \
  unsigned COMBINE(prefix, size)(COMBINE(prefix, t)* list) {        \
    return list->count;                                             \
  }                                                                 \
                                                                    \
  int COMBINE(prefix, peek)(COMBINE(prefix, t)* list, T* elt) {     \
    if (list->count) {                                              \
      *elt = list->list[ list->count ];                             \
      return 0;                                                     \
    } else {                                                        \
      return ~0;                                                    \
    }                                                               \
  }                                                                 \
                                                                    \
  int COMBINE(prefix, pop)(COMBINE(prefix, t)* list, T* elt) {      \
    if (list->count) {                                              \
      if (elt) { *elt = list->list[ --(list->count) ]; }            \
      if (list->count == 0) {                                       \
        free(list->list);                                           \
        list->list = 0;                                             \
        list->allocated = 0;                                        \
      } else if (list->count < list->allocated - 32) {              \
        list->allocated = list->count + 16;                         \
        list->list = realloc(                                       \
          list->list,                                               \
          sizeof(T) * list->allocated                               \
        );                                                          \
        if (list->list == NULL) { abort(); }                        \
      }                                                             \
      return 0;                                                     \
    } else {                                                        \
      return ~0;                                                    \
    }                                                               \
  }                                                                 \
                                                                    \
  int COMBINE(prefix, get)(COMBINE(prefix, t)* list, unsigned index, T* elt) {\
    if (index < list->count) {                                      \
      if (elt) { *elt = list->list[ index ]; }                      \
      return 0;                                                     \
    } else {                                                        \
      return ~0;                                                    \
    }                                                               \
  }                                                                 \
                                                                    \
  int COMBINE(prefix, rem)(COMBINE(prefix, t)* list, unsigned index, T* elt) {\
    if (index < list->count) {                                      \
      if (elt) { *elt = list->list[ index ]; }                      \
      memmove(                                                      \
        &(list->list[ index ]),                                     \
        &(list->list[ index + 1]),                                  \
        sizeof(T) * (list->count - (index+1))                       \
      );                                                            \
      --(list->count);                                              \
      if (list->count == 0) {                                       \
        free(list->list);                                           \
        list->list = 0;                                             \
        list->allocated = 0;                                        \
      } else if (list->count < list->allocated - 32) {              \
        list->allocated = list->count + 16;                         \
        list->list = realloc(                                       \
          list->list,                                               \
          sizeof(T) * list->allocated                               \
        );                                                          \
        if (list->list == NULL) { abort(); }                        \
      }                                                             \
      return 0;                                                     \
    } else {                                                        \
      return ~0;                                                    \
    }                                                               \
  }                                                                 \
                                                                    \
  int COMBINE(prefix, set)(COMBINE(prefix, t)* list, unsigned index, T elt) {\
    if (index < list->count) {                                      \
      list->list[ index ] = elt;                                    \
      return 0;                                                     \
    } else {                                                        \
      return ~0;                                                    \
    }                                                               \
  }                                                                 \
                                                                    \
  void COMBINE(prefix, ins)(COMBINE(prefix, t)* list, unsigned index, T elt) {\
    if (list->count >= list->allocated) {                           \
      list->allocated = list->count + 16;                           \
      list->list = realloc(                                         \
        list->list,                                                 \
        sizeof(T) * list->allocated                                 \
      );                                                            \
      if (list->list == NULL) { abort(); }                          \
    }                                                               \
    if (index < list->count) {                                      \
      memmove(                                                      \
        &(list->list[ index + 1]),                                  \
        &(list->list[ index ]),                                     \
        sizeof(T) * (list->count - index)                           \
      );                                                            \
      list->list[ index ] = elt;                                    \
      ++(list->count);                                              \
    } else {                                                        \
      COMBINE(prefix, push)(list, elt);                             \
    }                                                               \
  }                                                                 \

#endif
