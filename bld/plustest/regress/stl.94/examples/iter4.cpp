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
  vector<int> v; // Empty vector of integers.
  v.push_back (1);
  v.push_back (2);
  v.push_back (3);
  // Position immediately after last item.
  vector<int>::iterator i = v.end (); 
  // Move back one and then access.
  cout << "last element is " << *--i << endl; 
  i -= 2; // Jump back two items.
  cout << "first element is " << *i << endl;
  return 0;
}
