// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

int vector1[5] = { 1, 2, 3, 4, 5 };
int vector2[5] = { 1, 2, 3, 4, 5 };

int main ()
{
  int result;
  result = inner_product (vector1, vector1 + 5, vector2, 0);
  cout << "Inner product = " << result << endl;
  return 0;
}
