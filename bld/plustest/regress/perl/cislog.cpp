//
// Display monthly connect time to CIS
//

#include        <fstream.h>
#include        <stdlib.h>
#include        "perlclass.h"
#include        "perlassoc.h"

int main()
{
//char buf[80];
ifstream fin("cis.log");
int ln= 1, gotit= 0;
Assoc<int> tot("", 0);
Regexp reet("(..):(..):(..)"), remnth("^(..)/../..");
Regexp r1("^(../../..) (..:..).. (.*)");
PerlString s;

    fin >> s;   // eat first line

    while(fin >> s){
        ln++;
//      cout << "line " << ln << ": <" << s << ">" << endl;

        PerlStringList l;

//05/20/92 10:48PM CIS 2400 988-5366
//05/21/92 09:24PM OFFLINE                                   00:00:06
        if(s.m(r1, l)){
            if(l.scalar() < 4){
                cerr << "Didn't match all expressions" << endl;
                exit(1);
            }
//            cout << "Expressions matched: " << endl << l << endl;
            PerlString a= l[3];
            if(a.m("^CIS")) gotit= 1;
            else if(a.m("^OFFLINE") && gotit){ // extract Elapsed time
                PerlStringList et, mnth;
                int hr, mn, sc, tm;

                if(a.m(reet, et) != 4){
                    cerr << "Failed to extract Elapsed time" << endl;
                    exit(1);
                }
                hr= atoi(et[1]); mn= atoi(et[2]); sc= atoi(et[3]);
                tm= (hr*60) + mn + ((sc >= 30) ? 1 : 0);

                gotit= 0;
                // extract month
                if(l[1].m(remnth, mnth) != 2){
                    cerr << "Failed to extract Month" << endl;
                    exit(1);
                }

//                cout << "Month: " << mnth[1] << " Elapsed Time = " << tm << " mins" << endl;
                tot(mnth[1]) += tm;
                                                
            }else gotit= 0;
                
        }else{
            cerr << "Didn't match any expressions" << endl;
            exit(1);
        }

    };
//    cout << "tot = " << endl << tot << endl;
    Assoc<PerlString> months;
    months("01")= "January"; months("02")= "February"; months("03")= "March";
    months("04")= "April"; months("05")= "May"; months("06")= "June";
    months("07")= "July"; months("08")= "August"; months("09")= "September";
    months("10")= "October"; months("11")= "November"; months("12")= "December";

    for(int i=0;i<tot.scalar();i++)
        cout << months(tot[i].key()) << ": " << tot[i].value() << " mins $"
             << tot[i].value() * (12.50/60.0) << endl;
    exit(0);
}

