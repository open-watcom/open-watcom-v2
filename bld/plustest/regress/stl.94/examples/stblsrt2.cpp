// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

bool string_less(const char* a_, const char* b_)
{
  return ::strcmp (a_, b_) < 0 ? 1 : 0;
}

char* letters[6] = {"bb", "aa", "ll", "dd", "qq", "cc" };

int main ()
{
  stable_sort (letters, letters + 6, string_less);
  for (int i = 0; i < 6; i++)
    cout << letters[i] << ' ';
  cout << endl;
  return 0;
}
