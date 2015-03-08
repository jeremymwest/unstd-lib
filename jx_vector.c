/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_vector.h"
#include "jx_buffer.h"
#define VALID(self) \
  JX_NOT_NEG(self->count); \
  JX_POSITIVE(self->itemsize)

jx_result jx_vector_init(jx_vector *out_self, size_t isz, int capacity,
    jx_destructor destroy) {

   JX_NOT_NULL(out_self);
   JX_POSITIVE(isz);
   JX_NOT_NEG(capacity);

   out_self->itemsize = isz;
   out_self->destroy = destroy;
   out_self->count = 0;

   return jx_buffer_init(&out_self->buffer, isz*capacity);
}

void jx_vector_destroy(void *vector) {
  jx_vector *self = vector;
  VALID(self);

  jx_vector_clear(self);
  jx_buffer_destroy(&self->buffer);

  JX_CLEAR(self);
}

/******************************************************************************/

bool jx_vector_isempty(const jx_vector *self) {
  VALID(self);

  return !self->count;
}

int jx_vector_size(const jx_vector *self) {
  VALID(self);

  return self->count;
}

int jx_vector_capacity(const jx_vector *self) {
  VALID(self);

  return jx_buffer_size(&self->buffer) / self->itemsize;
}

jx_result jx_vector_reserve(jx_vector *self, int num) {
  VALID(self);
  JX_NOT_NEG(num);

  /* only resize if the request is larger than current size */
  if (num > self->count) {  
    size_t cap = num*self->itemsize;
    return jx_buffer_resize(&self->buffer, cap);
  }
  return JX_OK;
}

jx_result jx_vector_shrink(jx_vector *self) {
  VALID(self);

  return jx_buffer_resize(&self->buffer, self->count*self->itemsize);
}

/******************************************************************************/

void* jx_vector_front(const jx_vector *self) {
  return jx_vector_at(self, 0);
}

void* jx_vector_back(const jx_vector *self) {
  return jx_vector_at(self, self->count-1);
}


void* jx_vector_at(const jx_vector *self, int i) {
  VALID(self);
  JX_RANGE(i, -self->count, self->count);

  i = (i >= 0 ? i : self->count + i);
  return jx_buffer_at(&self->buffer, i*self->itemsize);
}

void jx_vector_set(jx_vector *self, int i, int num, const void *item) {
  void *dest;
  VALID(self);
  JX_INTERVAL(i, num, -self->count, self->count);

  i = (i >= 0 ? i : self->count + i);
  dest = jx_buffer_at(&self->buffer, i*self->itemsize);
  memcpy(dest, item, num*self->itemsize);
}

void* jx_vector_data(const jx_vector *self) {
  VALID(self);
  return jx_buffer_data(&self->buffer);
}

/******************************************************************************/


jx_result jx_vector_prepend(jx_vector *self, int num, jx_outptr out_ptr) {
  return jx_vector_insert(self, 0, num, out_ptr);
}

jx_result jx_vector_append(jx_vector *self, int num, jx_outptr out_ptr) {
  VALID(self);
  JX_POSITIVE(num);

  self->count += num;
  JX_TRY(jx_buffer_resize(&self->buffer, self->count * self->itemsize));
  JX_SET(out_ptr, jx_buffer_at(&self->buffer, 
        (self->count-num)*self->itemsize));
  
  return JX_OK;
}

jx_result jx_vector_insert(jx_vector *self, int i, int num, jx_outptr out_ptr) {
  VALID(self);
  JX_POSITIVE(num);
  JX_RANGE(i, -self->count, self->count+1);

  i = (i >= 0 ? i : self->count + i);
  self->count += num;
  JX_TRY(jx_buffer_reserve(&self->buffer, self->count*self->itemsize));
  /* slide items over from index i to create space for the inserted items */
  jx_buffer_move_tail(&self->buffer, i*self->itemsize, num*self->itemsize);
  JX_SET(out_ptr, jx_buffer_at(&self->buffer, i*self->itemsize));

  return JX_OK;
}

/******************************************************************************/

void jx_vector_remove(jx_vector *self, int i, int num) {
  VALID(self);
  JX_INTERVAL(i, num, -self->count, self->count);

  if (num > 0) {
    i = (i >= 0 ? i : i + self->count);
    self->count -= num;

    /* call destructor on all removed items. */
    jx_destroy_range(self->destroy, num, self->itemsize,
        jx_buffer_at(&self->buffer, i*self->itemsize));

    jx_buffer_move_tail(&self->buffer, (i+num)*self->itemsize,
        -num*((int)self->itemsize));
    jx_buffer_resize(&self->buffer, self->count*self->itemsize);
  }
}

void jx_vector_pop_back(jx_vector *self, int num) {
  jx_vector_remove(self, -num, num);
}

void jx_vector_clear(jx_vector *self) {
  VALID(self);

  /* call destructor on all items */
  jx_destroy_range(self->destroy, self->count, self->itemsize, 
      jx_buffer_data(&self->buffer));

  self->count = 0;
}

/******************************************************************************/

#ifdef JX_TESTING

static jx_vector vec_var, *vec = &vec_var;

jx_test vector_init() {
  JX_CATCH(jx_vector_init(vec, sizeof(int), 10, NULL));
  JX_EXPECT(0 == jx_vector_size(vec),
      "The initial count is incorrect.");
  JX_EXPECT(true == jx_vector_isempty(vec),
      "The vector should be empty, but isn't.");
  jx_vector_destroy(vec);
  return JX_PASS;
}

jx_test vector_sizing() {
  JX_CATCH(jx_vector_init(vec, sizeof(int), 10, NULL));
  JX_EXPECT(10 <= jx_vector_capacity(vec),
      "The initial capacity wasn't set correctly.");
  JX_CATCH(jx_vector_reserve(vec, 20));
  JX_EXPECT(20 <= jx_vector_capacity(vec),
      "Vector doesn't have as much capacity as requested.");
  JX_CATCH(jx_vector_shrink(vec));
  JX_EXPECT(0 == jx_vector_capacity(vec),
      "Shrunken vector contains more capacity than needed.");
  jx_vector_destroy(vec);
  return JX_PASS;
}

static jx_test check_vector_contents(jx_vector *self) {
  int i, *val;

  JX_EXPECT(0 == *(int*)jx_vector_front(self),
      "Incorrect first item.");
  JX_EXPECT(jx_vector_size(self)-1 == *(int*)jx_vector_back(vec),
      "Incorrect last item.");

  for (i = 0; i < jx_vector_size(self); ++i) {
    val = jx_vector_at(self, i);
    JX_EXPECT(i == *val, "Incorrect vector contents.");
  }

  for (i = 1; i<= jx_vector_size(self); ++i) {
    val = jx_vector_at(self, -i);
    JX_EXPECT(*val == jx_vector_size(self) - i,
        "Incorrect vector contents when iterated backwards.");
  }

  return JX_PASS;
}

jx_test vector_append() {
  jx_test results;
  int i, *val = NULL;

  JX_CATCH(jx_vector_init(vec, sizeof(int), 0, NULL));

  JX_CATCH(jx_vector_append(vec, 1, &val));
  JX_EXPECT(1 == jx_vector_size(vec), "Incorrect vector size after appending.");
  JX_EXPECT(NULL != val, "Output pointer not set after appending.");
  *val = 0;

  JX_CATCH(jx_vector_append(vec, 3, &val));
  JX_EXPECT(4 == jx_vector_size(vec), "Incorrect vector size after appending.");
  JX_EXPECT(NULL != val, "Output pointer not set after appending.");
  *val++ = 1;
  *val++ = 2;
  *val = 3;

  results = check_vector_contents(vec);
  jx_vector_destroy(vec);
  return results;
}

jx_test vector_prepend_and_insert() {
  jx_test results;
  int i, *val = NULL;

  JX_CATCH(jx_vector_init(vec, sizeof(int), 0, NULL));

  JX_CATCH(jx_vector_prepend(vec, 2, &val));
  JX_EXPECT(2 == jx_vector_size(vec), "Incorrect vector size.");
  JX_EXPECT(NULL != val, "Output pointer not set.");

  val[0] = 5; val[1] = 6; val = NULL;

  JX_CATCH(jx_vector_prepend(vec, 2, &val));
  JX_EXPECT(4 == jx_vector_size(vec), "Incorrect vector size.");
  JX_EXPECT(NULL != val, "Output pointer not set.");

  val[0] = 0; val[1] = 1; val = NULL;

  JX_CATCH(jx_vector_insert(vec, 2, 3, &val));
  JX_EXPECT(7 == jx_vector_size(vec), "Incorrect vector size.");
  JX_EXPECT(NULL != val, "Output pointer not set.");

  val[0] = 2; val[1] = 3; val[2] = 4; val = NULL;

  results = check_vector_contents(vec);
  jx_vector_destroy(vec);
  return results;
}

static int remove_counts = 0;
static int items_removed[10];

static void kill_int(void *item) {
  int* val = item;
  /* don't overflow test buffer! */
  if (remove_counts < 10) 
    items_removed[remove_counts++] = *val;
}

jx_test vector_remove() {
  jx_test results;
  int i, *val = NULL;

  remove_counts = 0;

  JX_CATCH(jx_vector_init(vec, sizeof(int), 0, kill_int));
  JX_CATCH(jx_vector_append(vec, 10, &val));
  JX_EXPECT(10 == jx_vector_size(vec), "Incorrect vector size.");
  for (i = 0; i < 10; ++i) {
    val[i] = i;
  }

  jx_vector_remove(vec, 2, 3);
  JX_EXPECT(7 == jx_vector_size(vec), "Incorrect vector size.");
  JX_EXPECT(3 == remove_counts, "Destroy not called.");
  JX_EXPECT(2 == items_removed[0], "Destroy not called on correct items.");
  JX_EXPECT(3 == items_removed[1], "Destroy not called on correct items.");
  JX_EXPECT(4 == items_removed[2], "Destroy not called on correct items.");

  jx_vector_remove(vec, 4, 2);
  JX_EXPECT(5 == jx_vector_size(vec), "Incorrect vector size.");
  JX_EXPECT(5 == remove_counts, "Destroy not called.");
  JX_EXPECT(7 == items_removed[3], "Destroy not called on correct items.");
  JX_EXPECT(8 == items_removed[4], "Destroy not called on correct items.");

  JX_EXPECT(0 == *(int*)jx_vector_at(vec, 0), "Incorrect vector contents.");
  JX_EXPECT(1 == *(int*)jx_vector_at(vec, 1), "Incorrect vector contents.");
  JX_EXPECT(5 == *(int*)jx_vector_at(vec, 2), "Incorrect vector contents.");
  JX_EXPECT(6 == *(int*)jx_vector_at(vec, 3), "Incorrect vector contents.");
  JX_EXPECT(9 == *(int*)jx_vector_at(vec, 4), "Incorrect vector contents.");

  jx_vector_destroy(vec);

  JX_EXPECT(10 == remove_counts, "Destroy not called on vector destruction.");
  JX_EXPECT(0 == items_removed[5], "Destroy not called on correct items.");
  JX_EXPECT(1 == items_removed[6], "Destroy not called on correct items.");
  JX_EXPECT(5 == items_removed[7], "Destroy not called on correct items.");
  JX_EXPECT(6 == items_removed[8], "Destroy not called on correct items.");
  JX_EXPECT(9 == items_removed[9], "Destroy not called on correct items.");
  remove_counts = 0;
  return JX_PASS;
}

jx_test vector_pop_back() {
  jx_test results;
  int vals[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  JX_CATCH(jx_vector_init(vec, sizeof(int), 0, NULL));
  JX_CATCH(jx_vector_append(vec, 10, NULL));
  jx_vector_set(vec, 0, 10, vals);

  jx_vector_pop_back(vec, 4);
  JX_EXPECT(6 == jx_vector_size(vec), "Incorrect vector size.");

  results = check_vector_contents(vec);
  jx_vector_destroy(vec);
  return results;
}

jx_test vector_clear() {
  int vals[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  JX_CATCH(jx_vector_init(vec, sizeof(int), 0, NULL));
  JX_CATCH(jx_vector_append(vec, 10, NULL));
  jx_vector_set(vec, 0, 10, vals);

  jx_vector_clear(vec);
  JX_EXPECT(0 == jx_vector_size(vec), "Incorrect vector size.");
  jx_vector_destroy(vec);
  return JX_PASS;
}

#endif /* unit testing section */



