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


#if !defined(__QNX__)
 #include <direct.h>
#endif
#include <env.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "macros.h"
#include "make.h"
#include "massert.h"
#include "memory.h"
#include "mhash.h"
#include "misc.h"
#include "mlex.h"
#include "mpathgrp.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"
#include "mtypes.h"
#include "mupdate.h"
#include "mvecstr.h"



/* arbitrary set the length of the command line */
#define MAX_COMMANDLINE _MAX_PATH*5

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

BOOLEAN     ImplicitDeMacro;


/*
 *   Only Useful in ms option if it is partDeMacro then we only deMacro
 *   everything except for special DeMacro characters
 *
 */
BOOLEAN     IsPartDeMacro;

BOOLEAN     DoingBuiltIn;   /* Are we parsing builtin macros            */

#ifdef CLEAN_ENVIRONMENT_VAR
/* This contains the old environment values before wmake actually */
/* redefines them   only valid in ms-option */
STATIC ELIST* OldEnvValues;
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
{
    assert (p != NULL);
    for( ; *p; ++p ) {
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


extern const char *procPath( const char *fullpath )
/**************************************************
 * process fullpath according to the form qualifier in CurAttr.num
 * returns: pointer to a static buffer
 */
{
    PGROUP      *pg;
    char*       current;

    if( fullpath == NULL ) {
        return( NULL );
    }

    getDirBuf();

    pg = SplitPath( fullpath );

    switch( CurAttr.num ) {
    case FORM_FULL:
        _makepath( dirBuf, pg->drive, pg->dir, pg->fname, pg->ext );
        break;

    case FORM_NOEXT:
        _makepath( dirBuf, pg->drive, pg->dir, pg->fname, NULL );
        break;

    case FORM_NOEXT_NOPATH:
        _makepath( dirBuf, NULL, NULL, pg->fname, NULL );
        break;

    case FORM_NOPATH:
        _makepath( dirBuf, NULL, NULL, pg->fname, pg->ext );
        break;

    case FORM_PATH:
        _makepath( dirBuf, pg->drive, pg->dir, NULL, NULL );
        if ( Glob.microsoft) {
            if (dirBuf[0] == NULLCHAR) {
                dirBuf[0] = '.';
                dirBuf[1] = NULLCHAR;
            } else {
                current = dirBuf;
                while (*current != NULLCHAR) {
                    ++current;
                }
                if (*(current-1) == '\\') {
                    *(current-1) = NULLCHAR;
                }
            }
        }
        break;

    case FORM_EXT:
        _makepath( dirBuf, NULL, NULL, NULL, pg->ext );
        break;
    default:
        dirBuf[0] = '\0';
    }

    DropPGroup( pg );
    massageDollarOctothorpe( dirBuf );
    return( (const char *)dirBuf );
}


STATIC const char *specialValue( TOKEN_T t )
/*******************************************
 * process the special macro t
 * returns: pointer to a static buffer
 */
{
    assert( ( t == MAC_CUR || t == MAC_FIRST || t == MAC_LAST ) &&
            ( FORM_MIN < CurAttr.num && CurAttr.num < FORM_MAX ) );

    switch( t ) {
        case MAC_CUR:       return( procPath( GetCurTarg() ) );
        case MAC_FIRST:     return( procPath( GetFirstDep() ) );
        case MAC_LAST:      return( procPath( GetLastDep() ) );
    }
    return( NULL );
}


STATIC void makeMacroName( char *buffer, const char *name )
/**********************************************************
 * convert name to internal form (upcased)
 * buffer must be at least as large as name
 * Microsoft environment variables are case insensitive
 */
{
    assert( IsMacroName( name ) );

    while( *name != NULLCHAR ) {
        if (Glob.microsoft) {
            *buffer = *name;
        } else {
            *buffer = toupper( *name );
        }
        ++name;
        ++buffer;
    }
    *buffer = NULLCHAR;
}


STATIC MACRO *getMacroNode( const char *name )
/*********************************************
 * returns: pointer to MACRO with this name
 */
{
    BOOLEAN caseSensitive;
    assert( name != NULL && *name != ENVVAR );

    if (Glob.microsoft) {
        caseSensitive = TRUE;
    } else {
        caseSensitive = FALSE;
    }

    return( (MACRO *) FindHashNode( macTab, name, caseSensitive ) );
}


/* returns NULL if error */
STATIC char* findEqual(const char* inString) {
    char* ret;

    ret = strrchr(inString,EQUAL);
    if (ret == inString) {
        ret = NULL;
    }

    return (ret);
}

STATIC RET_T getOldNewString(const char* inString,
                             char** oldString,
                             char** newString) {
    char* equal;

    equal = findEqual(inString);

    if (equal == NULL ) {
        return (RET_ERROR);
    } else {
        *oldString = (char*) inString;
        *equal     = NULLCHAR;
        *newString = equal+1;
        return (RET_SUCCESS);
    }
}

STATIC char *doStringSubstitute( const char *name, const char *oldString,
                                 const char *newString) {
/*****************************************************
 *   $(macroname:oldstr=newstr)
 *   substitute any occurence of oldstr with new str
 */
    VECSTR output;
    char*  current;
    char*  start;
    current = (char*) name;
    start   = (char*) name;

    output = StartVec();
    WriteVec(output,"");


    assert(name != NULL && oldString != NULL && newString != NULL);

    while (1) {
        if (*current == NULLCHAR) {
            break;
        }
        if (strncmp(current,oldString,strlen(oldString)) == 0) {
            CatNStrToVec(output,start,current-start);
            CatStrToVec(output,newString);
            start   = current + strlen(oldString);
            current = start - 1;
        }
        current ++;
    }
    CatStrToVec(output,start);

    return (FinishVec(output));

}


#pragma on (check_stack);
STATIC const char *GetMacroValueProcess( const char *name )
/***************************************************
 * returns: pointer to text of macro (incl. environment vars)
 */
{
    char    macro[ MAX_MAC_NAME ];
    MACRO   *cur;
    char    *env;
    BOOLEAN cdrive;
    BOOLEAN cwd;
    char    *p;
    int     pos;

    assert( IsMacroName( name ) );

    makeMacroName( macro, name );

    if( *macro == ENVVAR ) {
        Glob.disable_TZ_kludge = TRUE;
        env = getenv( macro + 1 );
        Glob.disable_TZ_kludge = FALSE;
        if( env != NULL ) {
            return( env );
        }
        cdrive = strcmp( macro+1, "CDRIVE" ) == 0 ||
                 strcmp( macro+1, "__CDRIVE__" ) == 0;
        cwd    = strcmp( macro+1, "CWD" ) == 0 ||
                 strcmp( macro+1, "__CWD__" ) == 0;
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
                return( dirBuf );
            } else {    /* cwd */
                if( p != NULL ) {
                    return( p + 1 );
                }
                return( dirBuf );
            }
        }
        return( NULL );
    }

    cur = getMacroNode( macro );
    if( cur != NULL ) {
        return( cur->value );
    }

    // If not defined as a macro then get it as a Environment variable
    if (Glob.microsoft) {
        // Check if macro is all caps
        for (pos = 0;macro[pos] != NULLCHAR; ++pos) {
            if (macro[pos] != toupper(macro[pos])) {
                return (NULL);
            }
        }
        env = getenv( macro );
        if (env != NULL) {
            return ( env );
        }
    }

    return( NULL );
}


#pragma off(check_stack);
extern char *GetMacroValue( const char *name ) {
/*******************************************
 * Now we need to check for string substitution
 * $(MACRONAME:oldstring=newstring)
 */

    char       *InName;
    const char *beforeSub;
    char       *afterSub;
    char       *current;
    char       *newString;
    char       *oldString;

    InName = StrDupSafe(name);
    current = InName;
    while (*current != NULLCHAR &&
           *current != COLON) {
        ++current;
    }

    if (*current == NULLCHAR) {
        beforeSub = GetMacroValueProcess (InName);
        if (beforeSub == NULL) {
            afterSub = NULL;
        } else {
            afterSub  = StrDupSafe(beforeSub);
        }
    } else {
        *current = NULLCHAR;
        beforeSub = GetMacroValueProcess (InName);
        ++current;
        if (beforeSub != NULL) {
            if (getOldNewString(current, &oldString, &newString) == RET_SUCCESS){
                afterSub  = doStringSubstitute( beforeSub, oldString,newString);
            } else {
                afterSub  = NULL;
                PrtMsg( ERR|LOC| INVALID_STRING_SUBSTITUTE);
            }
        } else {
            afterSub = NULL;
        }


    }

    FreeSafe(InName);
    return (afterSub);

}


STATIC char *trimMacroValue( char *v )
{
    int space;
    char *t;
    char *p;

    if( Glob.keep_spaces ) {
        return( v );
    }
    space = 0;
    t = v;
    for( p = v; *p != '\0'; ++p ) {
        if( ! isws( *p ) ) {
            if( space ) {
                *t = ' ';
                ++t;
            }
            *t = *p;
            ++t;
            space = 0;
        } else {
            space = 1;
        }
    }
    *t = '\0';
    return( v );
}

#pragma on (check_stack);
STATIC void addMacro( const char *name, char *value )
/****************************************************
 * post:    new macro possibly allocated, copy of name made
 */
{
    char    macro[ MAX_MAC_NAME ];
    MACRO   *new;

    assert( IsMacroName( name ) && *name != ENVVAR );

    value = trimMacroValue( value );
    makeMacroName( macro, name );

    new = getMacroNode( macro );     /* check if redefinition */

    if( new != NULL && !new->readonly ) {   /* reuse old node */
        FreeSafe( (void *) new->value );
        new->value = value;
        new->readonly = Glob.macreadonly;
    } else if ( new == NULL ) {
        new = MallocSafe( sizeof( *new ) ); /* get memory for new node */
        new->node.name = StrDupSafe( macro );
        AddHashNode( macTab, (HASHNODE *)new );
        new->value = value;
        new->readonly = Glob.macreadonly;
    } else {
        FreeSafe( value );  /* read only macro - don't change */
    }
}
#pragma off(check_stack);


extern BOOLEAN IsMacroName( const char *inName )
/*********************************************
 * returns: TRUE if name is a valid macro name, otherwise FALSE and print
 * an error message
 */
{
    char buffer[MAX_MAC_NAME+1]; // contains the macro name with the string
                                 // substitution character ":" removed
    char* name;
    char* current;

    assert( inName != NULL );

    current = (char*) inName;
    name    = buffer;


    while (*current          != NULLCHAR     &&
           *current          != COLON        &&
           (current - inName) <  MAX_MAC_NAME) {
        name[current - inName] = *current;
        ++current;
    }
    name[current-inName] = NULLCHAR;


    if( strlen( name ) >= MAX_MAC_NAME ) {
        PrtMsg( ERR|LOC| MAXIMUM_TOKEN_IS, MAX_MAC_NAME );
        return( FALSE );
    }
    if( *name == NULLCHAR ) {
        PrtMsg( ERR|LOC| INVALID_MACRO_NAME, name );
        return( FALSE );
    }
    while( ismacc( *name ) ) {
        ++name;
    }
    if( *name != NULLCHAR ) {
        PrtMsg( ERR|LOC| INVALID_MACRO_NAME, inName );
        return( FALSE );
    }
    return( TRUE );
}


#pragma on (check_stack);
extern void UnDefMacro( const char *name )
/*****************************************
 * pre:     IsMacroName( name ); getMacroNode( name ) != NULL
 * post:    MACRO node deallocated
 */
{
    char    macro[ MAX_MAC_NAME ];
    MACRO   *dead;
    BOOLEAN caseSensitive;

    assert( IsMacroName( name ) );

    makeMacroName( macro, name );

    if( *macro == ENVVAR ) {
        ENV_TRACKER     *env;

        env = MallocSafe( sizeof( ENV_TRACKER ) + strlen( macro ) + 1 );
        FmtStr( env->value, "%s=", macro + 1 );
        PutEnvSafe( env );
        return;
    }

    if (Glob.microsoft) {
        caseSensitive = TRUE;
    } else {
        caseSensitive = FALSE;
    }
    dead = (MACRO *)RemHashNode( macTab, macro , TRUE);

    assert( dead != NULL );

    FreeSafe( dead->node.name );
    FreeSafe( (void *) dead->value );
    FreeSafe( dead );
}
#pragma off(check_stack);


extern char *WrnGetMacroValue( const char *name )
/*****************************************************/
{
    const char *p;

    p = GetMacroValue( name );
    if( p == NULL ) {
        PrtMsg( DBG|WRN|LOC| MACRO_UNDEFINED, name );   /* 13-Dec-90 DJG */
        // we did this to minimize the number of debugging messages but
        // it causes problems when it defines a macro for the user
        //UnGetCH( EOL );
        //DefMacro( name );                             /* 12-jul-93 AFS */
    }
    return( (char*) p );
    /* note we return NULL if it was undefined! */
}

extern char *DeMacroSpecial ( char* InString) {
/* This function is to specially handle the special macros in the
   dependencies
  */
    char* old;
    char* current;
    VECSTR outString;
    char* tempString;
    char buffer[6];
    int pos;

    assert( InString != NULL);
    current = InString;
    old     = InString;

    outString = StartVec();

    while (*current != NULLCHAR) {
        if (*current == SPECIAL_TMP_DOL_C) {
            CatNStrToVec(outString,old,current-old);
            pos = 0;
            UnGetCH(STRM_MAGIC);
            buffer[pos++] = *(current++);
            if (ismsspecial(*current) && !ismsmodifier(*( current + 1 )) ) {
                buffer[pos++] = *(current++);
            } else {
                assert(ismsspecial(*current));
                buffer[pos++] = *(current++);
                if (ismsmodifier(*current)) {
                    buffer[pos++] = *(current++);
                }
            }
            old = current;
            buffer[pos] = NULLCHAR;
            InsString(buffer,FALSE);
            tempString = DeMacro(STRM_MAGIC);
            PreGetCH();   // eat STRM_MAGIC
            CatStrToVec(outString,tempString);
            FreeSafe(tempString);
        }
        ++current;
    }
    CatNStrToVec(outString,old,current-old+1);
    return(FinishVec(outString));
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


STATIC char *ProcessToken ( int depth, TOKEN_T end1,
                            TOKEN_T end2, TOKEN_T t ) {
/************************************************
 * Processes the tokens returned from lexToken in deMacroToEnd
 */
    STRM_T  temp;
    char    temp_str[2];
    char    macname[ MAX_TOK_SIZE ];
    int     pos;
    char    *p;
    char    *p2;


    switch( t ) {

    case MAC_START:                     /* recurse, get macro name */
        /*
         * in microsoft nmake,
         * $HELLO is considered as $(H)ELLO
         * if there are no parenthesis then it takes only first char.
         * after the $
         */
        if (!Glob.microsoft) {
            p = deMacroText( depth + 1, end1, MAC_PUNC );
        } else {
            temp = PreGetCH ();
            if (ismacc(temp)) {
                temp_str[1] = NULLCHAR;
                temp_str[0] = temp;
                p = StrDupSafe ( temp_str );
            } else {
                p = StrDupSafe ( "" );
            }

        }

        if( IsMacroName( p ) ) {
            p2 =  WrnGetMacroValue(p);
            FreeSafe( p );
            if (p2 != NULL) {
                return (p2);
            }
            return (NULL);
        }
        FreeSafe( p );
        break;

    case MAC_DOLLAR:
        return ( StrDupSafe(TMP_DOL_S) ); /* write a place holder */
        break;

    case MAC_COMMENT:
        return ( StrDupSafe(TMP_COMMENT_S) ); /* write a place holder */
        break;

    case MAC_OPEN:                      /* recurse, get macro name */
        if (!Glob.microsoft) {
            p = deMacroText( depth + 1, end1, MAC_CLOSE );
            if( IsMacroName( p ) ) {
                p2 =  WrnGetMacroValue(p);
                FreeSafe( p );
                if (p2 != NULL) {
                    return (p2);
                }
                return (NULL);
            }
            FreeSafe(p);
        } else {
            pos = 0;
            temp   = PreGetCH();
            if (ismsspecial(temp)) {
                UnGetCH( temp );
                // This is to invoke LexDollar
                t = LexToken(LEX_MS_MAC);
                // This is the only time to get the modifier
                GetModifier();
                p = ProcessToken (depth, end1, end2, t );
                temp = PreGetCH();
                if (temp != ')') {
                    PrtMsg( ERR|LOC| ILLEGAL_CHARACTER_IN_MAC,temp);
                    break;
                }
                return (p);
            } else {
                for (;;) {
                    if (temp == ')') {
                        break;
                    } else if (temp == STRM_MAGIC ||
                               temp == STRM_END   ||
                               temp == EOL) {
                        UnGetCH( temp );
                        break;
                    }
                    if (pos < MAX_TOK_SIZE -1 ) {
                        macname[pos++] = temp;
                    }
                    temp = PreGetCH();

                }
                macname[pos] = NULLCHAR;
                if( IsMacroName( macname ) ) {
                    p2 =  WrnGetMacroValue(macname);
                    if (p2 != NULL) {
                        return (p2);
                    }
                    return (NULL);
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
        p2 = (char*) specialValue( t );
        if (p2 != NULL) {
            return (StrDupSafe(p2));
        }
        return (NULL);
        break;

    case MAC_ALL_DEP:
        p = GetCurDeps( FALSE, FALSE );
        massageDollarOctothorpe( p );
        return ( p );
        break;

    case MAC_INF_DEP:
        p = GetCurDeps( FALSE, TRUE );
        massageDollarOctothorpe( p );
        return ( p );
        break;

    case MAC_YOUNG_DEP:
        p = GetCurDeps( TRUE, FALSE );
        massageDollarOctothorpe( p );
        return( p );
        break;

    case MAC_NAME:                      /* fall through */
    case MAC_WS:
    case MAC_PUNC:
        p = CurAttr.ptr;
        CurAttr.ptr  = NULL;
        return ( p );
        break;

    default:
#ifdef DEVELOPMENT
        PrtMsg( FTL|LOC| INVALID_TOKEN_IN, t, "deMacroToEnd()" );
#else
        PrtMsg( WRN|LOC| IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
        break;
#endif
    }

    return (NULL);
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

    assert(     end1 == EOL
            ||  end1 == MAC_PUNC
            ||  end1 == STRM_MAGIC
            ||  end1 == STRM_END
            ||  end1 == MAC_WS
    );

    assert(     end2 == EOL
            ||  end2 == MAC_PUNC
            ||  end2 == STRM_MAGIC
            ||  end2 == STRM_END
            ||  end2 == MAC_EXPAND_OFF
            ||  end2 == MAC_CLOSE
    );

    vec = StartVec();

    for(;;) {
        t = LexToken( LEX_MAC_SUBST );

        assert( t != MAC_TEXT );            /* should never recv this tok */

        if( t == MAC_CLOSE && end2 != MAC_CLOSE ) {
            t = MAC_PUNC;
            CurAttr.ptr = StrDupSafe( ")" );
        }

        if(     t == STRM_END               /* always stops at these */
            ||  t == STRM_MAGIC
            ||  t == EOL
            ||  t == end2
            ||  t == end1
            ||  ( t == MAC_WS && ( end2 == MAC_PUNC || end1 == MAC_PUNC ) )
                ) {
            break;
        }

        p = ProcessToken ( depth, end1, end2, t );
        if (p != NULL) {
            CatStrToVec(vec, p);
            FreeSafe (p);
        }

    }

            /* put back the token we demacro'd until */
    switch( t ) {
    case MAC_PUNC:
    case MAC_WS:
        InsString( CurAttr.ptr, TRUE );
        break;
    case EOL:               /* fall through */
    case STRM_END:
    case STRM_MAGIC:
        UnGetCH( t );
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
        PrtMsg( FTL|LOC| CANNOT_NEST_FURTHER );
    }

    result = deMacroToEnd( depth, end1, end2 );
    while( strchr( result, DOLLAR ) != NULL ) {

        UnGetCH( STRM_MAGIC );
        InsString( result, TRUE );

        ++depth;
        if( depth > MAX_MAC_NEST ) {
            PrtMsg( FTL|LOC| CANNOT_NEST_FURTHER );
        }

        /*
         * note that we don't pass end1 here because we put this data
         * into the stream ourselves - the caller has nothing to do
         * with it.
         */

        result = deMacroToEnd( depth, STRM_MAGIC, STRM_MAGIC );
        (void) LexToken( LEX_MAC_SUBST );      /* eat STRM_MAGIC */
    }
    p = result;

    /*
     * when it is in part demacro do not change the dollar and the comments
     * yet wait until the deMacro is called
     */
    if (!IsPartDeMacro) {
        while( *p != NULLCHAR ) {
            switch( *p ) {
            case TMP_DOL_C:     *p = DOLLAR;    break;
            case TMP_COMMENT_C: *p = COMMENT;   break;
#if 0
            case SPECIAL_TMP_DOL_C:
                  if (Glob.microsoft) {
                       if (ismsspecial(*(p+1))) {
                          *p = DOLLAR;
                       }
                  }
                  break;
#endif
            }
            ++p;
        }
    } else {
        while( *p != NULLCHAR ) {
            switch( *p ) {
            case SPECIAL_TMP_DOL_C:     *p = DOLLAR;    break;
            }
            ++p;
        }
    }


    return( result );
}

extern char *ignoreWSDeMacro(BOOLEAN partDeMacro, BOOLEAN ForceDeMacro) {
/**********************************
 * This is the same as deMacro however if the string has leading and trailing
 * ws then leave it in.
 * Boolean = TRUE when you want partDeMacro
 */

    VECSTR  DeMacroText;
    VECSTR  temp;
    char    text         [MAX_COMMANDLINE];
    char    leadingSpace [MAX_COMMANDLINE];
    char    *TrailSpace;
    char    *DeMacroStr;
    char    *current;
    TOKEN_T t;


    t = PreGetCH();
    current = leadingSpace;
    while (isws(t) &&
           current - leadingSpace < MAX_COMMANDLINE -1) {
        *current = t;
        ++current;
        t = PreGetCH();
    }
    *current = NULLCHAR;

    current = text;
    TrailSpace = text + strlen(text);
    while ( current - text < MAX_COMMANDLINE -1) {
        if (t == STRM_END   ||
            t == STRM_MAGIC ||
            t == EOL) {
            break;
        } else if (!isws(t)) {
            TrailSpace = current + 1;
        }
        *current = t;
        t = PreGetCH();
        ++current;
    }
    *current = NULLCHAR;
    UnGetCH(t);

    DeMacroText = StartVec();
    WriteNVec(DeMacroText,text,TrailSpace - text);
    InsString(FinishVec(DeMacroText),TRUE);

    if (!partDeMacro) {
        DeMacroStr = DeMacro( EOL );
    } else {
        DeMacroStr = PartDeMacro ( ForceDeMacro);
    }

    DeMacroText = StartVec();
    WriteVec(DeMacroText,leadingSpace);
    temp        = StartVec();
    WriteVec(temp,DeMacroStr);
    FreeSafe(DeMacroStr);
    CatVec(DeMacroText,temp);
    temp        = StartVec();
    WriteVec(temp,TrailSpace);
    CatVec(DeMacroText,temp);

    return(FinishVec(DeMacroText));


}

extern char *DeMacro( TOKEN_T end1 )
/***********************************
 * same as deMacroText
 */
{
    return( deMacroText( 0, end1, STRM_END ) );
}

STATIC char *PartDeMacroProcess ( void )
/*******************************
 * Partially DeMacro until EOL.  Copies verbatim, only expanding
 * $+$- delimited sequences.  Removes leading and trailing ws.
 */
{
    VECSTR  vec;        /* vector for macro defn                */
    VECSTR  wsvec;      /* vector to hold ws                    */
    BOOLEAN holdws;     /* holding ws from last time through    */
    BOOLEAN leadingws;  /* still trimming leading ws            */
    TOKEN_T t;

    vec = StartVec();

    leadingws = TRUE;
    holdws = FALSE;

    t = LexToken( LEX_MAC_DEF );
    while( t != STRM_END && t != EOL ) {

        switch( t ) {
        case STRM_MAGIC:        /* we ignore these */
            break;
        case MAC_EXPAND_ON:
            InsString( deMacroText( 0, EOL, MAC_EXPAND_OFF ), TRUE );
            break;
        case MAC_WS:
            if( !leadingws ) {
                if( !holdws ) {
                    holdws = TRUE;
                    wsvec = StartVec();
                }
                WriteVec( wsvec, CurAttr.ptr );
            }
            FreeSafe( CurAttr.ptr );
            break;
        case MAC_TEXT:
            if( holdws && !leadingws ) {
                CatVec( vec, wsvec );
                holdws = FALSE;
            }
            leadingws = FALSE;
            WriteVec( vec, CurAttr.ptr );
            FreeSafe( CurAttr.ptr );
            break;
        default:
#ifdef DEVELOPMENT
            PrtMsg( FTL| INVALID_TOKEN_IN, t, "PartDeMacro" );
#else
            PrtMsg( WRN|LOC| IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
#endif
        }

        t = LexToken( LEX_MAC_DEF );
    }

    if( holdws ) {          /* trim trailing ws */
        FreeVec( wsvec );
    }

    return( FinishVec( vec ) );
}


extern BOOLEAN ForceDeMacro ( void ) {
/******************************************
 * This function checks whether or not to deMacro the function later or not
 * because in Microsoft macros are expanded immediately and not after all
 * the processing.  Note that in the clist for implicit rules this is not
 * true so we need to expand the macros in inline files later
 * For Watcom the default is FALSE
 */
    return (Glob.microsoft && !ImplicitDeMacro);
}

extern char *PartDeMacro( BOOLEAN ForceDeMacro) {
/******************************************
 * the addition of microsoft option needs this option
 * since MACROS are always fully expanded sequentially
 * ForceDeMacro if set true will force full Demacro
 */
    STRM_T t;
    char  *temp;

    if (Glob.microsoft) {
        IsPartDeMacro = TRUE;
    }
    if ( ForceDeMacro ) {
        //remove white spaces at the beginning
        while (isws(t = PreGetCH())){
        }
        UnGetCH(t);
        temp = DeMacro(EOL);
        t = PreGetCH();
        if (Glob.microsoft) {
            IsPartDeMacro = FALSE;
        }
        return (temp);
    } else {
        if (Glob.microsoft) {
            IsPartDeMacro = FALSE;
        }
        return (PartDeMacroProcess());
    }
}


STATIC int CompareNMacroName (const char* name1,
                              const char* name2,
                              size_t len) {
    if (Glob.microsoft) {
        return (strncmp  (name1,name2,len) );
    } else {
        return (strnicmp (name1,name2,len) );
    }
}


STATIC char* DeMacroName ( char* text, const char *name ) {
/**********************************************************
 * Tries to find if there are occurrences of name in text
 * this is only for ms-option fix because nmake relies on this
 * functionality to function correctly
 * Note that this is possible since in nmake $(TEMP$(TEMP)) is illegal
 * in wmake this is legal syntax
 * Also in nmake, $HELLO will expand as $(H)ELLO unlike in wmake
 * $HELLO is synonymous to $(HELLO)
 */

    char*   current;
    char*   oldptr;
    char*   temp;
    char*   macronameStr;
    VECSTR  outtext;
    VECSTR  macroname;
    int     lengthToClose;

    assert (name != NULL && text != NULL && Glob.microsoft);

    current = text;
    oldptr  = text;

    outtext = StartVec();
    while(*current != NULLCHAR) {

        if( CompareNMacroName(current,name,strlen(name)) == 0 ) {
            if (text <= current -2) {
                // Checks if the MacroName match is preceded by
                // MAC_OPEN
                if ( *(current - 1) == '(' && *(current - 2) == DOLLAR) {
                    // Check if ended by MAC_CLOSE
                    // or ended by :   if we have string substitution
                    if (*(current + strlen(name)) == ')' ||
                        *(current + strlen(name)) == ':') {
                        lengthToClose = 0;
                        while (*(current +
                               strlen(name) + lengthToClose) != ')') {
                            ++lengthToClose;
                        }
                        CatNStrToVec(outtext, oldptr, current - 2 - oldptr);
                        macroname = StartVec();
                        CatNStrToVec(macroname,current,
                                     strlen(name)+lengthToClose);
                        macronameStr = FinishVec(macroname);
                        if (IsMacroName( macronameStr ) ) {
                            temp = GetMacroValue( macronameStr );
                            if (temp != NULL) {
                                CatStrToVec(outtext,temp);
                                FreeSafe( temp );
                            }
                        }
                        FreeSafe(macronameStr);
                        oldptr  = current + strlen(name) + 1 +lengthToClose;
                        current = oldptr;
                        continue;
                    }
                }
            }
            // Microsoft name without parenthesis is only of length 1
            if (text <= current -1 && strlen(name) == 1) {
                // Checks if the MacroName is preceded by just a dollar sign
                if (*(current -1 ) == DOLLAR) {
                    CatNStrToVec(outtext, oldptr, current - 1 - oldptr);
                    if (IsMacroName( name ) ) {
                        temp = GetMacroValue( name );
                        if (temp != NULL) {
                            CatStrToVec(outtext,temp);
                            FreeSafe( temp );
                        }
                    }
                    oldptr  = current + strlen(name);
                    current = oldptr;
                    continue;
                }
            }
        }
        ++current;
    }
    CatStrToVec(outtext,oldptr);
    return (FinishVec( outtext ));
}


extern void DefMacro( const char *name )
/***************************************
 * define macro named name with the characters in the stream
 * post:    characters up to first EOL | STRM_END removed from stream;
 *          macro named name defined
 *
 */
{
    char        *value;
    char        *temp;
    char        *EnvVarValue; /* used for env. variables (full demacro) */
    ENV_TRACKER *env;
#ifdef CLEAN_ENVIRONMENT_VAR
    ELIST       *tempEList;
#endif

    assert( IsMacroName( name ) );

    /*
     *  This is needed because in Microsoft defining a macro
     *  can have a recursive call defined variable is part of the
     *  macro definition or else an infinite loop might result
     *  ie. TEMP = $(TEMP) would result in an infinite loop
     *      so we need to force expansion of $(TEMP) if it is microsoft
     *      option
     */
    if(Glob.microsoft) {
        temp  = PartDeMacro ( FALSE);
        value = DeMacroName ( temp , name);
        FreeSafe( temp );
    } else {
        value = PartDeMacro ( FALSE );
    }

    if ( *name == ENVVAR || (Glob.microsoft && getenv(name) != NULL )) {
        if ( *name != ENVVAR ) {
            addMacro( name, value );
        }
        UnGetCH( EOL );
        InsString( value, FALSE );
        EnvVarValue = DeMacro( EOL );
        PreGetCH();  // eat EOL token (used to avoid assertion failure)
    }

    if( *name == ENVVAR ) {
            /* remember strlen( name ) is one byte larger than we want
             * because *name == ENVVAR, and we'll ignore that byte
             */
        env = MallocSafe( sizeof( ENV_TRACKER )
                + strlen( name ) + strlen( EnvVarValue ) + 1 );
        FmtStr( env->value, "%s=%s", name + 1, EnvVarValue );
        FreeSafe( value );
        FreeSafe( EnvVarValue );
        PutEnvSafe( env );
    } else {
        if (Glob.microsoft){
            if (!DoingBuiltIn) {
                if (getenv(name) != NULL) {
#ifdef CLEAN_ENVIRONMENT_VAR
                    tempEList = NewEList();
                    tempEList->next = OldEnvValues;
                    tempEList->envVarName = StrDupSafe(name);
                    tempEList->envOldVal  = getenv(name);
                    OldEnvValues = tempEList;
#endif
                    setenv(name,EnvVarValue,TRUE);
                } else {
                    addMacro( name, value );
                }
            } else {
                if (getenv(name) == NULL) {
                    addMacro( name, value);
                }
            }

        } else {
            addMacro( name, value );
        }
    }
}


#pragma off(unreferenced);
BOOLEAN printMac( const void *node, const void *ptr )
#pragma on (unreferenced);
/**********************************************************/
{
    MACRO *mac = (void*) node;
    char  buff1[MAX_RESOURCE_SIZE];
    char  buff2[MAX_RESOURCE_SIZE];

    /* mac->node.name is used as a parameter twice in this module because
     * it may substitute before or after the long string depending on the
     * way the message is translated */
    PrtMsg( INF|NEOL| PMAC_NAME_HAS_VALUE, mac->node.name );
    PrtMsg( INF|NEOL| PRNTSTR, mac->value );
    MsgGetTail( PMAC_NAME_HAS_VALUE, buff1 );
    FmtStr( buff2, buff1, mac->node.name );
    PrtMsg( INF| PRNTSTR, buff2 );
    return( FALSE );
}



extern void PrintMacros( void )
/*****************************/
{
    WalkHashTab( macTab, (BOOLEAN (*) (void*,void*))printMac, NULL );
    PrtMsg( INF| NEWLINE );
}

#ifdef CLEAN_ENVIRONMENT_VAR
STATIC void restoreEnvironment( void ) {
    ELIST* current;
    VECSTR EnvString;

    current = OldEnvValues;
    while (current!= NULL) {
        EnvString = StartVec();
        WriteVec(EnvString,current->envVarName);
        WriteVec(EnvString,"=");
        WriteVec(EnvString,current->envOldVal);
        putenv((const char*) FinishVec(EnvString));
        current = current->next;
    }
}
#endif

extern void MacroInit( void )
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
#pragma off(unreferenced);
STATIC BOOLEAN freeMacro( MACRO *mac, const void *ptr )
#pragma on (unreferenced);
/*****************************************************/
{
    FreeSafe( mac->node.name );
    FreeSafe( (char*)(mac->value) );
    FreeSafe( mac );
    return( FALSE );
}
#endif


extern void MacroFini( void )
/***************************/
{
#ifndef NDEBUG
    WalkHashTab( macTab, (BOOLEAN (*) (void*,void*))freeMacro, NULL );
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
