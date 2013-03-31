/*
sed.c -- execute compiled form of stream editor commands

   The single entry point of this module is the function execute(). It
may take a string argument (the name of a file to be used as text)  or
the argument NULL which tells it to filter standard input. It executes
the compiled commands in cmds[] on each line in turn.
   The function command() does most of the work. Match() and advance()
are used for matching text against precompiled regular expressions and
dosub() does right-hand-side substitution.  Getline() does text input;
readout() and memeql() are output and string-comparison utilities.

==== Written for the GNU operating system by Eric S. Raymond ====

18NOV86 Fixed bug in 'selected()' that prevented address ranges from
    working.                - Billy G. Allie.
21FEB88 Refixed bug in 'selected()'     - Charles Marslett
*/

#include <assert.h>
#include <stdio.h>                      /* {f}puts, {f}printf, etc. */
#include <ctype.h>                      /* isprint(), isdigit(), toascii() */
#include <stdlib.h>                     /* for exit() */
#include "wio.h"
#include "sed.h"                        /* command structures & constants */

#define MAXHOLD         MAXBUF          /* size of the hold space */
#define GENSIZ          MAXBUF          /* maximum genbuf size */

#define TRUE            1
#define FALSE           0

#if 0
/* LTLMSG was used when buffer overflow stopped sed */
static char const       LTLMSG[] = "sed: line too long \"%.*s\"\n";
#define ABORTEX(msg) fprintf( stderr, msg, sizeof genbuf, genbuf ), exit( 2 )
#endif

static char const       FRENUL[] = "sed: first RE must be non-null\n";
static char const       NOROOM[] = "sed: can only fit %d bytes at line %ld\n";
static char const       INTERR[] = "sed: internal error: %s\n";

static char     *spend;                 /* current end-of-line-buffer pointer */
static long     lnum = 0L;              /* current source line number */

                                        /* append buffer maintenance */
static sedcmd   *appends[MAXAPPENDS];   /* array of ptrs to a,i,c commands */
static sedcmd   **aptr = appends;       /* ptr to current append */

                                        /* genbuf and its pointers */
static char     genbuf[GENSIZ];
static char     *loc1;                  /* Where match() tried to find a BRE */
static char     *loc2;                  /* Immediately after advance() completing match() or last character to remove in dosub() */
static char     *locs;                  /* match() sets this as a backtrack backstop */

                                        /* command-logic flags */
static int      lastline;               /* do-line flag */
static int      jump;                   /* jump to cmd's link address if set */
static int      delete;                 /* delete command flag */

                                        /* tagged-pattern tracking */
static char     *bracend[MAXTAGS+1];    /* tagged pattern start pointers */
static char     *brastart[MAXTAGS+1];   /* tagged pattern end pointers */
static sedcmd   *pending = NULL;        /* next command to be executed */

static int      selected( sedcmd *ipc );
static int      match( char *expbuf, int gf, int is_cnt );
static int      advance( register char *lp, register char *ep );
static int      substitute( sedcmd const *ipc );
static void     dosub( char const *rhsbuf );
static char     *place( register char *asp, register char const *al1, register char const *al2 );
static void     listto( register char const *p1, FILE *fp );
static void     command( sedcmd *ipc );
static char     *getline( register char *buf );
static int      memeql( register char const *a, register char const *b, int count );
static void     readout( void );

/* execute the compiled commands in cmds[] on a file */
void execute( const char *file )        /* name of text source file to filter */
{
    register char const *p1;            /* dummy copy ptrs */
    register sedcmd     *ipc;           /* ptr to current command */
    char                *execp;         /* ptr to source */

    if( file != NULL ) {                /* filter text from a named file */
        if( freopen( file, "r", stdin ) == NULL )
            fprintf( stderr, "sed: can't open %s\n", file );
    } else
        if( isatty( fileno( stdin ) ) ) /* It is easy to be spuriously awaiting input */
            fprintf( stderr, "sed: reading from terminal\n" );

    if( pending ) {                     /* there's a command waiting */
        ipc = pending;                  /* it will be first executed */
        pending = NULL;                 /* turn off the waiting flag */
        goto doit;                      /* go to execute it immediately */
    }
                                        /* the main command-execution loop */
    for( ;; ) {
                                        /* get next line to filter */
                                        /* jump is set but not cleared by D */
        if( ( execp = getline( jump ? spend : linebuf ) ) == NULL ) {
            if( jump ) {
                for( p1 = linebuf; p1 < spend; p1++ )
                    putc( *p1, stdout );
                putc( '\n', stdout );
            }
            return;
        }
        jump = FALSE;
        spend = execp;
                                        /* compiled commands execute loop */
        for( ipc = cmds; ipc->command != 0; ipc++ ) {
            if( !selected( ipc ) )
                continue;
        doit:
            command( ipc );             /* execute the command pointed at */

            if( delete )                /* if delete flag is set */
                break;                  /* don't exec rest of compiled cmds */

            if( jump ) {                /* if jump set, follow cmd's link */
                jump = FALSE;
                if( ( ipc = ipc->u.link ) == NULL )
                    break;
            }
        }
                                        /* all commands now done on the line */
                                        /* output the transformed line is */
        if( !nflag && !delete ) {
            for( p1 = linebuf; p1 < spend; p1++ )
                putc( *p1, stdout );
            putc( '\n', stdout );
        }

        readout();                      /* emit any append text */

        delete = FALSE;                 /* clear delete flag; get next cmd */
    }
}

/* is current command selected */
static int selected( sedcmd *ipc )
{
    register char               *p1 = ipc->addr1;       /* first address */
    register char * const       p2 = ipc->addr2;        /*   and second */
    int                         c;
    int const                   allbut = ipc->flags.allbut;

    if( !p1 )
        return( !allbut );

    if( ipc->flags.inrange ) {
        if( *p2 == CEND )
            p1 = NULL;
        else if( *p2 == CLNUM ) {
            c = p2[1];
            if( lnum > linenum[c] ) {
                ipc->flags.inrange = FALSE;
                return( allbut );
            }
            if( lnum == linenum[c] )
                ipc->flags.inrange = FALSE;
        } else if( match( p2, 0, 0 ) )
            ipc->flags.inrange = FALSE;
    } else if( *p1 == CEND ) {
        if( !lastline )
            return( allbut );
    }
    else if( *p1 == CLNUM ) {
        c = p1[1];
        if( lnum != linenum[c] )
            return( allbut );
        if( p2 )
            ipc->flags.inrange = TRUE;
    } else if( match( p1, 0, 0 ) ) {
        if( p2 )
            ipc->flags.inrange = TRUE;
    } else {
        return( allbut );
    }
    return( !allbut );
}

/* match RE at expbuf against linebuf; if gf set, copy linebuf from genbuf */
static int match(
    char                *expbuf,
    int                 gf,
    int                 is_cnt )
{
    register char       *p1;
    register char       *p2;
    register char       c;
    static char         *lastre = NULL; /* old RE pointer */

    if( *expbuf == CEOF ) {
        if( lastre == NULL )
            fprintf( stderr, "%s", FRENUL ), exit( 2 ); /* no previous RE */
        expbuf = lastre;
    }
    else
        lastre = expbuf;

    if( gf ) {
        if( *expbuf )
            return( FALSE );
        p1 = linebuf;
        p2 = genbuf;
        while( ( *p1++ = *p2++ ) != 0 ) ;
        locs = p1 = loc2;
    } else {
        p1 = ( is_cnt ) ? loc2 : linebuf;
        locs = NULL;
    }

    p2 = expbuf;
    if( *p2++ ) {
        loc1 = p1;
        if( *p2 == CCHR && p2[1] != *p1 ) /* 1st char is wrong */
            return( FALSE );            /*   so fail */
        return( advance( p1, p2 ) );    /* else try to match rest */
    }
                                        /* literal 1st character quick check */
    if( *p2 == CCHR ) {
        c = p2[1];                      /* pull out character to search for */
        do {
            if( *p1 == c )              /* scan the source string */
                if( advance( p1, p2 ) ) /* found it, match the rest */
                    return( loc1 = p1, 1 );
        } while( *p1++ );
        return( FALSE );                /* didn't find that first char */
    }
                                        /* else try unanchored pattern match */
    do {
        if( advance( p1, p2 ) )
            return( loc1 = p1, 1 );
    } while( *p1++ );
                                        /* didn't match either way */
    return( FALSE );
}

/* attempt to advance match pointer by one pattern element */
static int advance(
    register char       *lp,            /* source (linebuf) ptr */
    register char       *ep )           /* regular expression element ptr */
{
    register char const *curlp;         /* save ptr for closures */
    char                c;              /* scratch character holder */
    char                *bbeg;
    char                *tep;
    int                 ct;
    int                 i1;
    int                 i2;

    for( ;; )
        switch( *ep++ ) {
        case CCHR:                      /* literal character */
            if( *ep++ != *lp++ )        /* if chars unequal */
                return( FALSE );        /* return false */
            break;                      /* matched */

        case CDOT:                      /* anything but NUL */
            if( *lp++ == 0 )            /* first NUL is at EOL */
                return( FALSE );        /* return false */
            break;                      /* matched */

        case CNL:                       /* start-of-line */
        case CDOL:                      /* end-of-line */
            if( *lp != 0 )              /* found that first NUL? */
                return( FALSE );        /* return false */
            break;                      /* matched */

        case CEOF:                      /* end-of-address mark */
            loc2 = lp;                  /* set second loc */
            return( TRUE );             /* return true */

        case CCL:                       /* a closure */
            c = *lp++ &0177;
            if( !( ep[c >> 3] & bits[c & 07] ) ) /* is char in set? */
                return( FALSE );        /* return false */
            ep += 16;                   /* skip rest of bitmask */
            break;                      /*   and keep going */

        case CBRA:                      /* start of tagged pattern */
            brastart[(int)*ep++] = lp;  /* mark it */
            break;                      /* and go */

        case CKET:                      /* end of tagged pattern */
            bracend[(int)*ep++] = lp;   /* mark it */
            break;                      /* and go */

        case CBACK:
            bbeg = brastart[(int)*ep];
            ct = bracend[(int)*ep++] - bbeg;

            if( !memeql( bbeg, lp, ct ) )
                return( FALSE );        /* return false */
            lp += ct;
            break;                      /* matched */

        case CBACK | STAR:
            bbeg = brastart[(int)*ep];
            if( ( ct = bracend[(int)*ep++] - bbeg ) <= 0 )
                break;                  /* zero (or negative ??) length match */
            curlp = lp;
            while( memeql( bbeg, lp, ct ) )
                lp += ct;

            while( lp >= curlp ) {
                if( advance( lp, ep ) )
                    return( TRUE );
                lp -= ct;
            }
            return( FALSE );

        case CDOT | STAR:               /* match .* */
            curlp = lp;                 /* save closure start loc */
            while( ( *lp++ ) != 0 ) ;   /* match anything */
            goto star;                  /* now look for followers */

        case CCHR | STAR:               /* match <literal char>* */
            curlp = lp;                 /* save closure start loc */
            while( (int)( *lp++ == *ep ) != 0 ) ; /* match many of that char */
            ep++;                       /* to start of next element */
            goto star;                  /* match it and followers */

        case CCL | STAR:                /* match [...]* */
            curlp = lp;                 /* save closure start loc */
            while( c = *lp++ & 0x7F, ep[c >> 3] & bits[c & 07] ) ;
            ep += 16;                   /* skip past the set */
            goto star;                  /* match followers */

        star:                           /* the repeat part of a * or + match */
            if( --lp == curlp )         /* 0 matches */
                break;

            switch( *ep ) {
                case CCHR:
                    c = ep[1];
                    break;
                case CBACK:
                    c = *brastart[(int)ep[1]];
                    break;
                default:
                    do {
                        if( lp == locs )
                            break;
                        if( advance( lp, ep ) )
                            return( TRUE );
                    } while( lp-- > curlp );
                    return( FALSE );
            }

            do {
                if( *lp == c )
                    if( advance( lp, ep ) )
                        return( TRUE );
            } while( lp-- > curlp );
            return( FALSE );

        default:
            fprintf( stderr, "sed: RE error, %o\n", *--ep );
            exit( 2 );

        case CBRA | STAR:               /* start of starred tagged pattern */
            {
                int const       tagindex = *ep;
                int             matched = FALSE;

                curlp = lp;                 /* save closure start loc */
                ct = ep[1];
                bracend[tagindex] = bbeg = tep = lp;
                ep++;
                while( advance( brastart[tagindex] = bracend[tagindex], ep+1 ) && bracend[tagindex] > brastart[tagindex] )
                    if( advance( bracend[tagindex], ep + ct ) ) { /* Try to match RE after \(...\) */
                        matched = TRUE;          /* Remember greediest match */
                        bbeg = brastart[tagindex];
                        tep = bracend[tagindex];
                    }

                if( matched ) {                  /* Did we match RE after \(...\) */
                    brastart[tagindex] = bbeg;    /* Set details of match */
                    bracend[tagindex] = tep;
                    return( TRUE );
                }

                return( advance( bracend[tagindex], ep + ct ) ); /* Zero matches */
            }

        case CBRA | MTYPE:              /* \(...\)\{m,n\} WFB */
            {
                int const       tagindex = *ep;
                int             matched = FALSE;

                curlp = lp;             /* save closure start loc */
                ct = ep[1];
                i1 = ep[ct-1] & 0xFF, i2 = ep[ct] & 0xFF;
                bracend[tagindex] = bbeg = tep = lp;

                while( i1 && advance( lp, ep+2 ) && bracend[tagindex] > lp )
                    brastart[tagindex] = lp, lp = bracend[tagindex], i1--;
                if( i1 )
                    return( FALSE );
                if( !i2 )
                    return( advance( bracend[tagindex], ep + ct + 1 ) ); /* Zero matches */
                if( i2 == 0xFF )
                    i2 = MAXBUF;

                while( advance( brastart[tagindex] = bracend[tagindex], ep+2 ) && bracend[tagindex] > brastart[tagindex] && i2 )
                    if( i2--, advance( bracend[tagindex], ep + ct + 1 ) ) { /* Try to match RE after \(...\) */
                        matched = TRUE; /* Remember greediest match */
                        bbeg = brastart[tagindex];
                        tep = bracend[tagindex];
                    }

                if( matched ) {         /* Did we match RE after \(...\) */
                    brastart[tagindex] = bbeg; /* Set details of match */
                    bracend[tagindex] = tep;
                    return( TRUE );
                }

                if( i1 )
                    return( FALSE );
                return( advance( bracend[tagindex], ep + ct + 1 ) ); /* Zero matches */
            }

        case CCHR | MTYPE:              /* Match <literal char>\{...\} */
            c = *ep++;                  /* Get byte and skip to next element */
            i1 = *ep++ & 0xFF, i2 = *ep++ & 0xFF;
            while( c == *lp && i1 )
                lp++, i1--;
            if( i1 )
                return( FALSE );
            if( !i2 )
                break;
            if( i2 == 0xFF )
                i2 = MAXBUF;
            curlp = lp;
            while( c == *lp++ && i2 )
                i2--;
            goto star;

        case CKET | STAR:               /* match \(..\)* */
        case CKET | MTYPE:              /* match \(..\)\{...\} */
            bracend[(int)*ep] = lp;     /* mark it */
            return( TRUE );

        case CDOT | MTYPE:              /* match .\{...\} */
            i1 = *ep++ & 0xFF, i2 = *ep++ & 0xFF;
            while( *lp && i1 )
                lp++, i1--;
            if( i1 )
                return( FALSE );
            if( !i2 )
                break;
            if( i2 == 0xFF )
                i2 = MAXBUF;
            curlp = lp;
            while( *lp++ && i2 )
                i2--;
            goto star;

        case CCL | MTYPE:               /* match [...]\{...\} */
            tep = ep;
            ep += 16;
            i1 = *ep++ & 0xFF, i2 = *ep++ & 0xFF;
            /* WFB 1 CCL|MTYPE handler must be like CCHR|MTYPE or off by 1 */
            while( ct = *lp, tep[(unsigned)ct >> 3] & bits[ct & 7] && i1 )
                lp++, i1--;
            if( i1 )
                return( FALSE );
            if( !i2 )
                break;
            if( i2 == 0xFF )
                i2 = MAXBUF;
            curlp = lp;
            while( ct = *lp++ & 0xff, tep[(unsigned)ct >> 3] & bits[ct & 7] && i2 )
                i2--;
            goto star;

        case CBACK | MTYPE:             /* e.g. \(.\)\1\{5\} */
            bbeg = brastart[(int)*ep];
            ct = bracend[(int)*ep++] - bbeg;
            i1 = *ep++ & 0xFF, i2 = *ep++ & 0xFF;
            while( memeql( bbeg, lp, ct ) && i1 )
                lp += ct, i1--;
            if( i1 )
                return( FALSE );
            if( !i2 )
                break;
            if( i2 == 0xFF )
                i2 = MAXBUF;
            curlp = lp;
            while( memeql( bbeg, lp, ct ) && i2 )
                lp+= ct, i2--;
            while( lp >= curlp ) {
                if( advance( lp, ep ) )
                    return( TRUE );
                lp -= ct;
            }
            return( FALSE );

        } /* switch( *ep++ ) */
}

/* perform s command */
static int substitute( sedcmd const *ipc ) /* ptr to s command struct */
{
    int fcnt = ipc->flags.nthone;

    if( !match( ipc->u.lhs, 0, 0 ) )    /* if no match */
        return( FALSE );                /* command fails */

    if( fcnt ) {
        while( --fcnt > 0 && *loc2 && match( ipc->u.lhs, 0, 1 ) )
            ;
        if( fcnt != 0 )
            return( FALSE );            /* command fails */
    }

    dosub( ipc->rhs );                  /* perform it once */

    if( ipc->flags.global )             /* if global flag enabled */
        while( *loc2 && match( ipc->u.lhs, 1, 0 ) ) /* cycle through possibles */
            dosub( ipc->rhs );          /* so substitute */
    return( TRUE );                     /* we succeeded */
}

/* generate substituted right-hand side (of s command) */
static void dosub( char const *rhsbuf ) /* where to put the result */
                                        /* uses linebuf, genbuf, spend */
{
    register char       *lp;
    register char       *sp;
    register char const *rp;
    int                 c;
                                        /* linebuf upto location 1 -> genbuf */
    lp = linebuf;
    sp = genbuf;
    while( lp < loc1 ) {
        if( sp >= genbuf + sizeof genbuf ) { /* Not exercised by sedtest.mak */
            fprintf( stderr, NOROOM, sizeof genbuf, lnum );
            break;
        }
        *sp++ = *lp++;
    }

    for( rp = rhsbuf; ( c = *rp++ ) != 0; ) {
        if( c == '&' ) {
            sp = place( sp, loc1, loc2 );
        } else if( c & 0200 && ( c &= 0177 ) >= '1' && c < MAXTAGS + '1' ) {
            sp = place( sp, brastart[c - '0'], bracend[c - '0'] );
        } else {
            if( sp >= genbuf + sizeof genbuf ) { /* Not exercised by sedtest.mak */
                fprintf( stderr, NOROOM, sizeof genbuf, lnum );
                break;
            }
            *sp++ = c & 0177;
        }
    }
    lp = loc2;
    loc2 = sp - ( genbuf - linebuf );   /* Last character to remove */
    do{
        if( sp >= genbuf + sizeof genbuf ) { /* Not exercised by sedtest.mak */
            fprintf( stderr, NOROOM, sizeof genbuf, lnum );
            break;
        }
    } while( ( *sp++ = *lp++ ) != 0 );
    lp = linebuf;
    sp = genbuf;
    while( ( *lp++ = *sp++ ) != 0 ) ;
    spend = lp - 1;
}

/* place chars at *al1...*(al1 - 1) at asp... in genbuf[] */
static char *place(
    register char       *asp,
    register char const *al1,
    register char const *al2 )
{
    while( al1 < al2 ) {
        if( asp >= genbuf + sizeof genbuf ) { /* Not exercised by sedtest.mak */
            fprintf( stderr, NOROOM, sizeof genbuf, lnum );
            break;
        }
        *asp++ = *al1++;
    }
    return( asp );
}

/* write a hex dump expansion of *p1... to fp */
static void listto(
    register char const *p1,            /* the source */
    FILE                *fp )           /* output stream to write to */
{
    int const   linesize = 64;
    int         written = 0;

    p1--;
    while( *++p1 ) {
        if( ++written >= linesize )
            fprintf( fp, "%c\n", '\\' ), written = 1;
        if( *p1 == '\\' )
            putc( *p1, fp ), putc( *p1, fp ), written++; /* Double literal backslash */
        else if( *p1 == '\n' || isprint( *p1 ) )
            putc( *p1, fp );            /* pass it through */
        else {
            written++;
            putc( '\\', fp );           /* emit a backslash */
            switch( *p1 ) {
            case '\a':
                putc( 'a', fp );
                break;
            case '\b':
                putc( 'b', fp );
                break;
            case '\f':
                putc( 'f', fp );
                break;
            case '\n':                  /* Never activated */
                putc( 'n', fp );
                break;
            case '\r':
                putc( 'r', fp );
                break;
            case '\t':
                putc( 't', fp );
                break;
            case '\v':
                putc( 'v', fp );
                break;
            default:
                written++;
                fprintf( fp, "%02x", *p1 & 0xFF );
            }
        }
    }
    putc( '$', fp );
    putc( '\n', fp );
}

/* execute compiled command pointed at by ipc */
static void command( sedcmd *ipc )
{
    static int      didsub;             /* true if last s succeeded */
    static char     holdsp[MAXHOLD];    /* the hold space */
    static char     *hspend = holdsp;   /* hold space end pointer */
    register char   *p1;
    register char   *p2;
    char            *execp;

    switch( ipc->command ) {
    case ACMD:                          /* append */
        if( aptr >= appends + MAXAPPENDS ) /* Not exercised by sedtest.mak */
            fprintf( stderr, "sed: too many appends after line %ld\n", lnum );
        *aptr++ = ipc;
        break;

    case CCMD:                          /* change pattern space */
        delete = TRUE;
        if( !ipc->flags.inrange || lastline )
            printf( "%s\n", ipc->u.lhs );
        break;

    case DCMD:                          /* delete pattern space */
        delete++;
        break;

    case CDCMD:                         /* delete a line in pattern space */
        p1 = p2 = linebuf;
        while( *p1 && *p1 != '\n' ) p1++;
        if( !*p1++ )
            return;
        while( ( *p2++ = *p1++ ) != 0 ) ;
        spend = p2 - 1;
        delete = jump = TRUE;
        break;

    case EQCMD:                         /* show current line number */
        fprintf( stdout, "%ld\n", lnum );
        break;

    case GCMD:                          /* copy hold space to pattern space */
        p1 = linebuf;
        p2 = holdsp;
        while( ( *p1++ = *p2++ ) != 0 ) ;
        spend = p1 - 1;
        break;

    case CGCMD:                         /* append hold space to pattern space */
        *spend++ = '\n';
        p1 = spend;
        p2 = holdsp;
        while( ( *p1++ = *p2++ ) != 0 )
            if( p1 >= linebuf + MAXBUF ) {
                fprintf( stderr, NOROOM, MAXBUF, lnum );
                break;
            }
        spend = p1 - 1;
        break;

    case HCMD:                          /* copy pattern space to hold space */
        p1 = holdsp;
        p2 = linebuf;
        while( ( *p1++ = *p2++ ) != 0 ) ;
        hspend = p1 - 1;
        break;

    case CHCMD:                         /* append pattern space to hold space */
        *hspend++ = '\n';
        p1 = hspend;
        p2 = linebuf;
        while( ( *p1++ = *p2++ ) != 0 )
            if( p1 >= holdsp + MAXBUF ) {
                fprintf( stderr, NOROOM, MAXBUF, lnum );
                break;
            }
        hspend = p1 - 1;
        break;

    case ICMD:                          /* insert text */
        printf( "%s\n", ipc->u.lhs );
        break;

    case BCMD:                          /* branch to label */
        jump = TRUE;
        break;

    case LCMD:                          /* list text */
        listto( linebuf, ( ipc->fout != NULL ) ? ipc->fout : stdout );
        break;

    case NCMD:                          /* read next line into pattern space */
        if( !nflag )
            puts( linebuf );            /* flush out the current line */
        readout();                      /* do any pending a, r commands */
        if( ( execp = getline( linebuf ) ) == NULL ) {
            pending = ipc;
            delete = TRUE;
            break;
        }
        spend = execp;
        break;

    case CNCMD:                         /* append next line to pattern space */
        readout();                      /* do any pending a, r commands */
        *spend++ = '\n';                /* seperate lines with '\n' */
        if( ( execp = getline( spend ) ) == NULL ) {
            *--spend = '\0';            /* Remove '\n' added for new line */
            pending = ipc;
            delete = TRUE;
            break;
        }
        spend = execp;
        break;

    case PCMD:                          /* print pattern space */
        puts( linebuf );
        break;

    case CPCMD:                         /* print one line from pattern space */
        for( p1 = linebuf; *p1 != '\n' && *p1 != '\0'; p1++ )
            putc( *p1, stdout );
        putc( '\n', stdout );
        break;

    case QCMD:                          /* quit the stream editor */
        if( !nflag )
            puts( linebuf );            /* flush out the current line */
        readout();                      /* do any pending a and r commands */
        exit( 0 );

    case RCMD:                          /* read a file into the stream */
        if( aptr >= appends + MAXAPPENDS ) /* Not exercised by sedtest.mak */
            fprintf( stderr, "sed: too many reads after line %ld\n", lnum );
        *aptr++ = ipc;
        break;

    case SCMD:                          /* substitute RE */
        didsub = substitute( ipc );
        if( didsub ) {
            switch( ipc->flags.print ) {
            case 1:
                puts( linebuf );
                break;
            case 2:
                for( p1 = linebuf; *p1 != '\n' && *p1 != '\0'; p1++ )
                    putc( *p1, stdout );
                putc( '\n', stdout );
                break;
            }
            if( ipc->fout )
                fprintf( ipc->fout, "%s\n", linebuf );
        }
        break;

    case TCMD:                          /* branch on last s successful */
    case CTCMD:                         /* branch on last s failed */
        if( didsub == ( ipc->command == CTCMD ) )
            break;                      /* no branch if last s failed, else */
        didsub = FALSE;
        jump = TRUE;                    /*  set up to jump to assoc'd label */
        break;

    case CWCMD:                         /* write one line from pattern space */
        for( p1 = linebuf; *p1 != '\n' && *p1 != '\0'; )
            putc( *p1, ipc->fout ), p1++;
        putc( '\n', ipc->fout );
        break;

    case WCMD:                          /* write pattern space to file */
        fprintf( ipc->fout, "%s\n", linebuf );
        break;

    case XCMD:                          /* exchange pattern and hold spaces */
        p1 = linebuf;
        p2 = genbuf;
        while( ( *p2++ = *p1++ ) != 0 ) ;
        p1 = holdsp;
        p2 = linebuf;
        while( ( *p2++ = *p1++ ) != 0 ) ;
        spend = p2 - 1;
        p1 = genbuf;
        p2 = holdsp;
        while( ( *p2++ = *p1++ ) != 0 ) ;
        hspend = p2 - 1;
        break;

    case YCMD:
        p1 = linebuf;
        p2 = ipc->u.lhs;
        while( ( *p1 = p2[*(unsigned char *)p1] ) != 0 )
            p1++;
        break;

    default: /* Can never happen */
        fprintf( stderr, INTERR, "unrecognised command" );
    }
}

/* get next line of text to be filtered */
static char *getline( register char *buf )  /* where to send the input */
{
    static char const * const   linebufend = linebuf + MAXBUF + 2;
    int const                   room = linebufend - buf;
    int                         temp;
    char const * const          sbuf = buf;

    assert( buf >= linebuf && buf < linebufend );

    /* The OW fgets has some strange behavior:
     * 0 on input is not treated specially. sed ignores the rest of the line.
     * 26 (^Z) stops reading the current line and is stripped.
     */
    if (fgets(buf, room, stdin) != NULL) { /* gets() can smash program - WFB */
        lnum++;                         /* note that we got another line */
        while( ( *buf++ ) != 0 ) ;      /* find the end of the input */
        if( buf-- - sbuf >= room )
            fprintf( stderr, NOROOM, room, lnum ), buf++;
        if( *--buf != '\n' ) buf++;
        *buf = 0;
        if( eargc == 0 ) {              /* if no more args */
            lastline = ( ( temp = getc( stdin ) ) == EOF );
            (void)ungetc( temp, stdin );
        }
        return( buf );                  /* return ptr to terminating null */
    } else {
        if( eargc == 0 )                /* if no more args */
            lastline = TRUE;            /*    set a flag */
        return( NULL );
    }
}

/* return TRUE if *a... == *b... for count chars, FALSE otherwise */
static int memeql(
    register char const *a,
    register char const *b,
    int                 count )
{
    while( count-- )                    /* look at count characters */
        if( *a++ != *b++ )              /* if any are nonequal   */
            return( FALSE );            /*    return FALSE for false */
    return( TRUE );                     /* compare succeeded */
}

/* write file indicated by r command to output */
static void readout( void )
{
    register int        t;              /* hold input char or EOF */
    FILE                *fi;            /* ptr to file to be read */
    sedcmd * const      *ap;            /* Loops through appends */

    for( ap = appends; ap < aptr; ap ++ ) {
        sedcmd const * const    a = *ap;
        char const * const      lhs = a->u.lhs;

        if( a->command == ACMD )        /* process "a" cmd */
            printf( "%s\n", lhs );
        else {                          /* process "r" cmd */
            if( ( fi = fopen( lhs, "r" ) ) != NULL ) {
                while( ( t = getc( fi ) ) != EOF )
                    putc( ( char ) t, stdout );
                fclose( fi );
            }
        }
    }

    aptr = appends;                     /* reset the append ptr */
}

/* sed.c ends here */
