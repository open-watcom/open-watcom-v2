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
  vector <int> v1(10);
  for (int i = 0; i < v1.size (); i++)
    v1[i] = (v1.size () - i - 1) % 5;
  ostream_iterator <int> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  inplace_merge (v1.begin (), v1.begin () + 5,
                 v1.end (),
                 greater<int>());
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  return 0;
}
