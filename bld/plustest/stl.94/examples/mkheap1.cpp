// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers[6] = { 5, 10, 4, 13, 11, 19 };

int main ()
{
  make_heap (numbers, numbers + 6, greater<int> ());
  for (int i = 6; i >= 1; i--)
  {
    cout << numbers[0] << endl;
    pop_heap (numbers, numbers + i, greater<int> ());
  }
  return 0;
}
