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

#include <cool/Property.h>
#include <cool/test.h>

void test_property () {
  CoolProperty plist;
  CoolString ints = "int";
  CoolString longs = "long";
  CoolString doubles = "double";
  CoolString voidps = "void*";
  CoolValue v0;                                 // empty value
  TEST ("get", (plist.get(ints, v0)), FALSE);
  TEST ("get", (plist.get(voidps, v0)), FALSE);
  CoolValue i = (int) 1, ii;                    // integer value
  TEST ("put", plist.put(ints, i), TRUE);
  TEST ("get", (plist.get(ints, ii), ((int)ii==1)), TRUE);
  CoolString ints2 = "int";
  TEST ("get", (plist.get(ints2, ii), (ii==i)), TRUE);
  CoolValue l = (long) 10000, ll;               // long value
  TEST ("put", plist.put(longs, l), TRUE);
  TEST ("get", (plist.get(longs, ll), ((long)ll==10000)), TRUE);
  TEST ("find", plist.find(ints), TRUE);
  TEST ("find", plist.find(doubles), FALSE);
  CoolValue d = (double) -15.6, dd;
  TEST ("put", plist.put(doubles, d), TRUE);
  TEST ("get", (plist.get(doubles, dd), ((double)dd==-15.6)), TRUE);
  TEST ("remove(ints)", plist.remove(ints), TRUE);
  TEST ("remove(ints)", plist.remove(ints), FALSE);
  TEST ("get", (plist.get(ints, ii)), FALSE);
  TEST ("puts", (plist.put(ints, (i=(int)1)), plist.put(ints2, (i=(int)2)),
                 plist.get(ints, ii), ((int)ii==2)), TRUE);
  void* v1 = (void *)0xbeef;                            // void* value
  void* v2 = (void *)0xdead;
  CoolValue v = (void*) NULL, vv;
  CoolString void2ps = "void2*";
  TEST ("put", (plist.put(voidps, (v=v1)), plist.get(voidps, vv) && 
                ((void*)vv==v1)), TRUE);
  TEST ("put", (plist.put(void2ps, (v=v2)), plist.get(void2ps, vv) && 
                ((void*)vv==v2)), TRUE);
  cout << plist << "=" << &plist << endl;
  TEST ("<<", TRUE, TRUE);
}

void test_leak () {
  for (;;)
    test_property();
}

int main (void) {
  START("CoolProperty");
  test_property();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
