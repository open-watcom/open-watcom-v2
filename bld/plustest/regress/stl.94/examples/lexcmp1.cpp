// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

const unsigned size = 6;
char n1[size] = "shoe";
char n2[size] = "shine";

int main ()
{
  bool before = lexicographical_compare (n1, n1 + size, n2, n2 + size);
  if (before)
    cout << n1 << " is before " << n2 << endl;
  else
    cout << n2 << " is before " << n1 << endl;
  return 0;
}
