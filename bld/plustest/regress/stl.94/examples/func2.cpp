// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

bool bigger_than (int x_, int y_)
{
  return x_ > y_;
}

int main ()
{
  vector<int>v;
  v.push_back (4);
  v.push_back (1);
  v.push_back (5);
  sort (v.begin (), v.end (), bigger_than);
  vector<int>::iterator i;
  for (i = v.begin (); i != v.end (); i++)
    cout << *i << endl;
  return 0;
}
