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

const unsigned size = 5;
char* n1[size] = { "Brett", "Graham", "Jack", "Mike", "Todd" };

int main ()
{
  char* n2[size];
  copy (n1, n1 + 5, n2);
  pair <char**, char**> result(0,0);
  result = mismatch (n1, n1+ size, n2, str_equal);
  if (result.first == n1 + size && result.second == n2 + size)
    cout << "n1 and n2 are the same" << endl;
  else
    cout << "mismatch at index: " << (result.first - n1) << endl;
  n2[2] = "QED";
  result = mismatch (n1, n1 + size, n2, str_equal);
  if (result.first == n2 + size && result.second == n2 + size)
    cout << "n1 and n2 are the same" << endl;
  else
    cout << "mismatch at index: " << (result.first - n1) << endl;
  return 0;
}
