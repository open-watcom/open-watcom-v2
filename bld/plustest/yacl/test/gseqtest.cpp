

// A simple program to demonstrate the GenericSequence class.

#include "base/genseq.h"
#include <string.h>
#include <iostream.h>

class MyComparator: public CL_AbstractComparator {

public:
    short operator() (CL_VoidPtr p1, CL_VoidPtr p2) const
        { return strcmp ((const char*) p1, (const char*) p2);};
};


main ()
{
    MyComparator my_comp;
    CL_GenericSequence my_seq (0, &my_comp);
    my_seq.Add ("Line 1");
    my_seq.Add ("Some string");
    my_seq.Add ("44");
    my_seq.Add ("Have a nice day");

    my_seq.Sort ();
    register long i, n = my_seq.Size();
    for (i = 0; i < n; i++)
        cout << (const char*) my_seq[i] << endl;
    return 0;
}
