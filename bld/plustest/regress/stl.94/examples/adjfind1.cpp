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
  typedef vector<int> IntVector;
  IntVector v (10);
  for (int i = 0; i < v.size (); i++)
    v[i] = i;
  IntVector::iterator location;
  location = adjacent_find (v.begin (), v.end ());
  if (location != v.end ())
    cout << "Found adjacent pair of: " << *location << endl;
  else
    cout << "No adjacent pairs" << endl;
  v[6] = 7;
  location = adjacent_find (v.begin (), v.end ());
  if (location != v.end ())
    cout << "Found adjacent pair of: " << *location << endl;
  else
    cout << "No adjacent pairs" << endl;
  return 0;
}
