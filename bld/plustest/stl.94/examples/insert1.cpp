// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

char* array1 [] = { "laurie", "jennifer", "leisa" };
char* array2 [] = { "amanda", "saskia", "carrie" };

int main ()
{
  deque<char*> names (array1, array1 + 3);
  deque<char*>::iterator i = names.begin () + 2;
  copy (array2, array2 + 3, insert_iterator<deque <char*> > (names, i));
  deque<char*>::iterator j;
  for (j = names.begin (); j != names.end (); j++)
    cout << *j << endl;
  return 0;
}
