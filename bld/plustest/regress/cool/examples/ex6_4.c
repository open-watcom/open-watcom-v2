//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//

#include <cool/String.h>                        // COOL String class
#include <cool/Vector.h>                        // COOL Vector class

#include <cool/Vector.C>

int my_sort (const CoolString& s1, const CoolString& s2) {
  return ((s1 >= s2) ? -1 : 1);                 // Reverse alphabetize
}

int main (void) {
    cout << __FILE__ << endl;
  CoolVector<CoolString> v1(5);                 // Declare CoolVector of CoolStrings
  v1.push ("Texas");                            // Add "Texas" 
  v1.push ("Alaska");                           // Add "Alaska" 
  v1.push ("New York");                         // Add "New York"
  v1.push ("Alabama");                          // Add "Alabama"
  v1.push ("North Dakota");                     // Add "North Dakota"
  cout << v1 << "\n";                           // Output the CoolVector
  v1.sort (my_sort);                            // Reverse sort the CoolVector
  for (v1.reset(); v1.next(); )                 // For each element 
    cout << v1.value() << "\n";                 // Output the value
  return (0);                                   // Exit with OK status
}
