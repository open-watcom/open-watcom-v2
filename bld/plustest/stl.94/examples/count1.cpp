// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int main ()
{
  vector <int> numbers(100);
  for (int i = 0; i < 100; i++)
    numbers[i] = i % 3;
  int elements = 0;
  count (numbers.begin (), numbers.end (), 2, elements);
  cout << "Found " << elements << " 2's." << endl;
  return 0;
}
