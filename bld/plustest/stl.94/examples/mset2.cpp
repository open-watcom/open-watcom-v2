// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

char* names [] = { "dave", "alf", "chas", "bob", "ed", "chas" };

struct less_s : binary_function<char *, char *, bool> {
        typedef char * T;
    bool operator()(const T& x, const T& y) const { return strcmp( x, y ) < 0; }
};

int main ()
{
  typedef multiset<char*, less_s> mset;
  mset s;
  s.insert (names, names + 6);
  for (mset::iterator i = s.begin (); i != s.end (); i++)
    cout << *i << endl;
  return 0;
}
