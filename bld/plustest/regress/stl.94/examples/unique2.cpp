// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

bool str_equal (const char* a_, const char* b_)
{
  return ::strcmp (a_, b_) == 0 ? 1 : 0;
}

char* labels[] = { "Q","Q","W","W","E","E","R","T","T","Y","Y" };

int main ()
{
  const unsigned count = sizeof (labels) / sizeof (labels[0]);
  ostream_iterator <char*> iter (cout);
  copy (labels, labels + count, iter);
  cout << endl;
  unique (labels, labels + count, str_equal);
  copy (labels, labels + count, iter);
  cout << endl;
  return 0;
}
