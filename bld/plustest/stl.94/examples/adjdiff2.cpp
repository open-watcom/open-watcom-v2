// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int mult (int a_, int b_)
{
  return a_ * b_;
}

int main ()
{
  vector <int> v (10);
  for (int i = 0; i < v.size (); i++)
    v[i] = i + 1;
  vector <int> rslt (v.size ());
  adjacent_difference (v.begin (), v.end (), rslt.begin (), mult);
  ostream_iterator<int> iter (cout, " ");
  copy (v.begin (), v.end (), iter);
  cout << endl;
  copy (rslt.begin (), rslt.end (), iter);
  cout << endl;
  return 0;
}
