#include <string.h>
#include "useful.h"

/* $Log:        USEFUL.C $
Revision 1.1  92/08/20  15:50:28  Anthony_Scian
.

// Revision 1.1  1992/08/20  17:14:07  peter
// Initial revision
//
 */

bool operator==(const SubString& s1, const SubString& s2){
    return (bool) (s1.len == s2.len && memcmp(s1.str, s2.str, s1.len) == 0);
}

void SubString::out(ostream& o) const {
    o.write(str, len);
}

String::String(const SubString& s) : SubString(new char[s.len], s.len) {
    memcpy(str, s.str, s.len);
}

String::String(String& s) : SubString(s.str, s.len) {
    s.str = NULL;
    s.len = 0;
}

String::String() : SubString((char*) NULL, 0) {
    ;
}


String::~String() {
    delete str;
}
