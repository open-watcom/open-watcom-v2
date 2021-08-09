/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Command line parsing for Novell Netware file formats.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "linkstd.h"
#include "alloc.h"
#include "cmdutils.h"
#include "msg.h"
#include "exenov.h"
#include "loadnov.h"
#include "wlnkmsg.h"
#include "cmdall.h"
#include "cmdnov.h"
#include "cmdline.h"


#ifdef _NOVELL

/*
//  should move these somewhere more suitable
*/
#define IS_NUMBER(ptr)     ((*ptr >= '0') && (*ptr <= '9'))
#define IS_WHITESPACE(ptr) (*(ptr) == ' ' || *(ptr) =='\t' || *(ptr) == '\r')

void SetNovFmt( void )
/********************/
{
    Extension = E_NLM;
}

void FreeNovFmt( void )
/*********************/
{
    _LnkFree( FmtData.description );
    _LnkFree( FmtData.u.nov.screenname );
    _LnkFree( FmtData.u.nov.checkfn );
    _LnkFree( FmtData.u.nov.exitfn );
    _LnkFree( FmtData.u.nov.customdata );
    _LnkFree( FmtData.u.nov.threadname );
    _LnkFree( FmtData.u.nov.copyright );
    _LnkFree( FmtData.u.nov.messages );
    _LnkFree( FmtData.u.nov.help );
    _LnkFree( FmtData.u.nov.rpcdata );
    _LnkFree( FmtData.u.nov.sharednlm );
/*  FreeList( FmtData.u.nov.exp.export );
    FreeList( FmtData.u.nov.exp.module );  Permalloc'd now */
}

void CmdNovFini( void )
/*********************/
{
}

static bool IsNetWarePrefix( const char *token, size_t tokenlen )
/***************************************************************/
{
    if( ( token != NULL ) && ( token[0] == '(' ) && ( token[tokenlen - 1] == ')' ) )
        return( true );
    return( false );
}

static bool NetWareSplitSymbol( const char *token, size_t tokenlen, const char **name, size_t *namelen, const char **prefix, size_t *prefixlen )
/**********************************************************************************************************************************************/
{
    const char  *findAt = token;
    size_t      len;

    if( (NULL == token) || (0 == tokenlen) || (NULL == name) || (NULL == namelen) || (NULL == prefix) || (NULL == prefixlen) )
        return( false );

    *name = *prefix = NULL;
    *namelen = *prefixlen = 0;

    for( len = tokenlen; len; len-- ) {
        if( '@' == *findAt )
            break;
        if( '\0' == *findAt ) {
            len = 0;   /* force zero */
            break;
        }
        findAt++;
    }

    if( 0 == len ) {
        *name = token;
        *namelen = tokenlen;
        return( true );
    }

    /*
     *  findAt now points at an @ symbol. this maybe a stdcall designator or a prefixed symbol.
     *  if the following character is a number then it must be stdcall as it is illegal to start
     *  a function name with a numeric character (I believe)
     */

    if( IS_NUMBER( &findAt[1] ) ) {
        *name = token;
        *namelen = tokenlen;
        return( true );
    }

    if( findAt != token ) {
        *prefix = token;
        *prefixlen = findAt - token;
    }

    *name = findAt + 1;
    *namelen = len - 1;

    return( true );
}

static bool SetCurrentPrefix( const char *str, size_t len )
/*********************************************************/
{
    const char  *s;
    char        *p;

    /*
     *  Always delete
     */
    if( CmdFile->symprefix != NULL ) {
        _LnkFree( CmdFile->symprefix );
        CmdFile->symprefix = NULL;
    }

    if( ( NULL == str ) || ( len == 0 ) ) {
        return( true );
    }
    /* it suppose string format as "(.....)" */
    str++;  /* skip opening parentheses */
    len--;  /* and record that */

    for( ; len > 0; --len, ++str ) {
        if( !IS_WHITESPACE( str ) ) {
            break;
        }
    }
    if( len == 0 )
        return( false );

    --len;  /* skip closing parentheses */
    if( len == 0 )
        return( false );

    for( s = str + len - 1; len > 0; --len, --s ) {
        if( !IS_WHITESPACE( s ) ) {
            break;
        }
    }
    if( len == 0 )
        return( false );

    /* convert to C string */
    _LnkAlloc( p, len + 1 );
    memcpy( p, str, len );
    p[len] = '\0';
    CmdFile->symprefix = p;
    return( true );
}

/*
 * Trouble! In files, import and export specifiers may or may not have a trailing comma
 * so we look ahead to Token.next and see if there is a comma next (after whitespace)
 * and if there is then we don't set this flag else we do
 * this also affects us using
 *      IMPORT x, (PREFIX), y, (PREFIX), x
 */
static bool DoWeNeedToSkipASeparator( bool CheckDirectives )
/**********************************************************/
{
    const char  *parse;

    if( ( Token.next == NULL ) || ( *Token.next == '\0' ) )
        return( false );

    for( parse = Token.next; *parse != '\0'; parse++ ) {
        if( !IS_WHITESPACE( parse ) ) {
            break;
        }
    }

    if( *parse == '\0' )
        return( false );

    if( *parse == ',' )
        return( false );

    /*
    //  skip cr-lf
    */
    if( ( *parse == '\n' ) || ( *parse == '\r' ) )
        parse++;
    if( ( *parse == '\n' ) || ( *parse == '\r' ) )
        parse++;

    /*
    //  always skip to the next token if the next available token is not a comma
    //  this will allow individual tokens without commas which isn't a big deal
    */
    if( ( *parse != '\0' ) && ( *parse != ',' ) ) {
        /*
        //  If the next token is __not__ a comma then we need to check that it is not a directive
        //  before allowing the skip!
        */
        if( CheckDirectives ) {
            size_t      len = 0;
            const char  *t;

            for( t = parse; !IS_WHITESPACE(t); ++t ) {
                len++;
            }

            if( DoMatchDirective( parse, len ) ) {
                return( false );
            }
        }
        return( true );
    }
    return( false );
}

void SetNovImportSymbol( symbol *sym )
/************************************/
{
    sym->p.import = DUMMY_IMPORT_PTR;
}

static bool GetSymbolImportExport( bool import )
/**********************************************/
{
    symbol      *sym;
    const char  *name;
    const char  *prefix;
    size_t      namelen;
    size_t      prefixlen;
    bool        result;

    name = NULL;
    namelen = 0;
    prefix = NULL;
    prefixlen = 0;
    /*
     *  we need to trap import/export prefixes here. Unfortunately the prefix context
     *  is not followed by a valid seperator so the GetToken() call in ProcArgList
     *  at the end of the do...while loop terminates the loop after we return from
     *  this call (and WildCard from where we were called of course
     */
    if( IsNetWarePrefix( Token.this, Token.len ) ) {
        result = SetCurrentPrefix( Token.this, Token.len );
        if( result ) {
            Token.skipToNext = DoWeNeedToSkipASeparator( false );
#ifndef NDEBUG
            printf( "Set new prefix. Skip = %d\n", Token.skipToNext );
#endif
        }
        return( result );
    }

    if( !NetWareSplitSymbol( Token.this, Token.len, &name, &namelen, &prefix, &prefixlen ) ) {
        return( false );
    }

    if( ( prefix == NULL || ( 0 == prefixlen ) ) && ( NULL != CmdFile->symprefix ) ) {
        prefix = CmdFile->symprefix;
        prefixlen = strlen( prefix );
    }
    if( import ) {
        sym = SymOpNWPfx( ST_CREATE_DEFINE_NOALIAS, name, namelen, prefix, prefixlen );
        if( sym == NULL || sym->p.import != NULL ) {
            return( true );
        }
#ifndef NDEBUG
        printf( "imported %s from %s\n", sym->name.u.ptr, ( sym->prefix != NULL ) ? sym->prefix : "(NONE)" );
#endif
        SET_SYM_TYPE( sym, SYM_IMPORTED );
        sym->info |= SYM_DCE_REF;   // make sure we don't try to get rid of these.
        SetNovImportSymbol( sym );
    } else {
        sym = SymOpNWPfx( ST_CREATE_REFERENCE, name, namelen, prefix, prefixlen );
        sym->info |= SYM_DCE_REF | SYM_EXPORTED;
        AddNameTable( name, namelen, true, &FmtData.u.nov.exp.export );
    }
    Token.skipToNext = DoWeNeedToSkipASeparator( true );

    return( true );
}


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

static bool ProcScreenName( void )
/********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    if( Token.len > MAX_SCREEN_NAME_LENGTH ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "SCREENNAME" );
    } else {
        if( FmtData.u.nov.screenname != NULL ) {
            _LnkFree( FmtData.u.nov.screenname );  // assume second is correct.
        }
        FmtData.u.nov.screenname = tostring();
    }
    return( true );
}

static bool ProcCheck( void )
/***************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    FmtData.u.nov.checkfn = tostring();
    return( true );
}

static bool ProcMultiLoad( void )
/*******************************/
{
    FmtData.u.nov.exeflags |= NOV_MULTIPLE;
    return( true );
}

static bool ProcAutoUnload( void )
/********************************/
{
    FmtData.u.nov.exeflags |= NOV_AUTOUNLOAD;
    return( true );
}


static bool ProcReentrant( void )
/*******************************/
{
    FmtData.u.nov.exeflags |= NOV_REENTRANT;
    return( true );
}

static bool ProcSynch( void )
/***************************/
{
    FmtData.u.nov.exeflags |= NOV_SYNCHRONIZE;
    return( true );
}

static bool ProcPseudoPreemption( void )
/**************************************/
{
    FmtData.u.nov.exeflags |= NOV_PSEUDOPREEMPTION;
    return( true );
}

static bool ProcNLMFlags( void )
/******************************/
{
    unsigned_32 value;

    if( !GetLong( &value ) )
        return( false );
    FmtData.u.nov.exeflags |= value;
    return( true );
}

static bool ProcCustom( void )
/****************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( false );
    }
    FmtData.u.nov.customdata = tostring();         // no default extension.
    return( true );
}

static bool ProcMessages( void )
/******************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( false );
    }
    FmtData.u.nov.messages = tostring();           // no default extension.
    return( true );
}

static bool ProcHelp( void )
/**************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( false );
    }
    FmtData.u.nov.help = tostring();       // no default extension.
    return( true );
}

static bool ProcXDCData( void )
/*****************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( false );
    }
    FmtData.u.nov.rpcdata = tostring();    // no default extension.
    return( true );
}

static bool ProcSharelib( void )
/******************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        return( false );
    }
    FmtData.u.nov.sharednlm = FileName( Token.this, Token.len, E_NLM, false );
    return( true );
}

static bool ProcExit( void )
/**************************/
{
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    FmtData.u.nov.exitfn = tostring();
    return( true );
}

static bool ProcThreadName( void )
/********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    } else if( Token.len > MAX_THREAD_NAME_LENGTH ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "THREADNAME" );
    } else {
        FmtData.u.nov.threadname = tostring();
    }
    return( true );
}

#define COPYRIGHT_START     "Copyright 1991"
#define DEFAULT_COPYRIGHT COPYRIGHT_START " Novell, Inc. All rights reserved"
#define DEFAULT_COPYRIGHT_LENGTH (sizeof( DEFAULT_COPYRIGHT ) - 1)
#define YEAR_OFFSET (sizeof( COPYRIGHT_START ) - 1)

static bool ProcCopyright( void )
/*******************************/
{
    struct tm       *currtime;
    time_t          thetime;
    unsigned        year;
    char            *copy_year;

    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) && !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        if( FmtData.u.nov.copyright != NULL ) {
            _LnkFree( FmtData.u.nov.copyright );  // assume second is correct.
        }
        _ChkAlloc( FmtData.u.nov.copyright, DEFAULT_COPYRIGHT_LENGTH + 1 );
        memcpy(FmtData.u.nov.copyright,DEFAULT_COPYRIGHT,DEFAULT_COPYRIGHT_LENGTH+1);
        copy_year = FmtData.u.nov.copyright + YEAR_OFFSET;
        thetime = time( NULL );
        currtime = localtime( &thetime );
        for( year = currtime->tm_year + 1900; year > 0; year /= 10 ) {
            *copy_year = '0' + (year % 10);
            copy_year--;
        }
    } else {
        if( Token.len > MAX_COPYRIGHT_LENGTH ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "COPYRIGHT" );
        } else {
            if( FmtData.u.nov.copyright != NULL ) {
                _LnkFree( FmtData.u.nov.copyright );  // assume second is correct.
            }
            FmtData.u.nov.copyright = tostring();
        }
    }
    return( true );
}

static bool ProcOSDomain( void )
/******************************/
{
    FmtData.u.nov.exeflags |= NOV_OS_DOMAIN;
    return( true );
}

static parse_entry  MainOptions[] = {
    "SCReenname",       ProcScreenName,         MK_NOVELL, 0,
    "CHeck",            ProcCheck,              MK_NOVELL, 0,
    "MULTILoad",        ProcMultiLoad,          MK_NOVELL, 0,
    "AUTOUNload",       ProcAutoUnload,         MK_NOVELL, 0,
    "REentrant",        ProcReentrant,          MK_NOVELL, 0,
    "SYnchronize",      ProcSynch,              MK_NOVELL, 0,
    "CUSTom",           ProcCustom,             MK_NOVELL, 0,
    "EXit",             ProcExit,               MK_NOVELL, 0,
    "THReadname",       ProcThreadName,         MK_NOVELL, 0,
    "PSeudopreemption", ProcPseudoPreemption,   MK_NOVELL, 0,
    "COPYRight",        ProcCopyright,          MK_NOVELL, 0,
    "MESsages",         ProcMessages,           MK_NOVELL, 0,
    "HElp",             ProcHelp,               MK_NOVELL, 0,
    "XDCdata",          ProcXDCData,            MK_NOVELL, 0,
    "SHArelib",         ProcSharelib,           MK_NOVELL, 0,
    "OSDomain",         ProcOSDomain,           MK_NOVELL, 0,
    "NLMFlags",         ProcNLMFlags,           MK_NOVELL, 0,
    NULL
};

bool ProcNovOptions( void )
/*************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}


/****************************************************************
 * "IMPort" Directive
 ****************************************************************/

static bool GetNovImport( void )
/******************************/
{
    return( GetSymbolImportExport( true ) );
}

bool ProcNovImport( void )
/************************/
{
    SetCurrentPrefix( NULL, 0 );
    return( ProcArgListEx( GetNovImport, TOK_INCLUDE_DOT, CmdFile ) );
}


/****************************************************************
 * "EXPort" Directive
 ****************************************************************/

static bool GetNovExport( void )
/******************************/
{
    return( GetSymbolImportExport( false ) );
}

bool ProcNovExport( void )
/************************/
{
    SetCurrentPrefix( NULL, 0 );
    return( ProcArgListEx( GetNovExport, TOK_INCLUDE_DOT, CmdFile ) );
}


/****************************************************************
 * "MODule" Directive
 ****************************************************************/

static bool GetNovModule( void )
/******************************/
{
    AddNameTable( Token.this, Token.len, false, &FmtData.u.nov.exp.module );
    return( true );
}

bool ProcNovModule( void )
/************************/
{
    return( ProcArgList( GetNovModule, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "Debug" Directive
 ****************************************************************/

bool ProcNovExportsDBI( void )
/****************************/
{
    DBIFlag |= DBI_ONLY_EXPORTS;
    FmtData.u.nov.flags |= DO_WATCOM_EXPORTS;
    return( true );
}

static bool ProcNovDBIExports( void )
/***********************************/
{
    FmtData.u.nov.flags |= DO_NOV_EXPORTS;
    return( true );
}

static bool ProcNovDBIReferenced( void )
/**************************************/
{
    FmtData.u.nov.flags |= DO_NOV_REF_ONLY;
    return( true );
}

static parse_entry  NovDBIOptions[] = {
    "ONLyexports",  ProcNovDBIExports,      MK_NOVELL,  0,
    "REFerenced",   ProcNovDBIReferenced,   MK_NOVELL,  0,
    NULL
};

bool ProcNovDBI( void )
/*********************/
{
    LinkFlags |= LF_NOVELL_DBI_FLAG;
    if( ProcOne( NovDBIOptions, SEP_NO ) ) {
        while( ProcOne( NovDBIOptions, SEP_COMMA ) ) {
            ; /*null loop*/
        }
    }
    return( true );
}


/****************************************************************
 * "Format" Directive
 ****************************************************************/

static bool ProcModuleTypeN( int n )
/**********************************/
{
    Extension = E_NLM;
    FmtData.u.nov.moduletype = n;
    return( true );
}

static bool ProcNLM( void )
/*************************/
{
    Extension = E_NLM;
    FmtData.u.nov.moduletype = 0;
    return( true );
}

static bool ProcLAN( void )
/*************************/
{
    Extension = E_LAN;
    FmtData.u.nov.moduletype = 1;
    return( true );
}

static bool ProcDSK( void )
/*************************/
{
    Extension = E_DSK;
    FmtData.u.nov.moduletype = 2;
    return( true );
}

static bool ProcNAM( void )
/*************************/
{
    Extension = E_NAM;
    FmtData.u.nov.moduletype = 3;
    return( true );
}

static bool ProcModuleType4( void )
/*********************************/
{
    Extension = E_NLM;
    FmtData.u.nov.moduletype = 4;
    return( true );
}

static bool ProcModuleType5( void )
/*********************************/
{
    Extension = E_NOV_MSL;
    FmtData.u.nov.moduletype = 5;
    return( true );
}

static bool ProcModuleType6( void )
/*********************************/
{
    Extension = E_NLM;
    FmtData.u.nov.moduletype = 6;
    return( true );
}

static bool ProcModuleType7( void )
/*********************************/
{
    Extension = E_NLM;
    FmtData.u.nov.moduletype = 7;
    return( true );
}

static bool ProcModuleType8( void )
/*********************************/
{
    Extension = E_NOV_HAM;
    FmtData.u.nov.moduletype = 8;
    return( true );
}

static bool ProcModuleType9( void )
/*********************************/
{
    Extension = E_NOV_CDM;
    FmtData.u.nov.moduletype = 9;
    return( true );
}

#if 0
/*
// as I have got tired of writing, module types 10 through 12 are reserved */
static bool ProcModuleType10( void )
/**********************************/
{
    Extension = ;
    FmtData.u.nov.moduletype = 10;
    return( true );
}

static bool ProcModuleType11( void )
/**********************************/
{
    Extension = ;
    FmtData.u.nov.moduletype = 11;
    return( true );
}

static bool ProcModuleType12( void )
/**********************************/
{
    Extension = ;
    FmtData.u.nov.moduletype = 12;
    return( true );
}
#endif

static parse_entry  NovFormats[] = {
    "NLM",          ProcNLM,            MK_NOVELL, 0,    /* 0 */
    "LAN",          ProcLAN,            MK_NOVELL, 0,    /* 1 */
    "DSK",          ProcDSK,            MK_NOVELL, 0,    /* 2 */
    "NAM",          ProcNAM,            MK_NOVELL, 0,    /* 3 */
    "0",            ProcNLM,            MK_NOVELL, 0,    /* 0 again */
    "1",            ProcLAN,            MK_NOVELL, 0,    /* etc */
    "2",            ProcDSK,            MK_NOVELL, 0,
    "3",            ProcNAM,            MK_NOVELL, 0,
    "4",            ProcModuleType4,    MK_NOVELL, 0,
    "5",            ProcModuleType5,    MK_NOVELL, 0,
    "6",            ProcModuleType6,    MK_NOVELL, 0,
    "7",            ProcModuleType7,    MK_NOVELL, 0,
    "8",            ProcModuleType8,    MK_NOVELL, 0,
    "9",            ProcModuleType9,    MK_NOVELL, 0,
#if 0
    /* NLM types 10 through 12 are currently reserved */
    "10",           ProcModuleType10,   MK_NOVELL, 0,
    "11",           ProcModuleType11,   MK_NOVELL, 0,
    "12",           ProcModuleType12,   MK_NOVELL, 0,
#endif
    NULL
};

bool ProcNovFormat( void )
/************************/
{
    if( !ProcOne( NovFormats, SEP_NO ) ) {          // get file type
        int     nType = 0;

        if( (nType = atoi( Token.this )) > 0 ) {
            GetToken( SEP_NO, TOK_INCLUDE_DOT );
            ProcModuleTypeN( nType );
        } else {
            ProcNLM();
        }
    }
    if( GetToken( SEP_QUOTE, TOK_INCLUDE_DOT ) ) {  // get description
        FmtData.description = tostring();
    }
    return( true );
}

#endif
