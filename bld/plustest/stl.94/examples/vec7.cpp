// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int array1 [] = { 1, 4, 25 };
int array2 [] = { 9, 16 };

int main ()
{
  vector<int> v (array1, array1 + 3);
  v.insert (v.begin (), 0); // Insert before first element.
  v.insert (v.end (), 36); // Insert after last element.
  int i;
  for (i = 0; i < v.size (); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
  // Insert contents of array2 before fourth element.
  v.insert (v.begin () + 3, array2, array2 + 2);
  for (i = 0; i < v.size (); i++)
    cout << "v[" << i << "] = " << v[i] << endl;
  cout << endl;
  return 0;
}
