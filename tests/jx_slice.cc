/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
extern "C" {
#include "jinks.h"
}
#include "gtest/gtest.h"

static const int SLICE_LEN = 10;

int get(jx_slice &self, int i) {
  return *(int*)jx_slice_at(&self, i);
}

void CheckNegativeIndices(jx_slice &self) {
  for (int i = 1; i <= self.size; ++i) {
    ASSERT_EQ(get(self, self.size - i), get(self, -i));
  }
}

class SliceTest : public ::testing::Test {
  protected:
    jx_slice s;

    virtual void SetUp() {
      jx_slice_init(&s, sizeof(int), SLICE_LEN);

      ASSERT_EQ(SLICE_LEN, s.size);
      ASSERT_EQ(sizeof(int), s.itemsize);

      for (int i = 0; i < SLICE_LEN; ++i) {
        *(int*)jx_slice_at(&s, i) = i;
      }
    }

    virtual void TearDown() {
      jx_slice_destroy(&s);
    }
};

TEST_F(SliceTest, InitAndDestroy) {
  jx_slice_destroy(&s); 
}

TEST_F(SliceTest, Values) {
  for (int i = 0; i < SLICE_LEN; ++i) {
    ASSERT_EQ(i, get(s, i));
  }
  CheckNegativeIndices(s);
}

TEST_F(SliceTest, Clone) {
  jx_slice copy;

  jx_slice_clone(&s, &copy);
  jx_slice_destroy(&s);

  ASSERT_EQ(SLICE_LEN, copy.size);
  ASSERT_EQ(sizeof(int), copy.itemsize);

  for (int i = 0; i < SLICE_LEN; ++i) {
    ASSERT_EQ(i, get(copy, i));
  }
  CheckNegativeIndices(copy);

  jx_slice_destroy(&copy);
}

TEST_F(SliceTest, Reslice) {
  jx_slice r;

  jx_slice_reslice(&s, 2, 2, 3, &r);
  ASSERT_EQ(3, r.size);
  ASSERT_EQ(sizeof(int), r.itemsize);

  ASSERT_EQ(2, get(r, 0));
  ASSERT_EQ(4, get(r, 1));
  ASSERT_EQ(6, get(r, 2));
  CheckNegativeIndices(r);
  jx_slice_destroy(&r);

  /* 7, 6, 5, 4 */
  jx_slice_reslice(&s, 7, -1, 4, &r);
  ASSERT_EQ(sizeof(int), r.itemsize);
  ASSERT_EQ(4, r.size);

  ASSERT_EQ(7, get(r, 0));
  ASSERT_EQ(6, get(r, 1));
  ASSERT_EQ(5, get(r, 2));
  ASSERT_EQ(4, get(r, 3));
  CheckNegativeIndices(r);
  jx_slice_destroy(&r);

  /* 6, 9 */
  jx_slice_reslice(&s, 6, 3, 5, &r);
  ASSERT_EQ(sizeof(int), r.itemsize);
  ASSERT_EQ(2, r.size);

  ASSERT_EQ(6, get(r, 0));
  ASSERT_EQ(9, get(r, 1));
  CheckNegativeIndices(r);
  jx_slice_destroy(&r);

  /* 8, 5, 2 */
  jx_slice_reslice(&s, -2, -3, -1, &r);
  ASSERT_EQ(sizeof(int), r.itemsize);
  ASSERT_EQ(3, r.size);

  ASSERT_EQ(8, get(r, 0));
  ASSERT_EQ(5, get(r, 1));
  ASSERT_EQ(2, get(r, 2));
  CheckNegativeIndices(r);
  jx_slice_destroy(&r);
}

TEST_F(SliceTest, CopyTo) {
  jx_slice r;
  int vals[5];

  /* 9, 7, 5, 3, 1 */
  jx_slice_reslice(&s, -1, -2, 5, &r);
  ASSERT_EQ(5, r.size);
  jx_slice_copyto(&r, vals);

  for (int i = 0; i < r.size; ++i) {
    ASSERT_EQ(get(r, i), vals[i]);
  }
  jx_slice_destroy(&r);
}

TEST_F(SliceTest, MakeArray) {
  jx_slice r;
  int *vals;

  jx_slice_reslice(&s, 2, 2, -1, &r);
  vals = (int*)jx_slice_make_array(&r);

  for (int i = 0; i < r.size; ++i) {
    ASSERT_EQ(get(r, i), vals[i]);
  }
  jx_slice_destroy(&r);
  free(vals);
}









