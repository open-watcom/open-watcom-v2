// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int array1 [] = { 1, 3, 6, 7 };
int array2 [] = { 2, 4 };

int main ()
{
  list<int> l1 (array1, array1 + 4);
  list<int> l2 (array2, array2 + 2);
  l1.merge (l2);
  for (list<int>::iterator i = l1.begin (); i != l1.end (); i++)
    cout << *i;
  cout << endl;
  return 0;
}
