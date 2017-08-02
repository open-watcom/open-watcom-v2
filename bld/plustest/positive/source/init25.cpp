#include "fail.h"

struct s {
    union {
	struct {
	    float a, b;
	};
	float c[2];
    };
};

s d = { 0.25, 0.125 };

int main() {
    if( d.a != 0.25 ) _fail;
    if( d.b != 0.125 ) _fail;
    if( d.c[0] != 0.25 ) _fail;
    if( d.c[1] != 0.125 ) _fail;
    _PASS;
}
