// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

bool less_than (int a_, int b_)
{
  return a_ < b_;
}
 
bool greater_than (int a_, int b_)
{
  return a_ > b_;
}
 
int array [] = { 3, 6, 1, 9 };

int main ()
{
  struct fn_type : pointer_to_binary_function<int, int, bool> {
      fn_type( bool (*x)( int, int ) = 0 )
          : pointer_to_binary_function<int, int, bool>( x ) {
          }
  };
  typedef multiset<int, fn_type> mset;
  fn_type f (less_than);
  mset s1 (array, array + 4, f);
  mset::const_iterator i = s1.begin ();
  cout << "Using less_than: " << endl;
  while (i != s1.end ())
    cout << *i++ << endl;
  fn_type g (greater_than);
  mset s2 (array, array + 4, g);
  i = s2.begin ();
  cout << "Using greater_than: " << endl;
  while (i != s2.end ())
    cout << *i++ << endl;
  return 0;
}
