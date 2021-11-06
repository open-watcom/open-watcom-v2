/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Macro parsing and processing routines.
*
****************************************************************************/


#if !defined( __UNIX__ )    // NAME_MAX clashes with that in UNIX <limits.h>
    #include <direct.h>     // Needed for getcwd()
#else
    #include <sys/types.h>  // Implicitly included by <direct.h>
#endif
#include <stdlib.h>
#include <ctype.h>
#include "make.h"
#include "wio.h"
#include "mstream.h"
#include "mlex.h"
#include "mhash.h"
#include "macros.h"
#include "mmemory.h"
#include "mmisc.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#include "mvecstr.h"
#include "pathgrp2.h"

#include "clibext.h"


/* arbitrarily set the length of the command line */
#define MAX_COMMANDLINE     (_MAX_PATH * 5)

/*
 * macros are stored in a hash table
 */
#define HASH_PRIME  37

typedef struct Macro {
    HASHNODE    node;       /* name is at name.node */
    const char  *value;
    bool        readonly;   /* ie: from command line */
} MACRO;

STATIC HASHTAB  *macTab;    /* head for the macro lookup table */

/*
 * dirBuf is a static buffer used by procPath to store the result of a
 * form-specifier macro; and it is used by GetMacroValue for %CDRIVE/%CWD.
 * It is not allocated until needed.
 */
STATIC char     *dirBuf;

/*
 * This variable is to be able to do partial deMacro intead of full deMacro
 * for ms-option.  This is needed by the inference rules
 * in ms nmake at which it deMacros the macros latter on in time
 */
bool            ImplicitDeMacro;

/*
 *   Only Useful in ms option if it is partDeMacro then we only deMacro
 *   everything except for special DeMacro characters
 *
 */
bool            IsPartDeMacro;

bool            DoingBuiltIn;   /* Are we parsing builtin macros            */

#ifdef CLEAN_ENVIRONMENT_VAR
/* This contains the old environment values before wmake actually */
/* redefines them -  only valid in ms-option */
STATIC ELIST    *OldEnvValues;
#endif

STATIC time_t   start_time = 0;

STATIC char *getDirBuf( void )
/****************************/
{
    if( dirBuf == NULL ) {
        dirBuf = MallocSafe( _MAX_PATH );
    }
    return( dirBuf );
}


static void massageDollarOctothorpe( char *p )
/********************************************/
{
    assert( p != NULL );
    for( ; *p != NULLCHAR; ++p ) {
        switch( *p ) {
        case '$':
            *p = TMP_DOLLAR;
            break;
        case '#':
            *p = TMP_COMMENT;
            break;
        }
    }
}


const char *procPath( const char *fullpath )
/**************************************************
 * process fullpath according to the form qualifier in CurAttr.num
 * returns: pointer to a static buffer
 */
{
    pgroup2     pg;
    char        *p;

    if( fullpath == NULL ) {
        return( NULL );
    }

    getDirBuf();

    _splitpath2( fullpath, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );

    switch( CurAttr.u.form ) {
    case FORM_FULL:
        _makepath( dirBuf, pg.drive, pg.dir, pg.fname, pg.ext );
        break;
    case FORM_NOEXT:
        _makepath( dirBuf, pg.drive, pg.dir, pg.fname, NULL );
        break;
    case FORM_NOEXT_NOPATH:
        _makepath( dirBuf, NULL, NULL, pg.fname, NULL );
        break;
    case FORM_NOPATH:
        _makepath( dirBuf, NULL, NULL, pg.fname, pg.ext );
        break;
    case FORM_PATH:
        _makepath( dirBuf, pg.drive, pg.dir, NULL, NULL );
        if( Glob.compat_nmake ) {
            if( dirBuf[0] == NULLCHAR ) {
                dirBuf[0] = '.';
                dirBuf[1] = NULLCHAR;
            } else {
                p = dirBuf + strlen( dirBuf ) - 1;
                if( cisdirc( *p ) && *p != ':' ) {
                    *p = NULLCHAR;
                }
            }
        }
        break;
    case FORM_EXT:
        _makepath( dirBuf, NULL, NULL, NULL, pg.ext );
        break;
    default:
        dirBuf[0] = NULLCHAR;
    }

    massageDollarOctothorpe( dirBuf );
    return( dirBuf );
}


STATIC const char *specialValue( MTOKEN_T t )
/********************************************
 * process the special macro t
 * returns: pointer to a static buffer
 */
{
    char const  *dirBufGot = NULL;

    assert( t == MAC_CUR || t == MAC_FIRST || t == MAC_LAST );
    assert( FORM_MIN < CurAttr.u.form && CurAttr.u.form < FORM_MAX );

    switch( t ) {
    case MAC_CUR:       dirBufGot = GetCurTarg();  break;
    case MAC_FIRST:     dirBufGot = GetFirstDep(); break;
    case MAC_LAST:      dirBufGot = GetLastDep();  break;
    default:    break;
    }
    return( procPath( dirBufGot ) );
}


STATIC void makeMacroName( char *buffer, const char *name )
/**********************************************************
 * convert name to internal form (upcased)
 * buffer must be at least as large as name
 * Microsoft and POSIX environment variables are case sensitive
 */
{
    assert( IsMacroName( name ) );

    if( Glob.compat_nmake || Glob.compat_posix ) {
        strcpy( buffer, name );
    } else {
        while( (*buffer = ctoupper( *name )) != NULLCHAR ) {
            ++buffer, ++name;
        }
    }
}


STATIC MACRO *getMacroNode( const char *name )
/*********************************************
 * returns: pointer to MACRO with this name
 */
{
    bool    caseSensitive;

    assert( name != NULL && *name != ENVVAR_C );

    if( Glob.compat_nmake || Glob.compat_posix ) {
        caseSensitive = CASESENSITIVE;
    } else {
        caseSensitive = NOCASESENSITIVE;
    }

    return( (MACRO *)FindHashNode( macTab, name, caseSensitive ) );
}


STATIC char *findEqual( char *inString )
/***************************************
 * returns" pointer to equals sign if found
 */
{
    char    *ret;

    ret = strrchr( inString, '=' );
    if( ret == inString ) {
        ret = NULL;
    }

    return( ret );
}


STATIC bool getOldNewString( char *inString, const char **oldString, const char **newString )
/*******************************************************************************************/
{
    char    *equal;

    equal = findEqual( inString );

    if( equal == NULL ) {
        return( false );
    } else {
        *oldString = inString;
        *equal     = NULLCHAR;
        *newString = equal + 1;
        return( true );
    }
}


STATIC char *doStringSubstitute( const char *name, const char *oldString, const char *newString )
/************************************************************************************************
 *   $(macroname:oldstr=newstr)
 *   substitute any occurence of oldstr with new str
 */
{
    VECSTR      output;
    char const  *p;
    char const  *start;
    size_t      old_len;

    output = StartVec();

    assert( name != NULL && oldString != NULL && newString != NULL );

    old_len = strlen( oldString );
    for( start = p = name; *p != NULLCHAR; p++ ) {
        if( strncmp( p, oldString, old_len ) == 0 ) {
            WriteNVec( output, start, p - start );
            WriteVec( output, newString );
            start   = p + old_len;
            p = start - 1;
        }
    }
    WriteVec( output, start );

    return( FinishVec( output ) );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC const char *GetMacroValueProcess( const char *name )
/**********************************************************
 * returns: pointer to text of macro (incl. environment vars)
 */
{
    char    macro[MAX_MAC_NAME];
    MACRO   *cur;
    char    *env;
    bool    cdrive;
    bool    cwd;
    bool    ctime;
    bool    cdate;
    bool    cyear;
    char    *p;
    int     pos;

    makeMacroName( macro, name ); // Does assert( IsMacroName( name ) );

    if( *macro == ENVVAR_C ) {
        env = GetEnvExt( macro + 1 );
        if( env != NULL ) {
            return( env );
        }
        cdrive = strcmp( macro + 1, "CDRIVE" ) == 0 ||
                 strcmp( macro + 1, "__CDRIVE__" ) == 0;
        cwd    = strcmp( macro + 1, "CWD" ) == 0 ||
                 strcmp( macro + 1, "__CWD__" ) == 0;
        ctime  = strcmp( macro + 1, "__CTIME__" ) == 0;
        cdate  = strcmp( macro + 1, "__CDATE__" ) == 0;
        cyear  = strcmp( macro + 1, "__CYEAR__" ) == 0;
        if( cdrive || cwd ) {
            if( getcwd( getDirBuf(), _MAX_PATH ) == NULL ) {
                return( NULL );
            }
            p = strchr( dirBuf, ':' );
            if( cdrive ) {
                if( p != NULL ) {
                    *p = NULLCHAR;
                } else {
                    dirBuf[0] = NULLCHAR;
                }
            } else {    /* cwd */
                if( p != NULL ) {
                    return( p + 1 );
                }
            }
            return( dirBuf );
        } else if( ctime || cdate || cyear ) {
            struct tm   *tm;

            tm = localtime( &start_time );
            if( ctime ) {
                FmtStr( getDirBuf(), "%D:%D:%D", tm->tm_hour, tm->tm_min, tm->tm_sec );
            } else if( cdate ) {
                FmtStr( getDirBuf(), "%d-%D-%D", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday );
            } else if( cyear ) {
                FmtStr( getDirBuf(), "%d", tm->tm_year + 1900 );
            }
            return( dirBuf );
        }
        return( NULL );
    }

    cur = getMacroNode( macro );
    if( cur != NULL ) {
        return( cur->value );
    }

    // If not defined as a macro then get it as a Environment variable
    if( Glob.compat_nmake || Glob.compat_posix ) {
        // Check if macro is all caps in NMAKE mode
        if( Glob.compat_nmake ) {
            for( pos = 0; macro[pos] != NULLCHAR; ++pos ) {
                if( macro[pos] != ctoupper( macro[pos] ) ) {
                    return( NULL );
                }
            }
        }
        env = GetEnvExt( macro );
        if( env != NULL ) {
            return( env );
        }
    }

    return( NULL );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


char *GetMacroValue( const char *name )
/*********************************************
 * Now we need to check for string substitution
 * $(MACRONAME:oldstring=newstring)
 */
{
    char        *InName;
    const char  *beforeSub;
    char        *afterSub;
    char        *p;
    const char  *new;
    const char  *old;
    char        *line;

    InName = StrDupSafe( name );
    p = strchr( InName, ':' );

    if( p == NULL ) {
        beforeSub = GetMacroValueProcess( InName );
        if( beforeSub == NULL ) {
            afterSub = NULL;
        } else {
            afterSub  = StrDupSafe( beforeSub );
        }
    } else {
        *p++ = NULLCHAR;
        beforeSub = GetMacroValueProcess( InName );
        if( beforeSub == NULL ) {
            afterSub = NULL;
        } else {
            line = NULL;
            // recursively expand so $(macro:sub) OK if macro contains another
            if( strchr( beforeSub, '$' ) != NULL ) {
                UnGetCHR( STRM_MAGIC );
                InsString( beforeSub, false );
                beforeSub = line = DeMacro( TOK_MAGIC );
                GetCHR();   // eat STRM_MAGIC
            }
            if( beforeSub == NULL ) {
                afterSub = NULL;
            } else {
                if( getOldNewString( p, &old, &new ) ) {
                    afterSub = doStringSubstitute( beforeSub, old, new );
                } else {
                    afterSub = NULL;
                    PrtMsg( ERR | LOC | INVALID_STRING_SUBSTITUTE );
                }
                if( line != NULL ) {
                    FreeSafe( line );
                }
            }
        }
    }

    FreeSafe( InName );
    return( afterSub );
}


STATIC char *trimMacroValue( char *v )
/************************************/
{
    bool    space;
    char    *t;
    char    *p;

    space = false;
    t = v;
    for( p = v; *p != NULLCHAR; ++p ) {
        if( !cisws( *p ) ) {
            if( space ) {
                *t++ = ' ';
            }
            *t++ = *p;
            space = false;
        } else {
            space = true;
        }
    }
    *t = NULLCHAR;
    return( v );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC bool addMacro( const char *name, char *value )
/****************************************************
 * post:    new macro possibly allocated, copy of name made
 */
{
    char    macro[MAX_MAC_NAME];
    MACRO   *new;
    bool    unused_value;

    assert( *name != ENVVAR_C );

    value = trimMacroValue( value );
    makeMacroName( macro, name ); // Does assert( IsMacroName( name ) );

    new = getMacroNode( macro );     /* check if redefinition */

    unused_value = false;
    if( new != NULL && !new->readonly ) {   /* reuse old node */
        FreeSafe( (void *)new->value );
        new->value = value;
        new->readonly = Glob.macreadonly;
    } else if( new == NULL ) {
        new = MallocSafe( sizeof( *new ) ); /* get memory for new node */
        new->node.name = StrDupSafe( macro );
        new->value = value;
        new->readonly = Glob.macreadonly;
        AddHashNode( macTab, (HASHNODE *)new );
    } else {
        unused_value = true;
    }
    return( unused_value );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


bool IsMacroName( const char *inName )
/*************************************
 * returns: true if name is a valid macro name, otherwise false and print
 * an error message
 */
{
    char const  *p = inName;
    int         pos = 0;

    assert( inName != NULL );

    while( pos < MAX_MAC_NAME && *p != NULLCHAR && *p != ':' ) {
        if( !cismacc( *p ) ) {
            PrtMsg( ERR | LOC | INVALID_MACRO_NAME, inName );
            return( false );
        }
        pos++, p++;
    }
    if( pos == 0 ) {
        PrtMsg( ERR | LOC | INVALID_MACRO_NAME, inName );
        return( false );
    }
    if( pos >= MAX_MAC_NAME ) {
        PrtMsg( ERR | LOC | MAXIMUM_TOKEN_IS, MAX_MAC_NAME - 1 );
        return( false );
    }

    return( true );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
void UnDefMacro( const char *name )
/*****************************************
 * pre:     IsMacroName( name ); getMacroNode( name ) != NULL
 * post:    MACRO node deallocated
 */
{
    char    macro[MAX_MAC_NAME];
    MACRO   *dead;
    bool    caseSensitive;

    makeMacroName( macro, name ); // Does assert( IsMacroName( name ) );

    if( *macro == ENVVAR_C ) {
        SetEnvSafe( macro + 1, NULL );
        return;
    }

    if( Glob.compat_nmake || Glob.compat_posix ) {
        caseSensitive = CASESENSITIVE;
    } else {
        caseSensitive = NOCASESENSITIVE;
    }

    dead = (MACRO *)RemHashNode( macTab, macro, caseSensitive );

    assert( dead != NULL );

    FreeSafe( dead->node.name );
    FreeSafe( (void *)dead->value );
    FreeSafe( dead );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


char *WrnGetMacroValue( const char *name )
/***********************************************/
{
    char    *p;

    p = GetMacroValue( name );
    if( p == NULL ) {
        PrtMsg( DBG | WRN | LOC | MACRO_UNDEFINED, name );
        // we did this to minimize the number of debugging messages but
        // it causes problems when it defines a macro for the user
        //UnGetCHR( '\n' );
        //DefMacro( name );
    }
    /* note we return NULL if it was undefined! */
    return( p );
}


char *DeMacroSpecial( const char *InString )
/*******************************************
 * This function is to specially handle the special macros
 * in the dependencies
 */
{
    const char  *old;
    const char  *p;
    VECSTR      outString;
    char        *tempString;
    char        buffer[6];
    int         pos;

    assert( InString != NULL );
    old     = InString;

    outString = StartVec();

    for( p = InString; *p != NULLCHAR; ++p ) {
        if( *p == SPECIAL_TMP_DOLLAR ) {
            WriteNVec( outString, old, p - old );
            pos = 0;
            UnGetCHR( STRM_MAGIC );
            buffer[pos++] = *(p++);
            if( cismsspecial( *p ) && !cismsmodifier( *(p + 1) ) ) {
                buffer[pos++] = *(p++);
            } else {
                assert( cismsspecial( *p ) );
                buffer[pos++] = *(p++);
                if( cismsmodifier( *p ) ) {
                    buffer[pos++] = *(p++);
                }
            }
            old = p;
            buffer[pos] = NULLCHAR;
            InsString( buffer, false );
            tempString = DeMacro( TOK_MAGIC );
            PreGetCHR();   // eat STRM_MAGIC
            WriteVec( outString, tempString );
            FreeSafe( tempString);
        }
    }
    WriteNVec( outString, old, p - old + 1 );
    return( FinishVec( outString ) );
}


/*
 * end1 is what the caller passed to DeMacro - it is pushed back into the
 * input stream.
 * end2 is for our own purposes
 *
 * deMacroText calls deMacroToEnd, and deMacroToEnd might call deMacroText.
 *
 * deMacroToEnd does a single pass of substitution.  It calls deMacroText to
 * handle constructs such as $(text); thus allowing $(text$(subtext)).  All
 * other constructs are written with their current value.
 *
 * deMacroText scans the string that deMacroToEnd returned for '$'s.  If
 * it doesn't find one it returns the string.  If a '$' is found, a
 * STRM_MAGIC is pushed, the string is pushed, and deMacroToEnd is called
 * again (ending at STRM_MAGIC).  This process is repeated until no '$'s
 * remain.
 *
 * This pair always stops at EOL, STRM_MAGIC, or STRM_END.
 *
 * Note that if a MAC_PUNC is passed for one of the end tokens, it is
 * assumed to imply MAC_WS as well.  This is used in mpreproc.c to find the
 * end of a macro name for things such as !ifdef, !undef, etc.
 */
STATIC char *deMacroText( int depth, MTOKEN_T end1, MTOKEN_T end2 );


STATIC char *ProcessToken( int depth, MTOKEN_T end1, MTOKEN_T end2, MTOKEN_T t )
/*******************************************************************************
 * Processes the tokens returned from lexToken in deMacroToEnd
 */
{
    STRM_T      s;
    char        macname[MAX_TOK_SIZE];
    int         pos;
    char        *p;
    char        *p2;
    char const  *cp2;

    switch( t ) {
    case MAC_START:                     /* recurse, get macro name */
        /*
         * in microsoft nmake and in POSIX mode,
         * $HELLO is considered as $(H)ELLO
         * if there are no parentheses then it takes only first char.
         * after the $
         */
        if( !Glob.compat_nmake && !Glob.compat_posix ) {
            p = deMacroText( depth + 1, end1, MAC_PUNC );
        } else {
            s = PreGetCHR();
            if( sismacc( s ) ) {
                p = CharToStrSafe( s );
            } else {
                p = CharToStrSafe( NULLCHAR );
            }
        }

        if( IsMacroName( p ) ) {
            p2 = WrnGetMacroValue( p );
            FreeSafe( p );
            return( p2 );
        }
        FreeSafe( p );
        break;
    case MAC_DOLLAR:
        return( CharToStrSafe( TMP_DOLLAR ) );      /* write a place holder */
    case MAC_COMMENT:
        return( CharToStrSafe( TMP_COMMENT ) );     /* write a place holder */
    case MAC_OPEN:
        /* recurse, get macro name */
        if( !Glob.compat_nmake && !Glob.compat_posix ) {
            p = deMacroText( depth + 1, end1, MAC_CLOSE );
            if( IsMacroName( p ) ) {
                p2 = WrnGetMacroValue( p );
                FreeSafe( p );
                return( p2 );
            }
            FreeSafe( p );
        } else {
            pos = 0;
            s = PreGetCHR();
            if( sismsspecial( s ) ) {
                UnGetCHR( s );
                // This is to invoke LexDollar
                t = LexToken( LEX_MS_MAC );
                // This is the only time to get the modifier
                GetModifier();
                p = ProcessToken( depth, end1, end2, t );
                s = PreGetCHR();
                if( s != ')' ) {
                    PrtMsg( ERR | LOC | ILLEGAL_CHARACTER_IN_MAC, s );
                    break;
                }
                return( p );
            } else {
                for( ;; ) {
                    if( s == ')' ) {
                        break;
                    } else if( s == STRM_MAGIC ||
                               s == STRM_END   ||
                               s == '\n' ) {
                        UnGetCHR( s );
                        break;
                    }
                    if( pos < MAX_TOK_SIZE -1 ) {
                        macname[pos++] = s;
                    }
                    s = PreGetCHR();
                }
                macname[pos] = NULLCHAR;
                if( IsMacroName( macname ) ) {
                    p2 = WrnGetMacroValue( macname );
                    return( p2 );
                }
            }
        }
        break;
    case MAC_EXPAND_ON:
    case MAC_EXPAND_OFF:
        /* do nothing */
        break;
    case MAC_CUR:
    case MAC_FIRST:
    case MAC_LAST:
        /* static pointer returned so we need to duplicate string */
        cp2 = specialValue( t );
        if( cp2 != NULL ) {
            return( StrDupSafe( cp2 ) );
        }
        return( NULL );
    case MAC_ALL_DEP:
        p = GetCurDeps( false, false );
        massageDollarOctothorpe( p );
        return( p );
    case MAC_INF_DEP:
        p = GetCurDeps( false, true );
        massageDollarOctothorpe( p );
        return( p );
    case MAC_YOUNG_DEP:
        p = GetCurDeps( true, false );
        massageDollarOctothorpe( p );
        return( p );
    case MAC_NAME:
    case MAC_WS:
    case MAC_PUNC:
        p = CurAttr.u.ptr;
        CurAttr.u.ptr = NULL;
        return( p );
    default:
#ifdef DEVELOPMENT
        PrtMsg( FTL | LOC | INVALID_TOKEN_IN, t, "deMacroToEnd()" );
        ExitFatal();
        // never return
#else
        PrtMsg( WRN | LOC | IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
        break;
#endif
    }

    return( NULL );
}


STATIC char *deMacroToEnd( int depth, MTOKEN_T end1, MTOKEN_T end2 )
/*******************************************************************
 * post:    0 or more characters removed from input; next character of
 *          input is STRM_END || STRM_MAGIC || EOL || end1 || end2
 * returns: pointer to demacro'd string (caller must FreeSafe)
 */
{
    MTOKEN_T    t;
    VECSTR      vec;        /* we build expansion here  */
    char        *p;         /* temp str */

    assert(     end1 == TOK_EOL
            ||  end1 == MAC_PUNC
            ||  end1 == TOK_MAGIC
            ||  end1 == TOK_END
            ||  end1 == MAC_WS
    );

    assert(     end2 == TOK_EOL
            ||  end2 == MAC_PUNC
            ||  end2 == TOK_MAGIC
            ||  end2 == TOK_END
            ||  end2 == MAC_EXPAND_OFF
            ||  end2 == MAC_CLOSE
    );

    vec = StartVec();

    for( ;; ) {
        t = LexToken( LEX_MAC_SUBST );

        assert( t != MAC_TEXT );            /* should never recv this tok */

        if( t == MAC_CLOSE && end2 != MAC_CLOSE ) {
            t = MAC_PUNC;
            CurAttr.u.ptr = CharToStrSafe( ')' );
        }

        if(     t == TOK_END               /* always stops at these */
            ||  t == TOK_MAGIC
            ||  t == TOK_EOL
            ||  t == end2
            ||  t == end1
            ||  ( t == MAC_WS && ( end2 == MAC_PUNC || end1 == MAC_PUNC ) )
                ) {
            break;
        }

        p = ProcessToken( depth, end1, end2, t );
        if( p != NULL ) {
            WriteVec( vec, p );
            FreeSafe( p );
        }
    }

    /* put back the token we demacro'd until */
    switch( t ) {
    case MAC_PUNC:
    case MAC_WS:
        InsString( CurAttr.u.ptr, true );
        CurAttr.u.ptr = NULL;
        break;
    case TOK_EOL:
        UnGetCHR( '\n' );
        break;
    case TOK_END:
        UnGetCHR( STRM_END );
        break;
    case TOK_MAGIC:
        UnGetCHR( STRM_MAGIC );
        break;
    default:
        break;
    }

    return( FinishVec( vec ) );
}


STATIC char *deMacroText( int depth, MTOKEN_T end1, MTOKEN_T end2 )
/******************************************************************
 * post:    same as deMacroToEnd
 * returns: same as deMacroToEnd
 */
{
    char    *result;
    char    *p;

    if( depth > MAX_MAC_NEST ) {
        PrtMsg( FTL | LOC | CANNOT_NEST_FURTHER );
        if( !Glob.compat_nmake ) {
            PrtMsg( WRN | LOC | MICROSOFT_MAKEFILE );
        }
        ExitFatal();
        // never return
    }

    result = deMacroToEnd( depth, end1, end2 );
    while( strchr( result, '$' ) != NULL ) {
        UnGetCHR( STRM_MAGIC );
        InsString( result, true );

        ++depth;
        if( depth > MAX_MAC_NEST ) {
            PrtMsg( FTL | LOC | CANNOT_NEST_FURTHER );
            if( !Glob.compat_nmake ) {
                PrtMsg( WRN | LOC | MICROSOFT_MAKEFILE );
            }
            ExitFatal();
            // never return
        }

        /*
         * note that we don't pass end1 here because we put this data
         * into the stream ourselves - the caller has nothing to do
         * with it.
         */

        result = deMacroToEnd( depth, TOK_MAGIC, TOK_MAGIC );
        LexToken( LEX_MAC_SUBST );      /* eat STRM_MAGIC */
    }

    /*
     * when it is in part demacro do not change the dollar and the comments
     * yet wait until the deMacro is called
     */
    if( !IsPartDeMacro ) {
        for( p = result; *p != NULLCHAR; ++p ) {
            switch( *p ) {
            case TMP_DOLLAR:    *p = '$';       break;
            case TMP_COMMENT:   *p = COMMENT_C; break;
#if 0
            case SPECIAL_TMP_DOLLAR:
                if( Glob.compat_nmake ) {
                    if( cismsspecial( *(p + 1) ) ) {
                        *p = '$';
                    }
                }
                break;
#endif
            }
        }
    } else {
        for( p = result; *p != NULLCHAR; ++p ) {
            switch( *p ) {
            case SPECIAL_TMP_DOLLAR:    *p = '$';   break;
            }
        }
    }
    return( result );
}


char *ignoreWSDeMacro( bool partDeMacro, bool forceDeMacro )
/*************************************************************************
 * This is the same as deMacro except that we retain any leading or trailing
 * ws. Ws is quietly truncated from pathologically long lines.
 * bool = true when you want partDeMacro
 */
{
    VECSTR  DeMacroText;
    VECSTR  temp;
    char    text        [MAX_COMMANDLINE];
    char    leadingSpace[MAX_COMMANDLINE];
    char    *TrailSpace;
    char    *DeMacroStr;
    char    *p;
    char    *p_max;
    STRM_T  s;
    char    *result;

    // Set leadingSpace - leave t set to first non-whitespace byte
    p = leadingSpace;
    p_max = p + MAX_COMMANDLINE - 1;
    for( ; sisws( s = PreGetCHR() ) && p < p_max; ++p ) {
        *p = s;
    }
    *p = NULLCHAR;

    // set text to non-whitespace string and TrailSpace to next character.
    p_max = text + MAX_COMMANDLINE - 1;
    for( TrailSpace = p = text; p < p_max; ++p ) {
        if( s == STRM_END || s == STRM_MAGIC || s == '\n' ) {
            break;
        }
        if( !sisws( s ) ) {
            TrailSpace = p + 1;
        }
        *p = s;
        s = PreGetCHR();
    }
    *p = NULLCHAR;
    UnGetCHR( s );                           // Put back last byte read

    DeMacroText = StartVec();
    WriteNVec( DeMacroText, text, TrailSpace - text );
    result = FinishVec( DeMacroText );
    InsString( result, true );              // Push graphic string to the stream

    if( partDeMacro ) {                     // Expand as far as EOL
        DeMacroStr = PartDeMacro( forceDeMacro );
    } else {
        DeMacroStr = DeMacro( TOK_EOL );
    }

    DeMacroText = StartVec();
    WriteVec( DeMacroText, leadingSpace );  // Write copy of leading whitespace
    temp        = StartVec();
    WriteVec( temp, DeMacroStr );
    FreeSafe( DeMacroStr );
    CatVec( DeMacroText, temp );            // Write graphic string expansion
    temp        = StartVec();
    WriteVec( temp, TrailSpace );
    CatVec( DeMacroText, temp );            // Write copy of trailing whitespace

    result = FinishVec( DeMacroText );
    return( result );                       // macro-expanded, whitespaced line
}


char *DeMacro( MTOKEN_T end1 )
/***********************************
 * same as deMacroText
 */
{
    return( deMacroText( 0, end1, TOK_END ) );
}


STATIC char *PartDeMacroProcess( void )
/**************************************
 * Partially DeMacro until EOL.  Copies verbatim, only expanding
 * $+$- delimited sequences.  Removes leading and trailing ws.
 */
{
    VECSTR      vec;            /* vector for macro defn                */
    VECSTR      wsvec = NULL;   /* vector to hold ws                    */
    bool        leadingws;      /* still trimming leading ws            */
    MTOKEN_T    t;
    char        *text;

    vec = StartVec();

    leadingws = true;

    while( ( t = LexToken( LEX_MAC_DEF ) ) != TOK_END && t != TOK_EOL ) {
        switch( t ) {
        case TOK_MAGIC:
            /* do nothing */
            break;
        case MAC_EXPAND_ON:
            text = deMacroText( 0, TOK_EOL, MAC_EXPAND_OFF );
            InsString( text, true );
            break;
        case MAC_WS:
            if( !leadingws ) {
                if( wsvec == NULL ) {
                    wsvec = StartVec();
                }
                WriteVec( wsvec, CurAttr.u.ptr );
            }
            FreeSafe( CurAttr.u.ptr );
            CurAttr.u.ptr = NULL;
            break;
        case MAC_TEXT:
            if( wsvec != NULL && !leadingws ) {
                CatVec( vec, wsvec );
                wsvec = NULL;
            }
            leadingws = false;
            WriteVec( vec, CurAttr.u.ptr );
            FreeSafe( CurAttr.u.ptr );
            CurAttr.u.ptr = NULL;
            break;
        default:
#ifdef DEVELOPMENT
            FreeVec( vec );
            if( wsvec != NULL ) {
                FreeVec( wsvec );
            }
            PrtMsg( FTL | INVALID_TOKEN_IN, t, "PartDeMacro" );
            ExitFatal();
            // never return
#else
            PrtMsg( WRN | LOC | IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
#endif
        }
    }

    if( wsvec != NULL ) {       /* trim trailing ws */
        FreeVec( wsvec );
    }
    text = FinishVec( vec );
    return( text );
}


bool ForceDeMacro ( void )
/*************************
 * This function checks whether or not to deMacro the function later or not
 * because in Microsoft macros are expanded immediately and not after all
 * the processing.  Note that in the clist for implicit rules this is not
 * true so we need to expand the macros in inline files later
 * For Watcom the default is false
 */
{
    return( (Glob.compat_nmake || Glob.compat_posix) && !ImplicitDeMacro );
}


char *PartDeMacro( bool forceDeMacro )
/*************************************
 * the addition of microsoft option needs this option
 * since MACROS are always fully expanded sequentially
 * forceDeMacro if set true will force full Demacro
 */
{
    STRM_T  s;
    char    *temp;

    if( Glob.compat_nmake || Glob.compat_posix ) {
        IsPartDeMacro = true;
    }
    if( forceDeMacro ) {
        //remove white spaces at the beginning
        while( sisws( s = PreGetCHR() ) ) {
        }
        UnGetCHR( s );
        temp = DeMacro( TOK_EOL );
        s = PreGetCHR();
        if( Glob.compat_nmake || Glob.compat_posix ) {
            IsPartDeMacro = false;
        }
        return( temp );
    } else {
        if( Glob.compat_nmake || Glob.compat_posix ) {
            IsPartDeMacro = false;
        }
        return( PartDeMacroProcess() );
    }
}


STATIC bool NMacroNameEq( const char *name1, const char *name2, size_t len )
/**************************************************************************/
{
    if( Glob.compat_nmake || Glob.compat_posix ) {
        return( strncmp( name1, name2, len ) == 0 );
    } else {
        return( strnicmp( name1, name2, len ) == 0 );
    }
}


STATIC char *DeMacroName( const char *text, const char *name )
/*************************************************************
 * Tries to find if there are occurrences of name in text
 * this is only for ms-option fix because nmake relies on this
 * functionality to function correctly
 * Note that this is possible since in nmake $(TEMP$(TEMP)) is illegal
 * in wmake this is legal syntax
 * Also in nmake, $HELLO will expand as $(H)ELLO unlike in wmake
 * $HELLO is synonymous to $(HELLO)
 */
{
    char const  *p;
    char const  *oldptr;
    char        *temp;
    char        *macronameStr;
    VECSTR      outtext;
    VECSTR      macroname;
    size_t      lengthToClose;
    size_t      len;

    assert( name != NULL && text != NULL );

    len = strlen( name );
    oldptr = p = text;

    outtext = StartVec();
    while( (p = strchr( p, '$' )) != NULL ) {
        switch( *++p ) {    // Swallow that '$'
        case '$':           // Swallow literal '$'.
            p++;
            break;
        case '(':           // Possible regular substitution
            p++;
            // bracket or colon (for string substitution) after matching name?
            if( NMacroNameEq( p, name, len ) && (p[len] == ')' || p[len] == ':') ) {
                lengthToClose = len;
                while( p[lengthToClose] != ')' ) {
                    ++lengthToClose;
                }
                WriteNVec( outtext, oldptr, p - 2 - oldptr );
                macroname = StartVec();
                WriteNVec( macroname, p, lengthToClose );
                macronameStr = FinishVec( macroname );
                temp = GetMacroValue( macronameStr );
                if( temp != NULL ) {
                    WriteVec( outtext, temp );
                    FreeSafe( temp );
                }
                FreeSafe( macronameStr );
                p = oldptr = p + 1 + lengthToClose;
            }
            break;
        default:    // Possible Microsoft name without parenthesis
            if( len == 1 && NMacroNameEq( p, name, 1 ) ) {
                WriteNVec( outtext, oldptr, p - 1 - oldptr );
                temp = GetMacroValue( name );
                if( temp != NULL ) {
                    WriteVec( outtext, temp );
                    FreeSafe( temp );
                }
                p = oldptr = p + 1;
            }
            break;
        }
    }
    WriteVec( outtext, oldptr );
    return( FinishVec( outtext ) );
}


void DefMacro( const char *name )
/***************************************
 * define macro named name with the characters in the stream
 * post:    characters up to first EOL | STRM_END removed from stream;
 *          macro named name defined
 *
 */
{
    char        *value;
    bool        unused_value;
    char        *temp;
    char        *EnvVarValue;   /* used for env. variables (full demacro) */
    char        *EnvOldValue;
#ifdef CLEAN_ENVIRONMENT_VAR
    ELIST       *tempEList;
#endif

    assert( IsMacroName( name ) );

    temp  = PartDeMacro( false );
    value = DeMacroName( temp, name );
    FreeSafe( temp );

    unused_value = true;
    EnvVarValue = NULL;
    EnvOldValue = NULL;
    if( *name != ENVVAR_C ) {
        unused_value = addMacro( name, value );
        if( Glob.compat_nmake ) {
            EnvOldValue = GetEnvExt( name );
        }
    }
    if( *name == ENVVAR_C || EnvOldValue != NULL ) {
        UnGetCHR( '\n' );
        InsString( value, false );
        EnvVarValue = DeMacro( TOK_EOL );
        PreGetCHR();  // eat EOL token (used to avoid assertion failure)
        if( *name == ENVVAR_C ) {
            SetEnvSafe( name + 1, EnvVarValue );
        } else {
            if( !DoingBuiltIn ) {
#ifdef CLEAN_ENVIRONMENT_VAR
                tempEList = NewEList();
                tempEList->next = OldEnvValues;
                tempEList->envVarName = StrDupSafe( name );
                tempEList->envOldVal  = StrDupSafe( EnvOldValue );
                OldEnvValues = tempEList;
#endif
                SetEnvSafe( name, EnvVarValue );
            }
        }
    }
    if( unused_value )
        FreeSafe( value );
    FreeSafe( EnvVarValue );
}


static bool printMac( const void *node, const void *ptr )
/*******************************************************/
{
    MACRO const *mac = node;
    char        buff1[MAX_RESOURCE_SIZE];
    char        buff2[MAX_RESOURCE_SIZE];

    /* unused parameters */ (void)ptr;

    /* mac->node.name is used as a parameter twice in this module because
     * it may substitute before or after the long string depending on the
     * way the message is translated */
    PrtMsg( INF | NEOL | PMAC_NAME_HAS_VALUE, mac->node.name );
    PrtMsg( INF | NEOL | PRNTSTR, mac->value );
    MsgGetTail( PMAC_NAME_HAS_VALUE, buff1 );
    FmtStr( buff2, buff1, mac->node.name );
    PrtMsg( INF | PRNTSTR, buff2 );
    return( false );
}


void PrintMacros( void )
/*****************************/
{
    WalkHashTab( macTab, (bool (*)(void *,void *))printMac, NULL );
    PrtMsg( INF | NEWLINE );
}


#ifdef CLEAN_ENVIRONMENT_VAR
STATIC void restoreEnvironment( void )
/************************************/
{
    ELIST       *p;
    ENV_TRACKER *env;
    size_t      len;

    while( (p = OldEnvValues) != NULL ) {
        OldEnvValues = p->next;
        len = strlen( p->envVarName );
        env = MallocSafe( sizeof( ENV_TRACKER ) + len + strlen( p->envOldVal ) + 1 );
        strcpy( env->name, p->envVarName );
#if defined( _MSC_VER )
        env->name[len] = '=';
#endif
        env->value = env->name + len + 1;
        strcpy( env->value, p->envOldVal );
        SetEnvExt( env );
        FreeSafe( p->envVarName );
        FreeSafe( p->envOldVal );
        FreeSafe( p );
    }
}
#endif


void MacroInit( void )
/***************************/
{
    macTab = NewHashTab( HASH_PRIME );
    dirBuf = NULL;
#ifdef CLEAN_ENVIRONMENT_VAR
    OldEnvValues = NULL;
#endif
    ImplicitDeMacro = false;
    IsPartDeMacro   = false;
    time( &start_time );
}


#ifndef NDEBUG
STATIC bool freeMacro( MACRO *mac, const void *ptr )
/**************************************************/
{
    /* unused parameters */ (void)ptr;

    FreeSafe( mac->node.name );
    FreeSafe( (char *)(mac->value) );
    FreeSafe( mac );
    return( false );
}
#endif


void MacroFini( void )
/***************************/
{
#ifndef NDEBUG
    WalkHashTab( macTab, (bool (*)(void *,void *))freeMacro, NULL );
    FreeHashTab( macTab );
    macTab = NULL;

    if( dirBuf != NULL ) {
        FreeSafe( dirBuf );
    }
#endif
#ifdef CLEAN_ENVIRONMENT_VAR
    restoreEnvironment();
#endif
}
