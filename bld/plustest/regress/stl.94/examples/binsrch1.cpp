// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int main ()
{
  int vector[100];
  for (int i = 0; i < 100; i++)
    vector[i] = i;
  if (binary_search (vector, vector + 100, 42))
    cout << "found 42" << endl;
  else
    cout << "did not find 42" << endl;
  return 0;
}
