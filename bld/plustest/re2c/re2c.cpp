/* $Log:        MAIN.C $
Revision 1.1  92/12/16  11:17:34  Anthony_Scian
.

// Revision 1.1  1992/08/20  17:14:07  peter
// Initial revision
//
 */

#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <new.h>

extern "C" {
    extern int getopt(int*, char**, char*,char**);
    extern char *optarg;
    extern int optind;
}

#include "globals.h"
#include "parser.h"
#include "dfa.h"

uchar xlat[256], talx[256];
char *fileName;
bool sFlag = false;
bool bFlag = false;

char *usage_msg[] = {
    "usage: re2c [-e] [-n name]\n",
    NULL
};

void out_of_mem( void )
{
    abort();
}

int main(int argc, char *argv[]){
    set_new_handler(out_of_mem);
    fileName = NULL;
    int o;
    for(uint c = 0; c < 256; ++c)
        xlat[c] = talx[c] = c;
    while((o = getopt(&argc, argv, "esbn:d:",usage_msg)) != -1){
        switch(o){
        case 'e':
            memcpy(xlat, asc2ebc, sizeof(xlat));
            memcpy(talx, ebc2asc, sizeof(talx));
            break;
        case 's':
            sFlag = true;
            break;
        case 'b':
            bFlag = true;
            break;
        case 'n':
            fileName = optarg;
            break;
        case 'd': {
            ifstream in(optarg);
            ofstream out("debug.out");
            parse(in, out);
            return 0;
        } case '?':
            goto errors;
        }
    }
    if(optind < argc){
    errors:
        cerr << "usage: re2c [-e] [-n name]\n";
        exit(2);
    }
    parse(cin, cout);
    return 0;
}
