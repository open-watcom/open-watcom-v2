// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int main ()
{
  char buffer [100];
  int i = 0;
  cin.unsetf (ios::skipws); // Disable white-space skipping.
  cout << "Please enter a string: ";
  istream_iterator<char, ptrdiff_t> s (cin);
  while (*s != '\n')
    buffer[i++] = *s++;
  buffer[i] = '\0'; // Null terminate buffer.
  cout << "read " << buffer << endl;
  return 0;
}
