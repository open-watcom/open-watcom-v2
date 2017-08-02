// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int main ()
{
  vector<int> v (4);
  v[0] = 1;
  v[1] = 4;
  v[2] = 9;
  v[3] = 16;
  cout << "front = " << v.front () << endl;
  cout << "back = " << v.back () << ", size = " << v.size () << endl;
  v.push_back (25);
  cout << "back = " << v.back () << ", size = " << v.size () << endl;
  v.pop_back ();
  cout << "back = " << v.back () << ", size = " << v.size () << endl;
  return 0;
}
