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
  vector<char> v1; // Empty vector of characters.
  v1.push_back ('h');
  v1.push_back ('i');
  cout << "v1 = " << v1[0] << v1[1] << endl;
  vector<char> v2 (v1);
  v2[1] = 'o'; // Replace second character.
  cout << "v2 = " << v2[0] << v2[1] << endl;
  cout << "(v1 == v2) = " << (v1 == v2) << endl;
  cout << "(v1 < v2) = " << (v1 < v2) << endl;
  return 0;
}
