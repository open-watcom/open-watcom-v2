// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

int add (int a_, int b_)
{
  return a_ + b_;
}

int mult (int a_, int b_)
{
  return a_ * b_;
}

int main ()
{
  vector <int> v1 (3);
  vector <int> v2 (v1.size ());
  for (int i = 0; i < v1.size (); i++)
  {
    v1[i] = i + 1;
    v2[i] = v1.size () - i;
  }
  ostream_iterator<int> iter (cout, " ");
  cout << "Inner product (product of sums):\n\t";
  copy (v1.begin (), v1.end (), iter);
  cout << "\n\t";
  copy (v2.begin (), v2.end (), iter);
  int result =
    inner_product (v1.begin (), v1.end (),
                   v2.begin (),
                   1,
                   mult, add);
  cout << "\nis: " << result << endl;
  return 0;
}
