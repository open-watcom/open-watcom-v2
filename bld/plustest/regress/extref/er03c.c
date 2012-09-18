#include "er03.h"

struct my_class {
    int v;
    my_class()
    {
	// should never execute this!
	puts( "fail" );
	throw D();
    }
};

my_class v;
