#ifdef  TESTCHGFNT
// Read an AmiPro style sheet and change any unknown font
#include        <fstream.h>
#include        <stdlib.h>

#ifdef  __TURBOC__
#pragma hdrstop
#endif

#include        "splash.h"
#include        "assoc.h"

int main(int argc, char **argv)
{
char c, *infn;
ifstream ini("\\win3\\win.ini");
ofstream fout("t.sty");
int ln= 0;
SPString w;
SPStringList l;
SPStringList ttfonts;
Assoc<SPString> repfnts("", "");  // saves font replacement names

    if(argc < 2) infn= "test.sty";
    else infn= argv[1];

    ifstream fin(infn);

    if(!ini){
        cerr << "Can't open \\win3\\win.ini" << endl;
        exit(1);
    }

    if(!fin){
        cerr << "Can't open " << infn << endl;
        exit(1);
    }

    if(!fout){
        cerr << "Can't open t.sty for write" << endl;
        exit(1);
    }

    cout << "Reading in truetype fonts" << endl;

    while(ini >> w){ // find the [fonts] section
        if(w.m("\\[fonts\\]")) break;
    }

//    cout << buf << endl;

    if(!ini.good()){ // checks all file state
        cerr << "Couldn't find [fonts] section in win.ini" << endl;
        exit(1);
    }

    // make a list of truetype fonts
    Regexp r1("^([a-zA-Z ]+) \\(([a-zA-Z ]+)\\)=");
    Regexp r2("^TrueType$");
    Regexp r3("\\[.*\\]");

    while(ini >> w){
        if(w.m(r3)) break; // found the start of another section
        if(w.m(r1, l) != 3) continue; // ignore this line
//        cout << "Font match:" << l[1] << ", " << l[2] << endl;
        if(l[2].m(r2)){
            ttfonts.push(l[1]);
        } 
    }

    cout << "ttfonts: " << endl << ttfonts << endl;
    ini.close();

    cout << "Looking for non-truetype fonts" << endl;

    SPString s, fnt, newfnt;
    SPStringList sl;
    while(fin >> s){
        ln++;
//      cout << "line " << ln << ": <" << s << ">" << endl;
        if(s.m("\\[fnt\\]")){
            fout << s << endl; // write out [fnt] line
                               // read next line which should have font in it
            if(!(fin >> s)){
                cerr << "Error reading font line " << ln << endl;
                exit(1);
            }
            ln++;
            fnt= s.split("' '").join(" "); // This trims whitespace
//          cout << "font name: <" << fnt << ">" << endl;
            if(!ttfonts.grep("^" + fnt + "$", "i")){ // not a truetype font
                int pos= s.index(fnt); // get position in string of font
                if(pos < 0){
                    cerr << "Couldn't find <" << fnt << "> in string <" << s << "> line " << ln << endl;
                    exit(1);
                }

                // See if we already know what to exchange it with
                if(repfnts.isin(fnt)) // just replace it
                    s.substr(pos, strlen(fnt)) = repfnts(fnt);
                else{ // need to ask what the new font name will be
                    do{
                        cout << "Replace font <" << fnt << "> with:"; cout.flush();
                        cin >> newfnt;
                        if(!(sl=ttfonts.grep("^" + newfnt + "$", "i"))){
                            cerr << "<" << newfnt << "> is not a valid font" << endl;
                            continue;
                        }
                    break;
                    }while(1);
                    s.substr(pos, strlen(fnt)) = sl[0]; // replace it
                    repfnts(fnt) = sl[0];  // remember for next time
                }
                fout << s << endl;
            }else{
//              cout << fnt << " is a truetype font" << endl;
                fout << s << endl; // write out original font line
            }

        }else{
            fout << s << endl; // echo line out
        }
    }while(!fin.eof());

    cout << "replacement fonts were:" << endl << repfnts << endl;

    if(fout.fail())
        cerr << "Something bad happened to the output file" << endl;
    fout.close();

    return 0;
}

#endif
