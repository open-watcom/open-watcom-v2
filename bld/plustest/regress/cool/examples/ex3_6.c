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

#include <cool/Complex.h>                       // Include complex header file

#define FREQUENCY 346.87
#define OMEGA (2 * 3.14159265358979323846 * FREQUENCY)

inline CoolComplex in_series (const CoolComplex& c1, const CoolComplex& c2) {
  return (c1+c2);
}

inline CoolComplex in_parallel (const CoolComplex& c1, const CoolComplex& c2) {
  return ((c1.invert() + c2.invert()).invert ());
}

inline CoolComplex resistor (double r) {
  return CoolComplex (r);
}

inline CoolComplex inductor (double i) {
  return CoolComplex (0.0, i * OMEGA);
}

inline CoolComplex capacitor (double c) {
  return CoolComplex (0.0, -1.0 / (c * OMEGA));
}

int main (void) {
    cout << __FILE__ << endl;
  CoolComplex circuit;
  circuit = in_series (resistor (1.0),
                       in_parallel (in_series (resistor (100.0),
                                               inductor (0.2)),
                                    in_parallel (capacitor (0.000001),
                                                 resistor (10000000.0))));
  cout << "Circuit impedence is " << circuit << " at frequency " << FREQUENCY << "\n";
  return 0;                                     // Exit with OK status
}
