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
  vector <int> v1 (10);
  for (int i = 0; i < v1.size (); i++)
    v1[i] = i;
  vector <int> v2 (10);
  if (equal (v1.begin (), v1.end (), v2.begin ()))
    cout << "v1 is equal to v2" << endl;
  else
    cout << "v1 is not equal to v2" << endl;
  copy (v1.begin (), v1.end (), v2.begin ());
  if (equal (v1.begin (), v1.end (), v2.begin ()))
    cout << "v1 is equal to v2" << endl;
  else
    cout << "v1 is not equal to v2" << endl;
  return 0;
}
