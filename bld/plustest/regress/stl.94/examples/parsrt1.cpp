// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

// 97/08/19 -- J.W.Welch        -- specify starting seed

#include <stl.h>
#include <stdlib.h>
#include <iostream.h>

int main ()
{
  srand( 1 );
  vector <int> v1 (10);
  for (int i = 0; i < v1.size (); i++)
    v1[i] = rand () % 10;
  ostream_iterator<int> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  partial_sort (v1.begin (),
                v1.begin () + v1.size () / 2,
                v1.end ());
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  return 0;
}
