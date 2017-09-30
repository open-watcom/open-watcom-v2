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
  vector <int> v1 (10);
  iota (v1.begin (), v1.end (), 0);
  vector <int> v2 (10);
  iota (v2.begin(), v2.end (), 7);
  ostream_iterator <int> iter (cout, " ");
  cout << "v1: ";
  copy (v1.begin (), v1.end (), iter);
  cout << "\nv2: ";
  copy (v2.begin (), v2.end (), iter);
  cout << endl;
  set_symmetric_difference (v1.begin (), v1.end (), 
                            v2.begin (), v2.end (), iter);
  cout << endl;
  return 0;
}
