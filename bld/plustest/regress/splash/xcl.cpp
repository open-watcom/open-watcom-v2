#include <iostream.h>
#include <fstream.h>
#include "splash.h"
#include "tracer.h"

#ifdef  _Windows
#include "../win/debugwin/debugstr.h"
#define OUTSTREAM dout
#else
#define OUTSTREAM cout
#endif

ifstream fin;

// Globals
SPString curln, curclass;
int bcnt;   // current brace count

/*
 * Reads in a line, and eliminates anything in between comments
 * or quotes. Also keeps count of the brace level
 */
int getline()
{
LTRACER("getline", 4)

    if(!(fin >> curln)) return 0;
    cout << curln << endl;
    LTRACE(2, curln)
    while(curln.m("\".*\"")){ // remove anything in quoted strings
        int o, c;
        SPString tl= curln.substr(0, o= curln.index("\""));
        do{ // make sure the quote is not backslashed
            c= curln.index("\"", o+1);
            o= c;
        }while(c > 1 && curln[c-1] == '\\');
        tl += curln.substr(c+1); // rest of line
        curln= tl;
//      cout << "quote stripped curln= " << curln <<endl;
    }

    if(curln.m("//")){ // found a comment in line
        curln.substr(curln.index("//"))= ""; // chop it off
    }

    if(curln.m("/\\*")){ // in c comment
        SPString tl= curln.substr(0, curln.index("/*"));
        if(curln.m("\\*/")){ // closing comment on same line
            tl += curln.substr(curln.index("*/")+2); // rest of line
            curln= tl;
//          cout << "comment stripped curln= " << curln <<endl;
        }else{ // find close of comment
            while(fin >> curln){
                cout << curln << endl;
                if(curln.m("\\*/")){
                    tl += curln.substr(curln.index("*/")+2); // get rest of line
                    break;              
                }
            }
            curln= tl;
        }
    }
    LTRACE(4, curln)
    int ob= curln.tr("{", "");
    int cb= curln.tr("}", "");
    int cnt= ob - cb;
    bcnt += cnt;
    LTRACE(4, "Brace count = " << bcnt);
    if(bcnt < 0){
        cout.flush();
        cerr << "Brace count dropped below zero,  oops" << endl;
        exit(1);
    }
    return 1;
}

void dofunction(const SPStringList &subm, int bc= 0)
{
TRACER("dofunction")
SPString fnname= subm[1], fnparams= subm[2];

LTRACE(1, "In Function " << fnname << "(" << fnparams << ")")

    if(curln.m("{.*}")){
//      cerr << "Can't insert TRACER here" << endl;
        return;
    }else if(curln.m("{")) bc--;

    while(bcnt == bc && getline()); // find opening brace
    LTRACE(1, "Inside Function " << fnname)

    if(fnname.m("^main$")){ // special treatment for main()
        cout << "FTRACER(\"main()\", 0)" << endl;
    }else{
        cout << "TRACER(\"";
        if(curclass.length()) cout << curclass << "::";
        cout << fnname << "(" << fnparams << ")\")" << endl;
    }

    while(getline()){
        if(bcnt == bc){ // no longer in function
            LTRACE(1, "End of function " << fnname)
            break;
        }
        
    }

}

void doclass(const SPStringList &subm)
{
TRACER("doclass")
SPStringList sl;

    if(curln.m(";")) return; // if a ';' on line then probably a forward reference
    LTRACE(1, "Seen Class " << subm[1])
    curclass= subm[1];  // remember class name
    
    while(bcnt == 0 && getline()); // find opening brace
    LTRACE(1, "Inside Class " << curclass)
    
    while(getline()){
        if(bcnt == 0){ // no longer in class
            LTRACE(1, "End of class " << curclass)
            break;
        }

        if(curln.m("[ \t]*([^ \t]+)[ \t]*\\(([^)]*)\\)", sl)){ // function def
            if(curln.m(";[ \t]*$")) continue; // declaration
            dofunction(sl, bcnt);
        }
    
    }
    curclass= "";
}

int main(int argc, char **argv)
{
int debug= 0;

    if(argc > 2 && *argv[1] == '-'){
        debug= atoi(&argv[1][1]);
        argc--; argv++;
    }


FTRACER("xcl", debug, OUTSTREAM)
SPStringList sl;

    if(argc < 2){
        cerr << "Usage: xcl [-n] fn" << endl;
        exit(1);
    }

    fin.open(argv[1], ios::in);
    if(!fin){
        cerr << "Couldn't open file: " << argv[1] << endl;
        exit(1);
    }
    
    while(getline()){
        if(curln.m("^[ \t]*class[ \t]+([a-zA-Z_]+[a-zA-Z_0-9]*)", sl)){
            doclass(sl);
            continue;
        }

        if(curln.m("[ \t]*([^ \t]+)[ \t]*\\(([^)]*)\\)", sl)){ // function def
            if(curln.m(";[ \t]*$")) continue; // declaration
            dofunction(sl, bcnt);
        }
    }
    return 0;
}

