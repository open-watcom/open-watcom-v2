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

#include <cool/Queue.h>                         // COOL Queue class
#include <cool/Random.h>                        // COOL Random number class

#include <cool/Queue.C>

int main (void) {
    cout << __FILE__ << endl;
  CoolQueue<double> q1;                         // CoolQueue of default size
  CoolRandom r (SIMPLE, 1, 3.0, 9.0);           // Simple CoolRandom generator
  for (int i = 0; i < 5; i++)                   // Put five CoolRandom numbers
    q1.put (r.next ());                         // into the CoolQueue
    cout << q1 << "\n";                         // Output CoolQueue elements
  for (q1.reset(); q1.next(); )                 // For each element in CoolQueue
    if (q1.value() < 4.5)                       // If less than tolerance
      q1.remove ();                             // Remove from CoolQueue
  cout << q1 << "\n";                           // Output CoolQueue elements
  return (0);
}
  
