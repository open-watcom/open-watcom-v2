/* sed.h -- types and constants for the stream editor */

/* data area sizes used by both modules */
#define MAXBUF          8192    /* current line buffer size */
#define MAXAPPENDS      20      /* maximum number of appends */
#define MAXTAGS         9       /* tagged patterns are \1 to \9 */

#define MAXCMDS         400     /* max number of compiled commands */
#define MAXLINES        256     /* max number of numeric addresses */

/* constants for compiled-command representation */
typedef enum cmdcode {
    EQCMD   = 0x01,     /* = -- print current line number               */
    ACMD    = 0x02,     /* a -- append text after current line          */
    BCMD    = 0x03,     /* b -- branch to label                         */
    CCMD    = 0x04,     /* c -- change current line                     */
    DCMD    = 0x05,     /* d -- delete all of pattern space             */
    CDCMD   = 0x06,     /* D -- delete first line of pattern space      */
    GCMD    = 0x07,     /* g -- copy hold space to pattern space        */
    CGCMD   = 0x08,     /* G -- append hold space to pattern space      */
    HCMD    = 0x09,     /* h -- copy pattern space to hold space        */
    CHCMD   = 0x0A,     /* H -- append hold space to pattern space      */
    ICMD    = 0x0B,     /* i -- insert text before current line         */
    LCMD    = 0x0C,     /* l -- print pattern space in escaped form     */
    NCMD    = 0x0D,     /* n -- get next line into pattern space        */
    CNCMD   = 0x0E,     /* N -- append next line to pattern space       */
    PCMD    = 0x0F,     /* p -- print pattern space to output           */
    CPCMD   = 0x10,     /* P -- print first line of pattern space       */
    QCMD    = 0x11,     /* q -- exit the stream editor                  */
    RCMD    = 0x12,     /* r -- read in a file after current line       */
    SCMD    = 0x13,     /* s -- regular-expression substitute           */
    TCMD    = 0x14,     /* t -- branch on last substitute successful    */
    CTCMD   = 0x15,     /* T -- branch on last substitute failed        */
    WCMD    = 0x16,     /* w -- write pattern space to file             */
    CWCMD   = 0x17,     /* W -- write first line of pattern space       */
    XCMD    = 0x18,     /* x -- exhange pattern and hold spaces         */
    YCMD    = 0x19,     /* y -- transliterate text                      */

    H       = 0x20,     /* command code attribute                       */
} cmdcode;

#define CHARSETSIZE     (256 / 8)

#define SETCHARSET(a,c)     (a[(unsigned char)c >> 3] |= bits[c & 7])
#define TESTCHARSET(a,c)    (a[(unsigned char)c >> 3] & bits[c & 7])

typedef struct cmd_t    sedcmd;         /* use this name for declarations */
struct cmd_t {                          /* compiled-command representation */
    char                *addr1;         /* first address for command */
    char                *addr2;         /* second address for command */
    union {
        char            *lhs;           /* s command lhs */
        sedcmd          *link;          /* label link */
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
};

#define BAD     ((char *)-1LL)          /* guaranteed not a string ptr */

/* address and regular expression compiled-form markers */
typedef enum recode {
    CCHR    = 0x01,    /* non-newline character to be matched follows */
    CDOT    = 0x02,    /* dot wild-card marker */
    CCL     = 0x03,    /* character class follows */
    CNL     = 0x04,    /* match line start */
    CDOL    = 0x05,    /* match line end */
    CBRA    = 0x06,    /* tagged pattern start marker */
    CKET    = 0x07,    /* tagged pattern end marker */
    CBACK   = 0x08,    /* backslash-digit pair marker */
    CLNUM   = 0x09,    /* numeric-address index follows */
    CEND    = 0x0A,    /* symbol for end-of-source */
    CEOF    = 0x0B,    /* end-of-field mark */

    STAR    = 0x10,    /* attr - marker for Kleene star */
    MTYPE   = 0x20,    /* attr - multiple counts |'d into RE    \{...\} */
} recode;

extern void     execute( const char *file ); /* In sed.c */

/* The following are in sedcomp.c */
                                                /* main data areas */
extern char             *linebuf;               /* current-line buffer */
extern sedcmd           cmds[MAXCMDS + 1];      /* hold compiled commands */
extern long             linenum[MAXLINES];      /* numeric-addresses table */

                                                /* miscellaneous shared variables */
extern bool                 nflag;              /* -n option flag */
extern int                  eargc;              /* scratch copy of argument count */
extern unsigned char const  bits[];             /* the bits table */

/* sed.h ends here */

