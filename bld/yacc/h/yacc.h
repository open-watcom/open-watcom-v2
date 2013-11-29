/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include "bool.h"
#include "clibext.h"

enum {
    TOKEN_EOF           = 0x0000,
    TOKEN_IMPOSSIBLE    = 0x0001,
    TOKEN_ERROR         = 0x0002,
    TOKEN_DENSE_BASE    = 0x0003, TOKEN_DENSE_MAX       = 0x00ff,
    TOKEN_SPARSE_BASE   = 0x007f, TOKEN_SPARSE_MAX      = 0x7fff,
};

#define MAX_AMBIGS      10

#define WSIZE           (sizeof(a_word)*8)

#define ACTION_NULL     0
#define ACTION_MASK     0x3FFF

#define ClearBit(x,i)   ((x)[(i)/WSIZE] &= ~( 1UL << ((i) % WSIZE )))
#define SetBit(x,i)     ((x)[(i)/WSIZE] |= ( 1UL << ((i) % WSIZE )))
#define IsBitSet(x,i)   ((x)[(i)/WSIZE] &  ( 1UL << ((i) % WSIZE )))

typedef enum flags {
    M_NULL              = 0x00,
    M_MARKED            = 0x01,
    M_STATE             = 0x02,
    M_DEAD              = 0x04,
    M_AMBIGUOUS         = 0x08,
    M_DONT_OPTIMIZE     = 0x10,
    M_ONLY_REDUCE       = 0x20
} flags;

#define IsState(c)              ((c)->flag &   M_STATE)
#define State(c)                ((c)->flag |=  M_STATE)
#define IsMarked(c)             ((c)->flag &   M_MARKED)
#define Mark(c)                 ((c)->flag |=  M_MARKED)
#define Unmark(c)               ((c)->flag &= ~M_MARKED)
#define IsDead(c)               ((c)->flag &   M_DEAD)
#define Dead(c)                 ((c)->flag |=  M_DEAD)
#define IsAmbiguous(c)          ((c)->flag &   M_AMBIGUOUS)
#define Ambiguous(c)            ((c)->flag |=  M_AMBIGUOUS)
#define IsDontOptimize(c)       ((c)->flag &   M_DONT_OPTIMIZE)
#define DontOptimize(c)         ((c)->flag |=  M_DONT_OPTIMIZE)
#define IsOnlyReduce(c)         ((c)->flag &   M_ONLY_REDUCE)
#define OnlyReduce(c)           ((c)->flag |=  M_ONLY_REDUCE)

enum assoc_t {
    NON_ASSOC           = 0,
    L_ASSOC             = 1,
    R_ASSOC             = 2
};

typedef struct a_prec {
    enum assoc_t        assoc;
    unsigned char       prec;
} a_prec;

typedef unsigned int    a_word;
typedef unsigned char   byte;
typedef unsigned short  set_size;

typedef unsigned short  token_n;
typedef unsigned short  action_n;
typedef unsigned short  index_n;
typedef unsigned short  base_n;
typedef unsigned short  rule_n;

typedef struct a_state  a_state;
typedef struct a_sym    a_sym;
typedef struct a_pro    a_pro;
typedef struct a_parent a_parent;
typedef struct a_SR_conflict a_SR_conflict;
typedef struct a_SR_conflict_list a_SR_conflict_list;
typedef struct a_link   a_link;

typedef struct an_item {
    union {
        a_sym           *sym;
        a_pro           *pro;
    }                   p;
    flags               flag;
} an_item;

struct a_SR_conflict {
    a_SR_conflict       *next;
    a_sym               *sym;           /* lookahead token causing ambiguity */
    a_state             *state;         /* final state that contains ambigity */
    a_state             *shift;         /* state if we were to shift token */
    a_SR_conflict_list  *thread;        /* all registered productions */
    unsigned            id;             /* numeric id assigned by user */
    index_n             reduce;         /* rule if we were to reduce on token */
};

struct a_SR_conflict_list {
    a_SR_conflict_list  *next;          /* list associated with a production */
    a_SR_conflict_list  *thread;        /* list associated with a SR conflict */
    a_pro               *pro;
    a_SR_conflict       *conflict;
};

struct a_pro {                          /* production: LHS -> RHS1 RHS2 ... */
    a_pro               *next;
    rule_n              pidx;           /* index of production [0..npro] */
    a_prec              prec;
    a_sym               *sym;           /* LHS of production */
    a_SR_conflict_list  *SR_conflicts;  /* list of S/R conflicts */
    unsigned            used : 1;       /* has rule been reduced */
    unsigned            unit : 1;       /* LHS -> RHS and no action specified */
    an_item             items[2];       /* must be the last field */
};

struct a_sym {                          /* symbol: terminal or non-terminal */
    a_sym               *next;
    char                *name;
    char                *type;
    char                *min;
    a_pro               *pro;           /* productions with this symbol as LHS*/
    a_state             *enter;
    unsigned            nullable : 1;
    a_prec              prec;
    index_n             idx;
    token_n             token;
};

typedef struct a_shift_action {
    a_sym               *sym;
    a_state             *state;
    unsigned            units_checked : 1;
    unsigned            is_default : 1;
} a_shift_action;

typedef struct a_look {
    a_shift_action      *trans;
    a_word              *follow;
    a_link              *include;
    unsigned short      depth;
} a_look;

typedef struct a_reduce_action {
    a_pro               *pro;
    a_word              *follow;
} a_reduce_action;

struct a_link {
    a_link              *next;
    a_look              *el;
};

struct a_state {
    a_state             *next;
    an_item             **items;
    a_shift_action      *trans;
    a_reduce_action     *redun;
    a_reduce_action     *default_reduction;
    a_parent            *parents;
    a_look              *look;
    a_state             *same_enter_sym;
    unsigned short      kersize;
    action_n            sidx;           /* index of state [0..nstates] */
    flags               flag;
};

struct a_parent {
    a_parent            *next;
    a_state             *state;
};

typedef enum value_size {
    FITS_A_BYTE,
    FITS_A_WORD
} value_size;

extern void     InitSets(unsigned );
extern a_word   *AllocSet( unsigned );
extern unsigned GetSetSize( unsigned );
extern void     Union(a_word *,a_word *);
extern void     Intersection( a_word *, a_word *);
extern void     Assign(a_word *,a_word *);
extern void     Clear(a_word *);
extern bool     Empty(a_word *);
extern void     AndNot(a_word *,a_word *);
extern void     UnionAnd(a_word *,a_word *,a_word *);
extern set_size *Members(a_word *);
extern bool     EmptyIntersection( a_word *, a_word * );
extern bool     Equal( a_word *, a_word * );
extern void     DumpSet(a_word *);
extern unsigned Cardinality( a_word * );

extern void     buildpro(void);
extern a_sym    *findsym(char *);
extern a_sym    *addsym(char *);
extern a_pro    *addpro(a_sym *,a_sym **,int );
extern void     showpro(void);
extern void     showitem(an_item *,char *);
extern void     show_unused(void);

extern void     lalr1(void );
extern void     showstates(void);
extern void     showstate(a_state *);

extern void     lr0(void );
extern void     SetupStateTable(void);
extern void     RemoveDeadStates( void );
extern void     MarkDefaultReductions( void );
extern void     CalcMinSentence( void );

extern void     EliminateUnitReductions(void);
extern void     MarkDefaultShifts( void );
extern void     ShowSentence( a_state *, a_sym *, a_pro *, a_state * );

extern void     FindUnused( void );

extern char     *getname( char * );

extern FILE     *fpopen( char *, char * );

extern void     defs(void);
extern void     rules(void);
extern void     parsestats( void );
extern void     tail(void);

extern void     genobj(void);

extern void     msg( char *, ... );
extern void     warn( char *, ... );
extern void     dumpstatistic( char *name, unsigned stat );

extern void     MarkNoUnitRuleOptimizationStates( void );

extern void     GenFastTables( void );

extern token_n  FirstNonTerminalTokenValue( void );
extern void     endtab( void );
extern void     putcompact( token_n token, action_n action );
extern void     begtab( char *tipe, char *name );
extern void     putnum( char *name, int i );
extern void     putambigs( base_n *base );
extern void     puttab( value_size fits, unsigned i );
extern void     puttokennames( token_n dtoken, value_size token_size );
extern void     putcomment( char *comment );

extern rule_n   npro;    /* # of productions */
extern index_n  nsym;    /* # of symbols */
extern index_n  nterm;   /* # of terminals */
extern index_n  nvble;   /* # of non-terminals */
extern index_n  nitem;   /* # of LR(0) items */

extern index_n  nbstate;
extern action_n nstate;
extern index_n  nvtrans;
extern index_n  nredun;
extern index_n  nstate_1_reduce;

extern index_n  RR_conflicts;
extern index_n  SR_conflicts;

extern char     lineflag;
extern char     bigflag;
extern char     denseflag;
extern char     default_shiftflag;
extern char     compactflag;
extern char     fastflag;
extern char     showflag;
extern char     translateflag;
extern char     defaultwarnflag;

extern a_sym    **symtab;
extern a_sym    *symlist;
extern a_sym    *startsym;
extern a_sym    *eofsym;
extern a_sym    *errsym;
extern a_sym    *goalsym;
extern a_sym    *nosym;
extern a_state  **statetab;
extern a_state  *statelist;
extern a_state  **statetail;
extern a_state  *startstate;
extern a_state  *errstate;
extern a_pro    **protab;
extern a_pro    *startpro;

extern set_size *setmembers;

extern char     *srcname;

extern FILE     *yaccin;
extern FILE     *actout;
extern FILE     *tokout;

extern int      lineno;

extern a_SR_conflict *ambiguousstates;

extern unsigned long bytesused;

extern unsigned keyword_id_low;
extern unsigned keyword_id_high;
