#include <stddef.h>

#pragma pack(4);	// now 4

#pragma pack(push,2);	// now 2
#pragma pack(push,1)	// now 1
#pragma pack(push,4);	// now 4

struct S4 {
    char c;
    unsigned long o4;
};

#pragma pack(pop);	// now 1

struct S1 {
    char c;
    unsigned long o1;
};

#pragma pack(pop)	// now 2

struct S2 {
    char c;
    unsigned long o2;
};

#pragma pack(pop);	// now 4

struct X4 {
    char c;
    unsigned long o4;
};

#pragma pack(pop);	// stays 4

struct Y4 {
    char c;
    unsigned long o4;
};

#pragma pack(push,2);	// now 2

struct Z2 {
    char c;
    unsigned long o2;
};

char x49o4[ offsetof(S4,o4) ];
char x50o2[ offsetof(S2,o2) ];
char x51o1[ offsetof(S1,o1) ];
char x52o4[ offsetof(X4,o4) ];
char x53o4[ offsetof(Y4,o4) ];
char x54o2[ offsetof(Z2,o2) ];

char x49o4[ 3 ];
char x50o2[ 3 ];
char x51o1[ 3 ];
char x52o4[ 3 ];
char x53o4[ 3 ];
char x54o2[ 3 ];
