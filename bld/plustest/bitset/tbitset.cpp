// tbitset.cpp:              Test the Bitset Class

#include <iostream.h>
#include "bitset.h"

main()
{
    Bitset x(12), y(18), null(0);

    x.set(0,5);
    cout << "x: " << x
         << " (size = " << x.size()
         << ", count = " << x.count()
         << ")\n";
    cout << "~x = " << ~x << endl;
    y.set(4,12);
    cout << "y: " << y
         << " (size = " << y.size()
         << ", count = " << y.count()
         << ")\n";
    cout << "x <= y? " << (x <= y) << endl;
    cout << "x & y = " << (x & y) << endl;
    cout << "x | y = " << (x | y) << endl;
    cout << "x ^ y = " << (x ^ y) << endl;
    cout << "x - y = " << x - y << endl;
    cout << "y - x = " << y - x << endl;
    y |= x;
    cout << "y, after y |= x: " << y << endl;

    // Test operator void*
    cout << endl;
    if (x)
        cout << "this should print" << endl;
    x.reset();
    if (x)
        cout << "this shouldn't print" << endl;
    cout << "x == null? " << (x == null) << endl;
    
    // Test subscripting
    x.toggle();
    Bitset z = x;
    cout << endl;
    cout << "z = " << z << endl;
    z[1] = 0;
    int test = z[1];
    cout << "test: " << test << endl;
    cout << "z[1]: " << z[1] << endl;
    cout << z << endl;
    if (z[1])
        cout << "this shouldn't print\n";

    z[1] = 1;
    test = z[1];
    cout << "test: " << test << endl;
    cout << "z[1]: " << z[1] << endl;
    cout << z << endl;
    if (z[1])
        cout << "this should print\n";

    // Null set tests    
    cout << endl;
    cout << "null <= y? " << (null <= y) << endl;
    cout << "y & null = " << (y & null) << endl;
    cout << "y | null = " << (y | null) << endl;
    cout << "y ^ null = " << (y ^ null) << endl;
    cout << "y - null = " << y - null << endl;
    cout << "null - y = " << null - y << endl;
    cout << "null <= null? " << (null <= null) << endl;
    return 0;
}
