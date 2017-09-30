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

#include <cool/Random.h>
#include <cool/misc.h>
#include <cool/test.h>

void test_random() {
  double f;
  int i;
  Boolean status;
  CoolRandom r1 (SIMPLE);
  TEST ("CoolRandom r1(SIMPLE)", 1, 1);
  TEST ("r1.next()", (f=r1.next(), (f >= 0.0 && f <= 100.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r1.next() != r1.next());
  TEST ("for(i=0;i<100;i++) r1.next() != r1.next()", status, TRUE);
  CoolRandom r2 (SHUFFLE);
  TEST ("CoolRandom r2(SHUFFLE)", 1, 1);
  TEST ("r2.next()", (f=r2.next(), (f >= 0.0 && f <= 100.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r2.next() != r2.next());
  TEST ("for(i=0;i<100;i++) r2.next() != r2.next()", status, TRUE);
  CoolRandom r3 (ONE_CONGRUENTIAL);
  TEST ("CoolRandom r3(ONE_CONGRUENTIAL)", 1, 1);
  TEST ("r3.next()", (f=r3.next(), (f >= 0.0 && f <= 100.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r3.next() != r3.next());
  TEST ("for(i=0;i<100;i++) r3.next() != r3.next()", status, TRUE);
  CoolRandom r4 (THREE_CONGRUENTIAL);
  TEST ("CoolRandom r4(THREE_CONGRUENTIAL)", 1, 1);
  TEST ("r4.next()", (f=r4.next(), (f >= 0.0 && f <= 100.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r4.next() != r4.next());
  TEST ("for(i=0;i<100;i++) r4.next() != r4.next()", status, TRUE);
  CoolRandom r5 (SUBTRACTIVE);
  TEST ("CoolRandom r5(SUBTRACTIVE)", 1, 1);
  TEST ("r5.next()", (f=r5.next(), (f >= 0.0 && f <= 100.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++) 
    status &= (r5.next() != r5.next());
  TEST ("for(i=0;i<100;i++) r5.next() != r5.next()", status, TRUE);

  CoolRandom r6 (SIMPLE, 1, 3.0, 9.0);
  TEST ("CoolRandom r6(SIMPLE,1,3.0,9.0)", 1, 1);
  TEST ("r6.next()", (f=r6.next(), (f >= 3.0 && f <= 9.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r6.next() != r6.next());
  TEST ("for(i=0;i<100;i++) r6.next() != r6.next()", status, TRUE);
  CoolRandom r7 (SHUFFLE, 1, 3.0, 9.0);
  TEST ("CoolRandom r7(SHUFFLE,1,3.0,9.0)", 1, 1);
  TEST ("r7.next()", (f=r7.next(), (f >= 3.0 && f <= 9.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r7.next() != r7.next());
  TEST ("for(i=0;i<100;i++) r7.next() != r7.next()", status, TRUE);
  CoolRandom r8 (ONE_CONGRUENTIAL, 1, 3.0, 9.0);
  TEST ("CoolRandom r8(ONE_CONGRUENTIAL,1,3.0,9.0)", 1, 1);
  TEST ("r8.next()", (f=r8.next(), (f >= 3.0 && f <= 9.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r8.next() != r8.next());
  TEST ("for(i=0;i<100;i++) r8.next() != r8.next()", status, TRUE);
  CoolRandom r9 (THREE_CONGRUENTIAL, 1, 3.0, 9.0);
  TEST ("CoolRandom r9(THREE_CONGRUENTIAL,1,3.0,9.0)", 1, 1);
  TEST ("r9.next()", (f=r9.next(), (f >= 3.0 && f <= 9.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r9.next() != r9.next());
  TEST ("for(i=0;i<100;i++) r9.next() != r9.next()", status, TRUE);
  CoolRandom r0 (SUBTRACTIVE, 1, 3.0, 9.0);
  TEST ("CoolRandom r0(SUBTRACTIVE,1,3.0,9.0)", 1, 1);
  TEST ("r0.next()", (f=r0.next(), (f >= 3.0 && f <= 9.0)), 1);
  status = TRUE;
  for (i = 0; i < 100; i++)
    status &= (r0.next() != r0.next());
  TEST ("for(i=0;i<100;i++) r0.next() != r0.next()", status, TRUE);
}

void test_leak() {
  for (;;) {
    test_random();
  }
}

int main () {
  START("CoolRandom");
  test_random();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
