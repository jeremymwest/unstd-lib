/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jinks.h"

#ifdef JX_TESTING

/* the value that represents a pass */
const jx_test JX_PASS = { NULL, NULL, NULL, 0 };

// this automatically generated file includes all the test definitions
#include "test_list.in"

static char line[128];
static char dots[75];

bool run_test(int i, int count) {
  const struct unit_test *test = &all_tests[i];
  size_t len;
  jx_test result;

  sprintf(line, "Running %s (%d of %d) ", test->name, i+1, count);
  len = strlen(line);
  if (len < sizeof(dots)) { // pad with dots 
    printf("\n%s%s ", line, &dots[len]);
  } else {
    printf("\n%s ", line);
  }
  result = test->func();

  if (NULL == result.file) {
    printf("PASS");
    return true;
  } else {
    printf("FAIL");
    printf("\n\tAssertion failed in file (%s) on line (%d): %s.\n\t\t%s", 
        result.file, result.line, result.expr, result.msg);
    return false;
  }
}

int main() {
  int i, count = (int) ( sizeof(all_tests) / sizeof(struct unit_test) );
  int failures = 0;

  for (i = 0; i < sizeof(dots); ++i) {
    dots[i] = '.';
  }

  for (i = 0; i < count; ++i) {
    if (!run_test(i, count)) {
      ++failures;
    }
  }
  printf("\n");
  printf("\nResults: %d of %d tests passed.\n", count-failures, count);

  return 0;
}

#endif



