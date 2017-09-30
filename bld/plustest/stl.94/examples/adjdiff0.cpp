// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int numbers[5] = { 1, 2, 4, 8, 16 };

int main ()
{
  int difference[5];
  adjacent_difference (numbers, numbers + 5, difference);
  int i;
  for (i = 0; i < 5; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  for (i = 0; i < 5; i++)
    cout << difference[i] << ' ';
  cout << endl;
  return 0;
}
