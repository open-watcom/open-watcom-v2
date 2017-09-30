/*
 * Simple program to read STDIN and justify the text to the
 * specified number of columns and output to STDOUT.
 * The padding is alternated from line to line so as to look even.
 * Blank lines in the input stream are preserved.
 * This was designed to be used as a filter.
 */

#include    <iostream.h>
#include    <stdlib.h>

#include    "splash.h"
#include    "tracer.h"

// Some useful synonyms
typedef SPString Str;
typedef SPStringList StrList;

int tog= 0;

static int nextnonspace(const Str& s, int n)
{
    while(n < s.length() && s[n] == ' ') n++;    
    return n;
}

static int prevnonspace(const Str& s, int n)
{
    while(n >= 0 && s[n] == ' ') n--;    
    return n;
}

void justify(Str& ln, int width)
{
TRACER("justify(Str ln, int width)")
    LTRACE(2, ln)
    int p, o= tog?0:ln.length();
    while(ln.length() < width){
        if(tog){ // left to right pad
            p= ln.index(" ", o); // find a space
            if(p > 0){
                ln.substr(p, 0)= " "; // insert a space
                o= nextnonspace(ln, p); // start from next non-space
            }else if(o) o= 0; // reset
            else ln += " "; // put at end of line
        }else{ // right to left pad
            p= ln.rindex(" ", o); // find a space
            if(p > 0){
                ln.substr(p, 0)= " "; // insert a space
                o= prevnonspace(ln, p); // start from previous non-space
            }else if(o != ln.length()) o= ln.length(); // reset
            else ln += " "; // put at end of line
        }       
    }
    tog ^= 1;
}

int main(int argc,  char **argv)
{
FTRACER("main()", 0, cout)
Str inl, curln, curword;
StrList words;
int width= 79;

    if(argc < 2){
        cerr << "Usage: justify width" << endl;
        exit(1);
    }

    width= atoi(argv[1]);
    
    while(cin >> inl){
        LTRACE(2, inl)
        if(inl.length() == 0){ // honour existing blank lines
            if(curln.length()) cout << curln << endl; // flush previous line
            cout << endl; // output blank line
            curln= "";
            continue;
        }
        words.push(inl.split("' '")); // put at end of word FIFO
        LTRACE(2, words)
        while(words){
            if(curln.length() == 0) curln= words.shift(); // get first word
            while(curln.length() < width){
                if(!words) break; // need to refill FIFO
                curword= words.shift(); // next word
                if(curln.length() + curword.length() + 1 > width){
                    words.unshift(curword); // put it back
                    justify(curln, width); // pads with spaces to width
                }else curln += (" " + curword); // add word to line
            }
            if(curln.length() >= width){ // done with this line
                cout << curln << endl;
                curln= "";
            }
        }
    }
    if(curln.length()) cout << curln << endl;
    return( 0 );
}
