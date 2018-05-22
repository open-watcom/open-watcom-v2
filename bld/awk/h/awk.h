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


#include <assert.h>
#include "bool.h"


#define DEBUG
#ifdef  DEBUG
            /* uses have to be doubly parenthesized */
#   define  dprintf(x)  if(dbg) printf x
#else
#   define  dprintf(x)
#endif

#define xfree(a)    { if((a) != NULL) { free( (void *)(a) ); (a) = NULL; } }

#define NN(p)       ((p) ? (p) : "(null)")  /* guaranteed non-null for dprintf */

#define RECSIZE     (8 * 1024)      /* sets limit on records, fields, etc., etc. */

#define NOPAT       ((size_t)-1)

#define NIL         ((Node *)0)

#define NSYMTAB     50              /* initial size of a symbol table */

/* function types */
#define FLENGTH     1
#define FSQRT       2
#define FEXP        3
#define FLOG        4
#define FINT        5
#define FSYSTEM     6
#define FRAND       7
#define FSRAND      8
#define FSIN        9
#define FCOS        10
#define FATAN       11
#define FTOUPPER    12
#define FTOLOWER    13
#define FFLUSH      14

/* ctypes */
typedef enum {
    OCELL = 1,
    OBOOL = 2,
    OJUMP = 3,
} cell_type;

/* Cell subtypes: csub */
typedef enum {
    CUNK        = 0,
    CFLD        = 1,
    CVAR        = 2,
    CNAME       = 3,
    CTEMP       = 4,
    CCON        = 5,
    CCOPY       = 6,
    CFREE       = 7,
/* bool subtypes */
    BTRUE       = 11,
    BFALSE      = 12,
/* jump subtypes */
    JEXIT       = 21,
    JNEXT       = 22,
    JBREAK      = 23,
    JCONT       = 24,
    JRET        = 25,
    JNEXTFILE   = 26,
} cell_subtyp;

/* node types */
typedef enum {
    NVALUE = 1,
    NSTAT  = 2,
    NEXPR  = 3,
} node_type;

#define notlegal(n)     (n <= FIRSTTOKEN || n >= LASTTOKEN || proctab[n - FIRSTTOKEN] == nullproc)
#define isvalue(n)      ((n)->ntype == NVALUE)
#define isexpr(n)       ((n)->ntype == NEXPR)
#define isjump(n)       ((n)->ctype == OJUMP)
#define isexit(n)       ((n)->csub == JEXIT)
#define isbreak(n)      ((n)->csub == JBREAK)
#define iscont(n)       ((n)->csub == JCONT)
#define isnext(n)       ((n)->csub == JNEXT || (n)->csub == JNEXTFILE)
#define isret(n)        ((n)->csub == JRET)
#define isrec(n)        (((n)->tval & REC)!=0)
#define isfld(n)        (((n)->tval & FLD)!=0)
#define isstr(n)        (((n)->tval & STR)!=0)
#define isnum(n)        (((n)->tval & NUM)!=0)
#define isarr(n)        (((n)->tval & ARR)!=0)
#define isfcn(n)        (((n)->tval & FCN)!=0)
#define istrue(n)       ((n)->csub == BTRUE)
#define istemp(n)       ((n)->csub == CTEMP)
#define isargument(n)   ((n)->nobj == ARG)
/* #define freeable(n)     (((n)->tval & DONTFREE)==0) */
#define freeable(n)     (((n)->tval & (STR|DONTFREE)) == STR)

/* structures used by regular expression matching machinery, mostly b.c: */

#define NCHARS      (256+3)     /* 256 handles 8-bit chars; 128 does 7-bit */
                                /* watch out in match(), etc. */
#define NSTATES     32


typedef double      Awkfloat;

/* unsigned char is more trouble than it's worth */

typedef unsigned char uschar;

/* Cell:  all information about a variable or constant */

/* Cell.tval values: */
typedef enum {
    NUM         = 0x01, /* number value is valid */
    STR         = 0x02, /* string value is valid */
    DONTFREE    = 0x04, /* string space is not freeable */
    CON         = 0x08, /* this is a constant */
    ARR         = 0x10, /* this is an array */
    FCN         = 0x20, /* this is a function name */
    FLD         = 0x40, /* this is a field $1, $2, ... */
    REC         = 0x80, /* this is $0 */
} type_info;

typedef struct Cell {
    cell_type   ctype;  /* OCELL, OBOOL, OJUMP, etc. */
    cell_subtyp csub;   /* CCON, CTEMP, CFLD, etc. */
    char        *nval;  /* name, for variables only */
    char        *sval;  /* string value */
    Awkfloat    fval;   /* value as number */
    type_info   tval;   /* type info: STR|NUM|ARR|FCN|FLD|CON|DONTFREE */
    struct Cell *cnext; /* ptr to next if chained */
} Cell;

typedef struct Array {  /* symbol table array */
    int         nelem;  /* elements in table right now */
    int         size;   /* size of tab */
    Cell        **tab;  /* hash table pointers */
} Array;

/* Node:  parse tree is made of nodes, with Cell's at bottom */

typedef struct Node {
    node_type   ntype;
    struct Node *nnext;
    int         lineno;
    int         nobj;
    struct Node *narg[1];   /* variable: actual size set by calling malloc */
} Node;

typedef struct rrow {
    long        ltype;      /* long avoids pointer warnings on 64-bit */
    union {
        int         i;
        Node        *np;
        uschar      *up;
    }           lval;       /* because Al stores a pointer in it! */
    int         *lfollow;
} rrow;

typedef struct fa {
    uschar      gototab[NSTATES][NCHARS];
    bool        out[NSTATES];
    char        *restr;
    int         *posns[NSTATES];
    bool        anchor;
    bool        reset;
    int         use;
    int         initstat;
    int         curstat;
    int         accept;
    struct rrow re[1]; /* variable: actual size set by calling malloc */
} fa;

extern int          compile_time;   /* 1 if compiling, 0 if running */
extern bool         safe;           /* false => unsafe, true => safe */

extern size_t       recsize;        /* size of current record, orig RECSIZE */

extern char         **FS;
extern char         **RS;
extern char         **ORS;
extern char         **OFS;
extern char         **OFMT;
extern Awkfloat     *NR;
extern Awkfloat     *FNR;
extern Awkfloat     *NF;
extern char         **FILENAME;
extern char         **SUBSEP;
extern Awkfloat     *RSTART;
extern Awkfloat     *RLENGTH;

extern char         *record;        /* points to $0 */
extern int          lineno;         /* line number in awk program */
extern int          errorflag;      /* 1 if error has occurred */
extern bool         donefld;        /* true if record broken into fields */
extern bool         donerec;        /* true if record is valid (no fld has changed */
extern char         inputFS[];      /* FS at time of input, for field splitting */

extern int          dbg;

extern const char   *patbeg;        /* beginning of pattern matched */
extern size_t       patlen;         /* length of pattern matched.  set in b.c */

extern Array        *symtab;

extern Cell         *nrloc;         /* NR */
extern Cell         *fnrloc;        /* FNR */
extern Cell         *nfloc;         /* NF */
extern Cell         *rstartloc;     /* RSTART */
extern Cell         *rlengthloc;    /* RLENGTH */

extern Node         *winner;
extern Node         *nullstat;
extern Node         *nullnode;

extern bool         pairstack[];
extern Cell         **fldtab;

#include "proto.h"
