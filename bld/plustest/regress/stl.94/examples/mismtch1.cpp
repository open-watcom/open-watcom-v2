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
  typedef vector<int> IntVec;
  IntVec v1 (10);
  IntVec v2 (v1.size ());
  iota (v1.begin (), v1.end (), 0);
  iota (v2.begin (), v2.end (), 0);
  pair <IntVec::iterator, IntVec::iterator> result(0,0);
  result = mismatch (v1.begin (), v1.end (), v2.begin ());
  if (result.first == v1.end () && result.second == v2.end ())
    cout << "v1 and v2 are the same" << endl;
  else
    cout << "mismatch at index: " << (result.first - v1.begin ()) << endl;
  v2[v2.size()/2] = 42;
  result = mismatch (v1.begin (), v1.end (), v2.begin ());
  if (result.first == v1.end () && result.second == v2.end ())
    cout << "v1 and v2 are the same" << endl;
  else
    cout << "mismatch at index: " << (result.first - v1.begin ()) << endl;
  return 0;
}
