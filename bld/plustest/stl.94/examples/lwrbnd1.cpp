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
  vector <int> v1 (20);
  for (int i = 0; i < v1.size (); i++)
  {
    v1[i] = i/4;
    cout << v1[i] << ' ';
  }
  int* location =  lower_bound (v1.begin (), v1.end (), 3);
  cout
    << "\n3 can be inserted at index: "
    << (location - v1.begin ())
    << endl;
  return 0;
}
