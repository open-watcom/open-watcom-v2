// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers1 [5] = { 1, 2, 4, 8, 16 };
int numbers2 [5] = { 5, 3, 2, 1, 1 };

int main ()
{
  int* location = adjacent_find (numbers1, numbers1 + 5);

  if (location != numbers1 + 5)
    cout
      << "Found adjacent pair of: "
      << *location
      << " at offset "
      << (location - numbers1)
      << endl;
  else
    cout << "No adjacent pairs" << endl;
  location = adjacent_find (numbers2, numbers2 + 5);
  if (location != numbers2 + 5)
    cout
      << "Found adjacent pair of: "
      << *location
      << " at offset "
      << (location - numbers2)
      << endl;
  else
    cout << "No adjacent pairs" << endl;
  return 0;
}
