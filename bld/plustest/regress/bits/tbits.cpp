// tbits.cpp:	Test the bits class

#include <iostream.h>
#include <iomanip.h>
#include "bits.h"

main()
{
    dbits x(5), y, null; // grb

    cout << "Initial x: " << x << endl;

    for (int i = 0; i <= 5; ++i)
        x.set(i);
    cout << "x: " << x << " (" << x.count() << " bits set)" << endl;
    cout << "x == 3f? " << (x == 0x3f) << endl;
    cout << "x <<= 3 = " << (x <<= 3) << endl;
    cout << "x >>= 3 = " << (x >>= 3) << endl;
    cout << "x ^ 3 = " << (x ^ 3) << endl;
    cout << "x & 3 = " << (x & 3) << endl;
    cout << "3 & x = " << (((dbits)(3)) & x) << endl; //grb
    cout << "~x = " << ~x << endl;
        
    for (i = 4; i <= 12; ++i)
        y.set(i);
    cout << "y: " << y << " (" << y.count() << " bits set)" << endl;
    cout << "x & y = " << (x & y) << endl;
    cout << "x | y = " << (x | y) << endl;
    cout << "x ^ y = " << (x ^ y) << endl;
    return 0;
}
