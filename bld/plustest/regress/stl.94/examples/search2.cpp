// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>
#include <string.h>

bool str_equal (const char* a_, const char* b_)
{
  return ::strcmp (a_, b_) == 0 ? 1 : 0;
}

char* grades[] = { "A", "B", "C", "D", "F" };
char* letters[] = { "Q", "E", "D" };

int main ()
{
  const unsigned gradeCount = sizeof (grades) / sizeof (grades[0]);
  const unsigned letterCount = sizeof (letters) / sizeof (letters[0]);
  ostream_iterator <char*> iter (cout, " ");
  cout << "grades: ";
  copy (grades, grades + gradeCount, iter);
  cout << "\nletters:";
  copy (letters, letters + letterCount, iter);
  cout << endl;

  char** location =
    search (grades, grades + gradeCount,
            letters, letters + letterCount,
            str_equal);

  if (location == grades + gradeCount)
    cout << "letters not found in grades" << endl;
  else
    cout << "letters found in grades at offset: " << location - grades << endl;

  copy (grades + 1, grades + 1 + letterCount, letters);

  cout << "grades: ";
  copy (grades, grades + gradeCount, iter);
  cout << "\nletters:";
  copy (letters, letters + letterCount, iter);
  cout << endl;

  location = search (grades, grades + gradeCount,
                     letters, letters + letterCount,
                     str_equal);

  if (location == grades + gradeCount)
    cout << "letters not found in grades" << endl;
  else
    cout
      << "letters found in grades at offset: " << location - grades << endl;
  return 0;
}
