// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

bool compare_strings (const char* s1_, const char* s2_)
{
  return ::strcmp (s1_, s2_) < 0 ? 1 : 0;
}

char* names[] = {  "Todd", "Mike", "Graham", "Jack", "Brett"};

int main ()
{
  const unsigned nameSize = sizeof (names)/sizeof (names[0]);
  vector <char*> v1(nameSize);
  for (int i = 0; i < v1.size (); i++)
  {
    v1[i] = names[i];
  }
  vector <char*> v2 (2);
  v2[0] = "foo";
  v2[1] = "bar";
  sort (v1.begin (), v1.end (), compare_strings);
  sort (v2.begin (), v2.end (), compare_strings);

  bool inc = includes (v1.begin (), v1.end (),
                       v2.begin (), v2.end (),
                       compare_strings);
  if (inc)
    cout << "v1 includes v2" << endl;
  else
    cout << "v1 does not include v2" << endl;
  v2[0] = "Brett";
  v2[1] = "Todd";
  inc = includes (v1.begin (), v1.end (),
                  v2.begin (), v2.end (),
                  compare_strings);
  if (inc)
    cout << "v1 includes v2" << endl;
  else
    cout << "v1 does not include v2" << endl;
  return 0;
}
