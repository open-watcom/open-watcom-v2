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
  vector <char> v1 (3);
  iota (v1.begin (), v1.end (), 'A');
  ostream_iterator<char> iter (cout);
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  for (int i = 0; i < 9; i++)
  {
    next_permutation (v1.begin (), v1.end (), less<char>());
    copy (v1.begin (), v1.end (), iter);
    cout << endl;
  }
  return 0;
}
