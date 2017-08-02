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
  typedef map<char, int, less<char> > maptype;
  maptype m;
  // Store mappings between roman numerals and decimals.
  m['l'] = 50;
  m['x'] = 20; // Deliberate mistake.
  m['v'] = 5;
  m['i'] = 1;
  cout << "m['x'] = " << m['x'] << endl;
  m['x'] = 10; // Correct mistake.
  cout << "m['x'] = " << m['x'] << endl;
  cout << "m['z'] = " << m['z'] << endl; // Note default value is added.
  cout << "m.count ('z') = " << m.count ('z') << endl;
  pair<maptype::iterator, bool> p = m.insert (pair<const char, int> ('c', 100));
  if (p.second)
    cout << "First insertion successful" << endl;
  p = m.insert (pair<const char, int> ('c', 100));
  if (p.second)
    cout << "Second insertion successful" << endl;
  else
    cout << "Existing pair " << (*(p.first)).first
         << " -> " << (*(p.first)).second << endl;
  return 0;
}
