// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

bool str_compare (const char* a_, const char* b_)
{
  return ::strcmp (a_, b_) < 0 ? 1 : 0;
}

char* names[] = { "aa", "ff", "dd", "ee", "cc", "bb" };

int main ()
{
  const unsigned nameSize = sizeof (names) / sizeof (names[0]);
  vector <char*> v1 (nameSize);
  for (int i = 0; i < v1.size (); i++)
    v1[i] = names[i];
  ostream_iterator<char*> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  partial_sort (v1.begin (),
                v1.begin () + nameSize / 2,
                v1.end (),
                str_compare);
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  return 0;
}
