// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

char* word1 = "ABCDEFGHIJKLMNO";
char* word2 = "LMNOPQRSTUVWXYZ";

int main ()
{
  ostream_iterator <char> iter (cout, " ");
  cout << "word1: ";
  copy (word1, word1 + ::strlen (word1), iter);
  cout << "\nword2: ";
  copy (word2, word2 + ::strlen (word2), iter);
  cout << endl;
  set_intersection (word1, word1 + ::strlen (word1),
                    word2, word2 + ::strlen (word2),
                    iter,
                    less<char>());
  cout << endl;
  return 0;
}
