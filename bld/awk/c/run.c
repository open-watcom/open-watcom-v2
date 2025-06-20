/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

#define DEBUG
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "awk.h"
#include "ytab.h"


#define NARGS   50  /* max args in a call */

#define tempfree(x) if (istemp(x)) tfree(x); else

struct files {
    FILE        *fp;
    const char  *fname;
    int         mode;   /* '|', 'a', 'w' => LE/LT, GT */
} *files;

struct Frame {          /* stack frame for awk function calls */
    int     nargs;      /* number of arguments in this call */
    Cell    *fcncell;   /* pointer to Cell for function */
    Cell    **args;     /* pointer to array of arguments after execute */
    Cell    *retval;    /* return value */
};

static int  nfiles;

static size_t       nframe = 0;         /* number of frames allocated */
static struct Frame *frame = NULL;     /* base of stack frames; dynamically allocated */
static struct Frame *framep = NULL;    /* frame pointer. bottom level unused */

/*
#undef tempfree

void tempfree(Cell *p) {
    if( p->ctype == OCELL && (p->csub < CUNK || p->csub > CFREE) ) {
        WARNING( "bad csub %d in Cell %d %s",
            p->csub, p->ctype, p->sval );
    }
    if( istemp( p ) ) {
        tfree( p );
    }
}
*/

/* do we really need these? */
/* #ifdef _NFILE */
/* #ifndef FOPEN_MAX */
/* #define FOPEN_MAX _NFILE */
/* #endif */
/* #endif */
/*  */
/* #ifndef  FOPEN_MAX */
/* #define  FOPEN_MAX   40 */   /* max number of open files */
/* #endif */
/*  */
/* #ifndef RAND_MAX */
/* #define RAND_MAX 32767 */    /* all that ansi guarantees */
/* #endif */

jmp_buf env;

extern  Awkfloat    srand_seed;

Node    *winner = NULL; /* root of parse tree */
Cell    *tmps;      /* free temporary cells for execution */

static Cell truecell     = { OBOOL, BTRUE, 0, 0, 1.0, NUM };
Cell    *True   = &truecell;
static Cell falsecell    = { OBOOL, BFALSE, 0, 0, 0.0, NUM };
Cell    *False  = &falsecell;
static Cell breakcell    = { OJUMP, JBREAK, 0, 0, 0.0, NUM };
Cell    *jbreak = &breakcell;
static Cell contcell     = { OJUMP, JCONT, 0, 0, 0.0, NUM };
Cell    *jcont  = &contcell;
static Cell nextcell     = { OJUMP, JNEXT, 0, 0, 0.0, NUM };
Cell    *jnext  = &nextcell;
static Cell nextfilecell = { OJUMP, JNEXTFILE, 0, 0, 0.0, NUM };
Cell    *jnextfile  = &nextfilecell;
static Cell exitcell     = { OJUMP, JEXIT, 0, 0, 0.0, NUM };
Cell    *jexit  = &exitcell;
static Cell retcell      = { OJUMP, JRET, 0, 0, 0.0, NUM };
Cell    *jret   = &retcell;
static Cell tempcell     = { OCELL, CTEMP, 0, "", 0.0, NUM|STR|DONTFREE };

Node    *curnode = NULL;    /* the node being executed, for debugging */

/* buffer memory management */
bool adjbuf( char **pbuf, size_t *psiz, size_t minlen, size_t quantum, char **pbptr, const char *whatrtn )
/* pbuf:    address of pointer to buffer being managed
 * psiz:    address of buffer size variable
 * minlen:  minimum length of buffer needed
 * quantum: buffer size quantum
 * pbptr:   address of movable pointer into buffer, or 0 if none
 * whatrtn: name of the calling routine if failure should cause fatal error
 *
 * return   false for realloc failure, true for success
 */
{
    if( minlen > *psiz ) {
        char *tbuf;
        size_t rminlen = ( quantum ) ? minlen % quantum : 0;
        size_t boff = ( pbptr != NULL ) ? *pbptr - *pbuf : 0;
        /* round up to next multiple of quantum */
        if( rminlen )
            minlen += quantum - rminlen;
        tbuf = (char *)realloc( *pbuf, minlen );
        dprintf(( "adjbuf %s: %d %d (pbuf=%p, tbuf=%p)\n", whatrtn, (int)*psiz, (int)minlen, *pbuf, tbuf ));
        if( tbuf == NULL ) {
            if( whatrtn != NULL )
                FATAL( "out of memory in %s", whatrtn );
                /* never return */
            return( false );
        }
        *pbuf = tbuf;
        *psiz = minlen;
        if( pbptr != NULL ) {
            *pbptr = tbuf + boff;
        }
    }
    return( true );
}

static void stdinit( void )
/* in case stdin, etc., are not constants */
{
    nfiles = FOPEN_MAX;
    files = calloc( nfiles, sizeof( *files ) );
    if( files == NULL )
        FATAL( "can't allocate file memory for %u files", nfiles );
        /* never return */
    files[0].fp = stdin;
    files[0].fname = "/dev/stdin";
    files[0].mode = LT;
    files[1].fp = stdout;
    files[1].fname = "/dev/stdout";
    files[1].mode = GT;
    files[2].fp = stderr;
    files[2].fname = "/dev/stderr";
    files[2].mode = GT;
}

static void flush_all( void )
{
    int i;

    for( i = 0; i < nfiles; i++ ) {
        if( files[i].fp ) {
            fflush( files[i].fp );
        }
    }
}

void run( Node *a )
/* execution of parse tree starts here */
{
    stdinit();
    execute(a);
    closeall();
}

Cell *execute( Node *u )
/* execute a node of the parse tree */
{
    Cell *(*proc)(Node **, int);
    Cell *x;
    Node *a;

    if( u == NULL )
        return( True );
    for( a = u; ; a = a->nnext ) {
        curnode = a;
        if( isvalue( a ) ) {
            x = (Cell *)(a->narg[0]);
            if( isfld( x ) && !donefld ) {
                fldbld();
            } else if( isrec( x ) && !donerec ) {
                recbld();
            }
            return( x );
        }
        if( notlegal( a->nobj ) )  /* probably a Cell* but too risky to print */
            FATAL( "illegal statement" );
            /* never return */
        proc = proctab[a->nobj-FIRSTTOKEN];
        x = (*proc)( a->narg, a->nobj );
        if( isfld( x ) && !donefld ) {
            fldbld();
        } else if( isrec( x ) && !donerec ) {
            recbld();
        }
        if( isexpr( a ) )
            return( x );
        if( isjump( x ) )
            return( x );
        if( a->nnext == NULL )
            return( x );
        tempfree( x );
    }
}


Cell *program( Node **a, int n )
/* execute an awk program */
/* a[0] = BEGIN, a[1] = body, a[2] = END */
{
    Cell *x;

    /* unused parameters */ (void)n;

    if( setjmp( env ) != 0 )
        goto ex;
    if( a[0] ) {     /* BEGIN */
        x = execute( a[0] );
        if( isexit( x ) )
            return( True );
        if( isjump( x ) )
            FATAL( "illegal break, continue, next or nextfile from BEGIN" );
            /* never return */
        tempfree( x );
    }
    if( a[1] || a[2] ) {
        while( getrec( &record, &recsize, true ) > 0 ) {
            x = execute( a[1] );
            if( isexit( x ) )
                break;
            tempfree( x );
        }
    }
  ex:
    if( setjmp( env ) != 0 )   /* handles exit within END */
        goto ex1;
    if( a[2] ) {     /* END */
        x = execute( a[2] );
        if( isbreak( x ) || isnext( x ) || iscont( x ) )
            FATAL( "illegal break, continue, next or nextfile from END" );
            /* never return */
        tempfree( x );
    }
  ex1:
    return( True );
}

Cell *call( Node **a, int n )
/* function call.  very kludgy and fragile */
{
    static Cell newcopycell = { OCELL, CCOPY, 0, "", 0.0, NUM|STR|DONTFREE };
    int i, ncall, ndef;
    bool freed = false; /* handles potential double freeing when fcn & param share a tempcell */
    Node *x;
    Cell *args[NARGS], *oargs[NARGS];   /* BUG: fixed size arrays */
    Cell *y, *z, *fcn;
    char *s;

    /* unused parameters */ (void)n;

    fcn = execute( a[0] );    /* the function itself */
    s = fcn->nval;
    if( !isfcn( fcn ) )
        FATAL( "calling undefined function %s", s );
        /* never return */
    if( frame == NULL ) {
        nframe += 100;
        framep = frame = (struct Frame *)calloc( nframe, sizeof( struct Frame ) );
        if( frame == NULL ) {
            FATAL( "out of space for stack frames calling %s", s );
            /* never return */
        }
    }
    for( ncall = 0, x = a[1]; x != NULL; x = x->nnext )  /* args in call */
        ncall++;
    ndef = (int)(fcn->fval);         /* args in defn */
    dprintf(( "calling %s, %d args (%d in defn), fp=%d\n", s, ncall, ndef, (int)( framep - frame ) ));
    if( ncall > ndef )
        WARNING( "function %s called with %d args, uses only %d", s, ncall, ndef );
    if( ncall + ndef > NARGS )
        FATAL( "function %s has %d arguments, limit %d", s, ncall + ndef, NARGS );
        /* never return */
    for( i = 0, x = a[1]; x != NULL; i++, x = x->nnext ) {   /* get call args */
        dprintf(( "evaluate args[%d], fp=%d:\n", i, (int)( framep - frame ) ));
        y = execute(x);
        oargs[i] = y;
        dprintf(( "args[%d]: %s %f <%s>, t=%o\n", i, NN( y->nval ), y->fval, (( isarr( y ) ) ? "(array)" : NN( y->sval )), y->tval ));
        if( isfcn( y ) )
            FATAL( "can't use function %s as argument in %s", y->nval, s );
            /* never return */
        if( isarr( y ) ) {
            args[i] = y;    /* arrays by ref */
        } else {
            args[i] = copycell( y );
        }
        tempfree( y );
    }
    for( ; i < ndef; i++ ) {    /* add null args for ones not provided */
        args[i] = gettemp();
        *args[i] = newcopycell;
    }
    framep++;   /* now ok to up frame */
    if( framep >= frame + nframe ) {
        size_t dfp = framep - frame;   /* old index */
        nframe += 100;
        frame = (struct Frame *)realloc( (char *)frame, nframe * sizeof( struct Frame ) );
        if( frame == NULL )
            FATAL( "out of space for stack frames in %s", s );
            /* never return */
        framep = frame + dfp;
    }
    framep->fcncell = fcn;
    framep->args = args;
    framep->nargs = ndef;   /* number defined with (excess are locals) */
    framep->retval = gettemp();

    dprintf(( "start exec of %s, fp=%d\n", s, (int)( framep - frame ) ));
    y = execute( (Node *)(fcn->sval) );   /* execute body */
    dprintf(( "finished exec of %s, fp=%d\n", s, (int)( framep - frame ) ));

    for( i = 0; i < ndef; i++ ) {
        Cell *t = framep->args[i];
        if( isarr( t ) ) {
            if( t->csub == CCOPY ) {
                if( i >= ncall ) {
                    freesymtab( t );
                    t->csub = CTEMP;
                    tempfree( t );
                } else {
                    oargs[i]->tval = t->tval;
                    oargs[i]->tval &= ~(STR | NUM | DONTFREE);
                    oargs[i]->sval = t->sval;
                    tempfree( t );
                }
            }
        } else if( t != y ) {    /* kludge to prevent freeing twice */
            t->csub = CTEMP;
            tempfree( t );
        } else if( t == y && t->csub == CCOPY ) {
            t->csub = CTEMP;
            tempfree( t );
            freed = true;
        }
    }
    tempfree( fcn );
    if( isexit( y ) || isnext( y ) )
        return( y );
    if( !freed ) {
        tempfree( y );    /* don't free twice! */
    }
    z = framep->retval;         /* return value */
    dprintf(( "%s returns %g |%s| %o\n", s, getfval( z ), getsval( z ), z->tval ));
    framep--;
    return( z );
}

Cell *copycell( Cell *x )
/* make a copy of a cell in a temp */
{
    Cell *y;

    y = gettemp();
    y->csub = CCOPY;    /* prevents freeing until call is over */
    y->nval = x->nval;  /* BUG? */
    if( isstr( x ) )
        y->sval = tostring( x->sval );
    y->fval = x->fval;
    y->tval = x->tval & ~(CON | FLD | REC | DONTFREE);    /* copy is not constant or field */
                            /* is DONTFREE right? */
    return( y );
}

Cell *arg( Node **a, int n )
/* nth argument of a function */
{
    n = ptoi( a[0] ); /* argument number, counting from 0 */
    dprintf(( "arg(%d), fp->nargs=%d\n", n, framep->nargs ));
    if( n + 1 > framep->nargs )
        FATAL( "argument #%d of function %s was not supplied", n + 1, framep->fcncell->nval );
        /* never return */
    return( framep->args[n] );
}

Cell *jump(Node **a, int n)
/* break, continue, next, nextfile, return */
{
    Cell *y;

    switch( n ) {
    case EXIT:
        if( a[0] != NULL ) {
            y = execute(a[0]);
            errorflag = (int)getfval( y );
            tempfree( y );
        }
        longjmp( env, 1 );
        /* never return */
    case RETURN:
        if( a[0] != NULL ) {
            y = execute( a[0] );
            if( (y->tval & (STR | NUM)) == (STR | NUM) ) {
                setsval( framep->retval, getsval( y ) );
                framep->retval->fval = getfval( y );
                framep->retval->tval |= NUM;
            } else if( isstr( y ) ) {
                setsval( framep->retval, getsval( y ) );
            } else if( isnum( y ) ) {
                setfval( framep->retval, getfval( y ) );
            } else {      /* can't happen */
                FATAL( "bad type variable %d", y->tval );
                /* never return */
            }
            tempfree( y );
        }
        y = jret;
        break;
    case NEXT:
        y = jnext;
        break;
    case NEXTFILE:
        nextfile();
        y = jnextfile;
        break;
    case BREAK:
        y = jbreak;
        break;
    case CONTINUE:
        y = jcont;
        break;
    default:    /* can't happen */
        FATAL( "illegal jump type %d", n );
        /* never return */
    }
    return( y );
}

Cell *awkgetline( Node **a, int n )
/* get next line from specific input */
/* a[0] is variable, a[1] is operator, a[2] is filename */
{
    Cell *r, *x;
    FILE *fp;
    char *buf;
    size_t bufsize = recsize;
    int mode;

    if( (buf = (char *)malloc( bufsize )) == NULL )
        FATAL( "out of memory in getline" );
        /* never return */

    fflush( stdout ); /* in case someone is waiting for a prompt */
    r = gettemp();
    if( a[1] != NULL ) {     /* getline < file */
        x = execute( a[2] );      /* filename */
        mode = ptoi( a[1] );
        if( mode == '|' )        /* input pipe */
            mode = LE;  /* arbitrary flag */
        fp = openfile( mode, getsval( x ) );
        tempfree( x );
        if( fp == NULL ) {
            n = -1;
        } else {
            n = readrec( &buf, &bufsize, fp );
        }
        if( n <= 0 ) {
            ;
        } else if( a[0] != NULL ) {  /* getline var <file */
            x = execute( a[0] );
            setsval( x, buf );
            tempfree( x );
        } else {            /* getline <file */
            setsval( fldtab[0], buf );
            if( is_number( fldtab[0]->sval ) ) {
                fldtab[0]->fval = atof( fldtab[0]->sval );
                fldtab[0]->tval |= NUM;
            }
        }
    } else {            /* bare getline; use current input */
        if( a[0] == NULL ) { /* getline */
            n = getrec( &record, &recsize, true );
        } else {        /* getline var */
            n = getrec( &buf, &bufsize, false );
            x = execute( a[0] );
            setsval( x, buf );
            tempfree( x );
        }
    }
    setfval( r, (Awkfloat)n );
    free( buf );
    return( r );
}

Cell *getnf( Node **a, int n )
/* get NF */
{
    /* unused parameters */ (void)n;

    if( !donefld )
        fldbld();
    return( (Cell *)a[0] );
}

Cell *array( Node **a, int n )
/* a[0] is symtab, a[1] is list of subscripts */
{
    Cell *x, *y, *z;
    char *s;
    Node *np;
    char *buf;
    size_t bufsz = recsize;
    size_t nsub = strlen( *SUBSEP );

    /* unused parameters */ (void)n;

    if( (buf = (char *)malloc( bufsz )) == NULL )
        FATAL( "out of memory in array" );
        /* never return */

    x = execute( a[0] );  /* Cell* for symbol table */
    buf[0] = '\0';
    for( np = a[1]; np != NULL; np = np->nnext ) {
        y = execute( np );    /* subscript */
        s = getsval( y );
        if( !adjbuf( &buf, &bufsz, strlen( buf ) + strlen( s ) + nsub + 1, recsize, 0, "array" ) )
            FATAL( "out of memory for %s[%s...]", x->nval, buf );
            /* never return */
        strcat( buf, s );
        if( np->nnext )
            strcat( buf, *SUBSEP );
        tempfree( y );
    }
    if( !isarr( x ) ) {
        dprintf(( "making %s into an array\n", NN( x->nval ) ));
        if( freeable( x ) )
            xfree( x->sval );
        x->tval &= ~(STR | NUM | DONTFREE);
        x->tval |= ARR;
        x->sval = (char *)makesymtab( NSYMTAB );
    }
    z = setsymtab( buf, "", 0.0, STR | NUM, (Array *)x->sval );
    z->ctype = OCELL;
    z->csub = CVAR;
    tempfree( x );
    free( buf );
    return( z );
}

Cell *awkdelete( Node **a, int n )
/* a[0] is symtab, a[1] is list of subscripts */
{
    Cell *x, *y;
    Node *np;
    char *s;
    size_t nsub = strlen( *SUBSEP );

    /* unused parameters */ (void)n;

    x = execute( a[0] );  /* Cell* for symbol table */
    if( !isarr( x ) )
        return( True );
    if( a[1] == 0 ) {    /* delete the elements, not the table */
        freesymtab( x );
        x->tval &= ~STR;
        x->tval |= ARR;
        x->sval = (char *)makesymtab( NSYMTAB );
    } else {
        size_t bufsz = recsize;
        char *buf;
        if( (buf = (char *)malloc( bufsz )) == NULL )
            FATAL( "out of memory in adelete" );
            /* never return */
        buf[0] = '\0';
        for( np = a[1]; np != NULL; np = np->nnext ) {
            y = execute( np );    /* subscript */
            s = getsval( y );
            if( !adjbuf( &buf, &bufsz, strlen( buf ) + strlen( s ) + nsub + 1, recsize, 0, "awkdelete" ) )
                FATAL( "out of memory deleting %s[%s...]", x->nval, buf );
                /* never return */
            strcat( buf, s );
            if( np->nnext )
                strcat( buf, *SUBSEP );
            tempfree( y );
        }
        freeelem( x, buf );
        free( buf );
    }
    tempfree( x );
    return( True );
}

Cell *intest( Node **a, int n )
/* a[0] is index (list), a[1] is symtab */
{
    Cell *x, *ap, *k;
    Node *p;
    char *buf;
    char *s;
    size_t bufsz = recsize;
    size_t nsub = strlen( *SUBSEP );

    /* unused parameters */ (void)n;

    ap = execute( a[1] );       /* array name */
    if( !isarr( ap ) ) {
        dprintf(( "making %s into an array\n", ap->nval ));
        if( freeable( ap ) )
            xfree( ap->sval );
        ap->tval &= ~(STR | NUM | DONTFREE);
        ap->tval |= ARR;
        ap->sval = (char *)makesymtab( NSYMTAB );
    }
    if( (buf = (char *)malloc( bufsz )) == NULL ) {
        FATAL( "out of memory in intest" );
        /* never return */
    }
    buf[0] = '\0';
    for( p = a[0]; p != NULL; p = p->nnext ) {
        x = execute( p );       /* expr */
        s = getsval( x );
        if( !adjbuf( &buf, &bufsz, strlen( buf ) + strlen( s ) + nsub + 1, recsize, 0, "intest" ) )
            FATAL( "out of memory deleting %s[%s...]", x->nval, buf );
            /* never return */
        strcat( buf, s );
        tempfree( x );
        if( p->nnext ) {
            strcat( buf, *SUBSEP );
        }
    }
    k = lookup( buf, (Array *)ap->sval );
    tempfree( ap );
    free( buf );
    if( k == NULL )
        return( False );
    return( True );
}


Cell *matchop( Node **a, int n )
/* ~ and match() */
{
    Cell *x, *y;
    char *s, *t;
    bool found;
    fa *pfa;
    bool (*mf)(fa *, const char *);
    bool anchor;

    if( n == MATCHFCN ) {
        mf = pmatch;
        anchor = true;
    } else {
        mf = match;
        anchor = false;
    }
    x = execute( a[1] );        /* a[1] = target text */
    s = getsval( x );
    if( a[0] == 0 ) {           /* a[1] == 0: already-compiled reg expr */
        found = (*mf)( (fa *)a[2], s );
    } else {
        y = execute( a[2] );    /* a[2] = regular expr */
        t = getsval( y );
        pfa = makedfa( t, anchor );
        found = (*mf)( pfa, s );
        tempfree( y );
    }
    tempfree( x );
    if( n == MATCHFCN ) {
        size_t start = patbeg - s + 1;
        if( patlen == NOPAT ) {
            start = 0;
            setfval( rlengthloc, (Awkfloat)-1 );
        } else {
            setfval( rlengthloc, (Awkfloat)patlen );
        }
        setfval( rstartloc, (Awkfloat)start );
        x = gettemp();
        x->tval = NUM;
        x->fval = (Awkfloat)start;
        return( x );
    } else if( ( n == MATCH && found ) || ( n == NOTMATCH && !found ) ) {
        return( True );
    } else {
        return( False );
    }
}


Cell *boolop( Node **a, int n )
/* a[0] || a[1], a[0] && a[1], !a[0] */
{
    Cell *x, *y;
    bool i;

    x = execute( a[0] );
    i = istrue( x );
    tempfree( x );
    switch( n ) {
    case BOR:
        if( i )
            return( True );
        y = execute( a[1] );
        i = istrue( y );
        tempfree( y );
        break;
    case AND:
        if( !i )
            return( False );
        y = execute( a[1] );
        i = istrue( y );
        tempfree( y );
        break;
    case NOT:
        i = !i;
        break;
    default:        /* can't happen */
        FATAL( "unknown boolean operator %d", n );
        /* never return */
    }
    if( i )
        return( True );
    return( False );
}

Cell *relop( Node **a, int n )
/* a[0 < a[1], etc. */
{
    int         i;
    Cell        *x, *y;
    Awkfloat    j;
    bool        rc;

    x = execute( a[0] );
    y = execute( a[1] );
    if( isnum( x ) && isnum( y ) ) {
        j = x->fval - y->fval;
        i = ( j < 0 ) ? -1 : (( j > 0 ) ? 1 : 0);
    } else {
        i = strcmp( getsval( x ), getsval( y ) );
    }
    tempfree( x );
    tempfree( y );
    rc = false;
    switch( n ) {
    case LT:    rc = ( i < 0 );  break;
    case LE:    rc = ( i <= 0 ); break;
    case NE:    rc = ( i != 0 ); break;
    case EQ:    rc = ( i == 0 ); break;
    case GE:    rc = ( i >= 0 ); break;
    case GT:    rc = ( i > 0 );  break;
    default:    /* can't happen */
        FATAL( "unknown relational operator %d", n );
        /* never return */
    }
    if( rc )
        return( True );
    return( False );
}

void tfree( Cell *a )
/* free a tempcell */
{
    if( freeable( a ) ) {
        dprintf(( "freeing %s %s %o\n", NN( a->nval ), NN( a->sval ), a->tval ));
        xfree( a->sval );
    }
    if( a == tmps )
        FATAL( "tempcell list is curdled" );
        /* never return */
    a->cnext = tmps;
    tmps = a;
}

Cell *gettemp( void )
/* get a tempcell */
{
    int     i;
    Cell    *x;

    if( tmps == NULL ) {
        tmps = (Cell *)calloc( 100, sizeof( Cell ) );
        if( tmps == NULL )
            FATAL( "out of space for temporaries" );
            /* never return */
        for( i = 1; i < 100; i++ )
            tmps[i - 1].cnext = &tmps[i];
        tmps[i - 1].cnext = NULL;
    }
    x = tmps;
    tmps = x->cnext;
    *x = tempcell;
    return( x );
}

Cell *indirect( Node **a, int n )
/* $( a[0] ) */
{
    Awkfloat val;
    Cell *x;
    int m;
    char *s;

    /* unused parameters */ (void)n;

    x = execute( a[0] );
    val = getfval( x );   /* freebsd: defend against super large field numbers */
    if( val > (Awkfloat)INT_MAX )
        FATAL( "trying to access out of range field %s", x->nval );
        /* never return */
    m = (int)val;
    if( m == 0 ) {
        s = getsval( x );
        if( !is_number( s ) ) {   /* suspicion! */
            FATAL( "illegal field $(%s), name \"%s\"", s, x->nval );
            /* BUG: can x->nval ever be null??? */
            /* never return */
        }
    }
    tempfree( x );
    x = fieldadr( m );
    x->ctype = OCELL;   /* BUG?  why are these needed? */
    x->csub = CFLD;
    return( x );
}

static size_t getidxval( Cell *c, int base )
{
    Awkfloat    val;

    val = getfval( c );
    return( ( val > base ) ? (int)val : base );
}

Cell *substr( Node **a, int nnn )
/* substr(a[0], a[1], a[2]) */
{
    size_t k, m, n;
    char *s;
    int temp;
    Cell *x, *y, *z = NULL;

    /* unused parameters */ (void)nnn;

    x = execute( a[0] );
    y = execute( a[1] );
    if( a[2] != 0 )
        z = execute( a[2] );
    s = getsval( x );
    k = strlen( s ) + 1;
    if( k == 1 ) {
        tempfree( x );
        tempfree( y );
        if( a[2] != 0 ) {
            tempfree( z );
        }
        x = gettemp();
        setsval( x, "" );
        return( x );
    }
    m = getidxval( y, 1 );
    if( m > k ) {
        m = k;
    }
    tempfree( y );
    if( a[2] != 0 ) {
        n = getidxval( z, 0 );
        tempfree( z );
    } else {
        n = k - 1;
    }
    if( n > k - m ) {
        n = k - m;
    }
    dprintf(( "substr: m=%d, n=%d, s=%s\n", (int)m, (int)n, s ));
    y = gettemp();
    temp = s[n + m - 1];    /* with thanks to John Linderman */
    s[n + m - 1] = '\0';
    setsval( y, s + m - 1 );
    s[n + m - 1] = temp;
    tempfree( x );
    return( y );
}

Cell *sindex( Node **a, int nnn )
/* index(a[0], a[1]) */
{
    Cell *x, *y, *z;
    char *s1, *s2, *p1, *p2, *q;
    Awkfloat v = 0.0;

    /* unused parameters */ (void)nnn;

    x = execute( a[0] );
    s1 = getsval( x );
    y = execute( a[1] );
    s2 = getsval( y );

    z = gettemp();
    for( p1 = s1; *p1 != '\0'; p1++ ) {
        for( q = p1, p2 = s2; *p2 != '\0' && *q == *p2; q++, p2++ ) {
            ;
        }
        if( *p2 == '\0' ) {
            v = (Awkfloat)( p1 - s1 + 1 );   /* origin 1 */
            break;
        }
    }
    tempfree( x );
    tempfree( y );
    setfval( z, v );
    return( z );
}

#define MAXNUMSIZE  50

int format( char **pbuf, size_t *pbufsize, const char *s, Node *a )
/* printf-like conversions */
{
    char        *fmt;
    char        *p, *t;
    const char  *os;
    Cell        *x;
    int         flag = 0;
    size_t      n;
    size_t      fmtwd; /* format width */
    size_t      fmtsz = recsize;
    char        *buf = *pbuf;
    size_t      bufsize = *pbufsize;
    int         val;
    int         c;

    p = buf;
    if( (fmt = (char *)malloc( fmtsz )) == NULL )
        FATAL( "out of memory in format()" );
        /* never return */
    os = s;
    while( (c = *(uschar *)s) != '\0' ) {
        adjbuf( &buf, &bufsize, MAXNUMSIZE + 1 + p - buf, recsize, &p, "format1" );
        if( c != '%' ) {
            *p++ = c;
            s++;
            continue;
        }
        if( *(uschar *)( s + 1 ) == '%' ) {
            *p++ = '%';
            s += 2;
            continue;
        }
        /* have to be real careful in case this is a huge number, eg, %100000d */
        val = atoi( s + 1 );
        fmtwd = ( val < 0 ) ? -val : val;
        adjbuf( &buf, &bufsize, fmtwd + 1 + p - buf, recsize, &p, "format2" );
        for( t = fmt; (c = *(uschar *)s) != '\0'; s++ ) {
            *t++ = c;
            if( !adjbuf( &fmt, &fmtsz, MAXNUMSIZE + 1 + t - fmt, recsize, &t, "format3" ) )
                FATAL( "format item %.30s... ran format() out of memory", os );
                /* never return */
            if( isalpha( c ) && c != 'l' && c != 'h' && c != 'L' )
                break;  /* the ansi panoply */
            if( c == '*' ) {
                x = execute( a );
                a = a->nnext;
                val = (int)getfval( x );
                sprintf( t - 1, "%d", val );
                fmtwd = ( val < 0 ) ? -val : val;
                adjbuf( &buf, &bufsize, fmtwd + 1 + p - buf, recsize, &p, "format" );
                t = fmt + strlen( fmt );
                tempfree( x );
            }
        }
        *t = '\0';
        adjbuf( &buf, &bufsize, fmtwd + 1 + p - buf, recsize, &p, "format4" );

        switch( c ) {
        case 'f':
        case 'e':
        case 'g':
        case 'E':
        case 'G':
            flag = 'f';
            break;
        case 'd':
        case 'i':
            flag = 'd';
            if( *(uschar *)( s - 1 ) == 'l' )
                break;
            *(t - 1) = 'l';
            *t = 'd';
            *++t = '\0';
            break;
        case 'o':
        case 'x':
        case 'X':
        case 'u':
            flag = ( *(uschar *)( s - 1 ) == 'l' ) ? 'd' : 'u';
            break;
        case 's':
            flag = 's';
            break;
        case 'c':
            flag = 'c';
            break;
        default:
            WARNING("weird printf conversion %s", fmt);
            flag = '?';
            break;
        }
        if( a == NULL )
            FATAL( "not enough args in printf(%s)", os );
            /* never return */
        x = execute(a);
        a = a->nnext;
        n = MAXNUMSIZE;
        if( n < fmtwd )
            n = fmtwd;
        adjbuf( &buf, &bufsize, 1 + n + p - buf, recsize, &p, "format5" );
        switch (flag) {
        case '?':
            sprintf(p, "%s", fmt);  /* unknown, so dump it too */
            t = getsval(x);
            n = strlen( t );
            if( n < fmtwd )
                n = fmtwd;
            adjbuf( &buf, &bufsize, 1 + strlen( p ) + n + p - buf, recsize, &p, "format6" );
            p += strlen( p );
            sprintf(p, "%s", t);
            break;
        case 'f':   sprintf(p, fmt, getfval(x)); break;
        case 'd':   sprintf(p, fmt, (long)getfval(x)); break;
        case 'u':   sprintf(p, fmt, (int)getfval(x)); break;
        case 's':
            t = getsval(x);
            n = strlen( t );
            if( n < fmtwd )
                n = fmtwd;
            if( !adjbuf( &buf, &bufsize, 1 + n + p - buf, recsize, &p, "format7" ) )
                FATAL( "huge string/format (%d chars) in printf %.30s... ran format() out of memory", n, t );
                /* never return */
            sprintf( p, fmt, t );
            break;
        case 'c':
            if( isnum( x ) ) {
                val = (int)getfval( x );
                if( val ) {
                    sprintf( p, fmt, val );
                } else {
                    *p++ = '\0'; /* explicit null byte */
                    *p = '\0';   /* next output will start here */
                }
            } else {
                sprintf( p, fmt, getsval( x )[0] );
            }
            break;
        default:
            FATAL( "can't happen: bad conversion %c in format()", flag );
            /* never return */
        }
        tempfree(x);
        p += strlen( p );
        s++;
    }
    *p = '\0';
    free(fmt);
    for( ; a != NULL; a = a->nnext ) {      /* evaluate any remaining args */
        execute( a );
    }
    *pbuf = buf;
    *pbufsize = bufsize;
    return( (int)( p - buf ) );
}

Cell *awksprintf(Node **a, int n)
/* sprintf(a[0]) */
{
    Cell *x;
    Node *y;
    char *buf;
    size_t bufsz = 3 * recsize;

    /* unused parameters */ (void)n;

    if( (buf = (char *)malloc( bufsz )) == NULL )
        FATAL( "out of memory in awksprintf" );
        /* never return */
    y = a[0]->nnext;
    x = execute(a[0]);
    if( format( &buf, &bufsz, getsval( x ), y ) == -1 )
        FATAL( "sprintf string %.30s... too long.  can't happen.", buf );
        /* never return */
    tempfree(x);
    x = gettemp();
    x->sval = buf;
    x->tval = STR;
    return(x);
}

Cell *awkprintf( Node **a, int n )
/* printf */
/* a[0] is list of args, starting with format string */
/* a[1] is redirection operator, a[2] is redirection file */
{
    FILE *fp;
    Cell *x;
    Node *y;
    char *buf;
    int len;
    size_t bufsz = 3 * recsize;

    /* unused parameters */ (void)n;

    if( (buf = (char *)malloc( bufsz )) == NULL )
        FATAL( "out of memory in awkprintf" );
        /* never return */
    y = a[0]->nnext;
    x = execute(a[0]);
    if( (len = format( &buf, &bufsz, getsval( x ), y )) == -1 )
        FATAL( "printf string %.30s... too long.  can't happen.", buf );
        /* never return */
    tempfree(x);
    if (a[1] == NULL) {
        /* fputs(buf, stdout); */
        fwrite( buf, len, 1, stdout );
        if( ferror( stdout ) ) {
            FATAL( "write error on stdout" );
            /* never return */
        }
    } else {
        fp = redirect( ptoi( a[1] ), a[2] );
        /* fputs( buf, fp ); */
        fwrite( buf, len, 1, fp );
        fflush( fp );
        if( ferror( fp ) ) {
            FATAL( "write error on %s", filename( fp ) );
            /* never return */
        }
    }
    free(buf);
    return(True);
}

Cell *arith(Node **a, int n)
/* a[0] + a[1], etc.  also -a[0] */
{
    Awkfloat i, j = 0;
    double v;
    Cell *x, *y, *z;

    x = execute( a[0] );
    i = getfval( x );
    tempfree( x );
    if( n != UMINUS ) {
        y = execute( a[1] );
        j = getfval( y );
        tempfree( y );
    }
    z = gettemp();
    switch( n ) {
    case ADD:
        i += j;
        break;
    case MINUS:
        i -= j;
        break;
    case MULT:
        i *= j;
        break;
    case DIVIDE:
        if( j == 0 )
            FATAL( "division by zero" );
            /* never return */
        i /= j;
        break;
    case MOD:
        if( j == 0 )
            FATAL( "division by zero in mod" );
            /* never return */
        modf( i / j, &v );
        i = i - j * v;
        break;
    case UMINUS:
        i = -i;
        break;
    case POWER:
        if( j >= 0 && modf( j, &v ) == 0.0 ) { /* pos integer exponent */
            i = ipow( i, (int)j );
        } else {
            i = errcheck( pow( i, j ), "pow" );
        }
        break;
    default:    /* can't happen */
        FATAL( "illegal arithmetic operator %d", n );
        /* never return */
    }
    setfval( z, i );
    return( z );
}

double ipow( double x, int n )
/* x**n.  ought to be done by pow, but isn't always */
{
    double v;

    if( n <= 0 )
        return( 1 );
    v = ipow( x, n / 2 );
    if( ( n % 2 ) == 0 ) {
        return( v * v );
    } else {
        return( x * v * v );
    }
}

Cell *incrdecr( Node **a, int n )
/* a[0]++, etc. */
{
    Cell *x, *z;
    int k;
    Awkfloat xf;

    x = execute( a[0] );
    xf = getfval( x );
    k = ( n == PREINCR || n == POSTINCR ) ? 1 : -1;
    if( n == PREINCR || n == PREDECR ) {
        setfval( x, xf + k );
        return( x );
    }
    z = gettemp();
    setfval( z, xf );
    setfval( x, xf + k );
    tempfree( x );
    return( z );
}

Cell *assign( Node **a, int n )
/* a[0] = a[1], a[0] += a[1], etc. */
/* this is subtle; don't muck with it. */
{
    Cell *x, *y;
    Awkfloat xf, yf;
    double v;

    y = execute( a[1] );
    x = execute( a[0] );
    if( n == ASSIGN ) {  /* ordinary assignment */
        if( x == y && !( x->tval & (FLD | REC) ) ) {   /* self-assignment: */
            ;       /* leave alone unless it's a field */
        } else if( (y->tval & (STR | NUM)) == (STR | NUM) ) {
            setsval( x, getsval( y ) );
            x->fval = getfval( y );
            x->tval |= NUM;
        } else if( isstr( y ) ) {
            setsval( x, getsval( y ) );
        } else if( isnum( y ) ) {
            setfval( x, getfval( y ) );
        } else {
            funnyvar( y, "read value of" );
        }
        tempfree( y );
        return( x );
    }
    xf = getfval( x );
    yf = getfval( y );
    switch( n ) {
    case ADDEQ:
        xf += yf;
        break;
    case SUBEQ:
        xf -= yf;
        break;
    case MULTEQ:
        xf *= yf;
        break;
    case DIVEQ:
        if( yf == 0 )
            FATAL( "division by zero in /=" );
            /* never return */
        xf /= yf;
        break;
    case MODEQ:
        if( yf == 0 )
            FATAL( "division by zero in %%=" );
            /* never return */
        modf( xf / yf, &v );
        xf = xf - yf * v;
        break;
    case POWEQ:
        if( yf >= 0 && modf( yf, &v ) == 0.0 ) {    /* pos integer exponent */
            xf = ipow( xf, (int)yf );
        } else {
            xf = errcheck( pow( xf, yf ), "pow" );
        }
        break;
    default:
        FATAL( "illegal assignment operator %d", n );
        /* never return */
    }
    tempfree( y );
    setfval( x, xf );
    return( x );
}

Cell *cat( Node **a, int q )
/* a[0] cat a[1] */
{
    Cell *x, *y, *z;
    size_t n1, n2;
    char *s;

    /* unused parameters */ (void)q;

    x = execute( a[0] );
    y = execute( a[1] );
    getsval( x );
    getsval( y );
    n1 = strlen( x->sval );
    n2 = strlen( y->sval );
    s = (char *)malloc( n1 + n2 + 1 );
    if( s == NULL )
        FATAL( "out of space concatenating %.15s... and %.15s...", x->sval, y->sval );
        /* never return */
    strcpy( s, x->sval );
    strcpy( s + n1, y->sval );
    tempfree( x );
    tempfree( y );
    z = gettemp();
    z->sval = s;
    z->tval = STR;
    return( z );
}

Cell *pastat( Node **a, int n )
/* a[0] { a[1] } */
{
    Cell *x;

    /* unused parameters */ (void)n;

    if( a[0] == 0 ) {
        x = execute( a[1] );
    } else {
        x = execute( a[0] );
        if( istrue( x ) ) {
            tempfree( x );
            x = execute( a[1] );
        }
    }
    return( x );
}

Cell *dopa2( Node **a, int n )
/* a[0], a[1] { a[2] } */
{
    Cell *x;
    int pair;

    /* unused parameters */ (void)n;

    pair = ptoi( a[3] );
    if( !pairstack[pair] ) {
        x = execute( a[0] );
        if( istrue( x ) )
            pairstack[pair] = true;
        tempfree( x );
    }
    if( pairstack[pair] ) {
        x = execute( a[1] );
        if( istrue( x ) )
            pairstack[pair] = false;
        tempfree( x );
        x = execute( a[2] );
        return( x );
    }
    return( False );
}

Cell *split( Node **a, int nnn )
/* split(a[0], a[1], a[2]); a[3] is type */
{
    Cell *x = NULL, *y, *ap;
    const char *s;
    const char *t;
    int sep;
    char temp, num[50], *fs = '\0';
    int n, tempstat, arg3type;

    /* unused parameters */ (void)nnn;

    y = execute( a[0] );  /* source string */
    s = getsval( y );
    arg3type = ptoi( a[3] );
    if( a[2] == 0 ) {   /* fs string */
        fs = *FS;
    } else if( arg3type == STRING ) {  /* split(str,arr,"string") */
        x = execute( a[2] );
        fs = getsval( x );
    } else if( arg3type == REGEXPR ) {
        fs = "(regexpr)";   /* split(str,arr,/regexpr/) */
    } else {
        FATAL( "illegal type of split" );
        /* never return */
    }
    sep = *fs;
    ap = execute( a[1] ); /* array name */
    freesymtab( ap );
    dprintf(( "split: s=|%s|, a=%s, sep=|%s|\n", s, NN( ap->nval ), fs ));
    ap->tval &= ~STR;
    ap->tval |= ARR;
    ap->sval = (char *)makesymtab( NSYMTAB );

    n = 0;
    if( arg3type == REGEXPR && strlen( ((fa *)a[2])->restr ) == 0 ) {
        /* split(s, a, //); have to arrange that it looks like empty sep */
        arg3type = 0;
        fs = "";
        sep = 0;
    }
    if( *s != '\0' && ( strlen( fs ) > 1 || arg3type == REGEXPR ) ) {    /* reg expr */
        fa *pfa;

        if( arg3type == REGEXPR ) {  /* it's ready already */
            pfa = (fa *)a[2];
        } else {
            pfa = makedfa( fs, 1 );
        }
        if( nematch( pfa, s ) ) {
            tempstat = pfa->initstat;
            pfa->initstat = 2;
            do {
                n++;
                sprintf( num, "%d", n );
                temp = *patbeg;
                *(char *)patbeg = '\0';
                if( is_number( s ) ) {
                    setsymtab( num, s, atof( s ), STR | NUM, (Array *)ap->sval );
                } else {
                    setsymtab( num, s, 0.0, STR, (Array *)ap->sval );
                }
                *(char *)patbeg = temp;
                s = patbeg + patlen;
                if( *( patbeg + patlen - 1 ) == '\0' || *s == '\0' ) {
                    n++;
                    sprintf( num, "%d", n );
                    setsymtab( num, "", 0.0, STR, (Array *)ap->sval );
                    pfa->initstat = tempstat;
                    goto spdone;
                }
            } while( nematch( pfa, s ) );
            pfa->initstat = tempstat;   /* bwk: has to be here to reset */
                                        /* cf gsub and refldbld */
        }
        n++;
        sprintf( num, "%d", n );
        if( is_number( s ) ) {
            setsymtab( num, s, atof( s ), STR | NUM, (Array *)ap->sval );
        } else {
            setsymtab( num, s, 0.0, STR, (Array *)ap->sval );
        }
  spdone:
        pfa = NULL;
    } else if( sep == ' ' ) {
        for( n = 0;; ) {
            while( *s == ' ' || *s == '\t' || *s == '\n' )
                s++;
            if( *s == '\0' )
                break;
            n++;
            t = s;
            do {
                s++;
            } while( *s != ' ' && *s != '\t' && *s != '\n' && *s != '\0' );
            temp = *s;
            *(char *)s = '\0';
            sprintf( num, "%d", n );
            if( is_number( t ) ) {
                setsymtab( num, t, atof( t ), STR | NUM, (Array *)ap->sval );
            } else {
                setsymtab( num, t, 0.0, STR, (Array *)ap->sval );
            }
            *(char *)s = temp;
            if( *s != '\0' ) {
                s++;
            }
        }
    } else if (sep == 0) {  /* new: split(s, a, "") => 1 char/elem */
        for( n = 0; *s != '\0'; s++ ) {
            char buf[2];
            n++;
            sprintf( num, "%d", n );
            buf[0] = *s;
            buf[1] = '\0';
            if( isdigit( (uschar)buf[0] ) ) {
                setsymtab( num, buf, atof( buf ), STR | NUM, (Array *)ap->sval );
            } else {
                setsymtab( num, buf, 0.0, STR, (Array *)ap->sval );
            }
        }
    } else if( *s != '\0' ) {
        for( ;; ) {
            n++;
            t = s;
            while( *s != sep && *s != '\n' && *s != '\0' )
                s++;
            temp = *s;
            *(char *)s = '\0';
            sprintf( num, "%d", n );
            if( is_number( t ) ) {
                setsymtab( num, t, atof( t ), STR | NUM, (Array *)ap->sval );
            } else {
                setsymtab( num, t, 0.0, STR, (Array *)ap->sval );
            }
            *(char *)s = temp;
            if( *s++ == '\0' ) {
                break;
            }
        }
    }
    tempfree( ap );
    tempfree( y );
    if( a[2] != 0 && arg3type == STRING ) {
        tempfree( x );
    }
    x = gettemp();
    x->tval = NUM;
    x->fval = n;
    return( x );
}

Cell *condexpr( Node **a, int n )
/* a[0] ? a[1] : a[2] */
{
    Cell *x;

    /* unused parameters */ (void)n;

    x = execute( a[0] );
    if( istrue( x ) ) {
        tempfree( x );
        x = execute( a[1] );
    } else {
        tempfree( x );
        x = execute( a[2] );
    }
    return( x );
}

Cell *ifstat( Node **a, int n )
/* if (a[0]) a[1]; else a[2] */
{
    Cell *x;

    /* unused parameters */ (void)n;

    x = execute( a[0] );
    if( istrue( x ) ) {
        tempfree( x );
        x = execute( a[1] );
    } else if( a[2] != 0 ) {
        tempfree( x );
        x = execute( a[2] );
    }
    return( x );
}

Cell *whilestat( Node **a, int n )
/* while (a[0]) a[1] */
{
    Cell *x;

    /* unused parameters */ (void)n;

    for( ;; ) {
        x = execute( a[0] );
        if( !istrue( x ) )
            return( x );
        tempfree( x );
        x = execute( a[1] );
        if( isbreak( x ) ) {
            x = True;
            return( x );
        }
        if( isnext( x ) || isexit( x ) || isret( x ) )
            return( x );
        tempfree( x );
    }
}

Cell *dostat( Node **a, int n )
/* do a[0]; while(a[1]) */
{
    Cell *x;

    /* unused parameters */ (void)n;

    for( ;; ) {
        x = execute( a[0] );
        if( isbreak( x ) )
            return( True );
        if( isnext( x ) || isexit( x ) || isret( x ) )
            return( x );
        tempfree( x );
        x = execute( a[1] );
        if( !istrue( x ) )
            return( x );
        tempfree( x );
    }
}

Cell *forstat( Node **a, int n )
/* for (a[0]; a[1]; a[2]) a[3] */
{
    Cell *x;

    /* unused parameters */ (void)n;

    x = execute( a[0] );
    tempfree( x );
    for( ;; ) {
        if( a[1] != 0 ) {
            x = execute( a[1] );
            if( !istrue( x ) ) {
                return( x );
            } else {
                tempfree( x );
            }
        }
        x = execute( a[3] );
        if( isbreak( x ) )     /* turn off break */
            return( True );
        if( isnext( x ) || isexit( x ) || isret( x ) )
            return( x );
        tempfree( x );
        x = execute( a[2] );
        tempfree( x );
    }
}

Cell *instat( Node **a, int n )
/* for (a[0] in a[1]) a[2] */
{
    Cell *x, *vp, *arrayp, *cp, *ncp;
    Array *tp;
    int i;

    /* unused parameters */ (void)n;

    vp = execute( a[0] );
    arrayp = execute( a[1] );
    if( !isarr( arrayp ) ) {
        return( True );
    }
    tp = (Array *)arrayp->sval;
    tempfree( arrayp );
    for( i = 0; i < tp->size; i++ ) {    /* this routine knows too much */
        for( cp = tp->tab[i]; cp != NULL; cp = ncp ) {
            setsval( vp, cp->nval );
            ncp = cp->cnext;
            x = execute( a[2] );
            if( isbreak( x ) ) {
                tempfree( vp );
                return( True );
            }
            if( isnext( x ) || isexit( x ) || isret( x ) ) {
                tempfree( vp );
                return( x );
            }
            tempfree( x );
        }
    }
    return( True );
}

Cell *bltin( Node **a, int n )
/* builtin functions. a[0] is type, a[1] is arg list */
{
    Cell *x, *y;
    Awkfloat u;
    int t;
    Awkfloat tmp;
    char *p, *buf;
    Node *nextarg;
    FILE *fp;
    int c;

    /* unused parameters */ (void)n;

    u = (Awkfloat)0;
    t = ptoi( a[0] );
    x = execute( a[1] );
    nextarg = a[1]->nnext;
    switch( t ) {
    case FLENGTH:
        if( isarr( x ) ) {
            u = ((Array *)x->sval)->nelem; /* GROT.  should be function*/
        } else {
            u = (Awkfloat)strlen( getsval( x ) );
        }
        break;
    case FLOG:
        u = errcheck( log( getfval( x ) ), "log" ); break;
    case FINT:
        modf( getfval( x ), &u ); break;
    case FEXP:
        u = errcheck( exp( getfval( x ) ), "exp" ); break;
    case FSQRT:
        u = errcheck( sqrt( getfval( x ) ), "sqrt" ); break;
    case FSIN:
        u = sin( getfval( x ) ); break;
    case FCOS:
        u = cos( getfval( x ) ); break;
    case FATAN:
        if( nextarg == NULL ) {
            WARNING( "atan2 requires two arguments; returning 1.0" );
            u = 1.0;
        } else {
            y = execute( a[1]->nnext );
            u = atan2( getfval( x ), getfval( y ) );
            tempfree( y );
            nextarg = nextarg->nnext;
        }
        break;
    case FSYSTEM:
        fflush( stdout );     /* in case something is buffered already */
        u = (Awkfloat)system( getsval( x ) ) / 256;   /* 256 is unix-dep */
        break;
    case FRAND:
        /* in principle, rand() returns something in 0..RAND_MAX */
        u = (Awkfloat)( rand() % RAND_MAX ) / RAND_MAX;
        break;
    case FSRAND:
        if( isrec( x ) ) { /* no argument provided */
            u = (Awkfloat)time( NULL );
        } else {
            u = getfval( x );
        }
        tmp = u;
        srand( (unsigned int)u );
        u = srand_seed;
        srand_seed = tmp;
        break;
    case FTOUPPER:
    case FTOLOWER:
        buf = tostring( getsval( x ) );
        if( t == FTOUPPER ) {
            for( p = buf; (c = *(uschar *)p) != '\0'; p++ ) {
                if( islower( c ) ) {
                    *p = toupper( c );
                }
            }
        } else {
            for( p = buf; (c = *(uschar *)p) != '\0'; p++ ) {
                if( isupper( c ) ) {
                    *p = tolower( c );
                }
            }
        }
        tempfree( x );
        x = gettemp();
        setsval( x, buf );
        free( buf );
        return( x );
    case FFLUSH:
        if( isrec( x ) || strlen( getsval( x ) ) == 0 ) {
            flush_all();    /* fflush() or fflush("") -> all */
            u = 0;
        } else if( (fp = openfile( FFLUSH, getsval( x ) )) == NULL ) {
            u = EOF;
        } else {
            u = fflush( fp );
        }
        break;
    default:    /* can't happen */
        FATAL( "illegal function type %d", t );
        /* never return */
    }
    tempfree( x );
    x = gettemp();
    setfval( x, u );
    if( nextarg != NULL ) {
        WARNING("warning: function has too many arguments");
        for( ; nextarg != NULL; nextarg = nextarg->nnext ) {
            execute( nextarg );
        }
    }
    return( x );
}

Cell *printstat( Node **a, int n )
/* print a[0] */
{
    Node *x;
    Cell *y;
    FILE *fp;

    /* unused parameters */ (void)n;

    if( a[1] == 0 ) {   /* a[1] is redirection operator, a[2] is file */
        fp = stdout;
    } else {
        fp = redirect( ptoi( a[1] ), a[2] );
    }
    for( x = a[0]; x != NULL; x = x->nnext ) {
        y = execute( x );
        fputs( getpssval( y ), fp );
        tempfree( y );
        if( x->nnext == NULL ) {
            fputs( *ORS, fp );
        } else {
            fputs( *OFS, fp );
        }
    }
    if( a[1] != 0 )
        fflush( fp );
    if( ferror( fp ) )
        FATAL( "write error on %s", filename( fp ) );
        /* never return */
    return( True );
}

Cell *nullproc( Node **a, int n )
{
    n = n;
    a = a;
    return 0;
}


FILE *redirect( int a, Node *b )
/* set up all i/o redirections */
{
    FILE *fp;
    Cell *x;
    char *fname;

    x = execute( b );
    fname = getsval( x );
    fp = openfile( a, fname );
    if( fp == NULL )
        FATAL( "can't open file %s", fname );
        /* never return */
    tempfree( x );
    return( fp );
}

FILE *openfile( int a, const char *us )
{
    const char *s = us;
    int i, m;
    FILE *fp = NULL;

    if( *s == '\0' )
        FATAL( "null file name in print or getline" );
        /* never return */
    for( i = 0; i < nfiles; i++ ) {
        if( files[i].fname && strcmp( s, files[i].fname ) == 0 ) {
            if( a == files[i].mode || ( a == APPEND && files[i].mode == GT ) )
                return( files[i].fp );
            if( a == FFLUSH ) {
                return( files[i].fp );
            }
        }
    }
    if( a == FFLUSH )    /* didn't find it, so don't create it! */
        return NULL;

    for( i = 0; i < nfiles; i++ ) {
        if( files[i].fp == 0 ) {
            break;
        }
    }
    if( i >= nfiles ) {
        struct files *nf;
        int nnf = nfiles + FOPEN_MAX;
        nf = realloc( files, nnf * sizeof( *nf ) );
        if( nf == NULL )
            FATAL( "cannot grow files for %s and %d files", s, nnf );
            /* never return */
        memset( &nf[nfiles], 0, FOPEN_MAX * sizeof( *nf ) );
        nfiles = nnf;
        files = nf;
    }
    fflush( stdout ); /* force a semblance of order */
    m = a;
    if( a == GT ) {
        fp = fopen( s, "w" );
    } else if( a == APPEND ) {
        fp = fopen( s, "a" );
        m = GT; /* so can mix > and >> */
    } else if( a == '|' ) {  /* output pipe */
        fp = popen( s, "w" );
    } else if( a == LE ) {   /* input pipe */
        fp = popen( s, "r" );
    } else if( a == LT ) {   /* getline <file */
        fp = ( strcmp( s, "-" ) == 0 ) ? stdin : fopen( s, "r" );   /* "-" is stdin */
    } else {    /* can't happen */
        FATAL( "illegal redirection %d", a );
        /* never return */
    }
    if( fp != NULL ) {
        files[i].fname = tostring( s );
        files[i].fp = fp;
        files[i].mode = m;
    }
    return( fp );
}

const char *filename( FILE *fp )
{
    int i;

    for( i = 0; i < nfiles; i++ ) {
        if( fp == files[i].fp ) {
            return( files[i].fname );
        }
    }
    return( "???" );
}

Cell *closefile( Node **a, int n )
{
    Cell *x;
    int i, stat;

    n = n;
    x = execute( a[0] );
    getsval( x );
    stat = -1;
    for( i = 0; i < nfiles; i++ ) {
        if( files[i].fname && strcmp( x->sval, files[i].fname ) == 0 ) {
            if( ferror( files[i].fp ) )
                WARNING( "i/o error occurred on %s", files[i].fname );
            if( files[i].mode == '|' || files[i].mode == LE ) {
                stat = pclose( files[i].fp );
            } else {
                stat = fclose( files[i].fp );
            }
            if( stat == EOF )
                WARNING( "i/o error occurred closing %s", files[i].fname );
            if( i > 2 )  /* don't do /dev/std... */
                xfree( files[i].fname );
            files[i].fname = NULL;  /* watch out for ref thru this */
            files[i].fp = NULL;
        }
    }
    tempfree( x );
    x = gettemp();
    setfval( x, (Awkfloat)stat );
    return( x );
}

void closeall( void )
{
    int i, stat;

    for( i = 0; i < FOPEN_MAX; i++ ) {
        if( files[i].fp ) {
            if( ferror( files[i].fp ) )
                WARNING( "i/o error occurred on %s", files[i].fname );
            if( files[i].mode == '|' || files[i].mode == LE ) {
                stat = pclose( files[i].fp );
            } else {
                stat = fclose( files[i].fp );
            }
            if( stat == EOF ) {
                WARNING( "i/o error occurred while closing %s", files[i].fname );
            }
        }
    }
}

static void backsub( char **pb_ptr, const char **sptr_ptr )
/* handle \\& variations */
/* sptr[0] == '\\' */
{
    char        *pb = *pb_ptr;
    const char  *sptr = *sptr_ptr;

    if( sptr[1] == '\\' ) {
        if( sptr[2] == '\\' && sptr[3] == '&' ) { /* \\\& -> \& */
            *pb++ = '\\';
            *pb++ = '&';
            sptr += 4;
        } else if( sptr[2] == '&' ) {   /* \\& -> \ + matched */
            *pb++ = '\\';
            sptr += 2;
        } else {                        /* \\x -> \\x */
            *pb++ = *sptr++;
            *pb++ = *sptr++;
        }
    } else if( sptr[1] == '&' ) {       /* literal & */
        sptr++;
        *pb++ = *sptr++;
    } else {                            /* literal \ */
        *pb++ = *sptr++;
    }
    *pb_ptr = pb;
    *sptr_ptr = sptr;
}

Cell *sub( Node **a, int nnn )
/* substitute command */
{
    const char *sptr;
    const char *q;
    char *pb;
    Cell *x, *y, *result;
    const char *t;
    char *buf;
    fa *pfa;
    size_t bufsz = recsize;

    /* unused parameters */ (void)nnn;

    if( (buf = (char *)malloc( bufsz )) == NULL )
        FATAL( "out of memory in sub" );
        /* never return */
    x = execute( a[3] );  /* target string */
    t = getsval( x );
    if( a[0] == 0 ) {    /* 0 => a[1] is already-compiled regexpr */
        pfa = (fa *)a[1];  /* regular expression */
    } else {
        y = execute( a[1] );
        pfa = makedfa( getsval( y ), 1 );
        tempfree( y );
    }
    y = execute( a[2] );  /* replacement string */
    result = False;
    if( pmatch( pfa, t ) ) {
        sptr = t;
        adjbuf( &buf, &bufsz, 1 + patbeg - sptr, recsize, 0, "sub" );
        pb = buf;
        while( sptr < patbeg )
            *pb++ = *sptr++;
        sptr = getsval( y );
        while( *sptr != '\0' ) {
            adjbuf( &buf, &bufsz, 5 + pb - buf, recsize, &pb, "sub" );
            if( *sptr == '\\' ) {
                backsub( &pb, &sptr );
            } else if( *sptr == '&' ) {
                sptr++;
                adjbuf( &buf, &bufsz, 1 + patlen + pb - buf, recsize, &pb, "sub" );
                for( q = patbeg; q < patbeg + patlen; ) {
                    *pb++ = *q++;
                }
            } else {
                *pb++ = *sptr++;
            }
        }
        *pb = '\0';
        if( pb > buf + bufsz )
            FATAL( "sub result1 %.30s too big; can't happen", buf );
            /* never return */
        sptr = patbeg + patlen;
        if( ( patlen == 0 && *patbeg ) || ( patlen && *( sptr - 1 ) ) ) {
            adjbuf( &buf, &bufsz, 1 + strlen( sptr ) + pb - buf, 0, &pb, "sub" );
            while( (*pb++ = *sptr++) != '\0' ) {
                ;
            }
        }
        if( pb > buf + bufsz )
            FATAL( "sub result2 %.30s too big; can't happen", buf );
            /* never return */
        setsval( x, buf );    /* BUG: should be able to avoid copy */
        result = True;
    }
    tempfree( x );
    tempfree( y );
    free( buf );
    return( result );
}

Cell *gsub( Node **a, int nnn )
/* global substitute */
{
    Cell *x, *y;
    char *rptr, *pb;
    const char *sptr;
    const char *t;
    const char *q;
    char *buf;
    fa *pfa;
    int mflag, tempstat, num;
    size_t bufsz = recsize;

    /* unused parameters */ (void)nnn;

    if( (buf = (char *)malloc( bufsz )) == NULL )
        FATAL( "out of memory in gsub" );
        /* never return */
    mflag = 0;  /* if mflag == 0, can replace empty string */
    num = 0;
    x = execute( a[3] );  /* target string */
    t = getsval( x );
    if( a[0] == 0 ) {    /* 0 => a[1] is already-compiled regexpr */
        pfa = (fa *)a[1];  /* regular expression */
    } else {
        y = execute( a[1] );
        pfa = makedfa( getsval( y ), 1 );
        tempfree( y );
    }
    y = execute( a[2] );  /* replacement string */
    if( pmatch( pfa, t ) ) {
        tempstat = pfa->initstat;
        pfa->initstat = 2;
        pb = buf;
        rptr = getsval( y );
        do {
            if( patlen == 0 && *patbeg != '\0' ) {  /* matched empty string */
                if( mflag == 0 ) {   /* can replace empty */
                    num++;
                    sptr = rptr;
                    while( *sptr != '\0' ) {
                        adjbuf( &buf, &bufsz, 5 + pb - buf, recsize, &pb, "gsub" );
                        if( *sptr == '\\' ) {
                            backsub( &pb, &sptr );
                        } else if( *sptr == '&' ) {
                            sptr++;
                            adjbuf( &buf, &bufsz, 1 + patlen + pb - buf, recsize, &pb, "gsub" );
                            for( q = patbeg; q < patbeg + patlen; ) {
                                *pb++ = *q++;
                            }
                        } else {
                            *pb++ = *sptr++;
                        }
                    }
                }
                if( *t == '\0' )    /* at end */
                    goto done;
                adjbuf( &buf, &bufsz, 2 + pb - buf, recsize, &pb, "gsub" );
                *pb++ = *t++;
                if( pb > buf + bufsz )   /* BUG: not sure of this test */
                    FATAL( "gsub result0 %.30s too big; can't happen", buf );
                    /* never return */
                mflag = 0;
            } else {  /* matched nonempty string */
                num++;
                sptr = t;
                adjbuf( &buf, &bufsz, 1 + ( patbeg - sptr ) + pb - buf, recsize, &pb, "gsub" );
                while( sptr < patbeg )
                    *pb++ = *sptr++;
                sptr = rptr;
                while( *sptr != '\0' ) {
                    adjbuf( &buf, &bufsz, 5 + pb - buf, recsize, &pb, "gsub" );
                    if( *sptr == '\\' ) {
                        backsub( &pb, &sptr );
                    } else if( *sptr == '&' ) {
                        sptr++;
                        adjbuf( &buf, &bufsz, 1 + patlen + pb - buf, recsize, &pb, "gsub" );
                        for( q = patbeg; q < patbeg + patlen; ) {
                            *pb++ = *q++;
                        }
                    } else {
                        *pb++ = *sptr++;
                    }
                }
                t = patbeg + patlen;
                if( patlen == 0 || *t == '\0' || *( t - 1 ) == '\0' )
                    goto done;
                if( pb > buf + bufsz )
                    FATAL( "gsub result1 %.30s too big; can't happen", buf );
                    /* never return */
                mflag = 1;
            }
        } while( pmatch( pfa, t ) );
        sptr = t;
        adjbuf( &buf, &bufsz, 1 + strlen( sptr ) + pb - buf, 0, &pb, "gsub" );
        while( (*pb++ = *sptr++) != '\0' ) {
            ;
        }
    done:
        if( pb < buf + bufsz ) {
            *pb = '\0';
        } else if( *( pb - 1 ) != '\0' ) {
            FATAL( "gsub result2 %.30s truncated; can't happen", buf );
            /* never return */
        }
        setsval( x, buf );    /* BUG: should be able to avoid copy + free */
        pfa->initstat = tempstat;
    }
    tempfree( x );
    tempfree( y );
    x = gettemp();
    x->tval = NUM;
    x->fval = num;
    free( buf );
    return( x );
}
