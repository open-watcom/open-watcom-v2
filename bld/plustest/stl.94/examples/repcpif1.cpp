// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

bool odd (int a_)
{
  return a_ % 2;
}

int main ()
{
  vector <int> v1 (10);
  for (int i = 0; i < v1.size (); i++)
    v1[i] = i % 5;
  ostream_iterator <int> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  vector <int> v2 (v1.size ());
  replace_copy_if (v1.begin (), v1.end (), v2.begin (), odd, 42);
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  copy (v2.begin (), v2.end (), iter);
  cout << endl;
  return 0;
}
