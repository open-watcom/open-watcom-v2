


// A test program to exercise some of the methods on the Sequence and
// related classes
//
// M. A. Sridhar
// January 1994

#include "base/objseq.h"
#include "base/strgseq.h"
#include "base/intset.h"
#include "base/intseq.h"
#include "io/binfile.h"
#include <stdio.h>


void PrintOut (const CL_StringSequence& a)
{
    for (short i = 0; i < a.Size(); i++)
        printf ("'%s'\n", a[i].AsPtr());
    printf ("-------------------------------------------\n");
}


main ()
{
    // CL_StringSequence tst (18000); // Testing big sequences
    CL_StringSequence a;
    a.Add ("world!");
    a.Add ("Hello,");
    a.Add ("this is");
    a.Add ("a great day.");
    PrintOut (a);

    a.Insert ("world! 2", 1);
    PrintOut (a);
    a.Insert ("Hello, 2", -1);
    PrintOut (a);
    a.Insert ("this is 2", 2);
    PrintOut (a);
    a.Insert ("a great day. 2", 3);
    PrintOut (a);


    a.Sort();
    PrintOut (a);

#ifndef __GNUC__  // GCC Still has bugs!
    CL_IntegerSet st1(2,4);
    st1.Add (4);
    st1.Add (7);
    st1.Add (8);
    st1.Add (9);
    CL_StringSequence sq = a - st1;
    PrintOut (sq);
#endif
    
    a.ShiftRightAt (2);
    PrintOut (a);

    a[2] = "Some string here";
    PrintOut (a);
    
    a.ShiftLeftAt (2, 2);
    PrintOut (a);

    CL_StringSequence* b = new CL_StringSequence(10);
    *b = a;
    PrintOut (*b);

    *b += a;
    PrintOut (*b);
    
    b->Sort();
    PrintOut (*b);
    
    CL_BinaryFile f ("seqtest.dat", TRUE);
    f << *b; // Testing persistent sequences
    b->MakeEmpty ();
    

    // CL_Builder<CL_String> aBuilder;
    // CL_ObjectSequence a2(0, &aBuilder);
    CL_ObjectSequence a2;
    CL_String* pStrg;
    pStrg = new CL_String ("1"); a2.Add (pStrg);
    pStrg = new CL_String ("2"); a2.Add (pStrg);
    pStrg = new CL_String ("3"); a2.Add (pStrg);
    pStrg = new CL_String ("10"); a2.Add (pStrg);
    pStrg = new CL_String ("20"); a2.Add (pStrg);
    pStrg = new CL_String ("30"); a2.Add (pStrg);
    pStrg = new CL_String ("11"); a2.Add (pStrg);
    pStrg = new CL_String ("22"); a2.Add (pStrg);
    pStrg = new CL_String ("31"); a2.Add (pStrg);
    pStrg = new CL_String ("10"); a2.Add (pStrg);
    printf ("a2's size: %ld\n", a2.Size());

    f << a2;             // Testing persistent sequences
    f.SeekToBegin();
    f >> *b;
    // b->ReadFrom (f);
    
    printf ("Restored b is: \n"); PrintOut (*b);
    delete b;

    long i;
    for (i = 0; i < a2.Size(); i++) {
        printf ("'%s'\n", ((CL_String*) a2[i])->AsPtr());
    }
    pStrg = (CL_String*) a2.ExtractLeftmost();
    printf ("Leftmost: '%s'\n", pStrg->AsPtr());
    for (i = 0; i < a2.Size(); i++) {
        printf ("'%s'\n", ((CL_String*) a2[i])->AsPtr());
    }
    delete pStrg;

    pStrg = (CL_String*) a2[4];
    a2.Remove (4);
    delete pStrg;
    for (i = 0; i < a2.Size(); i++) {
        printf ("'%s'\n", ((CL_String*) a2[i])->AsPtr());
    }
    
    a2.DestroyContents ();
    f >> a2;
    printf ("Restored a2 is:\n");
    for (i = 0; i < a2.Size(); i++) {
        printf ("'%s'\n", ((CL_String*) a2[i])->AsPtr());
    }
    printf ("------------------------------------\n");
    a2.DestroyContents ();
    
    
    

    CL_IntegerSequence iseq;
    for (i = 0; i < 100; i++) {
        iseq.Add (i);
        iseq.Add (5*i);
    }
    for (i = 5; i < 80; i++)
        iseq.Remove (2);
    iseq.Add (253);
    printf ("iseq size %ld\n", iseq.Size());
    long j;
    for (j = 0; j < iseq.Size(); j++) {
        printf ("iseq[%ld] = %ld\n", j, iseq[j]);
    }
    iseq.Sort();
    printf ("==============================\niseq size %ld\n", iseq.Size());
    for (j = 0; j < iseq.Size(); j++) {
        printf ("iseq[%ld] = %ld\n", j, iseq[j]);
    }
    iseq.Sort();

    return 0;
}


