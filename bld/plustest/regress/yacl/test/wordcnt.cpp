
// A program for counting the number of occurrences of each word in the
// input file.
//
// M. A. Sridhar
// July 29, 1994


#include "base/base.h"
#include <iostream.h>
main ()
{
    CL_String line;
    CL_StringIntMap word_map;
    long count = 0;
    const char* punctuation = " :~{}.();&,!?";
    while (line.ReadLine (cin)) {
        CL_StringSequence words = line.Split (punctuation);
        register long n = words.Size();
        for (register long i = 0; i < n; i++) {
            if (word_map.IncludesKey (words[i]))
                word_map[words[i]]++;
            else
                word_map.Add (words[i], 1);
        }
    }
    CL_StringIntMapIterator itr (word_map);
    CL_StringIntAssoc assoc;
    for (itr.Reset (); itr.More (); ) {
        assoc = itr.Next();
        cout << assoc.key << " " << assoc.value << endl;
    }
    cout << "Total " << word_map.Size() << " words." << endl;
return 0;
}
