// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

bool input [7] = { 1, 0, 0, 1, 1, 1, 1 };

int main ()
{
  int n = 0;
  count_if (input, input + 7, logical_not<bool> (), n);
  cout << "count = " << n << endl;
  return 0;
}
