// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int input1 [4] = { 6, 8, 10, 2 };
int input2 [4] = { 4, 2, 11, 3 };

int main ()
{
  int output [4];
  transform (input1, input1 + 4, input2, output, modulus<int> ());
  for (int i = 0; i < 4; i++)
    cout << output[i] << endl;
  return 0;
}
