/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "make.h"
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

#include "clibext.h"
#include "pathgrp.h"


/* arbitrarily set the length of the command line */
#define MAX_COMMANDLINE     (_MAX_PATH * 5)

/*
 * macros are stored in a hash table
 */
#define HASH_PRIME  37

typedef struct Macro {
    HASHNODE    node;       /* name is at name.node */
    const char  *value;
    BIT         readonly;   /* ie: from command line */
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
BOOLEAN         ImplicitDeMacro;

/*
 *   Only Useful in ms option if it is partDeMacro then we only deMacro
 *   everything except for special DeMacro characters
 *
 */
BOOLEAN         IsPartDeMacro;

BOOLEAN         DoingBuiltIn;   /* Are we parsing builtin macros            */

#ifdef CLEAN_ENVIRONMENT_VAR
/* This contains the old environment values before wmake actually */
/* redefines them -  only valid in ms-option */
STATIC ELIST    *OldEnvValues;
#endif


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
            *p = TMP_DOL_C;
            break;
        case '#':
            *p = TMP_COMMENT_C;
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
    PGROUP  pg;
    char    *current;

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
        if( Glob.compat_nmake) {
            if( dirBuf[0] == NULLCHAR ) {
                dirBuf[0] = '.';
                dirBuf[1] = NULLCHAR;
            } else {
                current = dirBuf;
                while( *current != NULLCHAR ) {
                    ++current;
                }
                if( *(current - 1) == '\\' ) {
                    *(current - 1) = NULLCHAR;
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


STATIC const char *specialValue( TOKEN_T t )
/*******************************************
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
        while( (*buffer = toupper( *name )) != NULLCHAR ) {
            ++buffer, ++name;
        }
    }
}


STATIC MACRO *getMacroNode( const char *name )
/*********************************************
 * returns: pointer to MACRO with this name
 */
{
    BOOLEAN caseSensitive;

    assert( name != NULL && *name != ENVVAR );

    if( Glob.compat_nmake || Glob.compat_posix ) {
        caseSensitive = TRUE;
    } else {
        caseSensitive = FALSE;
    }

    return( (MACRO *)FindHashNode( macTab, name, caseSensitive ) );
}


STATIC char *findEqual( char *inString )
/***************************************
 * returns" pointer to equals sign if found
 */
{
    char    *ret;

    ret = strrchr( inString, EQUAL );
    if( ret == inString ) {
        ret = NULL;
    }

    return( ret );
}


STATIC RET_T getOldNewString( char *inString, const char **oldString, const char **newString )
/********************************************************************************************/
{
    char    *equal;

    equal = findEqual( inString );

    if( equal == NULL ) {
        return( RET_ERROR );
    } else {
        *oldString = inString;
        *equal     = NULLCHAR;
        *newString = equal + 1;
        return( RET_SUCCESS );
    }
}


STATIC char *doStringSubstitute( const char *name, const char *oldString, const char *newString )
/************************************************************************************************
 *   $(macroname:oldstr=newstr)
 *   substitute any occurence of oldstr with new str
 */
{
    VECSTR      output;
    char const  *current;
    char const  *start;
    size_t      old_len;

    output = StartVec();
    WriteVec( output, "" );

    assert( name != NULL && oldString != NULL && newString != NULL );

    old_len = strlen( oldString );
    for( start = current = name; *current != NULLCHAR; current++ ) {
        if( strncmp( current, oldString, old_len ) == 0 ) {
            CatNStrToVec( output, start, current - start );
            CatStrToVec( output, newString );
            start   = current + old_len;
            current = start - 1;
        }
    }
    CatStrToVec( output, start );

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
    BOOLEAN cdrive;
    BOOLEAN cwd;
    BOOLEAN ctime;
    char    *p;
    int     pos;

    makeMacroName( macro, name ); // Does assert( IsMacroName( name ) );

    if( *macro == ENVVAR ) {
        env = getenv( macro + 1 );
        if( env != NULL ) {
            return( env );
        }
        cdrive = strcmp( macro + 1, "CDRIVE" ) == 0 ||
                 strcmp( macro + 1, "__CDRIVE__" ) == 0;
        cwd    = strcmp( macro + 1, "CWD" ) == 0 ||
                 strcmp( macro + 1, "__CWD__" ) == 0;
        ctime  = strcmp( macro + 1, "__CTIME__" ) == 0;
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
        } else if( ctime ) {
            time_t      timex;
            struct tm   *tm;

            time( &timex );
            tm = localtime( &timex );
            FmtStr( getDirBuf(), "%D:%D:%D", tm->tm_hour, tm->tm_min, tm->tm_sec );
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
                if( macro[pos] != toupper( macro[pos] ) ) {
                    return( NULL );
                }
            }
        }
        env = getenv( macro );
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
    char        *current;
    const char  *new;
    const char  *old;
    char        *line;

    InName = StrDupSafe( name );
    current = strchr( InName, COLON );

    if( current == NULL ) {
        beforeSub = GetMacroValueProcess( InName );
        if( beforeSub == NULL ) {
            afterSub = NULL;
        } else {
            afterSub  = StrDupSafe( beforeSub );
        }
    } else {
        *current++ = NULLCHAR;
        beforeSub = GetMacroValueProcess( InName );

        if( beforeSub == NULL ) {
            afterSub = NULL;
        } else {
            line = NULL;
            // recursively expand so $(macro:sub) OK if macro contains another
            if( strchr( beforeSub, DOLLAR ) != NULL ) {
                UnGetCH( STRM_MAGIC );
                InsString( beforeSub, FALSE );
                beforeSub = line = DeMacro( TOK_MAGIC );
                GetCHR();   // eat STRM_MAGIC
            }
            if( beforeSub == NULL ) {
                afterSub = NULL;
            } else {
                if( getOldNewString( current, &old, &new ) == RET_SUCCESS ) {
                    afterSub = doStringSubstitute( beforeSub, old, new );
                } else {
                    afterSub = NULL;
                    PrtMsg( ERR | LOC | INVALID_STRING_SUBSTITUTE );
                }
                if( line ) {
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
    int     space;
    char    *t;
    char    *p;

    space = 0;
    t = v;
    for( p = v; *p != NULLCHAR; ++p ) {
        if( !isws( *p ) ) {
            if( space ) {
                *t++ = ' ';
            }
            *t++ = *p;
            space = 0;
        } else {
            space = 1;
        }
    }
    *t = NULLCHAR;
    return( v );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC BOOLEAN addMacro( const char *name, char *value )
/******************************************************
 * post:    new macro possibly allocated, copy of name made
 */
{
    char    macro[MAX_MAC_NAME];
    MACRO   *new;
    BOOLEAN unused_value;

    assert( *name != ENVVAR );

    value = trimMacroValue( value );
    makeMacroName( macro, name ); // Does assert( IsMacroName( name ) );

    new = getMacroNode( macro );     /* check if redefinition */

    unused_value = FALSE;
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
        unused_value = TRUE;
    }
    return( unused_value );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


BOOLEAN IsMacroName( const char *inName )
/***********************************************
 * returns: TRUE if name is a valid macro name, otherwise FALSE and print
 * an error message
 */
{
    char const  *current = inName;
    int         pos = 0;

    assert( inName != NULL );

    while( pos < MAX_MAC_NAME && *current != NULLCHAR && *current != COLON ) {
        if( !ismacc( *current ) ) {
            PrtMsg( ERR | LOC | INVALID_MACRO_NAME, inName );
            return( FALSE );
        }
        pos++, current++;
    }
    if( pos == 0 ) {
        PrtMsg( ERR | LOC | INVALID_MACRO_NAME, inName );
        return( FALSE );
    }
    if( pos >= MAX_MAC_NAME ) {
        PrtMsg( ERR | LOC | MAXIMUM_TOKEN_IS, MAX_MAC_NAME - 1 );
        return( FALSE );
    }

    return( TRUE );
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

    makeMacroName( macro, name ); // Does assert( IsMacroName( name ) );

    if( *macro == ENVVAR ) {
        ENV_TRACKER     *env;

        env = MallocSafe( sizeof( ENV_TRACKER ) + strlen( macro ) + 1 );
        FmtStr( env->value, "%s=", macro + 1 );
        PutEnvSafe( env );
        return;
    }

    dead = (MACRO *)RemHashNode( macTab, macro, TRUE );

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
    const char  *p;

    p = GetMacroValue( name );
    if( p == NULL ) {
        PrtMsg( DBG | WRN | LOC | MACRO_UNDEFINED, name );   /* 13-Dec-90 DJG */
        // we did this to minimize the number of debugging messages but
        // it causes problems when it defines a macro for the user
        //UnGetCH( EOL );
        //DefMacro( name );                             /* 12-jul-93 AFS */
    }
    return( (char *)p );
    /* note we return NULL if it was undefined! */
}


char *DeMacroSpecial( const char *InString )
/*******************************************
 * This function is to specially handle the special macros
 * in the dependencies
 */
{
    const char  *old;
    const char  *current;
    VECSTR      outString;
    char        *tempString;
    char        buffer[6];
    int         pos;

    assert( InString != NULL );
    old     = InString;

    outString = StartVec();

    for( current = InString; *current != NULLCHAR; ++current ) {
        if( *current == SPECIAL_TMP_DOL_C ) {
            CatNStrToVec( outString, old, current - old );
            pos = 0;
            UnGetCH( STRM_MAGIC );
            buffer[pos++] = *(current++);
            if( ismsspecial( *current ) && !ismsmodifier( *(current + 1) ) ) {
                buffer[pos++] = *(current++);
            } else {
                assert( ismsspecial( *current ) );
                buffer[pos++] = *(current++);
                if( ismsmodifier( *current ) ) {
                    buffer[pos++] = *(current++);
                }
            }
            old = current;
            buffer[pos] = NULLCHAR;
            InsString( buffer, FALSE );
            tempString = DeMacro( TOK_MAGIC );
            PreGetCH();   // eat STRM_MAGIC
            CatStrToVec( outString, tempString );
            FreeSafe( tempString);
        }
    }
    CatNStrToVec( outString, old, current - old + 1 );
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
 * deMacroText scans the string that deMacroToEnd returned for DOLLARs.  If
 * it doesn't find one it returns the string.  If a DOLLAR is found, a
 * STRM_MAGIC is pushed, the string is pushed, and deMacroToEnd is called
 * again (ending at STRM_MAGIC).  This process is repeated until no DOLLARs
 * remain.
 *
 * This pair always stops at EOL, STRM_MAGIC, or STRM_END.
 *
 * Note that if a MAC_PUNC is passed for one of the end tokens, it is
 * assumed to imply MAC_WS as well.  This is used in mpreproc.c to find the
 * end of a macro name for things such as !ifdef, !undef, etc.
 */
STATIC char *deMacroText( int depth, TOKEN_T end1, TOKEN_T end2 );


STATIC char *ProcessToken( int depth, TOKEN_T end1, TOKEN_T end2, TOKEN_T t )
/****************************************************************************
 * Processes the tokens returned from lexToken in deMacroToEnd
 */
{
    STRM_T      s;
    char        temp_str[2];
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
            s = PreGetCH ();
            if( ismacc( s ) ) {
                temp_str[1] = NULLCHAR;
                temp_str[0] = s;
                p = StrDupSafe( temp_str );
            } else {
                p = StrDupSafe( "" );
            }
        }

        if( IsMacroName( p ) ) {
            p2 =  WrnGetMacroValue( p );
            FreeSafe( p );
            if( p2 != NULL ) {
                return( p2 );
            }
            return( NULL );
        }
        FreeSafe( p );
        break;

    case MAC_DOLLAR:
        return( StrDupSafe( TMP_DOL_S ) );      /* write a place holder */

    case MAC_COMMENT:
        return( StrDupSafe( TMP_COMMENT_S ) );  /* write a place holder */

    case MAC_OPEN:                      /* recurse, get macro name */
        if( !Glob.compat_nmake && !Glob.compat_posix ) {
            p = deMacroText( depth + 1, end1, MAC_CLOSE );
            if( IsMacroName( p ) ) {
                p2 =  WrnGetMacroValue( p );
                FreeSafe( p );
                if( p2 != NULL ) {
                    return( p2 );
                }
                return( NULL );
            }
            FreeSafe( p );
        } else {
            pos = 0;
            s = PreGetCH();
            if( ismsspecial( s ) ) {
                UnGetCH( s );
                // This is to invoke LexDollar
                t = LexToken( LEX_MS_MAC );
                // This is the only time to get the modifier
                GetModifier();
                p = ProcessToken( depth, end1, end2, t );
                s = PreGetCH();
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
                               s == EOL ) {
                        UnGetCH( s );
                        break;
                    }
                    if( pos < MAX_TOK_SIZE -1 ) {
                        macname[pos++] = s;
                    }
                    s = PreGetCH();
                }
                macname[pos] = NULLCHAR;
                if( IsMacroName( macname ) ) {
                    p2 =  WrnGetMacroValue( macname );
                    if( p2 != NULL ) {
                        return( p2 );
                    }
                    return( NULL );
                }
            }
        }
        break;

    case MAC_EXPAND_ON:                 /* fall through */
    case MAC_EXPAND_OFF:
        /* do nothing */
        break;

    case MAC_CUR:                       /* fall through */
    case MAC_FIRST:
    case MAC_LAST:
        /* static pointer returned so we need to duplicate string */
        cp2 = specialValue( t );
        if( cp2 != NULL ) {
            return( StrDupSafe( cp2 ) );
        }
        return( NULL );

    case MAC_ALL_DEP:
        p = GetCurDeps( FALSE, FALSE );
        massageDollarOctothorpe( p );
        return( p );

    case MAC_INF_DEP:
        p = GetCurDeps( FALSE, TRUE );
        massageDollarOctothorpe( p );
        return( p );

    case MAC_YOUNG_DEP:
        p = GetCurDeps( TRUE, FALSE );
        massageDollarOctothorpe( p );
        return( p );

    case MAC_NAME:                      /* fall through */
    case MAC_WS:
    case MAC_PUNC:
        p = CurAttr.u.ptr;
        CurAttr.u.ptr  = NULL;
        return( p );

    default:
#ifdef DEVELOPMENT
        PrtMsgExit(( FTL | LOC | INVALID_TOKEN_IN, t, "deMacroToEnd()" ));
#else
        PrtMsg( WRN | LOC | IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
        break;
#endif
    }

    return( NULL );
}


STATIC char *deMacroToEnd( int depth, TOKEN_T end1, TOKEN_T end2 )
/*****************************************************************
 * post:    0 or more characters removed from input; next character of
 *          input is STRM_END || STRM_MAGIC || EOL || end1 || end2
 * returns: pointer to demacro'd string (caller must FreeSafe)
 */
{
    TOKEN_T t;
    VECSTR  vec;            /* we build expansion here  */
    char    *p;             /* temp str */

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
            CurAttr.u.ptr = StrDupSafe( ")" );
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
            CatStrToVec( vec, p );
            FreeSafe( p );
        }
    }

    /* put back the token we demacro'd until */
    switch( t ) {
    case MAC_PUNC:
    case MAC_WS:
        InsString( CurAttr.u.ptr, TRUE );
        break;
    case TOK_EOL:       /* fall through */
        UnGetCH( EOL );
        break;
    case TOK_END:
        UnGetCH( STRM_END );
        break;
    case TOK_MAGIC:
        UnGetCH( STRM_MAGIC );
        break;
    }

    return( FinishVec( vec ) );
}


STATIC char *deMacroText( int depth, TOKEN_T end1, TOKEN_T end2 )
/****************************************************************
 * post:    same as deMacroToEnd
 * returns: same as deMacroToEnd
 */
{
    char    *result;
    char    *p;

    if( depth > MAX_MAC_NEST ) {
        PrtMsgExit(( FTL | LOC | CANNOT_NEST_FURTHER ));
    }

    result = deMacroToEnd( depth, end1, end2 );
    while( strchr( result, DOLLAR ) != NULL ) {
        UnGetCH( STRM_MAGIC );
        InsString( result, TRUE );

        ++depth;
        if( depth > MAX_MAC_NEST ) {
            PrtMsgExit(( FTL | LOC | CANNOT_NEST_FURTHER ));
        }

        /*
         * note that we don't pass end1 here because we put this data
         * into the stream ourselves - the caller has nothing to do
         * with it.
         */

        result = deMacroToEnd( depth, TOK_MAGIC, TOK_MAGIC );
        (void)LexToken( LEX_MAC_SUBST );      /* eat STRM_MAGIC */
    }

    /*
     * when it is in part demacro do not change the dollar and the comments
     * yet wait until the deMacro is called
     */
    if( !IsPartDeMacro ) {
        for( p = result; *p != NULLCHAR; ++p ) {
            switch( *p ) {
            case TMP_DOL_C:         *p = DOLLAR;    break;
            case TMP_COMMENT_C:     *p = COMMENT;   break;
#if 0
            case SPECIAL_TMP_DOL_C:
                  if( Glob.compat_nmake ) {
                       if( ismsspecial( *(p + 1) ) ) {
                          *p = DOLLAR;
                       }
                  }
                  break;
#endif
            }
        }
    } else {
        for( p = result; *p != NULLCHAR; ++p ) {
            switch( *p ) {
            case SPECIAL_TMP_DOL_C: *p = DOLLAR;    break;
            }
        }
    }
    return( result );
}


char *ignoreWSDeMacro( BOOLEAN partDeMacro, BOOLEAN forceDeMacro )
/*************************************************************************
 * This is the same as deMacro except that we retain any leading or trailing
 * ws. Ws is quietly truncated from pathologically long lines.
 * Boolean = TRUE when you want partDeMacro
 */
{
    VECSTR  DeMacroText;
    VECSTR  temp;
    char    text        [MAX_COMMANDLINE];
    char    leadingSpace[MAX_COMMANDLINE];
    char    *TrailSpace;
    char    *DeMacroStr;
    char    *current;
    char    *current_max;
    STRM_T  s;
    char    *result;

    // Set leadingSpace - leave t set to first non-whitespace byte
    current = leadingSpace;
    current_max = current + MAX_COMMANDLINE - 1;
    for( ; isws( s = PreGetCH() ) && current < current_max; ++current ) {
        *current = s;
    }
    *current = NULLCHAR;

    // set text to non-whitespace string and TrailSpace to next character.
    current_max = text + MAX_COMMANDLINE - 1;
    for( TrailSpace = current = text; current < current_max; ++current ) {
        if( s == STRM_END || s == STRM_MAGIC || s == EOL ) {
            break;
        }
        if( !isws( s ) ) {
            TrailSpace = current + 1;
        }
        *current = s;
        s = PreGetCH();
    }
    *current = NULLCHAR;
    UnGetCH( s );                           // Put back last byte read

    DeMacroText = StartVec();
    WriteNVec( DeMacroText, text, TrailSpace - text );
    result = FinishVec( DeMacroText );
    InsString( result, TRUE );              // Push graphic string to the stream

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


char *DeMacro( TOKEN_T end1 )
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
    VECSTR  vec;               /* vector for macro defn                */
    VECSTR  wsvec = NULL;      /* vector to hold ws                    */
    BOOLEAN leadingws;         /* still trimming leading ws            */
    TOKEN_T t;
    char    *text;

    vec = StartVec();

    leadingws = TRUE;

    while( ( t = LexToken( LEX_MAC_DEF ) ) != TOK_END && t != TOK_EOL ) {
        switch( t ) {
        case TOK_MAGIC:        /* we ignore these */
            break;
        case MAC_EXPAND_ON:
            text = deMacroText( 0, TOK_EOL, MAC_EXPAND_OFF );
            InsString( text, TRUE );
            break;
        case MAC_WS:
            if( !leadingws ) {
                if( wsvec == NULL ) {
                    wsvec = StartVec();
                }
                WriteVec( wsvec, CurAttr.u.ptr );
            }
            FreeSafe( CurAttr.u.ptr );
            break;
        case MAC_TEXT:
            if( wsvec != NULL && !leadingws ) {
                CatVec( vec, wsvec );
                wsvec = NULL;
            }
            leadingws = FALSE;
            WriteVec( vec, CurAttr.u.ptr );
            FreeSafe( CurAttr.u.ptr );
            break;
        default:
#ifdef DEVELOPMENT
            FreeVec( vec );
            if( wsvec != NULL ) {
                FreeVec( wsvec );
            }
            PrtMsgExit(( FTL | INVALID_TOKEN_IN, t, "PartDeMacro" ));
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


BOOLEAN ForceDeMacro ( void )
/***********************************
 * This function checks whether or not to deMacro the function later or not
 * because in Microsoft macros are expanded immediately and not after all
 * the processing.  Note that in the clist for implicit rules this is not
 * true so we need to expand the macros in inline files later
 * For Watcom the default is FALSE
 */
{
    return( (Glob.compat_nmake || Glob.compat_posix) && !ImplicitDeMacro );
}


char *PartDeMacro( BOOLEAN forceDeMacro )
/************************************************
 * the addition of microsoft option needs this option
 * since MACROS are always fully expanded sequentially
 * forceDeMacro if set true will force full Demacro
 */
{
    STRM_T  s;
    char    *temp;

    if( Glob.compat_nmake || Glob.compat_posix ) {
        IsPartDeMacro = TRUE;
    }
    if( forceDeMacro ) {
        //remove white spaces at the beginning
        while( isws( s = PreGetCH() ) ) {
        }
        UnGetCH( s );
        temp = DeMacro( TOK_EOL );
        s = PreGetCH();
        if( Glob.compat_nmake || Glob.compat_posix ) {
            IsPartDeMacro = FALSE;
        }
        return( temp );
    } else {
        if( Glob.compat_nmake || Glob.compat_posix ) {
            IsPartDeMacro = FALSE;
        }
        return( PartDeMacroProcess() );
    }
}


STATIC int CompareNMacroName( const char *name1, const char *name2, size_t len )
/******************************************************************************/
{
    if( Glob.compat_nmake || Glob.compat_posix ) {
        return( strncmp( name1, name2, len ) );
    } else {
        return( strnicmp( name1, name2, len ) );
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
    char const  *current;
    char const  *oldptr;
    char        *temp;
    char        *macronameStr;
    VECSTR      outtext;
    VECSTR      macroname;
    size_t      lengthToClose;
    size_t      len;

    assert( name != NULL && text != NULL );

    len = strlen( name );
    oldptr = current = text;

    outtext = StartVec();
    while( (current = strchr( current, DOLLAR )) != NULL ) {
        switch( *++current ) {  // Swallow that DOLLAR
        case '$':               // Swallow literal DOLLAR.
            current++;
            break;
        case '(':               // Possible regular substitution
            current++;
            // bracket or colon (for string substitution) after matching name?
            if( CompareNMacroName( current, name, len ) == 0
                    && (current[len] == ')' || current[len] == ':') ) {
                lengthToClose = len;
                while( current[lengthToClose] != ')' ) {
                    ++lengthToClose;
                }
                CatNStrToVec( outtext, oldptr, current - 2 - oldptr );
                macroname = StartVec();
                CatNStrToVec( macroname, current, lengthToClose );
                macronameStr = FinishVec( macroname );
                if( (temp = GetMacroValue( macronameStr )) != NULL ) {
                    CatStrToVec( outtext, temp );
                    FreeSafe( temp );
                }
                FreeSafe( macronameStr );
                current = oldptr = current + 1 + lengthToClose;
            }
            break;
        default:                // Possible Microsoft name without parenthesis
            if( len == 1 && CompareNMacroName( current, name, 1 ) == 0 ) {
                CatNStrToVec( outtext, oldptr, current - 1 - oldptr );
                if( (temp = GetMacroValue( name )) != NULL ) {
                    CatStrToVec( outtext, temp );
                    FreeSafe( temp );
                }
                current = oldptr = current + 1;
            }
            break;
        }
    }
    CatStrToVec( outtext, oldptr );
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
    BOOLEAN     unused_value;
    char        *temp;
    char        *EnvVarValue;   /* used for env. variables (full demacro) */
    ENV_TRACKER *env;
#ifdef CLEAN_ENVIRONMENT_VAR
    ELIST       *tempEList;
#endif

    assert( IsMacroName( name ) );

    temp  = PartDeMacro( FALSE );
    value = DeMacroName( temp, name );
    FreeSafe( temp );

    unused_value = TRUE;
    EnvVarValue = NULL;

    if( *name == ENVVAR || (Glob.compat_nmake && getenv( name ) != NULL ) ) {
        if( *name != ENVVAR ) {
            unused_value = addMacro( name, value );
        }
        UnGetCH( EOL );
        InsString( value, FALSE );
        EnvVarValue = DeMacro( TOK_EOL );
        PreGetCH();  // eat EOL token (used to avoid assertion failure)
    }

    if( *name == ENVVAR ) {
        /* remember strlen( name ) is one byte larger than we want
         * because *name == ENVVAR, and we'll ignore that byte
         */
        assert( EnvVarValue != NULL );
        env = MallocSafe( sizeof( ENV_TRACKER )
                + strlen( name ) + strlen( EnvVarValue ) + 1 );
        FmtStr( env->value, "%s=%s", name + 1, EnvVarValue );
        PutEnvSafe( env );
    } else {
        if( Glob.compat_nmake ) {
            if( !DoingBuiltIn ) {
                if( getenv( name ) != NULL ) {
#ifdef CLEAN_ENVIRONMENT_VAR
                    tempEList = NewEList();
                    tempEList->next = OldEnvValues;
                    tempEList->envVarName = StrDupSafe( name );
                    tempEList->envOldVal  = getenv( name );
                    OldEnvValues = tempEList;
#endif
                    setenv( name, EnvVarValue, TRUE );
                } else {
                    unused_value = addMacro( name, value );
                }
            } else {
                if( getenv( name ) == NULL) {
                    unused_value = addMacro( name, value);
                }
            }

        } else {
            unused_value = addMacro( name, value );
        }
    }
    if( unused_value )
        FreeSafe( value );
    FreeSafe( EnvVarValue );
}


static BOOLEAN printMac( const void *node, const void *ptr )
/**********************************************************/
{
    MACRO const *mac = node;
    char        buff1[MAX_RESOURCE_SIZE];
    char        buff2[MAX_RESOURCE_SIZE];

    (void)ptr; // Unused
    /* mac->node.name is used as a parameter twice in this module because
     * it may substitute before or after the long string depending on the
     * way the message is translated */
    PrtMsg( INF | NEOL | PMAC_NAME_HAS_VALUE, mac->node.name );
    PrtMsg( INF | NEOL | PRNTSTR, mac->value );
    MsgGetTail( PMAC_NAME_HAS_VALUE, buff1 );
    FmtStr( buff2, buff1, mac->node.name );
    PrtMsg( INF | PRNTSTR, buff2 );
    return( FALSE );
}


void PrintMacros( void )
/*****************************/
{
    WalkHashTab( macTab, (BOOLEAN (*)( void *, void * ))printMac, NULL );
    PrtMsg( INF | NEWLINE );
}


#ifdef CLEAN_ENVIRONMENT_VAR
STATIC void restoreEnvironment( void )
/************************************/
{
    ELIST   *current;
    VECSTR  EnvString;

    for( current = OldEnvValues; current != NULL; current = current->next ) {
        EnvString = StartVec();
        WriteVec( EnvString, current->envVarName );
        WriteVec( EnvString, "=" );
        WriteVec( EnvString, current->envOldVal );
        putenv( FinishVec( EnvString ) );
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
    ImplicitDeMacro = FALSE;
    IsPartDeMacro   = FALSE;
}


#ifndef NDEBUG
STATIC BOOLEAN freeMacro( MACRO *mac, const void *ptr )
/*****************************************************/
{
    (void)ptr; // Unused
    FreeSafe( mac->node.name );
    FreeSafe( (char *)(mac->value) );
    FreeSafe( mac );
    return( FALSE );
}
#endif


void MacroFini( void )
/***************************/
{
#ifndef NDEBUG
    WalkHashTab( macTab, (BOOLEAN (*)( void *, void * ))freeMacro, NULL );
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
