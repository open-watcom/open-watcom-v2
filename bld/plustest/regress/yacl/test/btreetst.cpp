

// Miscellaneous calls to B-tree methods for testing
// M. A. Sridhar 5/5/94

#include "base/tbtree.h"
#include "base/string.h"


#include <stdio.h>
#include <iostream.h>

extern unsigned _stklen = 24000;

typedef CL_BTree<CL_String>           CL_StringBTree;
typedef CL_BTreeIterator<CL_String>   CL_StringBTreeIterator;
main ()
{
    CL_StringBTree tree (4); // Order 4, just for kicks


    CL_String s;
    long i;

    for (i = 0; i < 100; i++) {
        tree.Add ("A " + CL_String (i, 3));
    }
//     cout << tree << flush;
//     cout << "-----------------------------------\n\n";

    tree.Add ("A 002");
    cout << tree << flush;
    cout << "0-----------------------------------\n\n";

    for (i = 0; i < 15; i++)
        cout << "ExtractMin: " << tree.ExtractMin () << endl;

    for (i = 0; i < 15; i++) {
        tree.Add ("A " + CL_String (i, 3));
    }

    {
        CL_String s;
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "Axxx";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A  36";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A 002";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A   8";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A  141";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A  14";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A   0";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
    }

    s = "A 003";
    printf ("%s is %s in the tree (it shouldn't be)\n", s.AsPtr(),
            (tree.Find (s)) ? "" : "not");
    s = "A   3";
    printf ("%s is %s in the tree (it should be)\n", s.AsPtr(),
            (tree.Find (s)) ? "" : "not"); 
    long rank = 0;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());
    rank = 15;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());
    rank = 17;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());
    rank = 36;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());
    rank = 40;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());
    rank = 50;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());
    rank = -30;
    printf ("The element with rank %ld is '%s'\n", rank,
            tree.ItemWithRank(rank).AsString().AsPtr());


    printf ("\nBegin Btree iterator test\n");
    CL_StringBTreeIterator iter (tree);
    for (iter.Reset(); iter.More();) {
        CL_String s = iter.Next();
        printf ("'%s'\n", (const char*) s);
    }
    printf ("z-----------------------------------------------\n\n");

    printf ("\nBegin Btree iterator test BeginFrom\n");
    iter.BeginFrom ("A   8");
    while (iter.More()) {
        CL_String s = iter.Next ();
        printf ("'%s'\n", (const char*) s);
    }
    printf ("zz-----------------------------------------------\n\n");
    printf ("\nBegin Btree iterator test BeginFrom\n");
    iter.BeginFrom ("A  26");
    while (iter.More()) {
        CL_String s = iter.Next ();
        printf ("'%s'\n", (const char*) s);
    }
    printf ("zz-----------------------------------------------\n\n");
    printf ("\nBegin Btree iterator test BeginFrom\n");
    iter.BeginFrom ("A  27");
    while (iter.More()) {
        CL_String s = iter.Next ();
        printf ("'%s'\n", (const char*) s);
    }
    printf ("zz-----------------------------------------------\n\n");
        
    printf ("\nBegin Btree iterator test BeginFrom\n");
    iter.BeginFrom ("Axxxx");
    while (iter.More()) {
        CL_String s = iter.Next ();
        printf ("'%s'\n", (const char*) s);
    }
    printf ("zz-----------------------------------------------\n\n");
        
    printf ("\nBegin Btree iterator test BeginFrom\n");
    iter.BeginFrom ("A   0");
    while (iter.More()) {
        CL_String s = iter.Next ();
        printf ("'%s'\n", (const char*) s);
    }
    printf ("zz-----------------------------------------------\n\n");
        
    for (i = 39; i >= 25; i--) {
        tree.Remove ("A " + CL_String (i, 3));
        cout << tree << flush;
        // printf ("-----------------------------------\n\n");
    }

    cout << tree << flush;
    printf ("a-----------------------------------\n\n");

    {
        CL_String s;
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "Axxx";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A  36";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A 002";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A   8";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A  141";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A  14";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
        s = "A   0";
        printf ("The rank of '%s' is %ld\n", s.AsPtr(), tree.RankOf (s));
    }

    i = 8;
    tree.Remove ("A " + CL_String (i, 3));
    cout << tree << flush;
    printf ("-1----------------------------------\n\n");

    i = 20;
    tree.Remove ("A " + CL_String (i, 3));
    cout << tree << flush;
    printf ("-2----------------------------------\n\n");

    i = 29;
    tree.Remove ("A " + CL_String (i, 3));
    cout << tree << flush;
    printf ("-3----------------------------------\n\n");

    i = 17;
    tree.Remove ("A " + CL_String (i, 3));
    cout << tree << flush;
    printf ("--4---------------------------------\n\n");

    i = 2;
    tree.Remove ("A " + CL_String (i, 3));
    cout << tree << flush;
    printf ("---5--------------------------------\n\n");

    i = 5;
    tree.Remove ("A " + CL_String (i, 3));
    cout << tree << flush;
    printf ("-6----------------------------------\n\n");

    for (i = 0; i <= 80; i++) {
        tree.Remove ("A " + CL_String (i, 3));
        cout << tree << flush;
        printf ("-7----------------------------------\n\n");
    }
    cout << tree << flush;
    printf ("-8----------------------------------\n\n");



    cout << tree << flush;
    printf ("-9----------------------------------\n\n");
    
    for (i = 0; i < 40; i++) {
        switch (i % 3) {
        case 0:
            s = "K " + CL_String(i);
            break;
            
        case 1:
            s = "B " + CL_String(i);
            break;
            
        case 2:
            s = "Q " + CL_String(i);
            break;
        }
        tree.Add (s);
        cout << tree << flush;
        // printf ("-----------------------------------\n\n");
    }
            
    tree.Add ("Z Goodbye, cruel world!");
    cout << tree << flush;
    // return 0;

    printf ("-10--------------------------------------\n\n");
    tree.Remove ("Q 2");
    cout << tree << flush;
    printf ("-11--------------------------------------\n\n");
    tree.Remove ("Q 29");
    cout << tree << flush;
    printf ("-12--------------------------------------\n\n");
    tree.Remove ("Q 27");
    cout << tree << flush;


    return 0;
}


    
