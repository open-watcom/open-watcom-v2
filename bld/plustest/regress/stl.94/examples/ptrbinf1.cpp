// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int sum (int x_, int y_)
{
  return x_ + y_;
}

int input1 [4] = { 7, 2, 3, 5 };
int input2 [4] = { 1, 5, 5, 8 };

int main ()
{
  int output [4];
  transform (input1, input1 + 4, input2, output, 
    pointer_to_binary_function<int, int, int> (sum));
  for (int i = 0; i < 4; i++)
    cout << output[i] << endl;
  return 0;
}
