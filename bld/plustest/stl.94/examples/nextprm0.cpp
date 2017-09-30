// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int v1[3] = { 0, 1, 2 };

int main ()
{
  next_permutation (v1, v1 + 3);
  for (int i = 0; i < 3; i++)
    cout << v1[i] << ' ';
  cout << endl;
  return 0;
}
