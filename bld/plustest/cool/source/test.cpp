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
// Created: LGO 11/27/89 -- Initial design
// Updated: JAM 08/10/92 -- just #include <iostream.h> intead of stream.*
// Updated: JAM 08/11/92 -- anach. form() replaced with iomanips
//

#include <iostream.h>
#include <iomanip.h>

static int num_test;
static int tests_passed;
static int tests_failed;
static char* test_name;

void test_start(char* name = NULL) {
  num_test = 0;
  tests_passed = 0;
  tests_failed = 0;
  test_name = name;
  cout << "-----------------------------------------------------------------------------\n";
  cout << "Start Testing";      
  if (test_name != NULL) cout << " " << test_name;
  cout << ":\n-----------------------------------------------------------------------------\n";
  cout.flush();
 }

void test_begin(const char* msg) {
  num_test++;
  cout << " Test " << setfill('0') << setw(3) << num_test << setfill(' ') << ": "
         << setiosflags(ios::right) << setw(53) << msg << " --> ";
  cout.flush();
}

// NOTE: We don't pass in the message (see test_begin) because
//       we want to ensure that the message is printed BEFORE
//       the test is executed.  This way when a test crashes
//       we can tell if it was during a test, or between tests.
void test_perform(int success) {
  if (success) {
    tests_passed++;
    cout << "  PASSED\n";
  } else {
    tests_failed++;
    cout << "**FAILED**\n";
  }
  cout.flush();
}

void test_summary() {
  cout << "-----------------------------------------------------------------------------\n";
  if (test_name != NULL) cout << test_name << " ";
  cout << "Test Summary: ";
  cout << tests_passed << " passed, " << tests_failed << " failed";
  if (tests_failed > 0) cout << "\t\t\t*****";
  cout << "\n-----------------------------------------------------------------------------\n";
  cout.flush();
}
