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
  vector<int> v1; // Empty vector of integers.
  cout << "empty = " << v1.empty () << endl;
  cout << "size = " << v1.size () << endl;
  cout << "max_size = " << v1.max_size () << endl;
  v1.push_back (42); // Add an integer to the vector.
  cout << "size = " << v1.size () << endl;
  cout << "v1[0] = " << v1[0] << endl;
  return 0;
}
