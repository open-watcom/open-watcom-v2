// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

typedef vector <char*> CStrVector;

int equal_length (const char* v1_, const char* v2_)
{
  return ::strlen (v1_) == ::strlen(v2_);
}

char* names[] = { "Brett", "Graham", "Jack", "Mike", "Todd" };

int main ()
{
  const int nameCount = sizeof (names)/sizeof(names[0]);
  CStrVector v (nameCount);
  for (int i = 0; i < nameCount; i++)
    v[i] = names[i];
  CStrVector::iterator location;
  location = adjacent_find (v.begin (), v.end (), equal_length);
  if (location != v.end ())
    cout
      << "Found two adjacent strings of equal length: "
      << *location
      << " -and- "
      << *(location + 1)
      << endl;
  else
    cout << "Didn't find two adjacent strings of equal length.";
  return 0;
}
