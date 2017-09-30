#include "fail.h"
// Listing 8 - iomanip.h Macro Version

//#define IOMANIPdeclare(T) SMANIP_define(T)
//#define OMANIP(T) OMANIP##T

#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <string.h>

typedef ostream OS; // an abbrev
typedef char* CP;    // single token

IOMANIPdeclare(CP);

// qSTR - manip function
OS& qSTR(OS& os,CP s)
{ return os << "'" << s << "'"; }

// qStr - manip inteface
// for manip function qSTR
OMANIP(CP) qStr(CP s)
{ return OMANIP(CP)(qSTR,s); }

int main()
{
    // sample output expression
    strstream out;
    out << "Output is a " << qStr("string") << ends;
    char *p = out.str();
    if( strcmp( p, "Output is a 'string'" ) != 0 ) fail(__LINE__);
    _PASS;
}
