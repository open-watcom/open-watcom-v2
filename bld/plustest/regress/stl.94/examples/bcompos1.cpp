// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

struct odd : public unary_function<int, bool>
{
  odd () {}
  bool operator () (int n_) const { return (n_ % 2) == 1; }
};
 
struct positive : public unary_function<int, bool>
{
  positive () {}
  bool operator () (int n_) const { return n_ >= 0; }
};

int array [6] = { -2, -1, 0, 1, 2, 3 };

int main ()
{ 
  binary_compose<logical_and<bool>, odd, positive> b ((logical_and<bool> ()), odd (), positive ());
  int* p = find_if (array, array + 6, b);
  if (p != array + 6)
    cout << *p << " is odd and positive" << endl;

  return 0;
}
