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
  vector<int> years;
  years.push_back (1962);
  years.push_back (1992);
  years.push_back (2001);
  years.push_back (1999);
  sort (years.begin (), years.end ());
  vector<int>::iterator i;
  for (i = years.begin (); i != years.end (); i++)
    cout << *i << endl;
  return 0;
}
