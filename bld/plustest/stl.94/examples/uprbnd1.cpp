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
  int array[20];
  for (int i = 0; i < 20; i++)
  {
    array[i] = i/4;
    cout << array[i] << ' ';
  }
  cout
    << "\n3 can be inserted at index: "
    << upper_bound (array, array + 20, 3) - array
    << endl;
  return 0;
}
