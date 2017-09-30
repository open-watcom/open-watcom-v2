// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers1[5] = { 1, 6, 13, 25, 101 };
int numbers2[5] = {-5, 26, 36, 46, 99 };

int main ()
{
  int result[10];
  merge (numbers1, numbers1 + 5, numbers2, numbers2 + 5, result);
  for (int i = 0; i < 10; i++)
    cout << result[i] << ' ';
  cout << endl;
  return 0;
}
