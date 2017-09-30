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
  typedef multimap<char, int, less<char> > mmap;
  mmap m;
  cout << "count ('X') = " << m.count ('X') << endl;
  m.insert (pair<const char, int> ('X', 10)); // Standard way.
  cout << "count ('X') = " << m.count ('X') << endl;
  m.insert (pair<const char, int> ('X', 20)); // Standard way.
  cout << "count ('X') = " << m.count ('X') << endl;
  m.insert (pair<const char, int> ('Y', 32)); // Standard way.
  mmap::iterator i = m.find ('X'); // Find first match.
  while (i != m.end ()) // Loop until end is reached.
  {
    cout << (*i).first << " -> " << (*i).second << endl;
    i++;
  }
  int count = m.erase ('X');
  cout << "Erased " << count << " items" << endl;
  return 0;
}
