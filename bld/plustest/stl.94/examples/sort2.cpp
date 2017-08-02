// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int array[] = { 1, 50, -10, 11, 42, 19 };

int main ()
{
  int count = sizeof (array) / sizeof (array[0]);
  ostream_iterator <int> iter (cout, " ");
  cout << "before: ";
  copy (array, array + count, iter);
  cout << "\nafter: ";
  sort (array, array + count, greater<int>());
  copy (array, array + count, iter);
  cout << endl;
  return 0;
}
