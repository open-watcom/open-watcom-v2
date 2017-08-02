// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

// 97/08/19 -- J.W.Welch        -- specify starting seed

#include <stl.h>
#include <iostream.h>
#include <stdlib.h>

int main ()
{
  srand( 1 );
  int numbers[10];
  generate (numbers, numbers + 10, rand);
  for (int i = 0; i < 10; i++)
    cout << numbers[i] << ' ';
  cout << endl;
  return 0;
}
