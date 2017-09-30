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
  typedef vector<int> IVec;
  vector<int> v1 (10);
  for (int loc = 0; loc < v1.size (); loc++)
    v1[loc] = loc;
  vector<int> v2;
  insert_iterator<IVec> i (v2, v2.begin ());
  copy (v1.begin (), v1.end (), i);
  ostream_iterator<int> outIter (cout, " ");
  copy (v2.begin (), v2.end (), outIter);
  cout << endl;
  return 0;
}
