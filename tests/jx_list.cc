/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "gtest/gtest.h"
extern "C" {
#include "jinks.h"
}

static const int num = 100;

static int destroy_calls;

static void mydestroy(void *item) {
  ASSERT_TRUE(NULL != item);
  ++destroy_calls;
}

typedef void (*initer)(jx_list *out_self, size_t isz, jx_destructor destroy);

class ListTest : public ::testing::TestWithParam<initer> {
  protected:
    jx_list list;

    virtual void SetUp() {
      initer f = GetParam();
      f(&list, sizeof(int), mydestroy);
      destroy_calls = 0;
    }

    virtual void TearDown() {
      jx_list_destroy(&list);
    }
};

INSTANTIATE_TEST_CASE_P(Lists, ListTest,
    ::testing::Values(jx_list_init_vector));

static void CheckNegativeIndices(jx_list &list) {
  for (int i = 1; i <= list.size; ++i) {
    ASSERT_EQ(*(int*)jx_list_at(&list, list.size - i),
        *(int*)jx_list_at(&list, -i));
  }
}

TEST_P(ListTest, InitAndDestroy) {
  ASSERT_EQ(sizeof(int), list.itemsize);
  ASSERT_EQ(0, list.size);
  jx_list_destroy(&list);
}

TEST_P(ListTest, AppendOnly) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 0; i < list.size; ++i) {
    int *item = (int*)jx_list_at(&list, i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(i, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, PrependOnly) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_prepend(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 1; i <= list.size; ++i) {
    int *item = (int*)jx_list_at(&list, -i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(i - 1, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, InsertOnly) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_insert(&list, list.size/2);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 0; i < list.size/2; ++i) {
    int *item = (int*)jx_list_at(&list, i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(2*i+1, *item);
    item = (int*)jx_list_at(&list, -(i+1));
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(2*i, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, AllAddMethods) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = 2*num + i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = num - 1; i >= 0; --i) {
    int *item = (int*)jx_list_prepend(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(2*num, list.size);

  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_insert(&list, num + i);
    ASSERT_TRUE(NULL != item);
    *item = num + i;
  }
  ASSERT_EQ(3*num, list.size);

  for (int i = 0; i < list.size; ++i) {
    int *item = (int*)jx_list_at(&list, i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(i, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, AddThenRemove) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 0; i < list.size; ++i) {
    jx_list_remove(&list, i);
  }
  ASSERT_EQ(num/2, list.size);

  for (int i = 0; i < list.size; ++i) {
    int *item = (int*)jx_list_at(&list, i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(2*i+1, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, AddThenRemoveThenAdd) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 0; i < list.size; ++i) {
    jx_list_remove(&list, i);
  }
  ASSERT_EQ(num/2, list.size);

  for (int i = 0; i < list.size; i+=2) {
    int *item = (int*)jx_list_insert(&list, i);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 0; i < list.size; ++i) {
    int *item = (int*)jx_list_at(&list, i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(i, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, Clear) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  jx_list_clear(&list);
  ASSERT_EQ(0, list.size);

  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  for (int i = 0; i < list.size; ++i) {
    int *item = (int*)jx_list_at(&list, i);
    ASSERT_TRUE(NULL != item);
    ASSERT_EQ(i, *item);
  }
  CheckNegativeIndices(list);
}

TEST_P(ListTest, DestroyCalled) {
  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);

  while (list.size > 0) {
    jx_list_remove(&list, -1);
    ASSERT_EQ(num, destroy_calls + list.size);
  }
  ASSERT_EQ(0, list.size);

  for (int i = 0; i < num; ++i) {
    int *item = (int*)jx_list_append(&list);
    ASSERT_TRUE(NULL != item);
    *item = i;
  }
  ASSERT_EQ(num, list.size);
  jx_list_clear(&list);
  ASSERT_EQ(2*num, destroy_calls);
}




