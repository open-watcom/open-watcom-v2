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

extern void test_start(char* name);
extern void test_begin(const char* msg);
extern void test_perform(int success);
extern void test_summary();

#define START(s) test_start(s);

#define TEST(s,p,v)             \
 {                              \
  test_begin(s);                \
  test_perform(p==v);           \
 }

#define TEST_RUN(s,x,p,v)       \
 {                              \
  x;                            \
  test_begin(s);                \
  test_perform(p==v);           \
 }

#define SUMMARY() test_summary();

