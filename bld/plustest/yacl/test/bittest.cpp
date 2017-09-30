


// Test program for BitSets
//
//  M. A. Sridhar 5/5/94

#include "base/bitset.h"
#include "base/intset.h"
#include <iostream.h>

void TestBigSet ()
{
    CL_BitSet bigSet (15, 80000, 256000); // Create a humongous set
    long i;
    for (i = 79999; i < 80004; i++)
        cout << i << " is" << (bigSet.Includes (i) ? " " : " not ")
             << "in the set." << endl;
}

void ItrTest ()
{
    CL_BitSet set;
    set = set + 15 + 45 + 12 + 31 + 16 + 77 + 11 + 32 + 34;
    cout << "ItrTest: set is " << set << endl;
    CL_BitSetIterator itr (set);
    short l;

    l = 3; itr.BeginFromRank (l); cout << "BeginFromRank " << l << endl;
    while (itr.More())
        cout << "next: " << itr.Next() << endl;

    l = 0; itr.BeginFromRank (l); cout << "BeginFromRank " << l << endl;
    while (itr.More())
        cout << "next: " << itr.Next() << endl;

    l = 1; itr.BeginFromRank (l); cout << "BeginFromRank " << l << endl;
    while (itr.More())
        cout << "next: " << itr.Next() << endl;

    l = 20; itr.BeginFromRank (l); cout << "BeginFromRank " << l << endl;
    while (itr.More())
        cout << "next: " << itr.Next() << endl;

    l = 9; itr.BeginFromRank (l); cout << "BeginFromRank " << l << endl;
    while (itr.More())
        cout << "next: " << itr.Next() << endl;

    l = 10; itr.BeginFromRank (l); cout << "BeginFromRank " << l << endl;
    while (itr.More())
        cout << "next: " << itr.Next() << endl;

}


void SuccessorTest ()
{
    CL_BitSet set;
    set = set + 15 + 45 + 12 + 31 + 16 + 77 + 11 + 32 + 34;
    cout << "SuccessorTest: set is " << set << endl;
    long l;

    l = 0; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 14; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 15; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 16; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 31; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 32; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 76; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 77; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 85; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 128; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
    l = 1000; cout << "Successor of " << l << " is " << set.Successor(l) << endl;
}

int main ()
{
    SuccessorTest ();
    ItrTest ();
    CL_BitSet set1 = CL_BitSet (25) + 1 + 3 + 15 + 21 + 7;
    CL_BitSet set2;
    set2 = set2 + 2 + 3 +  1 +  15;
    TestBigSet ();
    
    CL_String strg = set2.AsString();
    cout << "set2: " << strg << endl;

    strg = set1.AsString ();
    cout << "set1: " <<  strg << endl;

    CL_BitSet set5 (5, 15, 30);
    cout << "Set5: " << set5 << endl;

    cout << "Complement of set5: " << ~set5 << endl;
    CL_BitSet set3;
    set3 = set1 * set2;

    CL_IntegerSet set = set1.AsSet ();
    cout << "Set1:" << endl;
    short i;
    for (i = 0; i < set.Size(); i++)
        cout << " " << set.ItemWithRank (i);
    cout << endl;

    long r = 3;
    cout << "Element of rank " << r << " in set2 is "
        << set2.ItemWithRank(r) << endl;
    cout << "Set2:" << endl;
    for (i = 0; i < set2.Size(); i++)
        cout <<  " " << set2.ItemWithRank(i);
    cout << endl;
            
    set = set3.AsSet ();
    cout << "Set3:" << endl;
    CL_BitSetIterator itr1 (set3);
    for (i = 0; i < set.Size(); i++)
        cout <<  " " << itr1.Next();
    cout << endl;
            
    CL_BitSetIterator itr (set1);
    cout << "set1 via iterator:" << endl;
    for (itr.Reset(); (i = itr.Next()) >= 0; ) {
        cout << "i = " << i << endl;
    }

    cout <<  endl << "set3: smallest " << set3.Smallest()
         << " largest " << set3.Largest() << endl;
    cout <<  endl;
    cout << "set1: smallest " << set1.Smallest()
         << " largest " << set1.Largest() << endl;
    cout << "set5: smallest " << set5.Smallest()
         << " largest " << set5.Largest() << endl;
return(0);
}
