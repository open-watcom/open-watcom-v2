// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

char array [] = { 'x', 'l', 'x', 't', 's', 's' };

int main ()
{
  list<char> str (array, array + 6);
  list<char>::iterator i;
  cout << "original: ";
  for (i = str.begin (); i != str.end (); i++)
    cout << *i;
  cout << endl;
  cout << "reversed: ";
  str.reverse ();
  for (i = str.begin (); i != str.end (); i++)
    cout << *i;
  cout << endl;
  cout << "removed: ";
  str.remove ('x');
  for (i = str.begin (); i != str.end (); i++)
    cout << *i;
  cout << endl;
  cout << "uniqued: ";
  str.unique ();
  for (i = str.begin (); i != str.end (); i++)
    cout << *i;
  cout << endl;
  cout << "sorted: ";
  str.sort ();
  for (i = str.begin (); i != str.end (); i++)
    cout << *i;
  cout << endl;
  return 0;
}
