// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers[10] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3 };

int main ()
{
  pair <int*, int*> range(0,0);
  range = equal_range (numbers, numbers + 10, 2);
  cout
    << "2 can be inserted from before index "
    << (range.first - numbers)
    << " to before index "
    << (range.second - numbers)
    << endl;
  return 0;
}
