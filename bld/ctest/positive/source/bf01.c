// parsing problem
#include "fail.h"

typedef int TYPENAME;

struct s1 {
    TYPENAME          : 4;
    TYPENAME TYPENAME : 4;
};

struct s2 {
    int : 4;
    int TYPENAME : 4;
};

struct s3 {
    int : 4;
    int bf : 4;
    int : 0;
};

struct s4 {
    TYPENAME : 4;
    TYPENAME bf : 4;
    TYPENAME : 0;
};

int main() {
    union {
        struct s1 f1;
        struct s2 f2;
        struct s3 f3;
        struct s4 f4;
    } u;

    memset( &u, 0, sizeof(u) );
    if( u.f1.TYPENAME != 0 ) _fail;
    if( u.f2.TYPENAME != 0 ) _fail;
    if( u.f3.bf != 0 ) _fail;
    if( u.f4.bf != 0 ) _fail;
    u.f1.TYPENAME = 1;
    if( u.f1.TYPENAME != 1 ) _fail;
    if( u.f2.TYPENAME != 1 ) _fail;
    if( u.f3.bf != 1 ) _fail;
    if( u.f4.bf != 1 ) _fail;
    u.f2.TYPENAME++;
    if( u.f1.TYPENAME != 2 ) _fail;
    if( u.f2.TYPENAME != 2 ) _fail;
    if( u.f3.bf != 2 ) _fail;
    if( u.f4.bf != 2 ) _fail;
    _PASS;
}
