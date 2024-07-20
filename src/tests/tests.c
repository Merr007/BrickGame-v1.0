
#include <check.h>
#include <stdio.h>
#include <unistd.h>

#include "tests_back.h"

int main(void) {
  Suite *cases = suite_tests_back();
  SRunner *runner = srunner_create(cases);
  srunner_run_all(runner, CK_NORMAL);
  srunner_free(runner);

  return 0;
}