/* popen and pclose are not part of win 95 and nt,
   but it appears that _popen and _pclose "work".
   if this won't load, use the return NULL statements. */

#include <stdio.h>
#include "awk.h"
#include "proto.h"

FILE *popen( const char *s, const char *m ) {
#ifdef __DOS__
    s=s;m=m;
    return( NULL );
#else
    return( _popen( s, m ) );   /* return NULL; */
#endif
}

int pclose( FILE *f ) {
#ifdef __DOS__
    f=f;
    return( 0 );
#else
    return( _pclose( f ) );     /* return 0; */
#endif
}
