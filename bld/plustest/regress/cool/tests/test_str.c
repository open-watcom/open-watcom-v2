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

#include <cool/String.h>
#include <cool/test.h>

 
void test_string() {
  CoolString s1;
  TEST ("CoolString s1", 0, 0);
  TEST ("strlen(s1) == 0", strlen (s1), 0);
  s1 = "Hello";
  TEST ("s1 = \"Hello\"", strlen (s1), 5);
  TEST ("strlen(s1) == 5", strlen (s1), 5);
  TEST ("strcmp(s1,\"Hello\")", strcmp(s1,"Hello"), 0);
  CoolString s5("World",5L);
  TEST ("CoolString s5(\"World\",5)", 0, 0);
  CoolString s2("World");
  TEST ("CoolString s2(\"World\")", 0, 0);
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
  TEST ("s1 != s2", (s1 != s2), TRUE);
  CoolString s3 = s2;
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
  s1 = "my string";
  CoolString s33(s1);
  Boolean ans = s1.insert("big ",3);
  TEST ("s1.insert(\"big \",3)",strcmp(s1,"my big string"), 0);
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
  CoolString s32(s1);
  ans = s1.remove(3,5);
  TEST ("s1.remove(3,5)",strcmp(s1,"my ring"), 0);
  TEST ("return value",ans==TRUE,1);
  TEST ("s32==\"my string\"",strcmp(s32,"my string"),0);
  TEST ("return value",s1.remove(4,-1),FALSE);
  TEST ("return value",s1.remove(-1,4),FALSE);
  TEST ("return value",s1.remove(4,4),FALSE);
  TEST ("return value",s1.remove(6,4),FALSE);
  TEST ("return value",s1.remove(8,4),FALSE);
  TEST ("return value",s1.remove(2,8),FALSE);
  ans = s1.remove(5,7);
  TEST ("s1.remove(5,7)",strcmp(s1,"my ri"), 0);
  TEST ("return value",ans==TRUE,1);
  ans = s1.remove(0,1);
  TEST ("s1.remove(0,1)",strcmp(s1,"y ri"), 0);
  TEST ("return value",ans==TRUE,1);

  s1 = "my string";
  TEST ("s1.replace(\"bo\",3,5)", s1.replace("bo",3,5), TRUE);
  TEST ("strcmp(s1,\"my boring\")", strcmp (s1,"my boring"), 0);
  TEST ("s1.replace(\" very\",2,3)",s1.replace(" very ",2,3), TRUE);
  TEST ("strcmp (s1,\"my very boring\")",strcmp(s1,"my very boring"), 0);
  TEST ("s1.replace(\"my,\",3,7)",s1.replace("my,",3,7), TRUE);
  TEST ("strcmp(s1,\"my my, boring\")",strcmp(s1,"my my, boring"), 0);
  TEST ("s1.replace(\"nothing\",3,14)",s1.replace("nothing",3,14), FALSE);
  TEST ("s1.replace(\"ed\",10,13)",s1.replace("ed",10,13), TRUE);
  TEST ("strcmp(s1,\"my my, bored\")",strcmp(s1,"my my, bored"), 0);
  TEST ("s1.replace(\"b\",0,1)",s1.replace("b",0,1), TRUE);
  TEST ("strcmp(s1,\"by my, bored\")",strcmp(s1,"by my, bored"), 0);

  CoolString temp;
  s1 = "my string";
  CoolString s30(s1);
  s1.yank(temp,3,5);
  TEST ("s1.yank(3,5)",strcmp(s1,"my ring"), 0);
  TEST ("return value",strcmp(temp,"st"),0);
  TEST ("s30==\"my string\"",strcmp(s30,"my string"),0);
  s1.yank(temp,5,7);
  TEST ("s1.yank(5,7)",strcmp(s1,"my ri"), 0);
  TEST ("return value",strcmp(temp,"ng"),0);
  s1.yank(temp,0,1);
  TEST ("s1.yank(0,1)",strcmp(s1,"y ri"), 0);
  TEST ("return value",strcmp(temp,"m"),0);

  s1 = "my string";
  CoolString answ;
  CoolString s20(s1);
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
  CoolString s19(s1);
  answ = strncpy(s1,"hello there",5);
  TEST ((char*)(const char*) s1,0,0);
  TEST ("strncpy(3,5)",strcmp(s1,"hello"), 0);
  TEST ("return value",strcmp(answ,"hello"),0);
  TEST ("s19==\"garbage to be deleted\"",strcmp(s19,"garbage to be deleted"),0);
  TEST ((const char*) s1,0,0);
  answ = strncpy(s1,"hi",5);
  TEST ("strncpy(s1,\"hi\",5)",strcmp(s1,"hi"), 0);
  TEST ("return value",strcmp(answ,"hi"),0);
}

void test_leak() {
  for (;;) {
    test_string();
  }
}

int main (void) {
  START("CoolString");
  test_string();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
