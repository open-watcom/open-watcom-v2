// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers[5] = { 1, 2, 3, 4, 5 };

int main ()
{
  int result[5];
  copy_backward (numbers, numbers + 5, result + 5);
  int i;
  for (i = 0; i < 5; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  for (i = 0; i < 5; i++)
    cout << result[i] << ' ';
  cout << endl;
  return 0;
}
