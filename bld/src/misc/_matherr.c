#include <stdio.h>
#include <math.h>

extern  void    __set_EDOM();
extern  void    __set_ERANGE();
extern  int     __matherr( struct exception * );
#pragma aux     __matherr "*";

static char *Msgs[] = {
    0,
    "Domain error",
    "Argument singularity",
    "Overflow range error",
    "Underflow range error",
    "Total loss of significance",
    "Partial loss of significance"
};

int     (*__MathErrRtn)( struct exception * ) = __matherr;      /* 24-mar-91 */

double _matherr( struct _exception *excp )
/***************************************/
{
    if( (*__MathErrRtn)( excp ) == 0 ) {
/*      fprintf( stderr, "%s in %s\n", Msgs[excp->type], excp->name ); */
        fputs( Msgs[excp->type], stderr );
        fputs( " in ", stderr );
        fputs( excp->name, stderr );
        fputc( '\n', stderr );
        excp->type == DOMAIN ? __set_EDOM() : __set_ERANGE();
    }
    return( excp->retval );
}
