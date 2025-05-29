/* sedcomp.c -- stream editor main and compilation phase

   The stream editor compiles its command input  (from files or -e options)
into an internal form using compile() then executes the compiled form using
execute(). Main() just initializes data structures, interprets command line
options, and calls compile() and execute() in appropriate sequence.
   The data structure produced by compile() is an array of compiled-command
structures (type sedcmd).  These contain several pointers into pool[], the
regular-expression and text-data pool, plus a command code and g & p flags.
In the special case that the command is a label the struct  will hold a ptr
into the labels array labels[] during most of the compile,  until resolve()
resolves references at the end.
   The operation of execute() is described in its source module.

==== Written for the GNU operating system by Eric S. Raymond ==== */

#include <assert.h>
#include <ctype.h>                      /* isdigit(), isspace() */
#include <stdio.h>                      /* uses getc, fprintf, fopen, fclose */
#include <stdlib.h>                     /* uses exit */
#include <string.h>                     /* imported string functions */
#include "wio.h"
#include "bool.h"
#include "sed.h"                        /* command type struct & name defines */

#define POOLSIZE        10000           /* size of string-pool space */
#define WFILES          10              /* max number of w output files */
#define RELIMIT         256             /* max chars in compiled RE */
#define MAXDEPTH        20              /* maximum {}-nesting level */
#define MAXLABS         50              /* max number of labels */

#define SKIPWS(pc)      while( isspace( *pc ) ) pc++
#define ABORT(msg)      fprintf( stderr, msg, linebuf ), myexit( 2 )

typedef struct                          /* represent a command label */
{
    char        *name;                  /* the label name */
    sedcmd      *last;                  /* it's on the label search list */
    sedcmd      *link;                  /* pointer to the cmd it labels */
}               label;

                                        /* main data areas */
char            *linebuf = NULL;        /* current-line buffer */
sedcmd          cmds[MAXCMDS + 1];      /* hold compiled commands */
long            linenum[MAXLINES];      /* numeric-addresses table */

                                        /* miscellaneous shared variables */
bool            nflag = false;          /* -n option flag */
int             eargc;                  /* scratch copy of argument count */
unsigned char const      bits[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

                                        /***** module common stuff *****/

                                        /* error messages */
static char const       AGMSG[] = "sed: garbled address %s\n";
static char const       CGMSG[] = "sed: garbled command %s\n";
static char const       TMTXT[] = "sed: too much text: %s\n";
static char const       AD1NG[] = "sed: no addresses allowed for %s\n";
static char const       AD2NG[] = "sed: only one address allowed for %s\n";
static char const       TMCDS[] = "sed: too many commands, last was %s\n";
static char const       COCFI[] = "sed: cannot open command-file %s\n";
static char const       UFLAG[] = "sed: unknown flag %c\n";
static char const       CCOFI[] = "sed: cannot create %s\n";
static char const       ULABL[] = "sed: undefined label \":%s\"\n";
static char const       TMLBR[] = "sed: too many {'s\n";
static char const       NSCAX[] = "sed: no such command as %s\n";
static char const       TMRBR[] = "sed: too many }'s\n";
static char const       DLABL[] = "sed: duplicate label \"%s\"\n";
static char const       TMLAB[] = "sed: too many labels \"%s\"\n";
static char const       TMWFI[] = "sed: too many w files\n";
static char const       REITL[] = "sed: RE too long: %s\n";
static char const       TMLNR[] = "sed: too many line numbers\n";
static char const       TRAIL[] = "sed: command \"%s\" has trailing garbage\n";
static char const       NEEDB[] = "sed: error processing: %s\n";
static char const       INERR[] = "sed: internal error: %s\n";
static char const       SMCNT[] = "sed: bad value for match count on s command %s\n";
static char const       UNCLS[] = "sed: invalid character class name %s\n";
static char const       NOMEM[] = "sed: no memory for input buffer\n";
static char const       *USAGE[] = {
    "Usage: sed [-g] [-n] script file ...",
    "       sed [-g] [-n] -e script ... -f script_file ... file ...",
    "",
    "  -g: global substitute             -n: no default copy,",
    "  script: append to commands,       script_file: append contents to commands",
    "  file: a text file - lines of upto 8K bytes, \\n not needed on last line",
    "        default is to read from standard input",
    "",
    "General command format:",
    "    [addr1][,addr2] [!]function [arguments]",
    "    [addr1][,addr2] [!]s/[RE]/replace/[p|P][g][w file]",
    "",
    "Command list:",
    "    a: append,      b: branch,    c: change,   d/D: delete,",
    "    g/G: get,       h/H: hold,    i: insert,   l: list,",
    "    n/N: next,      p/P: print,   q: quit,     r: read,",
    "    s: substitute,  t/T: test,    w/W: write,  x: xchange,",
    "    y: translate,   :label,       =: number,   {}: grouping",
    "",
    "Regular expression elements:",
    "    \\n, and \\t: newline, and tab",
    "    ^: line begin,             $: line end,",
    "    [a-z$0-9]: class,          [^a-z$]: inverse-class,",
    "    .: one character,          *: 0 or more repeats,",
    "    +: one or more repeats,    \\{n,m\\} n to m repeats,",
    "    \\(..\\): subexpression,     \\1..\\9: matched subexpression,",
    "",
    "Substitution patterns:",
    "    \\n,\\t: newline and tab",
    "    &: match pattern,          \\1..\\9: matched subexpression",
    NULL
};

                                        /* label handling */
static label        labels[MAXLABS];        /* here's the label table */
                                            /* first label is end of script */
static label        *curlab = labels + 1;   /* pointer to current label */
static label        *lablst = labels;       /* header for search list */

                                        /* string pool for REs, etc. */
static char         pool[POOLSIZE];         /* the pool */
static char         *poolcurr = pool;       /* current pool pointer */
static char         *poolend = pool + POOLSIZE; /* pointer past pool end */

                                        /* compilation state */
static FILE         *cmdf   = NULL;         /* current command source */
static char         *cp     = NULL;         /* compile pointer */
static sedcmd       *cmdp   = cmds;         /* current compiled-cmd ptr */
static int          bdepth  = 0;            /* current {}-nesting level */
static int          bcount  = 0;            /* # tagged patterns in current RE */
static char         **eargv;                /* scratch copy of argument list */

                                        /* compilation flags */
static int          eflag = 0;              /* -e option flag */
static bool         gflag = false;          /* -g option flag */

static char const   *fname[WFILES];         /* w file name pointers */
static FILE         *fout[WFILES];          /* w file stream ptrs */
static int          nwfiles = 0;            /* count of open w files */

#if defined( __WATCOMC__ ) || defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L )
#define my_isblank  isblank
#else
static int my_isblank( int c )
{
    return( c == ' ' || c == '\t' );
}
#endif

/* Avoid race condition with calls like echo hello | fail */
static void myexit( int status )
{
    assert( status != 0 );              /* Call only needed for failures */
    if( !isatty( fileno( stdin ) ) ) {
        while( fgets( linebuf, MAXBUF, stdin ) != NULL ) {
            ;
        }
    }
    exit( status );
}

/*
 * accept multiline input from *cp... to pool ,
 * optionally skipping leading whitespace
 */
static void gettext( int accept_whitespace )
{
    char        c;

    if( !accept_whitespace )
        SKIPWS( cp );                   /* discard whitespace */
    while( poolcurr < poolend && (c = *cp++) != '\0' ) {
        switch( c ) {
        case '\\':                      /* handle escapes */
            c = *cp++;
            break;
        case '\n':                      /* SKIPWS after newline */
            SKIPWS( cp );
            break;
        }
        *poolcurr++ = c;
    }
    if( poolcurr >= poolend )
        ABORT( TMTXT );                 /* Not exercised by sedtest.mak */
    --cp;
    return;
}

static void outfiles_init( void )
{
    fout[0] = stdout;
    fname[0] = "";
    nwfiles = 1;
}

static bool outfiles_open( void )
{
    int     i;

    if( nwfiles >= WFILES )
        ABORT( TMWFI );
    fname[nwfiles] = (const char *)poolcurr; /* filename is in pool */
    gettext( 0 );
    /* match it in table */
    for( i = nwfiles; i-- > 0; ) {
        if( strcmp( fname[nwfiles], fname[i] ) == 0 ) {
            cmdp->fout = fout[i];
            return( true );
        }
    }
    /* if didn't find one, open new file */
    cmdp->fout = fopen( fname[nwfiles], "w" );
    if( cmdp->fout == NULL ) {
        fprintf( stderr, CCOFI, fname[nwfiles] );
        myexit( 2 );
    }
#ifndef _MSC_VER
    if( setvbuf( cmdp->fout, NULL, _IOLBF, 0 ) ) {
        fprintf( stderr, CCOFI, fname[nwfiles] );
        myexit( 2 );
    }
#endif
    fout[nwfiles++] = cmdp->fout;
    return( false );
}

static void outfiles_fini( void )
{
    int     i;

    fflush( stdout );
    for( i = 1; i < nwfiles; i++ ) {
        if( fout[i] != NULL ) {
            fclose( fout[i] );
            fout[i] = NULL;
        }
    }
}

/* compile a regular expression to internal form */
static char *recomp(
    char            *expbuf,            /* place to compile it to */
    char            redelim )           /* RE end-marker to look for */
                                        /* uses cp, bcount */
{
    char            *ep = expbuf;       /* current-compiled-char pointer */
    char            *sp = cp;           /* source-character ptr */
    char            c;                  /* current-character */
    char            negclass;           /* all-but flag */
    char            *lastep;            /* ptr to last expr compiled */
    char const      *svclass;           /* start of current char class */
    char            brnest[MAXTAGS+1];  /* bracket-nesting array */
    char            *brnestp;           /* ptr to current bracket-nest */
    char const      *pp;                /* scratch pointer */
    int             tags;               /* # of closed tags */
    char            *obr[MAXTAGS+1] = {0}; /* ep values when \( seen */
    int             opentags = 0;       /* Used to index obr */
    int             i;
    char            tagindex;
    int             ilow;
    int             ihigh;

    if( *cp == redelim ) {              /* if first char is RE endmarker */
        cp++;
        *ep++ = CEOF;
        return( ep );                   /* use existing RE. */
    }
    lastep = NULL;                      /* there's no previous RE */
    brnestp = brnest;                   /* initialize ptr to brnest array */
    tags = bcount = 0;                  /* initialize counters */

    if( *sp == '^' ) {                  /* check for start-of-line syntax */
        *ep++ = 1;
        ++sp;
    } else {
        *ep++ = 0;
    }
    for( ;; ) {
        if( ep >= expbuf + RELIMIT ) {  /* match is too large */
            cp = sp;
            return( BAD );              /* Not exercised by sedtest.mak */
        }
        c = *sp++;
        if( c == redelim ) {            /* found the end of the RE */
            cp = sp;
            if( brnestp != brnest )     /* \(, \) unbalanced */
                return( BAD );
            *ep++ = CEOF;               /* write end-of-pattern mark */
            return( ep );               /* return ptr to compiled RE */
        }
        if( !( c == '*' || c == '+' || c == '\\' && *sp == '{' ) ) {
                                        /* if we aren't a postfix op */
            lastep = ep;                /* get ready to match last */
        }
        switch( c ) {
        case '\\':
            c = *sp++;
            switch( c ) {
            case '(':                   /* start tagged section */
                if( ++bcount <= MAXTAGS ) { /* bump tag count */
                    *brnestp++ = (char)bcount; /* update tag stack */
                    obr[opentags] = ep; /* Remember for /(.../)* */
                }
                tagindex = (char)bcount;
                opentags++;
                *ep++ = CBRA;           /* enter tag-start */
                *ep++ = tagindex;
                break;
            case ')':                   /* end tagged section */
                if( --opentags < 0 ) {  /* extra \) */
                    cp = sp;
                    return( BAD );
                }
                tagindex = 0;
                if( ++tags <= MAXTAGS ) /* count closed tags */
                    tagindex = *--brnestp; /* pop tag stack */
                *ep++ = CKET;           /* enter end-of-tag */
                *ep++ = tagindex;
                break;
            case '\0':
            case '\n':                  /* escaped newline no good */
                cp = sp;
                return( BAD );
            case 'n':                   /* match a newline */
            case 't':                   /* match a tab */
            case '\\':                  /* match a literal backslash */
                switch( c ) {
                case 'n':               /* match a newline */
                    c = '\n';
                    break;
                case 't':               /* match a tab */
                    c = '\t';
                    break;
                }
                *ep++ = CCHR;           /* insert character mark */
                *ep++ = c;
                break;
            case '1':                   /* tag use */
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                tagindex = c - '0';
                if( tagindex > tags )   /* too few */
                    return( BAD );
                *ep++ = CBACK;          /* enter tag mark */
                *ep++ = tagindex;       /* and the number */
                break;
            case '{':
                ilow = 0;
                ihigh = 0;
                if( lastep == NULL ) {
                    cp = sp;
                    return( BAD );  /* rep error */
                }
                *lastep |= MTYPE;
                if( !isdigit( *sp ) ) {
                    cp = sp;
                    return( BAD );
                }
                while( isdigit( *sp ) )
                    ilow = ilow * 10 + *sp++ - '0';
                if( ilow > 255 ) {
                    cp = sp;
                    return( BAD );
                }
                *ep++ = (char)ilow;
                if( sp[0] == '\\' && sp[1] == '}' ) {
                    sp += 2;
                    *ep++ = 0;
                } else if( sp[0] == ',' && sp[1] == '\\' && sp[2] == '}' ) {
                    sp += 3;
                    *ep++ = '\xFF';
                } else if( *sp++ == ',' ) {
                    if( !isdigit( *sp ) ) {
                        *ep++ = '\xFF';
                    } else {
                        while( isdigit( *sp ) )
                            ihigh = ihigh * 10 + *sp++ - '0';
                        *ep++ = (char)( ihigh - ilow );
                    }
                    if( sp[0] != '\\' || sp[1] != '}' || ihigh < ilow || ihigh > 255 ) {
                        cp = sp;
                        return( BAD );
                    }
                    sp += 2;
                } else {
                    cp = sp;
                    return( BAD );
                }
                break;
            default:
#if 1
                /* This allows \ to stop "special" even if it is not. */
                *ep++ = CCHR;           /* insert character mark */
                *ep++ = c;
                break;
#else
                /* This is IEEE 2001 behavior */
                cp = sp;
                return( BAD ); /* Otherwise /\?/ && /\\?/ synonymous */
#endif
            }
            break;

        case '\n':                      /* Can not happen? WFB 20040801 */
        case '\0':
            cp = sp;
            return( BAD );              /* incomplete regular expression */

        case '.':                       /* match any char except newline */
            *ep++ = CDOT;
            break;

        case '+':
        case '*':
            /* '*' is 0 to any repeats of previous pattern
             * '+' is 1 to any repeats of previous pattern and is converted
             * to single pattern and '*' pattern
             */
            if( lastep == NULL ) {      /* if '+' or '*' first on line match a literal '+' or '*' */
                *ep++ = CCHR;           /* insert character mark */
                *ep++ = c;
                break;
            }
#if 0                                   /* Removed constraint WFB 20040804 */
            if( *lastep == CKET ) {     /* can't iterate a tag */
                cp = sp;
                return( BAD );
            }
#endif
            if( c == '+' ) {            /* if '+' then convert to single and '*' patterns */
                pp = ep;            /* duplicate previous pattern */
                while( lastep < pp ) {
                    *ep++ = *lastep++;
                }
            }
            *lastep |= STAR;            /* flag the pattern with '*' */
            break;

        case '$':                       /* match only end-of-line */
            if( *sp != redelim ) {      /* if we're not at end of RE match a literal $ */
                *ep++ = CCHR;           /* insert character mark */
                *ep++ = c;
                break;
            }
            *ep++ = CDOL;               /* insert end-symbol mark */
            break;

        case '[':                       /* begin character set pattern */
            if( ep + CHARSETSIZE + 1 >= expbuf + RELIMIT )
                ABORT( REITL );         /* Not exercised by sedtest.mak */
            *ep++ = CCL;                /* insert class mark */
            c = *sp++;
            negclass = ( c == '^' );
            if( negclass )
                c = *sp++;
            svclass = sp;               /* save ptr to class start */
            do {
                switch( c ) {
                case '\0':
                    ABORT( CGMSG );
                case '-':               /* handle character ranges */
                    if( sp > svclass && *sp != ']' ) {
                        int     hi = *(unsigned char *)sp;

                        i = *(unsigned char *)( sp - 2 );
                        if( i > hi )
                            ABORT( CGMSG );
                        if( sp[1] == '-' && sp[2] != ']' )
                            ABORT( CGMSG );
                        for( ; i <= hi; i++ )
                            SETCHARSET( ep, i );
                        continue;
                    }
                    break;
                case '\\':              /* handle escape sequences in sets */
                    c = *sp++;
                    if( c == 'n' ) {
                        c = '\n';
                    } else if( c == 't' ) {
                        c = '\t';
                    } else {
                        --sp;
                        c = '\\';       /* \n and \t are special, \* is not */
                    }
                    break;
                case '[':               /* Handle named character class */
                    if( *sp == ':' ) {
                        static const struct {
                            const char *s;
                            int ( *isf )( int c );
                        } t[] = {
                            { "alnum:]", isalnum },
                            { "alpha:]", isalpha },
                            { "blank:]", my_isblank },
                            { "cntrl:]", iscntrl },
                            { "digit:]", isdigit },
                            { "graph:]", isgraph },
                            { "lower:]", islower },
                            { "print:]", isprint },
                            { "punct:]", ispunct },
                            { "space:]", isspace },
                            { "upper:]", isupper },
                            { "xdigit:]", isxdigit },
                        };
                        int ( *isf )( int c );

                        for( i = 0; i < sizeof( t ) / sizeof( *t ); i++ ) {
                            if( strncmp( sp + 1, t[i].s, strlen( t[i].s ) ) == 0 ) {
                                break;
                            }
                        }
                        if( i >= sizeof( t ) / sizeof( *t ) ) /* Add class */
                            ABORT( UNCLS );
                        sp += 1 + strlen( t[i].s );
                        isf = t[i].isf;
                        for( i = 1; i < CHARSETSIZE * 8; i++ ) {
                            if( isf( i ) ) {
                                SETCHARSET( ep, i );
                            }
                        }
                        continue;
                    }
                    break;
                }
                /* add (maybe translated) char to set */
                SETCHARSET( ep, c );
            } while( (c = *sp++) != ']' );
            /* invert bitmask if all-but needed */
            if( negclass ) {
                for( i = 0; i < CHARSETSIZE; i++ ) {
                    ep[i] ^= 0xFF;
                }
            }
            ep[0] &= 0xFE;              /* never match ASCII 0 */
            ep += CHARSETSIZE;          /* advance ep past set mask */
            break;

        default:                        /* which is what we'd do by default */
            *ep++ = CCHR;               /* insert character mark */
            *ep++ = c;
            break;

        } /* switch( c ) */

        /* post processing for '*' or \{m,n\} */
        if( *lastep & (STAR | MTYPE) ) {
            switch( *lastep & ~(STAR | MTYPE) ) {
            case CCHR:
            case CDOT:
            case CCL:
                break;
            case CBRA:
                cp = sp;
                return( BAD );
            case CKET:
                {   /* Make room to insert skip marker in expbuf */
                    char * const    firstep = obr[opentags];
                    int             width = (int)( ep - firstep );

                    if( width >= 256 )
                        ABORT( REITL ); /* Not exercised by sedtest.mak */
                    *firstep |= ( *lastep ^ CKET ) ; /* Mark \( as * or \{ terminated */
                    i = width;
                    while( --i >= 2 )
                        firstep[i + 1] = firstep[i];
                    firstep[2] = (char)( width - 1 );
                    lastep++;
                    ep++;
                }
                break;
            case CBACK:
                break;
            case CEOF: /* Can't happen - would require * after end of expression WFB 20040813 */
            case CNL:  /* Can't happen - * is literal after special ^ */
            case CDOL: /* Can't happen - $ is literal if not last */
            case CLNUM:/* Can't happen - * after line number is nonsense */
            case CEND: /* Can't happen - CEND is always followed by CEOF */
            default:
                fprintf( stderr, INERR, "Unexpected symbol in RE" );
                myexit( 2 );
            }
        }
    } /* for( ;; ) */
}

/* read next command from -e argument or command file */
static bool cmdline( char *cbuf )        /* uses eflag, eargc, cmdf */
{
    int        inc;                     /* not char because must hold EOF */

    assert( cbuf == cp );
    cbuf--;                             /* so pre-increment points us at cbuf */

                                        /* e command flag is on */
    if( eflag ) {
        char            *p;             /* ptr to current -e argument */
        static char     *savep;         /* saves previous value of p */

        if( eflag > 0 ) {               /* there are pending -e arguments */
            eflag = -1;
            if( --eargc <= 0 ) {        /* barf if no argument */
                fprintf( stderr, NEEDB, eargv[0] );
                myexit( 2 );
            }
                                        /* else copy next e argument to cbuf */
            p = *++eargv;
            while( (*++cbuf = *p++) != '\0' ) {
                if( *cbuf == '\\' ) {   /* Could not sedtest this! WFB 20040802 */
                    if( (*++cbuf = *p++) == '\0' ) {
                        if( --eargc <= 0 ) { /* barf if no argument */
                            fprintf( stderr, NEEDB, eargv[0] );
                            myexit( 2 );
                        }
                        *cbuf = '\n';
                        p = *++eargv;
                    }
                } else if( *cbuf == '\n' ) { /* end of 1 cmd line */
                    *cbuf = '\0';
                    savep = p;
                    return( true );
                                        /* we'll be back for the rest... */
                }
            }
                                        /* have string-end;
                                         * can advance to next argument */
            savep = NULL;
            return( true );
        }

        if( (p = savep) == NULL )
            return( false );

        while( (*++cbuf = *p++) != '\0' ) {
            if( *cbuf == '\\' ) {
                if( (*++cbuf = *p++) == '0' ) {
                    savep = NULL;
                    return( false );
                }
            } else if( *cbuf == '\n' ) {
                *cbuf = '\0';
                savep = p;
                return( true );
            }
        }
        savep = NULL;
        return( true );
    }

                                        /* if no -e flag
                                         * read from command file descriptor */
    while( (inc = getc( cmdf )) != EOF ) { /* get next char */
        switch( *++cbuf = (char)inc ) {
        case '\\':                      /* if it's escape */
            inc = getc( cmdf );         /* get next char */
            *++cbuf = (char)inc;
            break;
        case '\n':                      /* end on newline */
            *cbuf = '\0';
            return( true );                /* cap the string */
        }
    }
    *++cbuf = '\0';
    return( cbuf >= cp + 1 );           /* end-of-file, no more chars */
}

/* expand an address at *cp... into expbuf, return ptr at following char */
static char *getaddress( char *expbuf ) /* uses cp, linenum */
{
    static int      numl = 0;           /* current ind in addr-number table */
    char            *rcp;               /* temp compile ptr for forwd look */
    long            lno;                /* computed value of numeric address */

    switch( *cp ) {
    case '$':                           /* end-of-source address */
        *expbuf++ = CEND;               /* write symbolic end address */
        *expbuf++ = CEOF;               /* and the end-of-address mark (!) */
        cp++;                           /* go to next source character */
        return( expbuf );               /* we're done */
    case '\\':                          /* posix \cBREc address */
        cp++;                           /* Point to delimiter */
        /* fall through */
    case '/':                           /* start of regular-expression match */
        return( recomp( expbuf, *cp++ ) ); /* compile the RE */
    }
    rcp = cp;
    lno = 0;                            /* now handle a numeric address */
    while( isdigit( *rcp ) )            /* collect digits */
        lno = lno * 10 + *rcp++ - '0';  /* compute their value */

    if( rcp > cp ) {                    /* if we caught a number... */
        *expbuf++ = CLNUM;              /* put a numeric-address marker */
        *expbuf++ = (char)numl;         /* and the address table index */
        linenum[numl++] = lno;          /* and set the table entry */
                                        /* Not exercised by sedtest.mak */
        if( numl >= MAXLINES )          /* oh-oh, address table overflow */
            ABORT( TMLNR );             /*   abort with error message */
        *expbuf++ = CEOF;               /* write the end-of-address marker */
        cp = rcp;                       /* point compile past the address */
        return( expbuf );               /* we're done */
    }

    return( NULL );                     /* no legal address was found */
}

/* find the label matching *ptr, return NULL if none */
static label *search( void )            /* uses globals lablst and curlab */
{
    label      *rp;
    char const * const  name = curlab->name;
    for( rp = lablst; rp < curlab; rp++ ) {
        if( strcmp( rp->name, name ) == 0 ) {
            return( rp );
        }
    }
    return( NULL );
}

/* write label links into the compiled-command space */
static void resolve( void )             /* uses global lablst */
{
    label const        *rp;
    sedcmd             *rptr;
    sedcmd             *trptr;
                                        /* loop through the label table */
    for( rp = lablst; rp < curlab; rp++ ) {
        if( rp->link == NULL ) {        /* barf if not defined */
            fprintf( stderr, ULABL, rp->name );
            myexit( 2 );
        } else if( rp->last != NULL ) { /* if last is non-null */
            for( rptr = rp->last; (trptr = rptr->u.link) != NULL; rptr = trptr )
                rptr->u.link = rp->link;
            rptr->u.link = rp->link - 1;
        }
    }
}

/* compile a y (transliterate) command */
static char *ycomp(
    char        *ep,            /* where to compile to */
    char        delim )         /* end delimiter to look for */
{
    char       c;
    char       *tp;
    char const *sp;
    int        index;

    if( delim == '\0' || delim == '\\' || delim == '\n' )
        return( BAD );
    /* scan 'from' for invalid chars */
    for( sp = tp = cp; (c = *tp++) != delim; ) {
        if( c == '\\' ) {
            c = *tp++;
        }
        if( ( c == '\n' ) || ( c == '\0' ) ) {
            return( BAD );
        }
    }
    /* tp points at first char of 'to' after delim */
    /* now rescan the 'from' section */
    while( (c = *sp++) != delim ) {
        if( c == '\\' ) {
            c = *sp++;
            switch( c ) {
            case 'n':                   /* '\\''n' -> '\n' */
                c = '\n';
                break;
            case '\\':
                break;
            default:
                if( c != delim )
                    return( BAD );
                break;
            }
        }
        index = (unsigned char)c;
        if( ep[index] != 0 )
            return( BAD );              /* c has already been mapped */
        c = *tp++;
        if( c == '\\' ) {
            c = *tp++;
            switch( c ) {
            case 'n':                   /* '\\''n' -> '\n' */
                c = '\n';
                break;
            case '\\':
                break;
            default:
                if( c != delim )
                    return( BAD );
                break;
            }
        }
        if( c == '\0' ) {
            return( BAD );
        }
        ep[index] = c;
    }

    if( *tp != delim )                  /* 'to', 'from' lengths unequal */
        return( BAD );

    cp = ++tp;                          /* point compile ptr past translit */

    /* fill in self-map entries in table */
    for( index = 0; index < CHARSETSIZE * 8; index++ ) {
        if( ep[index] == 0 ) {
            ep[index] = (char)index;
        }
    }

    return( ep + CHARSETSIZE * 8 );     /* first free location past table end */
}

/* generate replacement string for substitute command right hand side */
static char *rhscomp(
    char       *rhsp,                   /* place to compile expression to */
    char       delim )                  /* RE end-mark to find */
                                        /* uses bcount */
{
    char       c;

    for( ;; ) {
        c = *cp++;
        if( c == '\\' ) {           /* copy; if it's a \, */
            c = *cp++;
            if( c > bcount - 1 + '1' && c <= '9' ) /* check validity of pattern tag */
                return( BAD );
            if( c >= '1' && c <= '9' ) {
                c |= 0x80;          /* mark the good ones */
            }
        } else if( c == '&' ) {
            c |= 0x80;              /* mark the good ones */
        } else if( c == delim ) {   /* found RE end, hooray... */
            *rhsp++ = '\0';         /* cap the expression string */
            return( rhsp );         /* pt at 1 past the RE */
        } else if( c == '\0' ) {    /* last ch not RE end, help! */
            return( BAD );
        }
        *rhsp++ = c;
    }
}

/* compile a single command */
static int cmdcomp( char cchar )        /* character name of command */
{
    static sedcmd       **cmpstk[MAXDEPTH]; /* current cmd stack for {} */
    int                 i;              /* indexing dummy used in w */
    sedcmd              *sp1;           /* temps for label searches */
    sedcmd              *sp2;           /* temps for label searches */
    label               *lpt;           /* ditto, and the searcher */
    char                redelim;        /* current RE delimiter */

    switch( cchar ) {
    case '{':                           /* start command group */
        cmdp->flags.allbut = !cmdp->flags.allbut;
        cmpstk[bdepth++] = &( cmdp->u.link );
        if( ++cmdp >= cmds + MAXCMDS )
            ABORT( TMCDS );             /* Not exercised by sedtest.mak */
        if( *cp != '\0' )
            *--cp = ';';                /* get next cmd w/o lineread */
        return( 1 );

    case '}':                           /* end command group */
        if( cmdp->addr1 != NULL )
            ABORT( AD1NG );             /* no addresses allowed */
        if( --bdepth < 0 )
            ABORT( TMRBR );             /* too many right braces */
        *cmpstk[bdepth] = cmdp - 1;     /* set the jump address - (-1) to allow increment */
        return( 1 );

    case '=':                           /* print current source line number */
        break;

    case 'q':                           /* exit the stream editor */
        if( cmdp->addr2 != NULL )
            ABORT( AD2NG );
        break;

    case ':':                           /* label declaration */
        if( cmdp->addr1 != NULL )
            ABORT( AD1NG );             /* no addresses allowed */
        curlab->name = poolcurr;
        gettext( 0 );                   /* get the label name */
        lpt = search();
        if( lpt != NULL ) {             /* does it have a double? */
            if( lpt->link != NULL ) {
                ABORT( DLABL );         /* yes, abort */
            }
        } else {                        /* check label table doesn't overflow */
            if( curlab >= labels + MAXLABS )
                ABORT( TMLAB );         /* Not exercised by sedtest.mak */
            curlab->last = NULL;
            lpt = curlab++;
        }
        lpt->link = cmdp;
        return( 1 );

    case 'b':                           /* branch command */
    case 't':                           /* branch-on-succeed command */
    case 'T':                           /* branch-on-fail command */
        SKIPWS( cp );
        if( *cp == '\0' ) {             /* if branch is to start of cmds... */
            sp1 = lablst->last;         /* append command to label last */
            if( sp1 != NULL ) {
                while( (sp2 = sp1->u.link) != NULL )
                    sp1 = sp2;
                sp1->u.link = cmdp;
            } else {                    /* lablst->last == NULL */
                lablst->last = cmdp;
            }
            break;
        }
        curlab->name = poolcurr;
        gettext( 0 );                   /* get the label name */
        lpt = search();
        if( lpt != NULL ) {             /* enter branch to it */
            if( lpt->link != NULL ) {
                cmdp->u.link = lpt->link - 1;
            } else {
                sp1 = lpt->last;
                while( (sp2 = sp1->u.link) != NULL )
                    sp1 = sp2;
                sp1->u.link = cmdp - 1;
            }
        } else {                        /* matching named label not found */
            if( curlab >= labels + MAXLABS ) /* overflow if last */
                ABORT( TMLAB );         /* Not exercised by sedtest.mak */
            curlab->last = cmdp;        /* add the new label */
            curlab->link = NULL;        /* it's forward of here */
            curlab++;                   /* Get another label "object" */
        }
        break;

    case 'a':                           /* append text */
    case 'i':                           /* insert text */
    case 'r':                           /* read file into stream */
        if( cmdp->addr2 != NULL )
            ABORT( AD2NG );
        /* fall through */
    case 'c':                           /* change text */
        if( *cp == '\\' && *++cp == '\n' )
            cp++;
        cmdp->u.lhs = poolcurr;
        gettext( 1 );
        break;

    case 'D':                           /* delete current line in pattern space */
        cmdp->u.link = cmds;
        break;

    case 's':                           /* substitute regular expression */
        redelim = *cp++;                /* get delimiter from 1st ch */
        cmdp->u.lhs = poolcurr;
        poolcurr = recomp( poolcurr, redelim );
        if( poolcurr == BAD )
            ABORT( CGMSG );
        cmdp->rhs = poolcurr;
        if( poolcurr >= poolend )
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */
        poolcurr = rhscomp( cmdp->rhs, redelim );
        if( poolcurr == BAD )
            ABORT( CGMSG );
        if( gflag )
            cmdp->flags.global++;
        for( ;; ) {
            if( *cp == 'g' ) {
                cp++;
                cmdp->flags.global++;
            } else if( *cp == 'p' ) {
                cp++;
                cmdp->flags.print = 1;
            } else if( *cp == 'P' ) {
                cp++;
                cmdp->flags.print = 2;
            } else if( isdigit( *cp ) ) {
                i = 0;
                while( isdigit( *cp ) )
                    i = i * 10 + *cp++ - '0';
                if( i == 0 || i >= 2048 )
                    ABORT( SMCNT );
                cmdp->flags.nthone = i;
            } else {
                break;
            }
        }
        /* fall through */
    case 'l':                           /* list pattern space */
        if( *cp != 'w' )
            break;                      /* s or l is done */
        cp++;                           /* and execute a w command! */
        /* fall through */
    case 'w':                           /* write-pattern-space command */
    case 'W':                           /* write-first-line command */
        if( outfiles_open() ) {
            return( 0 );
        }
        break;

    case 'y':                           /* transliterate text */
        cmdp->u.lhs = poolcurr;
        poolcurr = ycomp( poolcurr, *cp++ );        /* compile translit */
        if( poolcurr == BAD )                 /* fail on bad form */
            ABORT( CGMSG );
        if( poolcurr >= poolend )             /* fail on overflow */
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */
        break;

    default:
        fprintf( stderr, INERR, "Unmatched command" );
        myexit( 2 );
    }
    return( 0 );                        /* interpreted one command */
}

/* precompile sed commands out of a file */
static void compile( void )
{
    #define LOWCMD      '8'     /* lowest char indexed in cmdmask */

    /* indirect through this to get command internal code, if it exists */
    static const cmdcode        cmdmask[] = {
        0,     0,      H,      0,      0,       H+EQCMD, 0,           0, /* 89:;<=>? */
        0,     0,      0,      0,      H+CDCMD, 0,       0,       CGCMD, /* @ABCDEFG */
        CHCMD, 0,      0,      0,      0,       0,       CNCMD,       0, /* HIJKLMNO */
        CPCMD, 0,      0,      0,      H+CTCMD, 0,       0,     H+CWCMD, /* PQRSTUVW */
        0,     0,      0,      0,      0,       0,       0,           0, /* XYZ[\]^_ */
        0,     H+ACMD, H+BCMD, H+CCMD, DCMD,    0,       0,        GCMD, /* `abcdefg */
        HCMD,  H+ICMD, 0,      0,      H+LCMD,  0,       NCMD,        0, /* hijklmno */
        PCMD,  H+QCMD, H+RCMD, H+SCMD, H+TCMD,  0,       0,      H+WCMD, /* pqrstuvw */
        XCMD,  H+YCMD, 0,      H+BCMD, 0,       H,       0,           0, /* xyz{|}~  */
    };

    cmdcode     ccode = 0;

    for( ;; ) {                         /* main compilation loop */
        if( cp == NULL ) {
            cp = linebuf;
            if( !cmdline( cp ) )
                break;
            if( cp[0] == '#' ) {          /* if the first two characters in the script are "#n" , */
                if( cp[1] == 'n' )
                    nflag = true;       /* the default output shall be suppressed */
                continue;
            }
        } else if( *cp != ';' ) {       /* get a new command line */
            cp = linebuf;
            if( !cmdline( cp ) ) {
                break;
            }
        }
        SKIPWS( cp );
        if( *cp == '\0' || *cp == '#' ) /* a comment */
            continue;
        while( *cp == ';' || isspace( *cp ) )
            cp++;                       /* ; separates cmds */

                                        /* compile first address */
        if( poolcurr >= poolend )
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */
        cmdp->addr1 = poolcurr;
        poolcurr = getaddress( poolcurr );
        if( poolcurr == BAD )
            ABORT( AGMSG );

        if( poolcurr == cmdp->addr1 ) {       /* if empty RE was found */
        } else if( poolcurr == NULL ) {       /* if poolcurr was NULL */
            poolcurr = cmdp->addr1;           /* use current pool location */
            cmdp->addr1 = NULL;
        } else {
            if( *cp == ',' || *cp == ';' ) { /* there's 2nd addr */
                cp++;
                if( poolcurr >= poolend )
                    ABORT( TMTXT );     /* Not exercised by sedtest.mak */
                cmdp->addr2 = poolcurr;
                poolcurr = getaddress( poolcurr );
                if( poolcurr == BAD || poolcurr == NULL ) {
                    ABORT( AGMSG );
                }
            } else {
                cmdp->addr2 = NULL;     /* no 2nd address */
            }
        }
        if( poolcurr >= poolend )
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */

        SKIPWS( cp );                   /* discard whitespace after address */
        if( *cp == '!' ) {
            cp++;
            cmdp->flags.allbut = 1;
        }

        SKIPWS( cp );                   /* get cmd char, range-check it */
        if( ( *cp < LOWCMD ) || ( *cp > '~' ) || ( (ccode = cmdmask[*cp - LOWCMD]) == 0 ) )
            ABORT( NSCAX );

        cmdp->command = ccode & ~H;     /* fill in command value */
        if( (ccode & H) == 0 ) {        /* if no compile-time code */
            cp++;                       /* discard command char */
        } else if( cmdcomp( *cp++ ) ) { /* execute it; if ret = 1 */
            continue;                   /* skip next line read */
        }

        if( ++cmdp >= cmds + MAXCMDS )
            ABORT( TMCDS );             /* Not exercised by sedtest.mak */

        SKIPWS( cp );                   /* look for trailing stuff */
        if( *cp != '\0' ) {
            if( *cp == ';' )
                continue;
            if( *cp != '#' ) {
                ABORT( TRAIL );
            }
        }
    }
}

static void usage( void )
{
    const char * const *cpp;

    for( cpp = USAGE; *cpp != NULL; cpp++ )
        fprintf( stderr, "%s\n", *cpp );
    myexit( 2 );
}

/* main sequence of the stream editor */
int main( int argc, char *argv[] )
{
    static char dummy_name[] = "progend\n";

    lablst->name = dummy_name;  /* Must set so strcmp can be done */
    cmdp->addr1 = pool;         /* 1st addr expand will be at pool start */

    if( argc <= 1 )
        usage();                /* exit immediately if no arguments */
    eargc   = argc;             /* set local copy of argument count */
    eargv   = argv;             /* set local copy of argument list */
    linebuf = malloc( MAXBUF + 3 );
    if( linebuf == NULL ) {
        fprintf( stderr, NOMEM );
        myexit( 2 );
    }

    outfiles_init();

    /* scan through the arguments, interpreting each one */
    /* We dont use the OW GetOpt() or the POSIX getopt() as we want to do
    * -e i\ hello -e "s/$/ world" */
    while( --eargc > 0 && **++eargv == '-' ) {
        int const       flag = eargv[0][1];

        /* Support "folded" flags such as -ng rather than -n -g */
        if( eargv[0][2] ) {
            char    *fr = eargv[0]+2;
            char    *to;

            switch( flag ) {
            case 'e':
            case 'f':
                to = fr - 2;
                break;
            case 'g':
            case 'n':
                to = fr - 1;
                break;
            default:
                to = fr;
                break;
            }
                                /* Move up remaining data */
            while( (*to++ = *fr++) != '\0' )
                ;
            ++eargc;
            --eargv;   /* Consider rest of argument "again" */
        }

        switch( flag ) {
        case 'e':
            eflag++;
            compile();          /* compile with e flag on */
            eflag = 0;
            break;              /* get another argument */
        case 'f':
            if( --eargc <= 0 ) { /* barf if no -f file */
                fprintf( stderr, NEEDB, eargv[0] );
                myexit( 2 );
            }
            cmdf = fopen( *++eargv, "r" );
            if( cmdf == NULL ) {
                fprintf( stderr, COCFI, *eargv );
                myexit( 2 );
            }
            compile();          /* file is O.K., compile it */
            fclose( cmdf );
            break;              /* go back for another argument */
        case 'g':
            gflag = true;       /* set global flag on all s cmds */
            break;
        case 'n':
            nflag = true;       /* no print except on p flag or w */
            break;
        default:
            fprintf( stderr, UFLAG, flag );
            /* Skip garbage argument */
            eargv++;
            eargc--;
            break;
        }
    }
    if( cp == NULL ) {          /* no commands have been compiled */
        if( eargc <= 0 )
            usage();            /* exit immediately if no commands */
        eargv--;
        eargc++;
        eflag++;
        compile();
        eflag = 0;
        eargv++;
        eargc--;
    }

    if( bdepth )                /* we have unbalanced squigglies */
        ABORT( TMLBR );

    lablst->link = cmdp;        /* set up header of label linked list */
    resolve();                  /* resolve label table indirections */
#ifndef _MSC_VER
    (void)setvbuf( stdout, NULL, _IOLBF, 0 ); /* Improve reactivity in a pipe */
#endif
    if( eargc <= 0 ) {          /* if there are no files specified */
#ifndef _MSC_VER
        (void)setvbuf( stdin, NULL, _IOLBF, 0 ); /* Improve reactivity in a pipe */
#endif
        execute( NULL );        /*   execute commands on stdin only */
    } else {
        while( --eargc >= 0 ) { /* else do commands on each file specified */
            execute( *eargv++ );
        }
    }
    outfiles_fini();
    free( linebuf );
    return( 0 );                /* everything was O.K. if we got here */
}

/* sedcomp.c ends here */
