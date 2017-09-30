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
  vector <int> v1 (5);
  vector <int> v2 (v1.size ());
  iota (v1.begin (), v1.end (), 0);
  iota (v2.begin (), v2.end (), 3);
  vector <int> result (v1.size () + v2.size ());
  merge (v1.begin (), v1.end (), v2.begin (), v2.end (), result.begin ());
  ostream_iterator <int> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  copy (v2.begin (), v2.end (), iter);
  cout << endl;
  copy (result.begin (), result.end (), iter);
  cout << endl;
  return 0;
}
