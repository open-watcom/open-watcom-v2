// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

int main ()
{
  char word1[] = "World";
  char word2[] = "Hello";
  cout << word1 << " " << word2 << endl;
  swap_ranges (word1, word1 + ::strlen (word1), word2);
  cout << word1 << " " << word2 << endl;
  return 0;
}
