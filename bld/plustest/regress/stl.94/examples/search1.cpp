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
  IntVec v1 (10);
  iota (v1.begin (), v1.end (), 0);
  IntVec v2 (3);
  iota (v2.begin (), v2.end (), 50);
  ostream_iterator <int> iter (cout, " ");

  cout << "v1: ";
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  cout << "v2: ";
  copy (v2.begin (), v2.end (), iter);
  cout << endl;

  IntVec::iterator location;
  location = search (v1.begin (), v1.end (), v2.begin (), v2.end ());

  if (location == v1.end ())
    cout << "v2 not contained in v1" << endl;
  else
    cout << "Found v2 in v1 at offset: " << location - v1.begin () << endl;

  iota (v2.begin (), v2.end (), 4);
  cout << "v1: ";
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  cout << "v2: ";
  copy (v2.begin (), v2.end (), iter);
  cout << endl;

  location = search (v1.begin (), v1.end (), v2.begin (), v2.end ());

  if (location == v1.end ())
    cout << "v2 not contained in v1" << endl;
  else
    cout << "Found v2 in v1 at offset: " << location - v1.begin () << endl;

  return 0;
}
