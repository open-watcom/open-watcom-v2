#include "fail.h"

struct s {
    union {
	struct {
	    int a:1, b:1;
	};
	int c:1;
    };
};

s d = { 0, 1 };

int main() {
    if( d.a != 0 ) _fail;
    if( d.b != 1 ) _fail;
    if( d.c != 0 ) _fail;
    d.a = 1;
    if( d.c != 1 ) _fail;
    _PASS;
}
