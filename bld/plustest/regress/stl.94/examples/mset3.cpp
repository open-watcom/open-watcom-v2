// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int array [] = { 3, 6, 1, 2, 3, 2, 6, 7, 9 };

int main ()
{
  multiset<int, less<int> > s (array, array + 9);
  multiset<int, less<int> >::iterator i;
  // Return location of first element that is not less than 3
  i = s.lower_bound (3);
  cout << "lower bound = " << *i << endl;
  // Return location of first element that is greater than 3
  i = s.upper_bound (3);
  cout << "upper bound = " << *i << endl;
  return 0;
}
