#include "er01.h"

// this module should never be included in the .EXE

extern unsigned not_defined( unsigned );

X *dont_import_me( unsigned n ) {
    n = not_defined( n );
    return new X[n];
}
