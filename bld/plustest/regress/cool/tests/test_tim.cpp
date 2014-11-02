//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//

#include <cool/misc.h>
#include <cool/Timer.h>
#include <cool/test.h>

void test_timer() {
  CoolTimer timer;
  // Note: This is just a touch test, to ensure things at least execute.
  TEST("mark", (timer.mark(), TRUE), TRUE);
  TEST("all_usec", timer.all_usec()>=0, TRUE);
  TEST("user_usec", timer.user_usec()>=0, TRUE);
  TEST("system_usec", timer.system_usec()>=0, TRUE);
  TEST("all", timer.all()>=0, TRUE);
  TEST("system", timer.system()>=0, TRUE);
  TEST("user", timer.user()>=0, TRUE);
  TEST("real", timer.real()>=0, TRUE);
}

int main (void) {
  START("CoolTimer");
  test_timer();
  SUMMARY();
  return 0;
}
