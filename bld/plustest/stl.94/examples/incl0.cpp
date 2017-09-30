// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers1[5] = { 1, 2, 3, 4, 5 };
int numbers2[5] = { 1, 2, 4, 8, 16 };
int numbers3[2] = { 4, 8 };

int main ()
{
  if (includes (numbers1, numbers1 + 5, numbers3, numbers3 + 2))
    cout << "numbers1 includes numbers3" << endl;
  else
    cout << "numbers1 does not include numbers3" << endl;
  if (includes (numbers2, numbers2 + 5, numbers3, numbers3 + 2))
    cout << "numbers2 includes numbers3" << endl;
  else
    cout << "numbers2 does not include numbers3" << endl;
  return 0;
}
