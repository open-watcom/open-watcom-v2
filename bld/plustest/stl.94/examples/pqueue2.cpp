// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

struct greater_s : binary_function<char *, char *, bool> {
        typedef char * T;
    bool operator()(const T& x, const T& y) const { return strcmp( x, y ) > 0; }
};

int main ()
{
  priority_queue<deque<char*>, greater_s> q;
  q.push ((char*) "cat");
  q.push ((char*) "dog");
  q.push ((char*) "ape");
  q.push ((char*) "c1t");
  q.push ((char*) "3o1");
  q.push ((char*) "1pe");
  q.push ((char*) "86sVI5ronsYGc" );
  q.push ((char*) "86vy3e2ODUryw" );
  q.push ((char*) "86sVI5ronsYGc" );
  q.push ((char*) "86LC5YEChyf3Y" );
  q.push ((char*) "86mNoM8ZngBzc" );
  q.push ((char*) "86f2ZdZHj3lrY" );
  while (!q.empty ())
  {
    cout << q.top () << endl;
    q.pop ();
  }
  return 0;
}
