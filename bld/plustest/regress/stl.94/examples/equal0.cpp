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
int numbers3[2] = { 1, 2 };

int main ()
{
  if (equal (numbers1, numbers1 + 5, numbers2))
    cout << "numbers1 is equal to numbers2" << endl;
  else
    cout << "numbers1 is not equal to numbers2" << endl;
  if (equal (numbers3, numbers3 + 2, numbers1))
    cout << "numbers3 is equal to numbers1" << endl;
  else
    cout << "numbers3 is not equal to numbers1" << endl;
  return 0;
}
