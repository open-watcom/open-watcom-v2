// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

char map_char (char a_, int b_)
{
  return char(a_ + b_);
}

int trans[] = {-4, 4, -6, -6, -10, 0, 10, -6, 6, 0, -1, -77};
char n[] = "Larry Mullen";

int main ()
{
  const unsigned count = ::strlen (n);
  ostream_iterator <char> iter (cout);
  transform (n, n + count, trans, iter, map_char);
  cout << endl;
  return 0;
}
