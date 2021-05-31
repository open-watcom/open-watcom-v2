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
* Description:  Pragma information processing.
*
****************************************************************************/


#include "ftnstd.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "global.h"
#include "fcgbls.h"
#include "wf77aux.h"
#include "wf77prag.h"
#include "errcod.h"
#include "cpopt.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "inout.h"
#include "cspawn.h"
#include "asmstmt.h"
#include "chain.h"
#include "symtab.h"
#include "option.h"
#include "cioconst.h"

#include "clibext.h"


#define AddDefaultLibConst(l,p) AddDefaultLib(l, sizeof( l ) - 1, p)


default_lib             *DefaultLibs;
dep_info                *DependencyInfo;

const char              *TokStart;
const char              *TokEnd;

#if _INTEL_CPU
static arr_info         *ArrayInfo;
#endif


void InitPragma( void )
//=====================
{
    DefaultLibs = NULL;
    DependencyInfo = NULL;

    InitPragmaAux();
}

void FiniPragma( void )
//=====================
{
    FiniPragmaAux();

    FreeChain( &DefaultLibs );
    // in case of fatal error, FiniAuxInfo() is called
    // from TDPurge()
#if _INTEL_CPU
    FreeChain( &ArrayInfo );
#endif
    FreeChain( &DependencyInfo );
}

void SubPragmaInit( void )
//========================
// Initialize pragmas information for a subprogram.
{
#if _INTEL_CPU
    ArrayInfo = NULL;
#endif
}

void SubPragmaFini( void )
//========================
// Finalize pragmas information for a subprogram.
{
#if _INTEL_CPU
    arr_info    *next;
    sym_id      arr;

    while( ArrayInfo != NULL ) {
        next = ArrayInfo->link;
        arr = SymFind( ArrayInfo->arr, ArrayInfo->len );
        if( ( arr != NULL ) && (arr->u.ns.flags & SY_SUBSCRIPTED) &&
            ( arr->u.ns.u1.s.typ != FT_CHAR ) &&
            ( (arr->u.ns.flags & SY_SUB_PARM) || _Allocatable( arr ) ) ) {
            arr->u.ns.si.va.u.dim_ext->dim_flags |= DIM_EXTENDED;
        }
        FMemFree( ArrayInfo );
        ArrayInfo = next;
    }
#endif
}

#if _INTEL_CPU
static bool cmp_ucased( const char *p1, const char *p2, size_t len )
//==================================================================
{
    size_t  i;

    for( i = 0; i < len; i++ ) {
        if( *p1++ != toupper( *p2++ ) ) {
            return( false );
        }
    }
    return( true );
}

static void AddArrayInfo( const char *arr_name, size_t arr_len )
//==============================================================
// Process aux information for an array.
{
    arr_info    **arr;
    arr_info    *new_arr;
    size_t      i;

    for( arr = &ArrayInfo; *arr != NULL; arr = &(*arr)->link ) {
        if( (*arr)->len != arr_len )
            continue;
        if( cmp_ucased( (*arr)->arr, arr_name, arr_len ) ) {
            return;
        }
    }
    new_arr = FMemAlloc( sizeof( arr_info ) + arr_len );
    new_arr->link = NULL;
    for( i = 0; i < arr_len; i++ ) {
        new_arr->arr[i] = toupper( arr_name[i] );
    }
    new_arr->arr[arr_len] = NULLCHAR;
    new_arr->len = arr_len;
    *arr = new_arr;
}
#endif

void AddDependencyInfo( source_t *fi )
//====================================
// Add dependency information for an included file.
{
    char        *p;
    dep_info    **dep;
    dep_info    *new_dep;
    struct stat stat_info;
    char        buff[_MAX_PATH];

    p = _fullpath( buff, fi->name, _MAX_PATH );
    if( p != NULL ) {
        for( dep = &DependencyInfo; *dep != NULL; dep = &(*dep)->link ) {
            if( strcmp( (*dep)->fn, p ) == 0 ) {
                return;
            }
        }
        if( stat( p, &stat_info ) != -1 ) {
            new_dep = FMemAlloc( sizeof( dep_info ) + strlen( p ) );
            new_dep->link = NULL;
            strcpy( new_dep->fn, p );
            new_dep->time_stamp = stat_info.st_mtime;
            *dep = new_dep;
        }
    }
}


static void AddDefaultLib( const char *lib_ptr, size_t lib_len, char priority )
//=============================================================================
{
    default_lib         **lib;
    default_lib         *new_lib;

    if( (Options & OPT_DFLT_LIB) == 0 )
        return;
    if( ( *lib_ptr == '"' ) && ( lib_ptr[lib_len - 1] == '"' ) ) {
        lib_len -= 2;
        ++lib_ptr;
    }
    for( lib = &DefaultLibs; *lib != NULL; lib = &(*lib)->link ) {
        if( strlen( &(*lib)->lib[1] ) != lib_len )
            continue;
        if( memcmp( &(*lib)->lib[1], lib_ptr, lib_len ) == 0 ) {
            return;
        }
    }
    new_lib = FMemAlloc( sizeof( default_lib ) + lib_len );
    new_lib->link = NULL;
    new_lib->lib[0] = priority;
    memcpy( &new_lib->lib[1], lib_ptr, lib_len );
    new_lib->lib[1+lib_len] = NULLCHAR;
    *lib = new_lib;
}


void DefaultLibInfo( void )
//=========================
{
#if _CPU == 386
    if( CGOpts & CGOPT_STK_ARGS ) {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flibs", '1' );
            AddDefaultLibConst( "math3s", '1' );
        } else {
            AddDefaultLibConst( "flib7s", '1' );
            AddDefaultLibConst( "math387s", '1' );
        }
        AddDefaultLibConst( "clib3s", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresfs", '1' );
        }
    } else {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flib", '1' );
            AddDefaultLibConst( "math3r", '1' );
        } else {
            AddDefaultLibConst( "flib7", '1' );
            AddDefaultLibConst( "math387r", '1' );
        }
        AddDefaultLibConst( "clib3r", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresf", '1' );
        }
    }
    if( CPUOpts & CPUOPT_FPI ) {
        AddDefaultLibConst( "emu387", '1' );
    } else if( CPUOpts & CPUOPT_FPI87 ) {
        AddDefaultLibConst( "noemu387", '1' );
    }
#elif _CPU == 8086
    if( CGOpts & CGOPT_M_MEDIUM ) {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flibm", '1' );
            AddDefaultLibConst( "mathm", '1' );
        } else {
            AddDefaultLibConst( "flib7m", '1' );
            AddDefaultLibConst( "math87m", '1' );
        }
        AddDefaultLibConst( "clibm", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresm", '1' );
        }
    } else {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flibl", '1' );
            AddDefaultLibConst( "mathl", '1' );
        } else {
            AddDefaultLibConst( "flib7l", '1' );
            AddDefaultLibConst( "math87l", '1' );
        }
        AddDefaultLibConst( "clibl", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresl", '1' );
        }
    }
    if( CPUOpts & CPUOPT_FPI ) {
        AddDefaultLibConst( "emu87", '1' );
    } else if( CPUOpts & CPUOPT_FPI87 ) {
        AddDefaultLibConst( "noemu87", '1' );
    }
#elif _CPU == _AXP
    AddDefaultLibConst( "flib", '1' );
    AddDefaultLibConst( "math", '1' );
    AddDefaultLibConst( "clib", '1' );
    if( Options & OPT_RESOURCES ) {
        AddDefaultLibConst( "wresaxp", '1' );
    }
#elif _CPU == _PPC
    AddDefaultLibConst( "flib", '1' );
    AddDefaultLibConst( "math", '1' );
    AddDefaultLibConst( "clib", '1' );
    if( Options & OPT_RESOURCES ) {
        AddDefaultLibConst( "wresppc", '1' );
    }
#endif
}


bool CurrToken( const char *tok )
//===============================
{
    const char  *ptr;

    ptr = TokStart;
    for(;;) {
        if( ptr == TokEnd )
            break;
        if( toupper( *ptr ) != *tok )
            break;
        ptr++;
        tok++;
    }
    if( ( ptr == TokEnd ) && ( *tok == NULLCHAR ) )
        return( true );
    return( false );
}


void ScanToken( void )
//====================
{
    const char  *ptr;
    bool        found_token;
    bool        first;

    ptr = TokEnd;
    ptr = SkipBlanks( ptr );
    TokStart = ptr;
    first = true;
    found_token = false;
    for(;;) {
        switch( *ptr ) {
        case ' ' :
        case '\t' :
        case NULLCHAR :
            found_token = true;
            break;
        case '"' :
            if( first ) {
                for(;;) {
                    ++ptr;
                    if( *ptr == NULLCHAR )
                        break;
                    if( *ptr == '"' ) {
                        ++ptr;
                        break;
                    }
                }
            }
            found_token = true;
            break;
        case '[' :
        case ']' :
        case '(' :
        case ')' :
        case ',' :
        case '=' :
        case '*' :
            if( first ) {
                ++ptr;
            }
            found_token = true;
            break;
        case '\\' :
            if( first ) {
                ReadSrc();
                ptr = SrcBuff;
                if( ( *ptr == '*' ) || ( *ptr == 'C' ) || ( *ptr == 'c' ) ) {
                    ++ptr;
                } else {
                    Error( PR_BAD_CONTINUATION );
                    CSuicide();
                }
                ptr = SkipBlanks( ptr );
                TokStart = ptr;
            } else {
                found_token = true;
            }
            break;
        default :
            first = false;
            ptr++;
        }
        if( found_token ) {
            break;
        }
    }
    TokEnd = ptr;
}

bool RecToken( const char *tok )
//==============================
{
    if( CurrToken( tok ) ) {
        ScanToken();
        return( true );
    }
    return( false );
}

static void ScanFnToken( void )
//=============================
{
    const char  *ptr;
    bool        found_token;
    bool        first;

    ptr = TokEnd;
    ptr = SkipBlanks( ptr );
    TokStart = ptr;
    first = true;
    for( found_token = false; !found_token; ) {
        switch( *ptr ) {
        case ' ' :
        case '\t' :
        case NULLCHAR :
            found_token = true;
            break;
        case '"' :
            if( first ) {
                for(;;) {
                    ++ptr;
                    if( *ptr == NULLCHAR )
                        break;
                    if( *ptr == '"' ) {
                        ++ptr;
                        break;
                    }
                }
            }
            found_token = true;
            break;
        default :
            first = false;
            ptr++;
        }
    }
    TokEnd = ptr;
}


static bool RecFnToken( const char *tok )
//=======================================
{
    if( CurrToken( tok ) ) {
        ScanFnToken();
        return( true );
    }
    return( false );
}


void SymbolId( void )
//===================
{
    const char  *ptr;
    char        c;

    /* invalidate first character if it is digit */
    ptr = TokStart;
    c = *ptr;
    if( isdigit( c ) )
        c = '!';
    while( ptr != TokEnd ) {
        if( ( isalnum( c ) == 0 ) && ( c != '$' ) && ( c != '_' ) ) {
            Error( PR_SYMBOL_NAME );
            CSuicide();
        }
        c = *(++ptr);
    }
}


static void     Pragma( void )
//============================
// Process a pragma.
{
#if _INTEL_CPU
    const char  *arr;
    size_t      arr_len;
#endif

    if( RecFnToken( "LIBRARY" ) ) {
        if( RecFnToken( "\0" ) ) {
            DefaultLibInfo();
        } else {
            while( !RecFnToken( "\0" ) ) {
                AddDefaultLib( TokStart, TokEnd - TokStart, '9' );
                ScanFnToken();
            }
        }
#if _INTEL_CPU
    } else if( RecToken( "ARRAY" ) ) {
        SymbolId();
        arr = TokStart;
        arr_len = TokEnd - TokStart;
        ScanToken();
        if( RecToken( "FAR" ) ) {
            if( _SmallDataModel( CGOpts ) ) {
                AddArrayInfo( arr, arr_len );
            }
  #if _CPU == 8086
        } else if( RecToken( "HUGE" ) ) {
            if( CGOpts & CGOPT_M_LARGE ) {
                AddArrayInfo( arr, arr_len );
            }
  #endif
        }
#endif
    } else if( RecToken( "LINKAGE" ) ) {
        PragmaLinkage();
    } else if( RecToken( "AUX" ) ) {
        PragmaAux();
    }
}


void DoPragma( const char *ptr )
//==============================
{
    int         status;

    TokStart = ptr;
    TokEnd = ptr;
    ScanToken();
    for(;;) {
        status = CSpawn( &Pragma );
        if( status != 0 ) {
            if( ProgSw & PS_FATAL_ERROR ) {
                CSuicide();
            }
            AsmSymFini();
            break;
        }
        if( RecToken( "\0" ) ) {
            break;
        }
    }
}


void ProcPragma( const char *ptr )
//================================
{
    // don't process auxiliary pragma's until pass 2
    if( ProgSw & PS_DONT_GENERATE )
        return;
    DoPragma( ptr );
}

