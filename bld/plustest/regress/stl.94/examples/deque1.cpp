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
  deque<int> d;
  d.push_back (4); // Add after end.
  d.push_back (9);
  d.push_back (16);
  d.push_front (1); // Insert at beginning.
  int i;
  for (i = 0; i < d.size (); i++)
    cout << "d[" << i << "] = " << d[i] << endl;
  cout << endl;
  d.pop_front (); // Erase first element.
  d[2] = 25; // Replace last element.
  for (i = 0; i < d.size (); i++)
    cout << "d[" << i << "] = " << d[i] << endl;
  return 0;
}
