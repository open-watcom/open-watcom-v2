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
  typedef multiset<int, less<int> > mset;
  mset s (array, array + 9);
  pair<mset::const_iterator, mset::const_iterator> p = s.equal_range (3);
  cout << "lower bound = " << *(p.first) << endl;
  cout << "upper bound = " << *(p.second) << endl;
  return 0;
}
