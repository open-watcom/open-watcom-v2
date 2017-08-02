// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <stdlib.h>

class Fibonacci
{
  public:
    Fibonacci () : v1 (0), v2 (1) {}
    int operator () ();
  private:
    int v1;
    int v2;
};

int
Fibonacci::operator () ()
{
  int r = v1 + v2;
  v1 = v2;
  v2 = r;
  return v1;
}

int main ()
{
  vector <int> v1 (10);
  Fibonacci generator;
  generate (v1.begin (), v1.end (), generator);
  ostream_iterator<int> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  return 0;
}
