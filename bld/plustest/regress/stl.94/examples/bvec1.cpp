// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int main ()
{
  bit_vector b (3);
  int i;
  for (i = 0; i < b.size (); i++)
    cout << b[i];
  cout << endl;
  b[0] = b[2] = 1;
  for (i = 0; i < b.size (); i++)
    cout << b[i];
  cout << endl;
  return 0;
}
