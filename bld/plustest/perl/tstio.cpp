#include        <fstream.h>
#include        <strstrea.h>
#include        <stdlib.h>
#include        "perlclas.h"
#include        "perlasso.h"

int main()
{
ifstream fin("t.in");
ofstream fout("t2.out");

PerlString s;
PerlStringList l;

    while(fin >> s){
        cout << "read in string: " << s << endl;
        fout << s << endl;
    }
    
#if __WATCOMC__ < 1000
    fin.clear();
#endif
    fin.close();
    fout.close();
    
    fin.open("t.in");
    
    fin >> l;
    
    cout << "List =" << endl << l << endl;

#if 1
    strstream ss, iss;
    ss << "one\n" << "two\nthree\nfour\n";
    ss >> l;
    cout << l << endl;
    PerlList<int> il;
    iss << 1 << " " << 2 << " " << 3 << " " << 4 << endl;
    iss << "5 6 7 8 9" << endl;
    ((istream&)(iss)) >> il;
    cout << il << endl;
#endif


    cout << "Start Typing:" << endl;
    while(cin >> s){
        cout << "read in string: " << s << endl;
    }

    
    
    exit(0);
}
