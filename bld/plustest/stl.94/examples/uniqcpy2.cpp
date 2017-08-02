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
  ostream_iterator <char const*> iter (cout);
  copy (labels, labels + count, iter);
  cout << endl;
  char const* uCopy[count];
  fill (uCopy, uCopy + count, "");
  unique_copy (labels, labels + count, uCopy, str_equal);
  copy (labels, labels + count, iter);
  cout << endl;
  copy (uCopy, uCopy + count, iter);
  cout << endl;
  return 0;
}
