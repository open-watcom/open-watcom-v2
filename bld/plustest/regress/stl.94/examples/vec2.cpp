// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

void print (vector<double>& vector_)
{
  for (int i = 0; i < vector_.size (); i++)
    cout << vector_[i] << " ";
  cout << endl;
}

int main ()
{
  vector<double> v1; // Empty vector of doubles.
  v1.push_back (32.1);
  v1.push_back (40.5);
  vector<double> v2; // Another empty vector of doubles.
  v2.push_back (3.56);
  cout << "v1 = ";
  print (v1);
  cout << "v2 = ";
  print (v2);
  v1.swap (v2); // Swap the vector's contents.
  cout << "v1 = ";
  print (v1);
  cout << "v2 = ";
  print (v2);
  v2 = v1; // Assign one vector to another.
  cout << "v2 = ";
  print (v2);
  return 0;
}
