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
  vector<int> i;
  i.push_back (1);
  i.push_back (4);
  i.push_back (2);
  i.push_back (8);
  i.push_back (2);
  i.push_back (2);
  int n = 0; // Must be initialized, as count increments n.
  count (i.begin (), i.end (), 2, n);
  cout << "Count of 2s = " << n << endl;
  return 0;
}
