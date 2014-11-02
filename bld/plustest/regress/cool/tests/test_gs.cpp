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

#include <cool/Gen_String.h>
#include <cool/test.h>

void test0() {
  CoolGen_String s1;
  TEST ("CoolGen_String s1", 0, 0);
  TEST ("strlen(s1) == 0", strlen (s1), 0);
  s1 = "Hello";
  TEST ("s1 = \"Hello\"", strlen (s1), 5);
  TEST ("strlen(s1) == 5", strlen (s1), 5);
  TEST ("strcmp(s1,\"Hello\")", strcmp(s1,"Hello"), 0);
  CoolGen_String s2("World");
  TEST ("CoolGen_String s2(\"World\")", 0, 0);
  TEST ("strlen(s2) == 5", strlen (s2), 5);
  TEST ("strcmp(s2,\"World\")", strcmp(s2,"World"), 0);
  s2 = s2 + '!';
  TEST ("s2 = s2 + '!'", strcmp(s2,"World!"), 0);
  strcat (s1, " ");
  TEST ("strcat(s1, \" \")", strlen (s1), 6);
  s1 += s2;
  TEST ("s1 += s2", strcmp (s1,"Hello World!"), 0);
  TEST ("strcmp (s1,\"Hello World!\")", strcmp (s1,"Hello World!"), 0);
  TEST ("s1 == \"Hello World!\"", s1 == "Hello World!", 1);
  TEST ("is_equal(s2,\"WORLD\",SENSITIVE)", is_equal(s2,"WORLD",INSENSITIVE),
        FALSE);
  TEST ("is_equal(s2,\"WORLD!\",INSENSITIVE)",
        is_equal(s2,"WORLD!",INSENSITIVE), TRUE);
  TEST ("strlen (s1) == 12", strlen (s1), strlen ("Hello World!"));
  TEST ("s1[6] == 'W'", (s1[6] == 'W'), 1);
  TEST ("s1 != s2", is_equal("Hellow World", "World",SENSITIVE), FALSE);
  TEST ("s1 != s2", (s1 != s2), TRUE);
  CoolGen_String s3 = s2;
  TEST ("s3 = s2", (s3 == s2), TRUE);
  s1.reverse ();
  TEST ("s1.reverse()", is_equal (s1, "!DLROW OLLEH", INSENSITIVE), TRUE);
  TEST ("upcase (s2)", strcmp (upcase(s2),"WORLD!"), 0);
  TEST ("downcase (s2)", strcmp (downcase(s2), "world!"), 0);
  TEST ("is_lt (s2, \"WORLD!\")", is_lt (s2, "WORLD!", SENSITIVE),
        FALSE);
  TEST ("is_le (s2, \"WORLD!\")", 
        is_le (s2, "WORLD!", INSENSITIVE), TRUE);
  s1.reverse ();
  TEST ("downcase (s1)", strcmp (downcase(s1), "hello world!"), 0);
  TEST ("capitalize (s1)", strcmp (capitalize (s1), "Hello World!"), 0);
  left_trim (s1, "Hello ");
  TEST ("left_trim (s1, \"Hello \")", is_equal (s1, s2, INSENSITIVE), TRUE);
  right_trim (s1, "!");
  TEST ("right_trim (s1, \"!\")", is_equal (s1, "WORLD", INSENSITIVE), TRUE);
  downcase (s1);
  TEST ("is_equal (s1, \"world\", SENSITIVE)",
        is_equal (s1, "world", SENSITIVE), TRUE);
  trim (s1, "or");
  TEST ("trim (s1, \"or\")", strcmp (s1, "wld"), 0);
  s1.clear();
  TEST ("s1.clear()", strlen(s1), 0);
  s1 += "12345";
  TEST ("s1 += \"12345\"", strcmp (s1, "12345"), 0);
  TEST ("strtol (s1)", (strtol(s1) == (long)12345), 1);
  TEST ("atol (s1)", (atol(s1) == (long)12345), 1);
  TEST ("itol (s1)", (atoi(s1) == (int)12345), 1);
  s1 += '.';
  TEST ("s1 += '.'", strcmp (s1, "12345."), 0);
  s1 = s1 + "7";
  TEST ("s1 = s1 + \"7\"", strcmp (s1,"12345.7"), 0); 
  TEST ("atof (s1)", (atof(s1) == (double)12345.7), 1);
  TEST ("strtod (s1)", (strtod(s1) == (double)12345.7), 1);
}

void test1() {
  CoolGen_String s1 = "my string";
  CoolGen_String s33(s1);
  Boolean ans = s1.insert("big ",3);
  TEST ("s1.insert(\"big\",3)",strcmp(s1,"my big string"), 0);
  TEST ("return value",ans==TRUE,1);
  TEST ("s33==\"my string\"",strcmp(s33,"my string"),0);
  ans = s1.insert("big ",-1);
  TEST ("return value",ans==FALSE,1);
  ans = s1.insert("big ", 14);
  TEST ("return value",ans==FALSE,1);
  ans = s1.insert("big ",13);
  TEST ("s1.insert(\"big\",13)",strcmp(s1,"my big stringbig "), 0);
  TEST ("return value",ans==TRUE,1);

  s1 = "my string";
  CoolGen_String s32(s1);
  ans = s1.remove(3,5);
  TEST ("s1.remove(3,5)",strcmp(s1,"my ring"), 0);
  TEST ("return value",ans==TRUE,1);
  TEST ("s32==\"my string\"",strcmp(s32,"my string"),0);
  ans = s1.remove(5,7);
  TEST ("s1.remove(5,7)",strcmp(s1,"my ri"), 0);
  TEST ("return value",ans==TRUE,1);
  ans = s1.remove(0,1);
  TEST ("s1.remove(0,1)",strcmp(s1,"y ri"), 0);
  TEST ("return value",ans==TRUE,1);


  s1 = "my string";
  CoolGen_String s31(s1);
  ans = s1.replace("bo",3,5);
  TEST ("s1.replace(3,5)",strcmp(s1,"my boring"), 0);
  TEST ("return value",ans==TRUE,1);
  TEST ("s31==\"my string\"",strcmp(s31,"my string"),0);
  ans = s1.replace(" very ",2,3);
  TEST ("s1.replace(2,3)",strcmp(s1,"my very boring"), 0);
  TEST ("return value",ans==TRUE,1);
  ans = s1.replace("my,",3,7);
  TEST ("s1.replace(3,7)",strcmp(s1,"my my, boring"), 0);
  TEST ("return value",ans==TRUE,1);
  ans = s1.replace("ed",10,13);
  TEST ("s1.replace(10,13)",strcmp(s1,"my my, bored"), 0);
  TEST ("return value",ans==TRUE,1);
  ans = s1.replace("b",0,1);
  TEST ("s1.replace(0,1)",strcmp(s1,"by my, bored"), 0);
  TEST ("return value",ans==TRUE,1);
}

void test2() {
  CoolGen_String s1 = "my string";
  CoolGen_String answ;
  CoolGen_String s30(s1);
  s1.yank(answ,3,5);
  TEST ("s1.yank(3,5)",strcmp(s1,"my ring"), 0);
  TEST ("return value",strcmp(answ,"st"),0);
  TEST ("s30==\"my string\"",strcmp(s30,"my string"),0);
  s1.yank(answ,5,7);
  TEST ("s1.yank(5,7)",strcmp(s1,"my ri"), 0);
  TEST ("return value",strcmp(answ,"ng"),0);
  s1.yank(answ,0,1);
  TEST ("s1.yank(0,1)",strcmp(s1,"y ri"), 0);
  TEST ("return value",strcmp(answ,"m"),0);

  s1 = "my string";
  CoolGen_String s20(s1);
  s1.sub_string(answ,3,5);
  TEST ("s1.sub_string(3,5)",strcmp(s1,"my string"), 0);
  TEST ("return value",strcmp(answ,"st"),0);
  TEST ("s20==\"my string\"",strcmp(s20,"my string"),0);
  s1.sub_string(answ,5,7);
  TEST ("s1.sub_string(5,7)",strcmp(s1,"my string"), 0);
  TEST ("return value",strcmp(answ,"ri"),0);
  s1.sub_string(answ,0,1);
  TEST ("s1.sub_string(0,1)",strcmp(s1,"my string"), 0);
  TEST ("return value",strcmp(answ,"m"),0);

  s1 = "garbage to be deleted";
  CoolGen_String s19(s1);
  answ = strncpy(s1,"hello there",5);
  TEST ((const char*) s1,0,0);
  TEST ("strncpy(3,5)",strcmp(s1,"hello"), 0);
  TEST ("return value",strcmp(answ,"hello"),0);
  TEST ("s19==\"garbage to be deleted\"",strcmp(s19,"garbage to be deleted"),0);
  TEST ((const char*) s1,0,0);
  answ = strncpy(s1,"hi",5);
  TEST ("strncpy(s1,\"hi\",5)",strcmp(s1,"hi"), 0);
  TEST ("return value",strcmp(answ,"hi"),0);
}

void test3() {
  CoolGen_String r1("garbage49r*%Hi Therean d more Gsarbage");
  TEST ("r1.is_valid()", r1.is_valid(), FALSE);
  r1.compile ("Hi There");
  TEST ("r1.compile(\"Hi There\")", r1.is_valid(), TRUE);
  TEST ("r1.find()", r1.find(), TRUE);
  TEST ("r1.start()",r1.start(), 12);
  TEST ("r1.end",r1.end(), 20);
  r1.compile ("^ABC");
  TEST ("r1.compile(\"^ABC\")", r1.is_valid(), TRUE);
  TEST ("r1.find()", r1.find(), FALSE);
  TEST ("r1=\"ABCDEFG\"", (r1="ABCDEFG",strlen (r1)), 7);
  r1.compile ("^ABC");
  TEST ("r1.compile(\"^ABC\")", r1.is_valid(), TRUE);
  TEST ("r1.find()", r1.find (), TRUE);
  TEST ("r1.start()", r1.start(), 0);
  TEST ("r1.end()", r1.end(), 3);
  r1.compile ("EFG$");
  TEST ("r1.compile(\"EFG$\")", r1.is_valid(), TRUE);
  TEST ("r1.find()", r1.find (), TRUE);
  TEST ("r1.start()", r1.start(), 4);
  TEST ("r1.end()", r1.end(), 7);
  TEST ("r1=\"ab123QQ59ba\"", (r1="ab123QQ59ba",strlen (r1)), 11);
  r1.compile("[^ab1-9]");
  TEST ("r1.compile(\"[^ab1-9]\")", r1.is_valid(), TRUE);
  TEST ("r1.find(ab123QQ59ba)", r1.find(), TRUE);
  TEST ("r1.start()",r1.start(),5);
  TEST ("r1.end()", r1.end(),6);
  TEST ("r1=\"A B C D E\"", (r1="A B C D E",strlen (r1)), 9);
  r1.compile("B . D");
  TEST ("r1.compile(\"B . D\")", r1.is_valid(), TRUE);
  TEST ("r1.find()", r1.find(), TRUE);
  TEST ("r1.start()",r1.start(),2);
  TEST ("r1.end()", r1.end(),7);
  TEST ("r1=\"<<<AE>>>\"", (r1="<<<AE>>>",strlen (r1)), 8);
  TEST ("r1.find()", r1.find(), FALSE);
}

void test_leak() {
  for (;;) {
  test0();
  test1();
  test2();  
  test3();
  }
}

int main (void) {
  START("CoolGen_String");
  test0();
  test1();
  test2();  
  test3();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
