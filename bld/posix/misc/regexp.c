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
* Description:  RegComp and RegExec
*
****************************************************************************/

/*
 * RegComp and RegExec
 *
 *      Copyright (c) 1986 by University of Toronto.
 *      Written by Henry Spencer.  Not derived from licensed software.
 *
 *      Modified By Craig Eisler
 *              - editor specific code/errors
 *              - case insensitive search
 *              - CurrentRegComp
 *              - nomagic
 *              - MakeExpressionNonRegular
 *              - case sensitivity atoms (~ and @)
 *              - SetMajickString
 *              - bug fixes, fun fun fun
 *
 *      Permission is granted to anyone to use this software for any
 *      purpose on any computer system, and to redistribute it freely,
 *      subject to the following restrictions:
 *
 *      1. The author is not responsible for the consequences of use of
 *              this software, no matter how awful, even if they arise
 *              from defects in it.
 *
 *      2. The origin of this software must not be misrepresented, either
 *              by explicit claim or by omission.
 *
 *      3. Altered versions must be plainly marked as such, and must not
 *              be misrepresented as being the original software.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "regexp.h"


/* IMPORTANT: must have ^$\\ FIRST */

#ifdef __WATCOMC__
char __near META[] = "^$\\.[()|?+*~@";
#else
char    META[] = "^$\\.[()|?+*~@";
#endif

#if !defined( REALTABS )
#define REALTABS        RealTabs
bool    RealTabs = true;
#endif

#if !defined( CASEIGNORE )
#define CASEIGNORE      CaseIgnore
bool    CaseIgnore = true;
#endif

#if !defined( MAGICFLAG )
#define MAGICFLAG       MagicFlag
bool    MagicFlag = true;
#endif

#if !defined( MAGICSTR )
#define MAGICSTR        MagicString
char    *MagicString = "()";
#endif

#if !defined( ALLOC )
#define ALLOC           malloc
#include <malloc.h>
#endif

#ifdef STANDALONE_RX
int     RegExpError;
#endif

/* regError - set regular expression error */
static void regError( int rc )
{
    RegExpError = rc;
}

/* StrChr - case sensitive/insensitive version of strchr */
static const char *StrChr( const char *s, char c )
{
    if( CASEIGNORE ) {
        while( ( tolower( *s ) != tolower( c ) ) && *s != 0 ) {
            s++;
        }
    } else {
        while( *s != c && *s != 0 ) {
            s++;
        }
    }
    if( !( *s ) ) {
        return( NULL );
    }
    return( s );

}

/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart     char that must begin a match; '\0' if none obvious
 * reganch      is the match anchored (at beginning-of-line only)?
 * regmust      string (pointer into program) that match must include, or NULL
 * regmlen      length of regmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that RegComp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in RegExec() needs it and RegComp() is computing
 * it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */

/* definition   number  opnd?   meaning */
#define END     0       /* no   End of program. */
#define BOL     1       /* no   Match "" at beginning of line. */
#define EOL     2       /* no   Match "" at end of line. */
#define ANY     3       /* no   Match any one character. */
#define ANYOF   4       /* str  Match any character in this string. */
#define ANYBUT  5       /* str  Match any character not in this string. */
#define BRANCH  6       /* node Match this alternative, or the next... */
#define BACK    7       /* no   Match "", "next" ptr points backward. */
#define EXACTLY 8       /* str  Match this string. */
#define NOTHING 9       /* no   Match empty string. */
#define STAR    10      /* node Match this (simple) thing 0 or more times. */
#define PLUS    11      /* node Match this (simple) thing 1 or more times. */
#define CASEI   12      /* no   Set CASEIGNORE to TRUE */
#define NOCASEI 13      /* no   Set CASEIGNORE to FALSE */
#define OPEN    20      /* no   Mark this point in input as start of #n. */
/*      OPEN+1 is number 1, etc. */
#define CLOSE   40      /* no   Analogous to OPEN. */

/*
 * Opcode notes:
 *
 * BRANCH       The set of branches constituting a single choice are hooked
 *              together with their "next" pointers, since precedence prevents
 *              anything being concatenated to any individual branch.  The
 *              "next" pointer of the last BRANCH in a choice points to the
 *              thing following the whole choice.  This is also where the
 *              final "next" pointer of each individual branch points; each
 *              branch starts with the operand node of a BRANCH node.
 *
 * BACK         Normal "next" pointers all implicitly point forward; BACK
 *              exists to make loop structures possible.
 *
 * STAR,PLUS    '?', and complex '*' and '+', are implemented as circular
 *              BRANCH structures using BACK.  Simple cases (one character
 *              per match) are implemented with STAR and PLUS for speed
 *              and to minimize recursive plunges.
 *
 * OPEN,CLOSE   ...are numbered at compile time.
 */

/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
#define OP(p)   (*(p) )
#define NEXT(p) ( ( (*( (p)+1)&0377)<<8) + (*( (p)+2)&0377) )
#define OPERAND(p)      ( (p) + 3)

/* See regmagic.h for one further detail of program structure.  */

/* Utility definitions.  */
#ifndef CHARBITS
#define UCHARAT(p)      ( (int)*(unsigned char *)(p) )
#else
#define UCHARAT(p)      ( (int)*(p)&CHARBITS)
#endif

#define FAIL(m) { regError(m); return(NULL); }
#define ISMULT(c)       ( (c) == '*' || (c) == '+' || (c) == '?')

/* Flags to be passed up and down.  */
#define HASWIDTH        01      /* Known never to match null string. */
#define SIMPLE          02      /* Simple enough to be STAR/PLUS operand. */
#define SPSTART         04      /* Starts with * or +. */
#define WORST           0       /* Worst case. */

/* Global work variables for RegComp().  */
static const char   *regparse;      /* Input-scan pointer. */
static char         regnpar;        /* () count. */
static char         regdummy;
static char         *regcode;       /* Code-emit pointer; &regdummy = don't. */
static long         regsize;        /* Code size. */

/* Forward declarations for RegComp()'s friends.  */
static char *reg( int paren, int *flagp );
static char *regbranch( int *flagp );
static char *regpiece( int *flagp );
static char *regatom( int *flagp );
static char *regnode( char op );
static char *regnext( char *p );
static void regc( char b );
static void reginsert( char op, char * opnd );
static void regtail( char *p, char *val );
static void regoptail( char *p, char *val );

/*
 - RegComp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
regexp *RegComp( const char *instr )
{
    regexp      *r;
    char        *scan;
    char        *longest;
    const char  *exp;
    char        buff[MAX_STR*2];
    int         flags, ignmag = FALSE;
    unsigned    j;
    size_t      i, k, len;

#ifdef WANT_EXCLAMATION
    if( instr[0] == '!' ) {
        instr++;
        ignmag = TRUE;
    }
#endif

    /*
     * flip roles of magic chars
     */
    if( !ignmag && ( !MAGICFLAG && MAGICSTR != NULL ) ) {
        j = 0;
        k = strlen( instr );
        for( i = 0; i < k; i++ ) {
            if( instr[i] == '\\' ) {
                if( strchr( MAGICSTR, instr[i + 1] ) == NULL ) {
                    buff[j++] = '\\';
                }
                i++;
            } else {
                if( strchr( MAGICSTR, instr[i] ) != NULL ) {
                    buff[j++] = '\\';
                }
            }
            buff[j++] = instr[i];

        }
        buff[j] = 0;
        exp = buff;
    } else {
        exp = instr;
    }

    regError( ERR_NO_ERR );
    if( exp == NULL ) {
        FAIL( ERR_RE_NULL_ARGUMENT );
    }

    /* First pass: determine size, legality. */
    regparse = exp;
    regnpar = 1;
    regsize = 0L;
    regcode = &regdummy;
    regc( MAGIC );
    if( reg( 0, &flags ) == NULL ) {
        return( NULL );
    }

    /* Allocate space. */
    r = ALLOC( sizeof( regexp ) + ( unsigned ) regsize );

    /* Second pass: emit code. */
    regparse = exp;
    regnpar = 1;
    regcode = r->program;
    regc( MAGIC );
    if( reg( 0, &flags ) == NULL ) {
        return( NULL );
    }

    /* Dig out information for optimizations. */
    r->regstart = '\0';     /* Worst-case defaults. */
    r->reganch = 0;
    r->regmust = NULL;
    r->regmlen = 0;
    scan = r->program + 1;                    /* First BRANCH. */
    if( OP( regnext( scan ) ) == END ) { /* Only one top-level choice. */
        scan = OPERAND( scan );

        /* Starting-point info. */
        if( OP( scan ) == EXACTLY ) {
            r->regstart = *OPERAND( scan );
        } else if( OP( scan ) == BOL ) {
            r->reganch++;
        }

        /*
         * If there's something expensive in the r.e., find the
         * longest literal string that must appear and make it the
         * regmust.  Resolve ties in favor of later strings, since
         * the regstart check works with the beginning of the r.e.
         * and avoiding duplication strengthens checking.  Not a
         * strong reason, but sufficient in the absence of others.
         */
        if( flags & SPSTART ) {
            longest = NULL;
            len = 0;
            for( ; scan != NULL; scan = regnext( scan ) ) {
                if( OP( scan ) == EXACTLY && strlen( OPERAND( scan ) ) >= len ) {
                    longest = OPERAND( scan );
                    len = strlen( OPERAND( scan ) );
                }
            }
            r->regmust = longest;
            r->regmlen = (short)len;
        }
    }

    return( r );
}

/*
 * reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
static char *reg( int paren, int *flagp )
{
    char        *ret, *br, *ender;
    int         flags;
    char        parno = 0;

    *flagp = HASWIDTH;      /* Tentatively. */

    /* Make an OPEN node, if parenthesized. */
    if( paren ) {
        if( regnpar >= NSUBEXP ) {
            FAIL( ERR_RE_TOO_MANY_ROUND_BRACKETS );
        }
        parno = regnpar;
        regnpar++;
        ret = regnode( OPEN + parno );
    } else {
        ret = NULL;
    }

    /* Pick up the branches, linking them together. */
    br = regbranch( &flags );
    if( br == NULL ) {
        return( NULL );
    }
    if( ret != NULL ) {
        regtail( ret, br );       /* OPEN -> first. */
    } else {
        ret = br;
    }
    if( !( flags & HASWIDTH ) ) {
        *flagp &= ~HASWIDTH;
    }
    *flagp |= flags & SPSTART;
    while( *regparse == '|' ) {
        regparse++;
        br = regbranch( &flags );
        if( br == NULL ) {
            return( NULL );
        }
        regtail( ret, br );       /* BRANCH -> BRANCH. */
        if( !( flags & HASWIDTH ) ) {
            *flagp &= ~HASWIDTH;
        }
        *flagp |= flags & SPSTART;
    }

    /* Make a closing node, and hook it on the end. */
    ender = regnode( ( paren ) ? CLOSE + parno : END );
    regtail( ret, ender );

    /* Hook the tails of the branches to the closing node. */
    for( br = ret; br != NULL; br = regnext( br ) ) {
        regoptail( br, ender );
    }

    /* Check for proper termination. */
    if( paren && *regparse++ != ')' ) {
        FAIL( ERR_RE_UNMATCHED_ROUND_BRACKETS );
    } else if( !paren && *regparse != '\0' ) {
        if( *regparse == ')' ) {
            FAIL( ERR_RE_UNMATCHED_ROUND_BRACKETS );
        } else {
            FAIL( ERR_RE_INTERNAL_FOULUP );    /* "Can't happen". */
        }
    }

    return( ret );
}

/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
static char *regbranch( int *flagp )
{
    char        *ret, *chain, *latest;
    int         flags;

    *flagp = WORST;         /* Tentatively. */

    ret = regnode( BRANCH );
    chain = NULL;
    while( *regparse != '\0' && *regparse != '|' && *regparse != ')' ) {
        latest = regpiece( &flags );
        if( latest == NULL )
            return( NULL );
        *flagp |= flags & HASWIDTH;
        if( chain == NULL ) { /* First piece. */
            *flagp |= flags & SPSTART;
        } else {
            regtail( chain, latest );
        }
        chain = latest;
    }
    if( chain == NULL ) { /* Loop ran zero times. */
        ( void ) regnode( NOTHING );
    }

    return( ret );
}

/*
 - regpiece - something followed by possible [*+?]
 *
 * Note that the branching code sequences used for ? and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
static char *regpiece( int *flagp )
{
    char        *ret, op, *next;
    int         flags;

    ret = regatom( &flags );
    if( ret == NULL ) {
        return( NULL );
    }

    op = *regparse;
    if( !ISMULT( op ) ) {
        *flagp = flags;
        return( ret );
    }

    if( !( flags & HASWIDTH ) && op != '?' ) {
        FAIL( ERR_RE_EMPTY_OPERAND );
    }
    *flagp = ( op != '+' ) ? ( WORST | SPSTART ) : ( WORST | HASWIDTH );

    if( op == '*' && ( flags & SIMPLE ) ) {
        reginsert( STAR, ret );
    } else if( op == '*' ) {
        /* Emit x* as (x&|), where & means "self". */
        reginsert( BRANCH, ret );                       /* Either x */
        regoptail( ret, regnode( BACK ) );              /* and loop */
        regoptail( ret, ret );                          /* back */
        regtail( ret, regnode( BRANCH ) );              /* or */
        regtail( ret, regnode( NOTHING ) );             /* null. */
    } else if( op == '+' && ( flags & SIMPLE ) ) {
        reginsert( PLUS, ret );
    } else if( op == '+' ) {
        /* Emit x+ as x(&|), where & means "self". */
        next = regnode( BRANCH );                       /* Either */
        regtail( ret, next );
        regtail( regnode( BACK ), ret );                /* loop back */
        regtail( next, regnode( BRANCH ) );             /* or */
        regtail( ret, regnode( NOTHING ) );             /* null. */
    } else if( op == '?' ) {
        /* Emit x? as (x|) */
        reginsert( BRANCH, ret );                       /* Either x */
        regtail( ret, regnode( BRANCH ) );              /* or */
        next = regnode( NOTHING );                      /* null. */
        regtail( ret, next );
        regoptail( ret, next );
    }
    regparse++;
    if( ISMULT( *regparse ) ) {
        FAIL( ERR_RE_NESTED_OPERAND );
    }

    return( ret );
}

/*
 * regatom - the lowest level
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Backslashed characters are exceptions, each becoming a
 * separate node; the code is simpler that way and it's not worth fixing.
 */
static char *regatom( int *flagp )
{
    char *ret;
    int flags;

    *flagp = WORST;         /* Tentatively. */

    switch( *regparse++ ) {
    case '~':
        if( *regparse == 0 ) {
            FAIL( ERR_RE_INVALID_CASETOGGLE );
        }
        ret = regnode( CASEI );
        break;
    case '@':
        if( *regparse == 0 ) {
            FAIL( ERR_RE_INVALID_CASETOGGLE );
        }
        ret = regnode( NOCASEI );
        break;
    case '^':
        ret = regnode( BOL );
        break;
    case '$':
        ret = regnode( EOL );
        break;
    case '.':
        ret = regnode( ANY );
        *flagp |= HASWIDTH | SIMPLE;
        break;
    case '[':
        {
            if( *regparse == '^' ) { /* Complement of range. */
                ret = regnode( ANYBUT );
                regparse++;
            } else {
                ret = regnode( ANYOF );
            }
            if( *regparse == ']' || *regparse == '-' ) {
                regc( *regparse++ );
            }
            while( *regparse != '\0' && *regparse != ']' ) {
                if( *regparse == '-' ) {
                    regparse++;
                    if( *regparse == ']' || *regparse == '\0' ) {
                        regc( '-' );
                    } else {
                        int class;
                        int classend;

                        class = UCHARAT( regparse - 2 ) + 1;
                        classend = UCHARAT( regparse );
                        if( class > classend + 1 ) {
                            FAIL( ERR_RE_INVALID_SB_RANGE );
                        }
                        for( ; class <= classend; class++ ) {
                            regc( (char)class );
                        }
                        regparse++;
                    }
                } else {
                    if( *regparse == '\\' && *( regparse + 1 ) == 't' && REALTABS ) {
                        regparse += 2;
                        regc( '\t' );
                    } else {
                        regc( *regparse++ );
                    }
                }
            }
            regc( '\0' );
            if( *regparse != ']' ) {
                FAIL( ERR_RE_UNMATCHED_SQUARE_BRACKET );
            }
            regparse++;
            *flagp |= HASWIDTH | SIMPLE;
        }
        break;
    case '(':
        ret = reg( 1, &flags );
        if( ret == NULL ) {
            return( NULL );
        }
        *flagp |= flags & ( HASWIDTH | SPSTART );
        break;
    case '\0':
    case '|':
    case ')':
        FAIL( ERR_RE_INTERNAL_FOULUP );   /* Supposed to be caught earlier. */
        break;
    case '?':
    case '+':
    case '*':
        FAIL( ERR_RE_OPERAND_FOLLOWS_NOTHING );
        break;
    case '\\':
        if( *regparse == '\0' ) {
            FAIL( ERR_RE_TRAILING_SLASH );
        }
        ret = regnode( EXACTLY );
        if( *regparse == 't' && REALTABS ) {
            regc( '\t' );
            regparse++;
        } else {
            regc( *regparse++ );
        }
        regc( '\0' );
        *flagp |= HASWIDTH | SIMPLE;
        break;
    default:
        {
            size_t len;
            char ender;

            regparse--;
            len = strcspn( regparse, META );
            if( len == 0 ) {
                FAIL( ERR_RE_INTERNAL_FOULUP );
            }
            ender = *( regparse + len );
            if( len > 1 && ISMULT( ender ) ) {
                len--;  /* Back off clear of ?+* operand. */
            }
            *flagp |= HASWIDTH;
            if( len == 1 ) {
                *flagp |= SIMPLE;
            }
            ret = regnode( EXACTLY );
            while( len > 0 ) {
                regc( *regparse++ );
                len--;
            }
            regc( '\0' );
        }
        break;
    }

    return( ret );
}

/*
 * regnode - emit a node
 */
static char *regnode( char op )
{
    char        *ret;

    ret = regcode;
    if( ret == &regdummy ) {
        regsize += 3;
        return( ret );
    }
    *regcode++ = op;
    *regcode++ = '\0';          /* Null "next" pointer. */
    *regcode++ = '\0';

    return( ret );
}

/*
 * regc - emit (if appropriate) a byte of code
 */
static void regc( char b )
{
    if( regcode != &regdummy ) {
        *regcode++ = b;
    } else {
        regsize++;
    }
}

/*
 * reginsert - insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
static void reginsert( char op, char *opnd )
{
    char        *src, *dst;

    if( regcode == &regdummy ) {
        regsize += 3;
        return;
    }

    src = regcode;
    regcode += 3;
    dst = regcode;
    while( src > opnd ) {
        *--dst = *--src;
    }

    /* Op node, where operand used to be. */
    *opnd++ = op;
    *opnd++ = '\0';
    *opnd++ = '\0';
}

/*
 * regtail - set the next-pointer at the end of a node chain
 */
static void regtail( char *p, char *val )
{
    char        *scan, *temp;
    short       offset;

    if( p == &regdummy ) {
        return;
    }

    /* Find last node. */
    scan = p;
    for( ;; ) {
        temp = regnext( scan );
        if( temp == NULL ) {
            break;
        }
        scan = temp;
    }

    if( OP( scan ) == BACK ) {
        offset = (short)( scan - val );
    } else {
        offset = (short)( val - scan );
    }
    *( scan + 1 ) = offset >> 8;
    *( scan + 2 ) = (char)offset;
}

/* regoptail - regtail on operand of first argument; nop if operandless */
static void regoptail( char *p, char *val )
{
    /* "Operandless" and "op != BRANCH" are synonymous in practice. */
    if( p == NULL || p == &regdummy || OP( p ) != BRANCH ) {
        return;
    }
    regtail( OPERAND( p ), val );
}

/* RegExec and friends */

/* * Global work variables for RegExec().  */
static const char   *reginput;      /* String-input pointer. */
static const char   *regbol;        /* Beginning of input, for ^ check. */
static const char   **regstartp;    /* Pointer to startp array. */
static const char   **regendp;      /* Ditto for endp. */

/* Forwards.  */
static bool     regtry( regexp *prog, const char *string );
static bool     regmatch( char *prog );
static size_t   regrepeat( char *p );

/* RegExec2 - match a regexp against a string */
static bool RegExec2( regexp *prog, const char *string, bool anchflag )
{
    const char  *s;

    regError( ERR_NO_ERR );

    /* If there is a "must appear" string, look for it. */
    if( prog->regmust != NULL ) {
        s = string;
        while( ( s = StrChr( s, prog->regmust[0] ) ) != NULL ) {
            if( CASEIGNORE ) {
                if( strnicmp( s, prog->regmust, prog->regmlen ) == 0 ) {
                    break;
                }
            } else {
                if( strncmp( s, prog->regmust, prog->regmlen ) == 0 ) {
                    break;
                }
            }
            s++;
        }
        if( s == NULL )
            return( false ); /* Not present. */
    }

    /* Mark beginning of line for ^ . */
    if( anchflag ) {
        regbol = string;
    } else
        regbol = NULL;

    /* Simplest case:  anchored match need be tried only once. */
    if( prog->reganch ) {
        return( regtry( prog, string ) );
    }

    /* Messy cases:  unanchored match. */
    s = string;
    if( prog->regstart != '\0' ) {
        /* We know what char it must start with. */
        while( ( s = StrChr( s, prog->regstart ) ) != NULL ) {
            if( regtry( prog, s ) ) {
                return( true );
            }
            s++;
        }
    } else {
        /* We don't -- general case. */
        do {
            if( regtry( prog, s ) ) {
                return( true );
            }
        } while( *s++ != '\0' );
    }

    return( false ); /* Failure. */
}

int RegExec( regexp *prog, const char *string, bool anchflag )
{
    bool        oldign;
    bool        rc;

    oldign = CASEIGNORE;
    rc = RegExec2( prog, string, anchflag );
    CASEIGNORE = oldign;
    return( rc );
}

/* regtry - try match at specific point */
static bool regtry( regexp *prog, const char *string )
{
    int         i;
    const char  **sp;
    const char  **ep;

    reginput = string;
    regstartp = prog->startp;
    regendp = prog->endp;

    sp = prog->startp;
    ep = prog->endp;
    for( i = NSUBEXP; i > 0; i-- ) {
        *sp++ = NULL;
        *ep++ = NULL;
    }
    if( regmatch( prog->program + 1 ) ) {
        prog->startp[0] = string;
        prog->endp[0] = reginput;
        return( true );
    } else {
        return( false );
    }
}

/*
 * regmatch - main matching routine
 *
 * Conceptually the strategy is simple:  check to see whether the current
 * node matches, call self recursively to see whether the rest matches,
 * and then act accordingly.  In practice we make some effort to avoid
 * recursion, in particular by going through "ordinary" nodes (that don't
 * need to know whether the rest of the match failed) by a loop instead of
 * by recursion.                      ( false failure, true success )
 */
static bool regmatch( char *prog )
{
    char        *scan;  /* Current node. */
    char        *next;  /* Next node. */

    scan = prog;
    while( scan != NULL ) {
        next = regnext( scan );

        switch( OP( scan ) ) {
        case BOL:
            if( reginput != regbol ) {
                return( false );
            }
            break;
        case EOL:
            if( *reginput != '\0' ) {
                return( false );
            }
            break;
        case ANY:
            if( *reginput == '\0' ) {
                return( false );
            }
            reginput++;
            break;
        case EXACTLY:
            {
                size_t len;
                char *opnd;

                opnd = OPERAND( scan );
                /* Inline the first character, for speed. */
                if( CASEIGNORE ) {
                    if( tolower( *opnd ) != tolower( *reginput ) ) {
                        return( false );
                    }
                } else {
                    if( *opnd != *reginput ) {
                        return( false );
                    }
                }
                len = strlen( opnd );
                if( len > 1 ) {
                    if( CASEIGNORE ) {
                        if( strnicmp( opnd, reginput, len ) != 0 ) {
                            return( false );
                        }
                    } else {
                        if( strncmp( opnd, reginput, len ) != 0 ) {
                            return( false );
                        }
                    }
                }
                reginput += len;
            }
            break;
        case ANYOF:
            if( *reginput == '\0' || StrChr( OPERAND( scan ), *reginput ) == NULL ) {
                return( false );
            }
            reginput++;
            break;
        case ANYBUT:
            if( *reginput == '\0' || StrChr( OPERAND( scan ), *reginput ) != NULL ) {
                return( false );
            }
            reginput++;
            break;
        case NOTHING:
            break;
        case BACK:
            break;
        case OPEN + 1:
        case OPEN + 2:
        case OPEN + 3:
        case OPEN + 4:
        case OPEN + 5:
        case OPEN + 6:
        case OPEN + 7:
        case OPEN + 8:
        case OPEN + 9:
        case OPEN + 10:
        case OPEN + 11:
        case OPEN + 12:
        case OPEN + 13:
        case OPEN + 14:
        case OPEN + 15:
        case OPEN + 16:
        case OPEN + 17:
        case OPEN + 18:
        case OPEN + 19:
            {
                int no;
                const char *save;

                no = OP( scan ) - OPEN;
                save = reginput;

                if( regmatch( next ) ) {
                    /*
                 * Don't set startp if some later
                 * invocation of the same parentheses
                 * already has.
                 */
                    if( regstartp[no] == NULL ) {
                        regstartp[no] = save;
                    }
                    return( true );
                }
                return( false );
            }
            break;
        case CLOSE + 1:
        case CLOSE + 2:
        case CLOSE + 3:
        case CLOSE + 4:
        case CLOSE + 5:
        case CLOSE + 6:
        case CLOSE + 7:
        case CLOSE + 8:
        case CLOSE + 9:
        case CLOSE + 10:
        case CLOSE + 11:
        case CLOSE + 12:
        case CLOSE + 13:
        case CLOSE + 14:
        case CLOSE + 15:
        case CLOSE + 16:
        case CLOSE + 17:
        case CLOSE + 18:
        case CLOSE + 19:
            {
                int no;
                const char *save;

                no = OP( scan ) - CLOSE;
                save = reginput;

                if( regmatch( next ) ) {
                    /*
                     * Don't set endp if some later
                     * invocation of the same parentheses
                     * already has.
                     */
                    if( regendp[no] == NULL ) {
                        regendp[no] = save;
                    }
                    return( true );
                }
                return( false );

            }
            break;
#if 0
        case MATCHPREV + 1:
        case MATCHPREV + 2:
        case MATCHPREV + 3:
        case MATCHPREV + 4:
        case MATCHPREV + 5:
        case MATCHPREV + 6:
        case MATCHPREV + 7:
        case MATCHPREV + 8:
        case MATCHPREV + 9:
        case MATCHPREV + 10:
        case MATCHPREV + 11:
        case MATCHPREV + 12:
        case MATCHPREV + 13:
        case MATCHPREV + 14:
        case MATCHPREV + 15:
        case MATCHPREV + 16:
        case MATCHPREV + 17:
        case MATCHPREV + 18:
        case MATCHPREV + 19:
            {
                int no;
                char *curr, *save;

                no = OP( scan ) - OPEN;

                if( regstartp[no] == NULL || regendp[no] == NULL )
                    return( false );
                curr = regstartp[no];
                save = reginput;
                if( CASEIGNORE ) {
                    while( 1 ) {
                        if( tolower( *curr ) == tolower( *reginput ) ) {
                            if( curr == regendp[no] ) {
                                return( true );
                            }
                            curr++;
                            reginput++;
                        } else {
                            reginput = save;
                            return( false );
                        }
                    }
                } else {
                    while( 1 ) {
                        if( *curr == *reginput ) {
                            if( curr == regendp[no] ) {
                                return( true );
                            }
                            curr++;
                            reginput++;
                        } else {
                            reginput = save;
                            return( false );
                        }
                    }

                }
            }
            break;
#endif
        case BRANCH:
            {
                const char *save;

                if( OP( next ) != BRANCH ) { /* No choice. */
                    next = OPERAND( scan );   /* Avoid recursion. */
                } else {
                    do {
                        save = reginput;
                        if( regmatch( OPERAND( scan ) ) ) {
                            return( true );
                        }
                        reginput = save;
                        scan = regnext( scan );
                    } while( scan != NULL && OP( scan ) == BRANCH );
                    return( false );
                    /* NOTREACHED */
                }
            }
            break;
        case STAR:
        case PLUS:
            {
                char nextch;
                size_t no;
                const char *save;
                unsigned min;

                /*
                 * Lookahead to avoid useless match attempts
                 * when we know what character comes next.
                 */
                nextch = '\0';
                if( OP( next ) == EXACTLY ) {
                    nextch = *OPERAND( next );
                }
                min = ( OP( scan ) == STAR ) ? 0 : 1;
                save = reginput;
                no = regrepeat( OPERAND( scan ) );
                while( no >= min ) {
                    /* If it could work, try it. */
                    if( CASEIGNORE ) {
                        if( nextch == '\0' || tolower( *reginput ) == tolower( nextch ) ) {
                            if( regmatch( next ) ) {
                                return( true );
                            }
                        }
                    } else {
                        if( nextch == '\0' || *reginput == nextch ) {
                            if( regmatch( next ) ) {
                                return( true );
                            }
                        }
                    }
                    /* Couldn't or didn't -- back up. */
                    no--;
                    reginput = save + no;
                }
                return( false );
            }
            break;
        case CASEI:
            CASEIGNORE = true;
            break;
        case NOCASEI:
            CASEIGNORE = false;
            break;
        case END:
            return( true );      /* Success! */
            break;
        default:
            regError( ERR_RE_MEMORY_CORRUPTION );
            return( false );
            break;
        }

        scan = next;
    }

    /*
     * We get here only if there's trouble -- normally "case END" is
     * the terminating point.
     */
    regError( ERR_RE_CORRUPTED_POINTER );
    return( false );
}

/* regrepeat - repeatedly match something simple, report how many */
static size_t regrepeat( char *p )
{
    size_t      count = 0;
    const char  *scan;
    char        *opnd;

    scan = reginput;
    opnd = OPERAND( p );
    switch( OP( p ) ) {
    case ANY:
        count = strlen( scan );
        scan += count;
        break;
    case EXACTLY:
        if( CASEIGNORE ) {
            while( tolower( *opnd ) == tolower( *scan ) ) {
                count++;
                scan++;
            }
        } else {
            while( *opnd == *scan ) {
                count++;
                scan++;
            }
        }
        break;
    case ANYOF:
        while( *scan != '\0' && StrChr( opnd, *scan ) != NULL ) {
            count++;
            scan++;
        }
        break;
    case ANYBUT:
        while( *scan != '\0' && StrChr( opnd, *scan ) == NULL ) {
            count++;
            scan++;
        }
        break;
    default:                /* Oh dear.  Called inappropriately. */
        regError( ERR_RE_INTERNAL_FOULUP );
        count = 0;      /* Best compromise. */
        break;
    }
    reginput = scan;
    return( count );
}

/* regnext - dig the "next" pointer out of a node */
static char *regnext( char *p )
{
    int offset;

    if( p == &regdummy ) {
        return( NULL );
    }

    offset = NEXT( p );
    if( offset == 0 ) {
        return( NULL );
    }

    if( OP( p ) == BACK ) {
        return( p - offset );
    } else {
        return( p + offset );
    }
}

void RegAnchor( regexp * reg )
/****************************/
/* mark a regular expression as being "anchored", i.e. as an expression that
 * occurs at the beginning-of-line only */
{
    if( reg != NULL ) {
        reg->reganch = TRUE;
    }
}
