// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

typedef multimap<int, char, less<int> > mmap;

typedef pair<const int, char> pair_type;

pair_type p1 (3, 'c');
pair_type p2 (6, 'f');
pair_type p3 (1, 'a');
pair_type p4 (2, 'b');
pair_type p5 (3, 'x');
pair_type p6 (6, 'f');

pair_type array [] =
  {
    p1,
    p2,
    p3,
    p4,
    p5,
    p6
  };

int main ()
{
  mmap m (array, array + 7);
  mmap::iterator i;
  // Return location of first element that is not less than 3
  i = m.lower_bound (3);
  cout << "lower bound:" << endl;
  cout << (*i).first << " -> " << (*i).second << endl;
  // Return location of first element that is greater than 3
  i = m.upper_bound (3);
  cout << "upper bound:" << endl;
  cout << (*i).first << " -> " << (*i).second << endl;
  return 0;
}
