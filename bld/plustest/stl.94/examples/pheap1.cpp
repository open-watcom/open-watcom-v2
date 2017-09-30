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
  vector<int> v;

  v.push_back (1);
  v.push_back (20);
  v.push_back (4);
  make_heap (v.begin (), v.end ());

  v.push_back (7);
  push_heap (v.begin (), v.end ());

  sort_heap (v.begin (), v.end ());
  ostream_iterator<int> iter (cout, " ");
  copy (v.begin (), v.end (), iter);
  cout << endl;

  return 0;
}
