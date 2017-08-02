// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int v1[6] = { 1, 1, 2, 3, 5, 8 };
int v2[6] = { 0, 1, 2, 3, 4, 5 };
int v3[2] = { 3, 4 };

int main ()
{
  int* location;
  location = search (v1, v1 + 6, v3, v3 + 2);
  if (location == v1 + 6)
    cout << "v3 not contained in v1" << endl;
  else
    cout << "Found v3 in v1 at offset: " << location - v1 << endl;
  location = search (v2, v2 + 6, v3, v3 + 2);
  if (location == v2 + 6)
    cout << "v3 not contained in v2" << endl;
  else
    cout << "Found v3 in v2 at offset: " << location - v2 << endl;
  return 0;
}
