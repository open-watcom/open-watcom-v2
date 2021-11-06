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
* Description:  macro processor
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "progsw.h"
#include "errcod.h"
#include "cpopt.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "fmacros.h"

#include "clibext.h"


typedef enum macro_flags {
    MACFLAG_NONE,
    MACFLAG_PERMANENT
} macro_flags;

typedef struct macro_entry {
    struct macro_entry  *link;
    size_t              name_len;
    macro_flags         flags;
    char                name[1];
} macro_entry;

static  macro_entry     *MacroList;
static  unsigned char   NestingLevel;
static  unsigned_16     NestingFlags;
static  unsigned_16     NestingStack;
static  macro_flags     MacroFlags;

#define MAX_NESTING     16

#define DEBUG_MACRO_LEN 9
static  char            DebugMacro[] = { "__debug__" };

void    InitPredefinedMacros( void )
//==================================
{
#if _CPU == 386
    MacroDEFINE( "__386__", 7 );
#elif _CPU == 8086
    MacroDEFINE( "__I86__", 7 );
#elif _CPU == _AXP
    MacroDEFINE( "__AXP__", 7 );
#elif _CPU == _PPC
    MacroDEFINE( "__PPC__", 7 );
#else
    #error Unknown platform/OS
#endif
#if _CPU == 386
    if( CGOpts & CGOPT_STK_ARGS ) {
        MacroDEFINE( "__stack_conventions__", 21 );
    }
#endif
#if _CPU == 8086  || _CPU == 386
    if( CPUOpts & (CPUOPT_FPR | CPUOPT_FPI | CPUOPT_FPI87 |
                   CPUOPT_FP287 | CPUOPT_FP387 | CPUOPT_FP5) ) {
        MacroDEFINE( "__FPI__", 7 );
    }
#endif
    MacroFlags = MACFLAG_NONE;
}


static  void    FreeMacros( bool free_perm )
//==========================================
// Free macros.
{
    macro_entry *link;

    while( MacroList != NULL ) {
        link = MacroList->link;
        if( !free_perm && ( MacroList->flags == MACFLAG_PERMANENT ) )
            break;
        FMemFree( MacroList );
        MacroList = link;
    }
}


static macro_entry *FindMacroEntry( const char *macro, size_t macro_len )
//=======================================================================
// Find a macro.
{
    macro_entry *me;

    for( me = MacroList; me != NULL; me = me->link ) {
        if( me->name_len != macro_len )
            continue;
        if( strnicmp( me->name, macro, macro_len ) == 0 ) {
            return( me );
        }
    }
    return( NULL );
}


bool    CompileDebugStmts( void )
//===============================
// Determine if debug statements should be compiled.
{
    return( FindMacroEntry( DebugMacro, DEBUG_MACRO_LEN ) != NULL );
}


void MacroDEFINE( const char *macro, size_t macro_len )
//=====================================================
// Define a macro.
{
    macro_entry         *me;

    me = FindMacroEntry( macro, macro_len );
    if( me == NULL ) {
        me = FMemAlloc( sizeof( macro_entry ) + macro_len - sizeof( char ) );
        me->link = MacroList;
        me->name_len = macro_len;
        me->flags = MacroFlags;
        memcpy( &me->name, macro, macro_len );
        MacroList = me;
    }
}


void MacroUNDEFINE( const char *macro, size_t macro_len )
//=======================================================
// Define a macro.
{
    macro_entry         **me;
    macro_entry         *free_me;

    for( me = &MacroList; *me != NULL; me = &(*me)->link ) {
        if( (*me)->name_len != macro_len )
            continue;
        if( strnicmp( (*me)->name, macro, macro_len ) == 0 ) {
            free_me = *me;
            *me = free_me->link;
            FMemFree( free_me );
            return;
        }
    }
}


static void SetSkipStatus( void )
//===============================
// Determine whether source is to be skipped.
{
    int         i;

    for( i = 0; i <= NestingLevel; ++i ) {
        if( NestingFlags & ( 1 << i ) ) {
            ProgSw |= PS_SKIP_SOURCE;
            return;
        }
    }
    ProgSw &= ~PS_SKIP_SOURCE;
}


static  void    MacroCondition( bool cond )
//=========================================
// Process a macro condition.
{
    NestingStack |= 1 << NestingLevel;
    if( cond ) {
        NestingFlags &= ~( 1 << NestingLevel );
    } else {
        NestingFlags |= 1 << NestingLevel;
    }
    SetSkipStatus();
}


static  void    IFCondition( bool cond )
//======================================
// Process a IF condition.
{
    if( NestingLevel == MAX_NESTING ) {
        InfoError( CO_MACRO_NESTING_EXCEEDED );
    } else {
        MacroCondition( cond );
        ++NestingLevel;
    }
}


void MacroIFDEF( const char *macro, size_t macro_len )
//====================================================
// Process IFDEF directive.
{
    IFCondition( FindMacroEntry( macro, macro_len ) != NULL );
}


void MacroIFNDEF( const char *macro, size_t macro_len )
//=====================================================
// Process IFNDEF directive.
{
    IFCondition( FindMacroEntry( macro, macro_len ) == NULL );
}


void MacroELIFDEF( const char *macro, size_t macro_len )
//======================================================
// Process ELIFDEF directive.
{
    --NestingLevel;
    if( (NestingStack & ( 1 << NestingLevel )) == 0 ) {
        InfoError( CO_MACRO_STRUCTURE_MISMATCH );
    } else {
        if( NestingFlags & ( 1 << NestingLevel ) ) {
            MacroCondition( FindMacroEntry( macro, macro_len ) != NULL );
        } else {
            NestingFlags |= 1 << NestingLevel;
            SetSkipStatus();
            NestingFlags &= ~( 1 << NestingLevel );
        }
    }
    ++NestingLevel;
}


void MacroELIFNDEF( const char *macro, size_t macro_len )
//=======================================================
// Process ELIFNDEF directive.
{
    --NestingLevel;
    if( (NestingStack & ( 1 << NestingLevel )) == 0 ) {
        InfoError( CO_MACRO_STRUCTURE_MISMATCH );
    } else {
        if( NestingFlags & ( 1 << NestingLevel ) ) {
            MacroCondition( FindMacroEntry( macro, macro_len ) == NULL );
        } else {
            NestingFlags |= 1 << NestingLevel;
            SetSkipStatus();
        }
    }
    ++NestingLevel;
}


void MacroELSE( void )
//====================
// Process ELSE directive.
{
    --NestingLevel;
    if( (NestingStack & ( 1 << NestingLevel )) == 0 ) {
        InfoError( CO_MACRO_STRUCTURE_MISMATCH );
    } else {
        NestingStack &= ~( 1 << NestingLevel );
        if( NestingFlags & ( 1 << NestingLevel ) ) {
            NestingFlags &= ~( 1 << NestingLevel );
        } else {
            NestingFlags |= 1 << NestingLevel;
        }
        SetSkipStatus();
    }
    ++NestingLevel;
}


void MacroENDIF( void )
//=====================
// Process ENDIF directive.
{
    if( NestingLevel == 0 ) {
        InfoError( CO_MACRO_STRUCTURE_MISMATCH );
    } else {
        --NestingLevel;
        NestingFlags &= ~( 1 << NestingLevel );
        SetSkipStatus();
    }
}


void    InitMacroProcessor( void )
//================================
// Initialize macro processor in case macros defined on command line.
{
    MacroFlags = MACFLAG_PERMANENT;
}


void    FiniMacroProcessor( void )
//================================
// Finalize macro processor.
{
    FreeMacros( true );
}


void    InitMacros( void )
//========================
// Initialize macro processor for a compilation.
{
    NestingLevel = 0;
    NestingFlags = 0;
    NestingStack = 0;
}


void    FiniMacros( void )
//========================
// Finalize macro processor for a compilation.
{
    if( NestingLevel != 0 ) {
        InfoError( CO_MACRO_STRUCTURE_MISMATCH );
    }
    FreeMacros( false );
}
