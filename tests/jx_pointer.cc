/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
extern "C" {
#include "jinks.h"
}
#include "gtest/gtest.h"

static const int ARRAY_LEN = 5;
static int sum;
static void sum_destroys(void *item) {
  int i, *vals = (int*)item;
  for (i = 0; i < ARRAY_LEN; ++i) { 
    sum += vals[i];
  }
}

class PointerTest : public ::testing::Test {
  protected:
    jx_pointer p;

    virtual void SetUp() {
      int *vals;
      jx_pointer_init(&p, ARRAY_LEN*sizeof(int), sum_destroys);

      vals = (int*)p.item;
      sum = 0;
      for (int i = 0; i < ARRAY_LEN; ++i) {
        vals[i] = i;
      }
    }

    virtual void TearDown() {
      /* shouldn't matter if it gets called twice. */
      jx_pointer_destroy(&p);
    }
};

TEST_F(PointerTest, CreateAndRelease) {
  ASSERT_TRUE(NULL != p.item);
  jx_pointer_destroy(&p);

  ASSERT_TRUE(NULL == p.item);
}

TEST_F(PointerTest, Clone) {
  jx_pointer copy;

  jx_pointer_clone(&p, &copy);

  ASSERT_TRUE(NULL != copy.item);
  ASSERT_EQ(p.item, copy.item);

  jx_pointer_destroy(&p);

  ASSERT_TRUE(NULL == p.item);
  ASSERT_TRUE(NULL != copy.item);

  int* vals = (int*)copy.item;
  for (int i = 0; i < ARRAY_LEN; ++i) {
    EXPECT_EQ(i, vals[i]);
  }

  jx_pointer_destroy(&copy);
}

TEST_F(PointerTest, DestroyCalled) {
  jx_pointer copy;
  jx_pointer_clone(&p, &copy);

  /* shouldn't destroy the array item */
  jx_pointer_destroy(&p);

  ASSERT_EQ(0, sum);

  /* now it should destroy the array item */
  jx_pointer_destroy(&copy);

  ASSERT_EQ(ARRAY_LEN*(ARRAY_LEN-1)/2, sum);
}




