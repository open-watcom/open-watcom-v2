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

#include <cool/char.h>
#include <cool/test.h>

static void test_strfind() {
  long start;
  long end;
  char* s = "this is the thing I am searching *star? seard";
  TEST("strfind(s, \"*thing*\", &start, &end)",
       strfind(s, "*thing*", &start, &end), s+12);
  TEST("start", start, 12);
  TEST("end", end, 17);
  TEST("strfind(s, \"*???m *\",  &start, &end)", 
       strfind(s, "*???m *", &start, &end), s+18);
  TEST("start", start, 18);
  TEST("end", end, 23);
  TEST("strfind(s, \"*\\**\", &start, &end)",
       strfind(s, "*\\**", &start, &end), s+33);
  TEST("start", start, 33 );
  TEST("end", end, 34);
  TEST("strfind(s, \"*\\?*\", &start, &end)",
       strfind(s, "*\\?*", &start, &end), s+38);
  TEST("start", start, 38);
  TEST("end", end, 39);
  s = "it can look ?t this and can look to that";
  TEST("strfind(s,\"*ook?t*\",&start,&end)",
       strfind(s, "*ook?t*", &start, &end), s+29);
  TEST("start", start, 29);
  TEST("end", end, 34);
  s = "ep*hello*heder";
  TEST("strfind(s, \"*p*\\*hed*\", &start, &end)",
       strfind(s,"*p*\\*hed*",&start,&end), s+1);
  TEST("start",start,1);
  TEST("end",end,12);
  TEST("strfind(s, \"*\\*hed*\", &start, &end)",
       strfind(s,"*\\*hed*",&start,&end), s+8);
  TEST("start",start,8);
  TEST("end",end,12);
  s = "ccddbabbzz*qqgegg?";
  TEST("strfind(s, \"*d*bb*\\**\", &start,&end)",
       strfind(s,"*d*bb*\\**",&start,&end),s+2);
  TEST("start",start,2);
  TEST("end",end,11);
}

static void test_strrfind() {
  long start;
  long end;
  char* s = "this is the thing I am searching *star? seard";
  TEST("strrfind(s, \"*thing*\", &start, &end)",
       strrfind(s, "*thing*", &start, &end), s+12);
  TEST("start", start, 12);
  TEST("end", end, 17);
  TEST("strrfind(s, \"*???m *\",  &start, &end)", 
       strrfind(s, "*???m *", &start, &end), s+18);
  TEST("start", start, 18);
  TEST("end", end, 23);
  TEST("strrfind(s, \"*\\**\", &start, &end)",
       strrfind(s, "*\\**", &start, &end), s+33);
  TEST("start", start, 33 );
  TEST("end", end, 34);
  TEST("strrfind(s, \"*\\?*\", &start, &end)",
       strrfind(s, "*\\?*", &start, &end), s+38);
  TEST("start", start, 38);
  TEST("end", end, 39);
  s = "it can look ?t this and can look to that";
  TEST("strrfind(s,\"*ook?t*\",&start,&end)",
       strrfind(s, "*ook?t*", &start, &end), s+29);
  TEST("start", start, 29);
  TEST("end", end, 34);
  s = "ep*hello*heder";
  TEST("strrfind(s, \"*p*\\*hed*\", &start, &end)",
       strrfind(s,"*p*\\*hed*",&start,&end), s+1);
  TEST("start",start,1);
  TEST("end",end,12);
  TEST("strrfind(s, \"*\\*hed*\", &start, &end)",
       strrfind(s,"*\\*hed*",&start,&end), s+8);
  TEST("start",start,8);
  TEST("end",end,12);
  s = "ccddbabbzz*qqgegg?";
  TEST("strrfind(s, \"*d*bb*\\**\", &start,&end)",
       strrfind(s,"*d*bb*\\**",&start,&end),s+3);
  TEST("start",start,3);
  TEST("end",end,11);
}

void test_leak () {
  for (;;) {
    test_strfind();
    test_strrfind();
  }
}
int main (void) {

  START("char")
  test_strfind();
  test_strrfind();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}

