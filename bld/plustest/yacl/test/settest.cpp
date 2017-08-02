

#include "base/base.h"
#include <iostream.h>

void test1 ()
{
    CL_IntegerSet mySet (17, 21);
    mySet += CL_IntegerSet (35, 39);
    mySet += CL_IntegerSet (26, 29);

    cout << "IntegerSet mySet is " << mySet << endl;
    cout << "Rank of 13 is " << mySet.RankOf (13) << endl;
    cout << "Rank of 17 is " << mySet.RankOf (17) << endl;
    cout << "Rank of 21 is " << mySet.RankOf (21) << endl;
    cout << "Rank of 25 is " << mySet.RankOf (25) << endl;
    cout << "Rank of 35 is " << mySet.RankOf (35) << endl;
}

void test2 ()
{
    CL_BitSet mySet (17, 21);
    mySet += CL_BitSet (35, 39);
    mySet += CL_BitSet (26, 29);

    cout << "BitSet mySet is " << mySet << endl;
    cout << "Rank of 13 is " << mySet.RankOf (13) << endl;
    cout << "Rank of 17 is " << mySet.RankOf (17) << endl;
    cout << "Rank of 21 is " << mySet.RankOf (21) << endl;
    cout << "Rank of 25 is " << mySet.RankOf (25) << endl;
    cout << "Rank of 35 is " << mySet.RankOf (35) << endl;
}


main ()
{
    test1();
    test2();
    return 0;
}

