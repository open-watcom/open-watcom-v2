


#include "base/strgset.h"
#include <iostream.h>


char* strings [] = {
    "Hello",
    "Hello, world!",
    "Help",
    "Hear, hear!",
    "Helix",
    "Helga",
    "Happy",
    "Heaven",
    0
};

    



main ()
{
    CL_StringSet aSet;

    for (short i = 0; strings[i] != 0; i++)
        aSet.Add (strings[i]);
    CL_StringSet anotherSet = aSet.StringsWithPrefix ("He");
    CL_StringSetIterator itr (anotherSet);
    for (itr.Reset(); itr.More(); ) 
        cout << "'" << itr.Next() << "'\n";
    cout << (anotherSet == aSet ? "equal" : "not equal") << endl;
    cout << (aSet.IncludesAll (anotherSet) ? "includes" : "does not include")
         << endl;
             
return(0);
}
