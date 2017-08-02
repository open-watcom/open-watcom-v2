// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int array1 [] = { 9, 16, 36 };
int array2 [] = { 1, 4 };

int main ()
{
  list<int> l1 (array1, array1 + 3);
  list<int> l2 (array2, array2 + 2);
  list<int>::iterator i1 = l1.begin ();
  l1.splice (i1, l2);
  list<int>::iterator i2 = l1.begin ();
  while (i2 != l1.end ())
    cout << *i2++ << endl;
  return 0;
}
