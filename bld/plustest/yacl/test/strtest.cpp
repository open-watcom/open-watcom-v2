

#include "base/base.h"

#include <stdio.h>
#include <strstream.h>

main ()
{
    {
        CL_String line ("A sample, with a comma, here.");
        CL_String f1 = line.Field (1); // f1 now contains "A"
        f1 = line.Field (2, " ,");     // f1 now contains "sample"
        f1 = line.Field (2, ",");      // f1 now contains " with a comma"
        CL_String fld[3];
        short n = line.Split (fld, 3, " ,");
        // Now fld[0] contains "A"
        //     fld[1] contains "sample"
        //     fld[2] contains ", with a comma, here."
        //     n contains 3.
        short i;
        i = 1;
    }
    {
        CL_String s ("This string has five words.");
        CL_StringSplitter split (s);
        CL_String field;
        split.Reset();
        field = split.Next ();     // Use blank as separator, so
                                   // field now contains "This"
        field = split.Next ("i");  // field is now " str", and the
                                   // separator set becomes "i"
        field = split.Next ();     // field is now "ng has f", since "i"
                                   // is the current separator set
    }
    istrstream st1 ("Line 1\nLine 02\n Line 3\n 12:30");
    CL_String s1;
    s1.ReadLine (st1);
    cout << s1 << endl;
    s1.ReadLine (st1);
    cout << s1 << endl;
    s1.ReadLine (st1);
    cout << s1 << endl;
    CL_TimeOfDay time1;
    st1 >> time1;
    cout << time1 << endl;
    
    CL_String strg1;

    strg1.Insert (" Hello ABC ");
    strg1.Insert ('W', 10);
    strg1.Insert (" 123456789 123456789 123456789 12345678 ", 5);
    strg1.Append ('!');
    
    strg1 = 9879873L;
    strg1 = 8989.893;

    CL_String s (5003L, 10);
    CL_String t (-51L, 4);
    CL_String u (3L, 2);
    CL_String v (0L, 3);

    v.AssignWithFormat ("String '%s' long %ld char '%c'", strg1.AsPtr(),
                        strg1.AsLong(), 'x');
    CL_String line (" A line where \"Hello, world\" is included");
    //    printf (" 'where' occurs at position %ld\n", line.Index ("where"));

    // Test the substring operations
    line(3, 5) = "HELLO";
    printf ("Line is '%s'\n", line.AsPtr());
    line (8,2) += "JUNK";

    line (39,3) += "Some tail-end junk";
    line (22,0) = " Inserted text "; // Insert before position 22
    line (22,9) = ""; // Remove the word " Inserted "
    
    printf ("Count %ld\n", line (5, 20).ToLower ());
    printf ("Count %ld\n", line(10,7).ToUpper ());
    CL_String flds[30];
    s = line.Field (0); printf ("Field 0: '%s'\n", s.AsPtr());
    s = line.Field (1); printf ("Field 1: '%s'\n", s.AsPtr());
    s = line.Field (2); printf ("Field 2: '%s'\n", s.AsPtr());
    s = line.Field (5); printf ("Field 5: '%s'\n", s.AsPtr());
    s = line.Field (7); printf ("Field 7: '%s'\n", s.AsPtr());
    CL_StringSplitter split (line);
    char* seps = " ";
    for (split.Reset(); s = split.Next(seps), s.Length() > 0;) {
        printf ("s: '%s'\n", (const char*) s);
        seps = "w";
    }

    printf ("Testing the split into a string sequence:\nString: '%s'\n",
            line.AsPtr());
    CL_Sequence<CL_String> seq = line.Split();
    for (short i0 = 0; i0 < seq.Size(); i0++)
        printf ("Field %d: '%s'\n", i0, seq[i0].AsPtr());
    printf ("------------------\n");
    
    line = "Another string";
    s = line.Field (2);

    s = CL_String ("The gcd of ") + 7L + " and " + CL_String (19L)
        + " is ";
    s = s + 1L;
    printf ("%s\n", (const char*) s);
    
    CL_Integer i1(5938), i2(9), i3(5), i4 (93);
    printf ("First: %d\n", i1.CompareWith (i4, CL_Object::OP_PREFIX));
    printf ("Second: %d\n", i1.CompareWith (i2, CL_Object::OP_EQUAL));
    printf ("Third: %d\n", i1.CompareWith (i4, CL_Object::OP_CONTAINS));
    printf ("Fourth: %d\n", i1.CompareWith (i3, CL_Object::OP_PREFIX));
return(0);
}
