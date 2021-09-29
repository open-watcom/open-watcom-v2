/* sed.h -- types and constants for the stream editor */

/* data area sizes used by both modules */
#define MAXBUF          8192    /* current line buffer size */
#define MAXAPPENDS      20      /* maximum number of appends */
#define MAXTAGS         9       /* tagged patterns are \1 to \9 */

#define MAXCMDS         400     /* max number of compiled commands */
#define MAXLINES        256     /* max number of numeric addresses */

/* constants for compiled-command representation */
typedef enum cmdcode {
    EQCMD = 1,  /* = -- print current line number               */
    ACMD,       /* a -- append text after current line          */
    BCMD,       /* b -- branch to label                         */
    CCMD,       /* c -- change current line                     */
    DCMD,       /* d -- delete all of pattern space             */
    CDCMD,      /* D -- delete first line of pattern space      */
    GCMD,       /* g -- copy hold space to pattern space        */
    CGCMD,      /* G -- append hold space to pattern space      */
    HCMD,       /* h -- copy pattern space to hold space        */
    CHCMD,      /* H -- append hold space to pattern space      */
    ICMD,       /* i -- insert text before current line         */
    LCMD,       /* l -- print pattern space in escaped form     */
    NCMD,       /* n -- get next line into pattern space        */
    CNCMD,      /* N -- append next line to pattern space       */
    PCMD,       /* p -- print pattern space to output           */
    CPCMD,      /* P -- print first line of pattern space       */
    QCMD,       /* q -- exit the stream editor                  */
    RCMD,       /* r -- read in a file after current line       */
    SCMD,       /* s -- regular-expression substitute           */
    TCMD,       /* t -- branch on last substitute successful    */
    CTCMD,      /* T -- branch on last substitute failed        */
    WCMD,       /* w -- write pattern space to file             */
    CWCMD,      /* W -- write first line of pattern space       */
    XCMD,       /* x -- exhange pattern and hold spaces         */
    YCMD,       /* y -- transliterate text                      */

    H   = 0x20, /* command code attribute                       */
} cmdcode;

#define CHARSETSIZE     (256 / 8)

#define SETCHARSET(a,c)     (a[(unsigned char)c >> 3] |= bits[c & 7])
#define TESTCHARSET(a,c)    (a[(unsigned char)c >> 3] & bits[c & 7])
#define TESTCHARSETINC(a,p) (a[*(unsigned char *)(p) >> 3] & bits[*(unsigned char *)(p++) & 7])

typedef struct sedcmd {                          /* compiled-command representation */
    char                *addr1;         /* first address for command */
    char                *addr2;         /* second address for command */
    union {
        char            *lhs;           /* s command lhs */
        struct sedcmd   *link;          /* label link */
    } u;
    cmdcode             command;        /* command code */
    char                *rhs;           /* s command replacement string */
    FILE                *fout;          /* associated output file descriptor */
    struct {
        int             nthone;         /* if !0 only nth replace */
        unsigned        allbut  : 1;    /* was negation specified? */
        unsigned        global  : 1;    /* was g postfix specified? */
        unsigned        print   : 2;    /* was p postfix specified? */
        unsigned        inrange : 1;    /* in an address range? */
    } flags;
} sedcmd;

#define BAD     ((char *)-1LL)          /* guaranteed not a string ptr */

/* address and regular expression compiled-form markers */
typedef enum recode {
    CCHR = 1,   /* non-newline character to be matched follows */
    CDOT,       /* dot wild-card marker */
    CCL,        /* character class follows */
    CNL,        /* match line start */
    CDOL,       /* match line end */
    CBRA,       /* tagged pattern start marker */
    CKET,       /* tagged pattern end marker */
    CBACK,      /* backslash-digit pair marker */
    CLNUM,      /* numeric-address index follows */
    CEND,       /* symbol for end-of-source */
    CEOF,       /* end-of-field mark */

    STAR    = 0x10,    /* attr - marker for Kleene star */
    MTYPE   = 0x20,    /* attr - multiple counts |'d into RE    \{...\} */
} recode;

/* The following are in sedcomp.c */
                                                /* main data areas */
extern char             *linebuf;               /* current-line buffer */
extern sedcmd           cmds[MAXCMDS + 1];      /* hold compiled commands */
extern long             linenum[MAXLINES];      /* numeric-addresses table */

                                                /* miscellaneous shared variables */
extern bool                 nflag;              /* -n option flag */
extern int                  eargc;              /* scratch copy of argument count */
extern unsigned char const  bits[];             /* the bits table */


extern void             execute( const char *file ); /* In sed.c */

/* sed.h ends here */
