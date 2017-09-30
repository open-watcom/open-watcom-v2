// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

char chars[] = "aabbccddggghhklllmqqqqssyyzz";

int main ()
{
  const unsigned count = sizeof (chars) - 1;
  ostream_iterator<char> iter (cout);
  cout << "Within the collection:\n\t";
  copy (chars, chars + count, iter);
  pair <char*, char*> range(0,0);
  range = equal_range (chars, chars + count, 'q', less<char>());
  cout
    << "\nq can be inserted from before index "
    << (range.first - chars)
    << " to before index "
    << (range.second - chars)
    << endl;
  return 0;
}
