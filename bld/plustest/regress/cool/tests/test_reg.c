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

#include <cool/Regexp.h>
#include <cool/test.h>

#if defined(DOS)
extern "C" {
#include <string.h>
}
#else
#include <string.h>
#endif

void test_regexp () {
  // GENERAL TEST OF MEMBER FUNCTIONS FOR REGEXP CLASS.

  // Test the CoolRegexp(char*) constructor [and thus also the compile(char*)
  // member function], and do a very basic test on the find(char*) function.

  CoolRegexp rd1("Hi There");
  TEST ("rd1.is_valid()",rd1.is_valid(),TRUE);
  rd1.set_invalid();
  TEST ("rd1.is_valid()",rd1.is_valid(),FALSE);
  CoolRegexp rrr;
  TEST ("rrr.is_valid()",rrr.is_valid(),FALSE);
  rrr.compile("hello");
  TEST ("rrr.is_valid()",rrr.is_valid(),TRUE);

  CoolRegexp r1("Hi There");
  char* strng = "garbage49r*%Hi Therean d more Gsarbage";
  TEST ("CoolRegexp r1(Hi There)", 0, 0);
  TEST ("r1.find(It is not here)", r1.find("It is not here"), 0);
  TEST ("r1.find(strng)", r1.find(strng), 1);
  TEST ("r1.start()",r1.start(), 12);
  TEST ("r1.end",r1.end(), 20);

  // Test the CoolRegexp(&CoolRegexp) constructor, and the == and deep_equal
  // functions.

  CoolRegexp r2(r1);
  TEST ("CoolRegexp r2(r1)", 0, 0);
  TEST ("r1==r2", r1==r2, 1);
  TEST ("r2==r1", r2==r1, 1);
  TEST ("r2.deep_equal(r1)", r2.deep_equal(r1), 1);
  TEST ("r1.deep_equal(r2)", r1.deep_equal(r2), 1);
  TEST ("r2.find(strng)", r2.find(strng), 1);
  TEST ("r1==r2", r1==r2, 1);
  TEST ("r2==r1", r2==r1, 1);
  TEST ("r2.deep_equal(r1)", r2.deep_equal(r1), 1);
  TEST ("r1.deep_equal(r2)", r1.deep_equal(r2), 1);
  TEST ("r2.find(Its not here)", r2.find("Its not here"), 0); 
  TEST ("r1==r2", r1==r2, 1);
  TEST ("r2==r1", r2==r1, 1);
  TEST ("r2.deep_equal(r1)", r2.deep_equal(r1), 1);
  TEST ("r1.deep_equal(r2)", r1.deep_equal(r2), 1);
  TEST ("r2.find(Another Hi Therestring.)", 
        r2.find("Another Hi Therestring."), 1);
  TEST ("r1==r2", r1==r2, 1);
  TEST ("r2==r1", r2==r1, 1);
  TEST ("r1!=r2", r1!=r2, 0);
  TEST ("r2!=r1", r2!=r1, 0);
  TEST ("r2.deep_equal(r1)", r2.deep_equal(r1), 0);
  TEST ("r1.deep_equal(r2)", r1.deep_equal(r2), 0);
 
  // Tests the CoolRegexp() constructor and finishes up the testing on
  // the == operator.

  CoolRegexp r3;
  r3.compile("hELl");
  char* strng2 = "o, hELlo";
  char* strng3 = "I don't think its here";
  TEST ("CoolRegexp r3()", 0, 0);
  TEST ("r3.compile(hELl)", 0, 0);
  TEST ("r3.find(strng2)", r3.find(strng2), 1);
  TEST ("r1==r3", r1==r3, 0);
  TEST ("r1.deep_equal(r3)", r1.deep_equal(r3), 0);
  TEST ("r3.start()",r3.start(), 3);
  TEST ("r3.end()",r3.end(), 7);
  CoolRegexp r4(r3);
  TEST ("r3.find(strng3)", r3.find(strng3), 0);
  TEST ("r4==r3", r4==r3, 1);
  TEST ("r4.deep_equal(r3)", r4.deep_equal(r3), 0);
  




  ////
  //
  // THIS COMPLETES THE GENERAL TESTS OF THE MEMBER FUNCTIONS.
  // 
  // THE FOLLOWING TESTS ARE FOR COMPILE AND FIND, AND TEST
  // VARIOUS POSSIBILITIES OF REGULAR EXPRESSIONS.
  //
  //// 


// A regular expression allows a programmer to specify complex patterns that
// can be searched for and matched against the character string of a String
// object.  In its simplest case, a regular expression is a sequence of
// characters with which you can search for exact character matches.  However,
// many times you may not know the exact sequence you want to find, or you may
// only want to find a match at the beginning or end of a String.  The CoolRegexp
// object allows specification of such patterns by utilizing the following
// regular expression

//meta-characters:
//
//         ^    Match at beginning of line
//         $    Match at end of line
//         .    Match any single character
//         [ ]  Match any one character inside the brackets
//         -    Match any character in range on either side of dash
//         *    Match preceding pattern zero or more times
//         +    Match preceding pattern one or more times
//         ?    Match preceding pattern zero or once only
//         ()   Save a matched expression and use it in a further match.

// BELOW ARE TESTS FOR THE REGULAR EXPRESSIONS USING THE SYMBOLS LISTED
// ABOVE.  THEY PROVIDE GOOD EXAMPLES OF REGULAR EXPRESSION USE.  

// OTHER GOOD EXAMPLES OF HOW REGULAR EXPRESSIONS ARE USED, CAN BE
// FOUND IN DOCUMENTATION FOR ED, GREP, AND AWK.

// THE ABOVE SYMBOLS CAN BE USED TOGETHER TO FORM COMPLEX REGULAR EXPRESSIONS
// FOR VERY UNUSUAL MATHING.  THERE ARE A FEW EXAMPLES OF THIS TYPE OF
// USE IN THE FOLLOWING TESTS.  --MNF


// TESTS FOR REGULAR EXPRESSIONS WITH ^

  char* s = "str at front";
  CoolRegexp rxp("^str");
  TEST ("CoolRegexp rxp(^str)", 0, 0);
  TEST ("rxp.find(str at front)", rxp.find(s), 1);
  TEST ("rxp.start()", rxp.start(), 0);
  TEST ("rxp.end()", rxp.end(), 3);
  TEST ("rxp.find(Str not present)", rxp.find("Str not present"), 0);
  TEST ("rxp.find(not at str front)", rxp.find("not at str front"), 0);
  TEST ("rxp.find(at end str)", rxp.find("at end str"), 0);
 

// TESTS FOR REGULAR EXPRESSIONS WITH $
  
  rxp.compile("str$");
  TEST ("rxp.compile(str$)", 0, 0);
  TEST ("rxp.find(str at front)", rxp.find("str at front"), 0);
  TEST ("rxp.find(Str not present)", rxp.find("Str not present"), 0);
  TEST ("rxp.find(not at str end)", rxp.find("not at str end"), 0);
  TEST ("rxp.find(at end str\0)", rxp.find("at end str\0"), 1);

  
// TESTS FOR REGULAR EXPRESSIONS WITH .

  rxp.compile("s..t.r");
  TEST ("rxp.compile(s..t.r)", 0, 0);
  TEST ("rxp.find(so three)", rxp.find("so three"), 1);
  TEST ("rxp.find(str too close)", rxp.find("str too close"), 0);
  TEST ("rxp.find(dl 32 s*4t0r ugh)", rxp.find("dl 32 s*4t0r ugh"), 1);
  TEST ("rxp.find(too far s  t  r)", rxp.find("too far s  t  r"), 0);
  

// TESTS FOR REGULAR EXPRESSIONS WITH [] [^  ] and -

  s = "ugh b is not 1";
  rxp.compile("[ab1-9]");
  TEST ("rxp.compile([ab1-9])", 0, 0);
  TEST ("rxp.find(6 is the num)", rxp.find("6 is the num"), 1);
  TEST ("rxp.find(ugh b is not 1)", rxp.find(s), 1);
  TEST ("rxp.start()",rxp.start(),4);
  TEST ("rxp.end()", rxp.end(),5);
  TEST ("rxp.find(this will not m'tch)", rxp.find("this will not m'tch"), 0);
  
  s = "ab123QQ59ba";
  rxp.compile("[^ab1-9]");
  TEST ("rxp.compile([^ab1-9])", 0, 0);
  TEST ("rxp.find(ab123QQ59ba)", rxp.find(s), 1);
  TEST ("rxp.start()",rxp.start(),5);
  TEST ("rxp.end()", rxp.end(),6);
  TEST ("rxp.find(ab123456789ba)", rxp.find("ab123456789ba"), 0);


// TESTS FOR REGULAR EXPRESSIONS WITH *, +, and ?

  s = "rrra   lddd";
  rxp.compile("a *l");
  TEST ("rxp.compile(a *l)",0,0);
  TEST ("rxp.find(a *l)", rxp.find(s), 1);
  TEST ("rxp.start()",rxp.start(),3);
  TEST ("rxp.end()", rxp.end(),8);
  TEST ("rxp.find(hello there)", rxp.find("hello there"), 0);
  TEST ("rxp.find(<<<al>>>)", rxp.find("<<<al>>>"), 1);

  rxp.compile("a +l");
  TEST ("rxp.compile(a +l)",0,0);
  TEST ("rxp.find(a *l)", rxp.find(s), 1);
  TEST ("rxp.start()",rxp.start(),3);
  TEST ("rxp.end()", rxp.end(),8);
  TEST ("rxp.find(hello there)", rxp.find("hello there"), 0);
  TEST ("rxp.find(<<<al>>>)", rxp.find("<<<al>>>"), 0);

  rxp.compile("a ? ? ?l");
  TEST ("rxp.compile(a ?l)",0,0);
  TEST ("rxp.find(a *l)", rxp.find(s), 1);
  TEST ("rxp.start()",rxp.start(),3);
  TEST ("rxp.end()", rxp.end(),8);
  TEST ("rxp.find(hello there)", rxp.find("hello there"), 0);
  TEST ("rxp.find(<<<al>>>)", rxp.find("<<<al>>>"), 1);

// TESTS FOR REGULAR EXPRESSIONS WITH TEMPORARY STORAGE i.e. ()

  // finds an expression ending with pb and beginning with whatever
  // the two characters before the first p encounterd in the line were.

  s = "rep drepa qrepb";
  rxp.compile("(..p)b");
  TEST ("rxp.compile((..p)b)", 0, 0);
  TEST ("rxp.find(rep drepa qrepb)", rxp.find(s), 1);
  TEST ("rxp.start()", rxp.start(), 11);
  TEST ("rxp.end()", rxp.end(), 15);
  TEST ("rxp.find(repreprep)", rxp.find("repreprep"), 0);
 


  // finds an expression ending with p, beginning with b, and having 
  // two characters in between that are the same as the two characters
  // before the first p encounterd in the line.

  rxp.compile("d(..p)");
  TEST ("rxp.compile(d(..p))", 0, 0);
  TEST ("rxp.find(rep drepa qrepb)", rxp.find(s), 1);
  TEST ("rxp.start()",rxp.start(), 4);
  TEST ("rxp.end()",rxp.end(),8);
  TEST ("rxp.find(repreprep)", rxp.find("repreprep"), 0);

  s = "Hi there \n this is another \n line";
  rxp.compile("Hi.*line");
  TEST ("find",rxp.find(s),1);
}

void test_leak() {
  for (;;) {
    test_regexp();
  }
}

int main (void) {
  START("CoolRegexp");
  test_regexp();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
 
