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
  typedef vector <int> IntVec;
  IntVec v (10);
  for (int i = 0; i < v.size (); i++)
    v[i] = i / 3;
  ostream_iterator<int> iter (cout, " ");
  cout << "Within the collection:\n\t";
  copy (v.begin (), v.end (), iter);
  pair <IntVec::iterator, IntVec::iterator> range(0,0);
  range = equal_range (v.begin (), v.end (), 2);
  cout
    << "\n2 can be inserted from before index "
    << (range.first - v.begin ())
    << " to before index "
    << (range.second - v.begin ())
    << endl;
  return 0;
}
