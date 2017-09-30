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
  vector<int> v1(10);
  vector<int> v2(3);
  int i;
  for (i = 0; i < v1.size (); i++)
  {
    v1[i] = i;
  }
  if (includes (v1.begin (), v1.end (), v2.begin (), v2.end ()))
    cout << "v1 includes v2" << endl;
  else
    cout << "v1 does not include v2" << endl;
  for (i = 0; i < v2.size (); i++)
    v2[i] = i + 3;
  if (includes (v1.begin (), v1.end (), v2.begin (), v2.end ()))
    cout << "v1 includes v2" << endl;
  else
    cout << "v1 does not include v2" << endl;
  return 0;
}
