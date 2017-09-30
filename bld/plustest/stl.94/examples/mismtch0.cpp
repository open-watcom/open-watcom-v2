// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int n1[5] = { 1, 2, 3, 4, 5 };
int n2[5] = { 1, 2, 3, 4, 5 };
int n3[5] = { 1, 2, 3, 2, 1 };

int main ()
{
  pair <int*, int*> result(0,0);
  result = mismatch (n1, n1 + 5, n2);
  if (result.first == (n1 + 5) && result.second == (n2 + 5))
    cout << "n1 and n2 are the same" << endl;
  else
    cout << "Mismatch at offset: " << (result.first - n1) << endl;
  result = mismatch (n1, n1 + 5, n3);
  if (result.first == (n1 + 5) && result.second == (n3 + 5))
    cout << "n1 and n3 are the same" << endl;
  else
    cout << "Mismatch at offset: " << (result.first - n1) << endl;
  return 0;
}
