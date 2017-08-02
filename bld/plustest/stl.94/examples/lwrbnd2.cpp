// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

bool char_str_less (const char* a_, const char* b_)
{
  return ::strcmp (a_, b_) < 0 ? 1 : 0;
}

char* str [] = { "a", "a", "b", "b", "q", "w", "z" };

int main ()
{
  const unsigned strCt = sizeof (str)/sizeof (str[0]);
  cout
    << "d can be inserted at index: "
    << (lower_bound (str,  str + strCt, "d", char_str_less) - str)
    << endl;
  return 0;
}
