// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int array [] = { 1, 5, 2, 4 };

int main ()
{
  char* string = "hello";
  ostream_iterator<char> it1 (cout);
  copy (string, string + 5, it1);
  cout << endl;
  ostream_iterator<int> it2 (cout);
  copy (array, array + 4, it2);
  cout << endl;
  return 0;
}
