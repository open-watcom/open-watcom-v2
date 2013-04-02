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
#include "sed.h"                        /* command type struct & name defines */

#define MAXCMDS         400             /* max number of compiled commands */
#define MAXLINES        256             /* max number of numeric addresses */

                                        /* main data areas */
char            linebuf[MAXBUF + 3];    /* current-line buffer */
sedcmd          cmds[MAXCMDS + 1];      /* hold compiled commands */
long            linenum[MAXLINES];      /* numeric-addresses table */

                                        /* miscellaneous shared variables */
int             nflag = 0;              /* -n option flag */
int             eargc;                  /* scratch copy of argument count */
unsigned char const      bits[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

                                        /***** module common stuff *****/

#define POOLSIZE        10000           /* size of string-pool space */
#define WFILES          10              /* max number of w output files */
#define RELIMIT         256             /* max chars in compiled RE */
#define MAXDEPTH        20              /* maximum {}-nesting level */
#define MAXLABS         50              /* max number of labels */

#define SKIPWS(pc)      while( isspace( *pc ) ) pc++
#define ABORT(msg)      fprintf( stderr, msg, linebuf ), myexit( 2 )
#define IFEQ(x, v)      if( *x == v ) x++ , /* do expression */

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

typedef struct                          /* represent a command label */
{
    char        *name;                  /* the label name */
    sedcmd      *last;                  /* it's on the label search list */
    sedcmd      *link;                  /* pointer to the cmd it labels */
}               label;

                                        /* label handling */
static label    labels[MAXLABS];        /* here's the label table */
                                        /* first label is end of script */
static label    *curlab = labels + 1;   /* pointer to current label */
static label    *lablst = labels;       /* header for search list */

                                        /* string pool for REs, etc. */
static char     pool[POOLSIZE];         /* the pool */
static char     *fp     = pool;         /* current pool pointer */
static char     *poolend  = pool + POOLSIZE;    /* pointer past pool end */

                                        /* compilation state */
static FILE     *cmdf   = NULL;         /* current command source */
static char     *cp     = NULL;         /* compile pointer */
static sedcmd   *cmdp   = cmds;         /* current compiled-cmd ptr */
static int      bdepth  = 0;            /* current {}-nesting level */
static int      bcount  = 0;            /* # tagged patterns in current RE */
static char     **eargv;                /* scratch copy of argument list */

/* compilation flags */
static int      eflag = 0;              /* -e option flag */
static int      gflag = 0;              /* -g option flag */

static void     compile( void );
static int      cmdcomp( register char cchar );
static char     *rhscomp( register char *rhsp, register char delim );
static char     *recomp( char *expbuf, char redelim );
static int      cmdline( register char *cbuf );
static char     *getaddress( register char *expbuf );
static void     gettext( int accept_whitespace );
static label    *search( void );
static void     resolve( void );
static char     *ycomp( register char *ep, char delim );
static void     myexit( int status );
static void     usage( void );

#if defined( __WATCOMC__ ) || defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L )
#define my_isblank  isblank
#else
static int my_isblank( int c )
{
    return( c == ' ' || c == '\t' );
}
#endif

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
            while( ( *to++ = *fr++ ) != 0 ) ;
            ++eargc, --eargv;   /* Consider rest of argument "again" */
        }

        switch( flag ) {
        case 'e':
            eflag++;
            compile();          /* compile with e flag on */
            eflag = 0;
            break;              /* get another argument */
        case 'f':
            if( --eargc <= 0 )  /* barf if no -f file */
                fprintf( stderr, NEEDB, eargv[0] ), myexit( 2 );
            if( ( cmdf = fopen( *++eargv, "r" ) ) == NULL )
                fprintf( stderr, COCFI, *eargv ), myexit( 2 );
            compile();          /* file is O.K., compile it */
            fclose( cmdf );
            break;              /* go back for another argument */
        case 'g':
            gflag++;            /* set global flag on all s cmds */
            break;
        case 'n':
            nflag++;            /* no print except on p flag or w */
            break;
        default:
            fprintf( stderr, UFLAG, flag );
            eargv++, eargc--;   /* Skip garbage argument */
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
    if( eargc <= 0 ) {           /* if there are no files specified */
#ifndef _MSC_VER
        (void)setvbuf( stdin, NULL, _IOLBF, 0 ); /* Improve reactivity in a pipe */
#endif
        execute( NULL );        /*   execute commands on stdin only */
    } else while( --eargc >= 0 )  /* else do commands on each file specified */
        execute( *eargv++ );
    return( 0 );                /* everything was O.K. if we got here */
}

/* precompile sed commands out of a file */
static void compile( void )
{
    #define H       0x80        /* 128 bit, on if there's code for command */
    #define LOWCMD  56          /* = '8', lowest char indexed in cmdmask */

    /* indirect through this to get command internal code, if it exists */
    static unsigned char const   cmdmask[] = {
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

    unsigned char   ccode = 0;

    for( ;; ) {                         /* main compilation loop */
        if( !cp ) {
            if( !cmdline( cp = linebuf ) )
                break;
            if( *cp == '#' ) {          /* if the first two characters in the script are "#n" , */
                if( cp[1] == 'n' )
                    nflag++;            /* the default output shall be suppressed */
                continue;
            }
        } else if( *cp != ';' )         /* get a new command line */
            if( !cmdline( cp = linebuf ) )
                break;
        SKIPWS( cp );
        if( *cp == '\0' || *cp == '#' ) /* a comment */
            continue;
        while( *cp == ';' || isspace( *cp ) )
            cp++;                       /* ; separates cmds */

                                        /* compile first address */
        if( fp >= poolend )
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */
        cmdp->addr1 = fp;
        fp = getaddress( fp );
        if( fp == BAD )
            ABORT( AGMSG );

        if( fp == cmdp->addr1 ) {       /* if empty RE was found */
        } else if( fp == NULL ) {       /* if fp was NULL */
            fp = cmdp->addr1;           /* use current pool location */
            cmdp->addr1 = NULL;
        } else {
            if( *cp == ',' || *cp == ';' ) { /* there's 2nd addr */
                cp++;
                if( fp >= poolend )
                    ABORT( TMTXT );     /* Not exercised by sedtest.mak */
                cmdp->addr2 = fp;
                fp = getaddress( fp );
                if( fp == BAD || fp == NULL )
                    ABORT( AGMSG );
            } else
                cmdp->addr2 = NULL;     /* no 2nd address */
        }
        if( fp >= poolend )
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */

        SKIPWS( cp );                   /* discard whitespace after address */
        IFEQ( cp, '!' )
            cmdp->flags.allbut = 1;

        SKIPWS( cp );                   /* get cmd char, range-check it */
        if( ( *cp < LOWCMD ) || ( *cp > '~' ) || ( ( ccode = cmdmask[*cp - LOWCMD] ) == 0 ) )
            ABORT( NSCAX );

        cmdp->command = ccode & ~H;     /* fill in command value */
        if( ( ccode & H ) == 0 )        /* if no compile-time code */
            cp++;                       /* discard command char */
        else if( cmdcomp( *cp++ ) )     /* execute it; if ret = 1 */
            continue;                   /* skip next line read */

        if( ++cmdp >= cmds + MAXCMDS )
            ABORT( TMCDS );             /* Not exercised by sedtest.mak */

        SKIPWS( cp );                   /* look for trailing stuff */
        if( *cp != '\0' )
            if( *cp == ';' )
                continue;
            else if( *cp != '#' )
                ABORT( TRAIL );
    }
}

/* compile a single command */
static int cmdcomp( register char cchar ) /* character name of command */
{
    static sedcmd       **cmpstk[MAXDEPTH]; /* current cmd stack for {} */
    static char const   *fname[WFILES]; /* w file name pointers */
    static FILE         *fout[WFILES];  /* w file file ptrs */
    static int          nwfiles = 1;    /* count of open w files */
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
        if( cmdp->addr1 )
            ABORT( AD1NG );             /* no addresses allowed */
        if( --bdepth < 0 )
            ABORT( TMRBR );             /* too many right braces */
        *cmpstk[bdepth] = cmdp - 1;     /* set the jump address - (-1) to allow increment */
        return( 1 );

    case '=':                           /* print current source line number */
        break;

    case 'q':                           /* exit the stream editor */
        if( cmdp->addr2 )
            ABORT( AD2NG );
        break;

    case ':':                           /* label declaration */
        if( cmdp->addr1 )
            ABORT( AD1NG );             /* no addresses allowed */
        curlab->name = fp;
        gettext( 0 );                   /* get the label name */
        if( ( lpt = search() ) != 0 ) { /* does it have a double? */
            if( lpt->link )
                ABORT( DLABL );         /* yes, abort */
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
                                        /* append command to label last */
            if( ( sp1 = lablst->last ) != 0 ) {
                while( ( sp2 = sp1->u.link ) != 0 )
                    sp1 = sp2;
                sp1->u.link = cmdp;
            } else                      /* lablst->last == NULL */
                lablst->last = cmdp;
            break;
        }
        curlab->name = fp;
        gettext( 0 );                   /* get the label name */
        if( ( lpt = search() ) != 0 ) { /* enter branch to it */
            if( lpt->link )
                cmdp->u.link = lpt->link - 1;
            else {
                sp1 = lpt->last;
                while( ( sp2 = sp1->u.link ) != 0 )
                    sp1 = sp2;
                sp1->u.link = cmdp - 1;
            }
        }
        else {                          /* matching named label not found */
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
        if( cmdp->addr2 )
            ABORT( AD2NG );
                                        /* Drop through */
    case 'c':                           /* change text */
        if( *cp == '\\' && *++cp == '\n' )
            cp++;
        cmdp->u.lhs = fp;
        gettext( 1 );
        break;

    case 'D':                           /* delete current line in pattern space */
        cmdp->u.link = cmds;
        break;

    case 's':                           /* substitute regular expression */
        redelim = *cp++;                /* get delimiter from 1st ch */
        if( ( fp = recomp( cmdp->u.lhs = fp, redelim ) ) == BAD )
            ABORT( CGMSG );
        if( ( cmdp->rhs = fp ) >= poolend )
            ABORT( TMTXT );            /* Not exercised by sedtest.mak */
        if( ( fp = rhscomp( cmdp->rhs, redelim ) ) == BAD )
            ABORT( CGMSG );
        if( gflag )
            cmdp->flags.global++;
        while( *cp == 'g' || *cp == 'p' || *cp == 'P' || isdigit( *cp ) ) {
            IFEQ( cp, 'g' )
                cmdp->flags.global++;
            IFEQ( cp, 'p' )
                cmdp->flags.print = 1;
            IFEQ( cp, 'P' )
                cmdp->flags.print = 2;
            if( isdigit( *cp ) ) {
                i = 0;
                while (isdigit(*cp))
                    i = i*10 + *cp++ - '0';
                if (i == 0 || i >= 2048)
                    ABORT(SMCNT);
                cmdp->flags.nthone = (unsigned)i;
            }
        }
                                        /* Drop through */
    case 'l':                           /* list pattern space */
        if( *cp != 'w' )
            break;                      /* s or l is done */
        cp++;                           /* and execute a w command! */
                                        /* drop through */
    case 'w':                           /* write-pattern-space command */
    case 'W':                           /* write-first-line command */
        if( nwfiles >= WFILES )
            ABORT( TMWFI );
        fout[0] = stdout;               /* Not initialized to humor lint */
        fname[0] = "";                  /* Set so strcmp( x, fname[0] ) OK */
        fname[nwfiles] = (const char*)fp; /* filename is in pool */
        gettext( 0 );
        for( i = nwfiles - 1; i >= 0; i-- ) /* match it in table */
            if( strcmp( fname[nwfiles], fname[i] ) == 0 ) {
                cmdp->fout = fout[i];
                return( 0 );
            }
                                        /* if didn't find one, open new file */
        if( ( cmdp->fout = fopen( fname[nwfiles], "w" ) ) == NULL
                                        /* setvbuf() for -w -r combination */
        || 0 != setvbuf( cmdp->fout, NULL, _IOLBF, 0 ) )
            fprintf( stderr, CCOFI, fname[nwfiles] ), myexit( 2 );
        fout[nwfiles++] = cmdp->fout;
        break;

    case 'y':                           /* transliterate text */
        fp = ycomp( cmdp->u.lhs = fp, *cp++ );    /* compile translit */
        if( fp == BAD )                 /* fail on bad form */
            ABORT( CGMSG );
        if( fp >= poolend )             /* fail on overflow */
            ABORT( TMTXT );             /* Not exercised by sedtest.mak */
        break;

    default:
        fprintf( stderr, INERR, "Unmatched command" ), myexit( 2 );
    }
    return( 0 );                        /* interpreted one command */
}

/* generate replacement string for substitute command right hand side */
static char *rhscomp(
    register char       *rhsp,          /* place to compile expression to */
    register char       delim )         /* RE end-mark to find */
                                        /* uses bcount */
{
    register char       *p = cp;        /* strictly for speed */

    for( ;; )
        if( ( *rhsp = *p++ ) == '\\' ) { /* copy; if it's a \, */
            *rhsp = *p++;               /* copy escaped char */
                                        /* check validity of pattern tag */
            if( *rhsp > bcount + '0' && *rhsp <= '9' )
                return( BAD );
            *rhsp++ |= 0x80;            /* mark the good ones */
        } else if( *rhsp == delim ) {   /* found RE end, hooray... */
            *rhsp++ = '\0';             /* cap the expression string */
            cp = p;
            return( rhsp );             /* pt at 1 past the RE */
        } else if( *rhsp++ == '\0' )    /* last ch not RE end, help! */
            return( BAD );
}

/* compile a regular expression to internal form */
static char *recomp(
    char            *expbuf,            /* place to compile it to */
    char            redelim )           /* RE end-marker to look for */
                                        /* uses cp, bcount */
{
    register char   *ep = expbuf;       /* current-compiled-char pointer */
    register char   *sp = cp;           /* source-character ptr */
    register int    c;                  /* current-character */
    char            negclass;           /* all-but flag */
    char            *lastep;            /* ptr to last expr compiled */
    char const      *svclass;           /* start of current char class */
    char            brnest[MAXTAGS+1];  /* bracket-nesting array */
    char            *brnestp;           /* ptr to current bracket-nest */
    char const      *pp;                /* scratch pointer */
    int             tags;               /* # of closed tags */
    char            *obr[MAXTAGS+1] = {0}; /* ep values when \( seen */
    int             opentags = 0;       /* Used to index obr */

    if( *cp == redelim )                /* if first char is RE endmarker */
        return( cp++, *ep++ = CEOF, ep ); /* use existing RE. */

    lastep = NULL;                      /* there's no previous RE */
    brnestp = brnest;                   /* initialize ptr to brnest array */
    tags = bcount = 0;                  /* initialize counters */

    if( ( *ep++ = (int)( *sp == '^' ) ) != 0 ) /* check for start-of-line syntax */
        sp++;

    for( ;; ) {
        if( ep >= expbuf + RELIMIT )    /* match is too large */
            return( cp = sp, BAD );     /* Not exercised by sedtest.mak */
        if( ( c = *sp++ ) == redelim ) {/* found the end of the RE */
            cp = sp;
            if( brnestp != brnest )     /* \(, \) unbalanced */
                return( BAD );
            *ep++ = CEOF;               /* write end-of-pattern mark */
            return( ep );               /* return ptr to compiled RE */
        }
        if( c == '*'
        ||  c == '+'
        ||  c == '\\' && *sp == '{' )   /* if we're a postfix op */
            ;
        else
            lastep = ep;                /* get ready to match last */

        switch( c ) {
        case '\\':
            switch( c = *sp++ ) {
            case '(':                   /* start tagged section */
                if( ++bcount <= MAXTAGS ) { /* bump tag count */
                    *brnestp++ = (char)bcount; /* update tag stack */
                    obr[opentags] = ep; /* Remember for /(.../)* */
                }
                opentags++;
                *ep++ = CBRA;           /* enter tag-start */
                *ep++ = (char)bcount;
                break;
            case ')':                   /* end tagged section */
                if( --opentags < 0 )    /* extra \) */
                    return( cp = sp, BAD );
                *ep++ = CKET;           /* enter end-of-tag */
                if( ++tags <= MAXTAGS ) /* count closed tags */
                    *ep++ = *--brnestp; /* pop tag stack */
                else
                    *ep++ = 0;          /* Placeholder - should not be used */
                break;
            case '\n':                  /* escaped newline no good */
                return( cp = sp, BAD );
            case 'n':                   /* match a newline */
                c = '\n';
                goto defchar;
            case 't':                   /* match a tab */
                c = '\t';
                goto defchar;
            case '\\':                  /* match a literal backslash */
                goto defchar;
            default:
                if( c >= '1' && c <= '9' ) { /* tag use */
                    if( ( c -= '0' ) > tags ) /* too few */
                        return( BAD );
                    *ep++ = CBACK;      /* enter tag mark */
                    *ep++ = (char)c;    /* and the number */
                    break;
                }
#if 1
                /* This allows \ to stop "special" even if it is not. */
                goto defchar;           /* else match \c */
#else
                /* This is IEEE 2001 behavior */
                return( cp = sp, BAD ); /* Otherwise /\?/ && /\\?/ synonymous */
#endif
            case '{':                   /* '}' should balance for vi */
                {
                    int i1 = 0;
                    int i2 = 0;

                    if( !lastep )
                        return( cp = sp, BAD ); /* rep error */
                    *lastep |= MTYPE;
                    if( !isdigit( *sp ) )
                        return( cp = sp, BAD );
                    while( isdigit( *sp ) )
                        i1 = i1*10 + *sp++ - '0';
                    if( i1 > 255 )
                        return( cp = sp, BAD );
                    *ep++ = (char)i1;
                    if( *sp == '\\' && sp[1] == /* '{' vi brace balancing */ '}' )
                        sp += 2, *ep++ = 0;
                    else if( *sp == ',' && sp[1] == '\\' && sp[2] == /* '{' vi brace balancing */ '}' )
                        sp += 3, *ep++ = '\xFF';
                    else if( *sp++ ==',') {
                      if ( !isdigit( *sp ) )
                          *ep++ = '\xFF';
                      else {
                        while (isdigit( *sp ) )
                            i2 = i2*10 + *sp++ - '0';
                        *ep++ = (char)(i2-i1);
                      }
                      if( *sp != '\\' || sp[1] != /* '{' vi brace balancing */ '}' || i2 < i1 || i2 > 255 )
                          return( cp = sp, BAD );
                      sp += 2;
                    } else
                        return( cp = sp, BAD );
                }
                goto handle_cket;
            }
            break;

        case '\n':                      /* Can not happen? WFB 20040801 */
        case '\0':
            return( cp = sp, BAD );     /* incomplete regular expression */

        case '.':                       /* match any char except newline */
            *ep++ = CDOT;
            break;

        case '+':                       /* 1 to n repeats of previous pattern */
            if( lastep == NULL )        /* if + not first on line */
                goto defchar;           /*   match a literal + */
#if 0                                   /* Removed constraint WFB 20040804 */
            if( *lastep == CKET )       /* can't iterate a tag */
                return( cp = sp, BAD );
#endif
            pp = ep;                    /* else save old ep */
            while( lastep < pp )        /* so we can blt the pattern */
                *ep++ = *lastep++;
            *lastep |= STAR;            /* flag the copy */
            break;

        case '*':                       /* 0..n repeats of previous pattern */
            if( lastep == NULL )        /* if * isn't first on line */
                goto defchar;           /*   match a literal * */
#if 0                                   /* Removed constraint WFB 20040804 */
            if( *lastep == CKET )       /* can't iterate a tag */
                return( cp = sp, BAD );
#endif
            *lastep |= STAR;            /* flag previous pattern */
            goto handle_cket;

        case '$':                       /* match only end-of-line */
            if( *sp != redelim )        /* if we're not at end of RE */
                goto defchar;           /*   match a literal $ */
            *ep++ = CDOL;               /* insert end-symbol mark */
            break;

        case '[':                       /* begin character set pattern */
            {
                int                     classct; /* class element count */
                register unsigned       uc; /* current-character */

                if( ep + 17 >= expbuf + RELIMIT )
                    ABORT( REITL );     /* Not exercised by sedtest.mak */
                *ep++ = CCL;            /* insert class mark */
                if( ( negclass = (int)( ( uc = *sp++ ) == '^' ) ) != 0 )
                    uc = *sp++;
                svclass = sp;           /* save ptr to class start */
                do {
                    switch( uc ) {
                    case '\0':
                        ABORT( CGMSG );
                    case '-':           /* handle character ranges */
                        if( sp > svclass && *sp != ']' ) {
                            unsigned const  lo = (unsigned)*( sp - 2 );
                            unsigned const  hi = (unsigned)*sp;
                            if( lo > hi )
                                ABORT( CGMSG );
                            if( sp[1] == '-' && sp[2] != ']' )
                                ABORT( CGMSG );
                            for( uc = lo; uc <= hi; uc++ )
                                ep[uc >> 3] |= bits[uc & 7];
                            continue;
                        }
                        break;
                    case '\\':          /* handle escape sequences in sets */
                        if( ( uc = *sp++ ) == 'n' )
                            uc = '\n';
                        else if( uc == 't' )
                            uc = '\t';
                        else
                            --sp, uc = '\\'; /* \n and \t are special, \* is not */
                    case '[':           /* Handle named character class */
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
                            size_t si;
                            int ( *isf )( int c );

                            for( si = 0; si < sizeof t / sizeof *t; si++ )
                                if( strncmp( sp+1, t[si].s, strlen( t[si].s ) ) == 0 )
                                    break;
                            if( si >= sizeof t / sizeof *t ) /* Add class */
                                ABORT( UNCLS );
                            sp += 1 + strlen( t[si].s );
                            for( isf = t[si].isf, uc = 1; uc < 128; uc++ )
                                if( isf( (int)uc ) )
                                    ep[uc >> 3] |= bits[uc & 7];
                            continue;
                        }
                        break;
                    }
                                        /* add (maybe translated) char to set */
                    ep[uc >> 3] |= bits[uc & 7];
                } while ( ( uc = *sp++ ) != ']' );
                                        /* invert bitmask if all-but needed */
                if( negclass )
                    for( classct = 0; classct < 16; classct++ )
                        ep[classct] ^= 0xFF;
                ep[0] &= 0xFE;          /* never match ASCII 0 */
                ep += 16;               /* advance ep past set mask */
            }
            break;

        defchar:                        /* match literal character */
        default:                        /* which is what we'd do by default */
            *ep++ = CCHR;               /* insert character mark */
            *ep++ = (char)c;
            break;

        handle_cket:
            switch( *lastep & ~STAR & ~MTYPE ) {
            case CCHR:
            case CDOT:
            case CCL:
                break;
            case CBRA:
                return( cp = sp, BAD );
            case CKET: {
                    /* Make room to insert skip marker in expbuf */
                    char * const    firstep = obr[opentags];
                    int const       width = ep - firstep;
                    int             i = width;

                    if( width >= 256 )
                        ABORT( REITL ); /* Not exercised by sedtest.mak */
                    *firstep |= ( *lastep ^ CKET ) ; /* Mark \( as * or \{ terminated */
                    while( --i >= 2 )
                        firstep[i+1] = firstep[i];
                    firstep[2] = (char)width-1;
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
                fprintf( stderr, INERR, "Unexpected symbol in RE" ), myexit( 2 );
            }
            break;
        } /* switch( c ) */
    } /* for( ;; ) */
}

/* read next command from -e argument or command file */
static int cmdline( register char *cbuf ) /* uses eflag, eargc, cmdf */
{
    register int        inc;            /* not char because must hold EOF */

    assert( cbuf == cp );
    cbuf--;                             /* so pre-increment points us at cbuf */

                                        /* e command flag is on */
    if( eflag ) {
        register char   *p;             /* ptr to current -e argument */
        static char     *savep;         /* saves previous value of p */

        if( eflag > 0 ) {               /* there are pending -e arguments */
            eflag = -1;
            if( --eargc <= 0 )          /* barf if no argument */
                fprintf( stderr, NEEDB, eargv[0] ), myexit( 2 );
                                        /* else copy next e argument to cbuf */
            p = *++eargv;
            while( ( *++cbuf = *p++ ) != 0 )
                if( *cbuf == '\\' ) {   /* Could not sedtest this! WFB 20040802 */
                    if( ( *++cbuf = *p++ ) == '\0' ) {
                        if( --eargc <= 0 ) /* barf if no argument */
                            fprintf( stderr, NEEDB, eargv[0] ), myexit( 2 );
                        *cbuf = '\n';
                        p = *++eargv;
                    }
                } else if( *cbuf == '\n' ) { /* end of 1 cmd line */
                    *cbuf = '\0';
                    return( savep = p, 1 );
                                        /* we'll be back for the rest... */
                }
                                        /* have string-end;
                                         * can advance to next argument */
            return( savep = NULL, 1 );
        }

        if( ( p = savep ) == NULL )
            return( 0 );

        while( ( *++cbuf = *p++ ) != 0 )
            if( *cbuf == '\\' ) {
                if( ( *++cbuf = *p++ ) == '0' )
                    return( savep = NULL, 0 );
            } else if( *cbuf == '\n' ) {
                *cbuf = '\0';
                return( savep = p, 1 );
            }

        return( savep = NULL, 1 );
    }

                                        /* if no -e flag
                                         * read from command file descriptor */
    while( ( inc = getc( cmdf ) ) != EOF ) { /* get next char */
        switch( *++cbuf = (char)inc ) {
        case '\\':                      /* if it's escape */
            inc = getc( cmdf ),         /* get next char */
            *++cbuf = (char)inc;
            break;
        case '\n':                      /* end on newline */
            return( *cbuf = '\0', 1 );  /* cap the string */
        }
    }

    return( *++cbuf = '\0', cbuf >= cp+1 );       /* end-of-file, no more chars */
}

/* expand an address at *cp... into expbuf, return ptr at following char */
static char *getaddress( register char *expbuf ) /* uses cp, linenum */
{
    static int          numl = 0;       /* current ind in addr-number table */
    register char       *rcp;           /* temp compile ptr for forwd look */
    long                lno;            /* computed value of numeric address */

    switch( *cp ) {
    case '$':                           /* end-of-source address */
        *expbuf++ = CEND;               /* write symbolic end address */
        *expbuf++ = CEOF;               /* and the end-of-address mark (!) */
        cp++;                           /* go to next source character */
        return( expbuf );               /* we're done */
    case '\\':                          /* posix \cBREc address */
        cp++;                           /* Point to delimiter */
    case '/':                           /* start of regular-expression match */
        return( recomp( expbuf, *cp++ ) ); /* compile the RE */
    }
    rcp = cp;
    lno = 0;                            /* now handle a numeric address */
    while( isdigit( *rcp ) )            /* collect digits */
        lno = lno*10 + *rcp++ - '0';    /* compute their value */

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

/*
 * accept multiline input from *cp... to *fp... ,
 * optionally skipping leading whitespace
 */
static void gettext( int accept_whitespace )
{
    char                c;

    if( !accept_whitespace )
        SKIPWS( cp );                   /* discard whitespace */
    while( fp < poolend && ( c = *fp++ = *cp++ ) != 0 ) {
        switch( c )
        {
        case '\\':                      /* handle escapes */
            fp[-1] = *cp++;
            break;
        case '\n':                      /* SKIPWS after newline */
            SKIPWS( cp );
            break;
        }
    }
    if( fp >= poolend )
        ABORT( TMTXT );                 /* Not exercised by sedtest.mak */
    --cp;
    return;
}

/* find the label matching *ptr, return NULL if none */
static label *search( void )            /* uses globals lablst and curlab */
{
    register label      *rp;
    char const * const  name = curlab->name;
    for( rp = lablst; rp < curlab; rp++ )
        if( strcmp( rp->name, name ) == 0 )
            return( rp );
    return( NULL );
}

/* write label links into the compiled-command space */
static void resolve( void )             /* uses global lablst */
{
    register label const        *rp;
    register sedcmd             *rptr;
    register sedcmd             *trptr;
                                        /* loop through the label table */
    for( rp = lablst; rp < curlab; rp++ )
        if( rp->link == NULL )          /* barf if not defined */
            fprintf( stderr, ULABL, rp->name ), myexit( 2 );
        else if( rp->last ) {           /* if last is non-null */
            for( rptr = rp->last; ( trptr = rptr->u.link ) != 0; rptr = trptr )
                rptr->u.link = rp->link;
            rptr->u.link = rp->link - 1;
        }
}

/* compile a y (transliterate) command */
static char *ycomp(
    register char       *ep,            /* where to compile to */
    char                delim )         /* end delimiter to look for */
{
    register int        c;
    register char       *tp;
    register char const *sp;

    if( delim == 0 || delim == '\\' || delim == '\n' )
        return( BAD );
                                        /* scan 'from' for invalid chars */
    for( sp = tp = cp; *tp != delim; tp++ ) {
        if( *tp == '\\' )
            tp++;
        if( ( *tp == '\n' ) || ( *tp == '\0' ) )
            return( BAD );
    }
    tp++;                               /* tp points at first char of 'to' */

                                        /* now rescan the 'from' section */
    while( ( c = (char)( *sp++ & 0x7F ) ) != delim ) {
        if( c == '\\' )
            switch( c = *sp++ & 0x7F ) {
            case 'n':                   /* '\\''n' -> '\n' */
                c = '\n';
                break;
            case '\\':
                break;
            default:
                if( c != delim )
                    return( BAD );
                c = delim;
                break;
            }
        if( ep[c] != 0 )
            return( BAD );              /* c has already been mapped */
        if( ( ep[c] = *tp++ & 0x7F ) == '\\' ) {
            switch( *tp ) {
            case 'n':                   /* '\\''n' -> '\n' */
                ep[c] = '\n';
                break;
            case '\\':
                break;
            default:
                if( *tp != delim )
                    return( BAD );
                ep[c] = delim;
                break;
            }
            tp++;
        }
        if( ep[c] == '\0' )
            return( BAD );
    }

    if( *tp != delim )                  /* 'to', 'from' lengths unequal */
        return( BAD );

    cp = ++tp;                          /* point compile ptr past translit */

    for( c = 0; c < 128; c++ )          /* fill in self-map entries in table */
        if( ep[c] == 0 )
            ep[c] = c;

    return( ep + 0x80 );                /* first free location past table end */
}

/* Avoid race condition with calls like echo hello | fail */
static void myexit( int status )
{
    assert( status != 0 );              /* Call only needed for failures */
    if( !isatty( fileno( stdin ) ) )
        while( fgets( linebuf, MAXBUF, stdin ) != NULL ) ;
    exit( status );
}

static void usage( void )
{
    const char * const *cpp;

    for( cpp = USAGE; *cpp != NULL; cpp++ )
        fprintf( stderr, "%s\n", *cpp );
    myexit( 2 );
}

/* sedcomp.c ends here */
