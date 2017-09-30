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
  vector<const char*> v; // Vector of character strings.
  v.push_back ((char*) "zippy"); // First element.
  v.push_back ((char*) "motorboy"); // Second element.
  vector<const char*>::reverse_iterator i;
  for (i = v.rbegin (); i != v.rend (); i++)
    cout << *i << endl; // Display item.
  return 0;
}
