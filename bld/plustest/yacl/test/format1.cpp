

// A very simple text-formatting program: simply fills out each line with
// blanks so that the line extends to 75 chars.
//
// M. A. Sridhar
// Feb 13, 1994
// Redone with iostreams: 6/28/94

#include "base/base.h"
#include <iostream.h>
#include <iomanip.h>
main ()
{
    short max_line_length = 72;
    CL_String line;
    while (line.ReadLine (cin)) {
        CL_StringSequence words = line.Split ();
        register short word_count = words.Size();
        // Find the sum of the lengths of all the words
        if (word_count > 1) {
            short word_len_sum = 0;
            short i;
            for (i = 0; i < word_count; i++)
                word_len_sum += words[i].Size();
            short space = max_line_length - word_len_sum;
            short space_per_word = space / (word_count - 1);
            short leftover = space % (word_count - 1);
            for (i = 0; i < word_count - 1; i++) {
                cout << words[i] << setw (space_per_word) << ' ';
                if (leftover) {
                    cout << ' ';
                    leftover--;
                }
            }
        }
        if (word_count >= 1)
            cout << words[word_count-1] << endl;
        else // Empty line
            cout << endl;
    }
    return 0;
}



        
