// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

int years[] = { 1942, 1952, 1962, 1972, 1982, 1992 };

int main ()
{
  const unsigned yearCount = sizeof (years) / sizeof (years[0]);
  int* location = find (years, years + yearCount, 1972);
  cout << "Found 1972 at offset " << (location - years) << endl;
  return 0;
}
