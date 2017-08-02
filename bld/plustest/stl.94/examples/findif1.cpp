// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

bool div_3 (int a_)
{
  return a_ % 3 ? 0 : 1;
}

int main ()
{
  typedef vector <int> IntVec;
  IntVec v (10);
  for (int i = 0; i < v.size (); i++)
    v[i] = (i + 1) * (i + 1);
  IntVec::iterator iter;
  iter = find_if (v.begin (), v.end (), div_3);
  if (iter != v.end ())
    cout
      << "Value "
      << *iter
      << " at offset "
      << (iter - v.begin ())
      << " is divisible by 3"
      << endl;
  return 0;
}
