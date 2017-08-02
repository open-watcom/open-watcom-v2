// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <math.h>
#include <stl.h>

struct square_root : public unary_function<double, double>
{
  square_root () {}
  double operator () (double x_) const { return sqrt (x_); }
};

int input [3] = { -1, -4, -16 };

int main ()
{ 
  int output [3];
  transform (input, input + 3, output, 
    compose1 (square_root (), negate<int> ()));
  for (int i = 0; i < 3; i++)
   cout << output[i] << endl;
  return 0;
}
