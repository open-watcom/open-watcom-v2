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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "plusplus.h"

#include <ctype.h>
#include <banner.h>

#include "memmgr.h"
#include "preproc.h"
#include "scan.h"
#include "cmdline.h"
#include "ring.h"
#include "ringname.h"
#include "cgdata.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "context.h"
#include "srcfile.h"
#include "cgback.h"
#include "vstk.h"
#include "vbuf.h"
#include "toggles.h"
#ifndef NDEBUG
    #include "togglesd.h"
#endif

#include "cmdlnprs.gh"
#include "cmdlnsys.h"


static  RINGNAMECTL undef_names =       // #UNDEF NAMES LIST
{   NULL                                // - ring
, true                                  // - case sensitive
};


static void nextSwitch(         // POSITION FOR NEXT SWITCH
    void )
{
    for( CmdScanChar(); ! CmdScanSwEnd(); CmdScanChar() );
}


void BadCmdLine(                // SIGNAL CMD-LINE ERROR
    MSG_NUM error_code )        // - error code
{
    CErr1( error_code );
    nextSwitch();
    CmdLnCtxInfo();
}

void CmdLnWarn(                 // ISSUE WARNING FOR A SWITCH
    MSG_NUM message )           // - message code
{
    CErr1( message );
    nextSwitch();
    CmdLnCtxInfo();
}

void CmdLnSwNotImplemented(     // ISSUE WARNING FOR UN-IMPLEMENTED SWITCH
    void )
{
    CmdLnWarn( WARN_SWITCH_NOT_IMPL );
}

static MEPTR defineStringMacro( // DEFINE A MACRO NAME
    const char *name )          // - name of macro
{
    MEPTR mentry;
    char const *save = CmdScanAddr();

    CmdScanInit( name );
    mentry = DefineCmdLineMacro( false );
    CmdScanInit( save );
    return( mentry );
}


MEPTR DefineCmdLineMacro(       // DEFINE A MACRO FROM THE COMMAND LINE
                                // (assumes position is one char after "-D")
    bool many_tokes )           // - true ==> scan multiple tokens
{
    MEPTR mentry;
    int (*old_scanner)( void );
    ppctl_t old_ppctl;

    old_scanner = NextChar;
    old_ppctl = PPControl;
    NextChar = &SrcFileCmdLnGetChar;
    PPCTL_ENABLE_EOL();
    PPCTL_DISABLE_MACROS();
    SrcFileCmdLnDummyOpen();
    CurrChar = NextChar();
    if( many_tokes ) {
        mentry = MacroScan( MSCAN_CMDLN_PLUS );
    } else {
        mentry = MacroScan( MSCAN_CMDLN_NORMAL );
    }
    SrcFileCmdLnDummyClose();
    NextChar = old_scanner;
    PPControl = old_ppctl;
    return( mentry );
}


void PreDefineStringMacro(      // PREDEFINE A MACRO
    const char *str )           // - macro definition
{
    char buffer[32];            // - buffer for name
    char *bptr;                 // - points into buffer
    const char *sptr;           // - points into string
    MEPTR mentry;               // - macro entry

    if( !CompFlags.undefine_all_macros ) {
        mentry = NULL;
        if( undef_names.ring == NULL ) {
            mentry = defineStringMacro( str );
        } else {
            for( bptr = buffer, sptr = str; ; ++ sptr, ++ bptr ) {
                if( *sptr == '=' ) {
                    *bptr = '\0';
                    break;
                }
                *bptr = *sptr;
                if( *bptr == '\0' ) {
                    break;
                }
            }
            if( NULL == RingNameLookup( &undef_names, buffer ) ) {
                mentry = defineStringMacro( str );
            }
        }
        if( mentry != NULL ) {
            MacroCanBeRedefined( mentry );
        }
    }
}


void AddUndefName()             // SAVE A #UNDEF NAME
{
    int len;                    // - length of option
    char const *opt;            // - option
    char *mac_name;             // - macro name

    CmdScanChar();
    len = CmdScanId( &opt );
    if( len == 0 ) {
        CompFlags.undefine_all_macros = true;
    } else {
        mac_name = RingNameAllocVct( &undef_names, opt, len );
        MacroCmdLnUndef( mac_name, len );
    }
}


static void defineKeywordMacros(   // PREDEFINE KEYWORD MACROS
    void )
{
    static const char *mac_table[] = { // - predefined macros
      "near=__near"
    , "far=__far"
    , "huge=__huge"
    , "cdecl=__cdecl"
    , "pascal=__pascal"
    , "fortran=__fortran"
    , "interrupt=__interrupt"
    , "_near=__near"
    , "_far=__far"
    , "_far16=__far16"
    , "_huge=__huge"
    , "_cdecl=__cdecl"
    , "_pascal=__pascal"
    , "_fortran=__fortran"
    , "_interrupt=__interrupt"
    , "_export=__export"
    , "_loadds=__loadds"
    , "_saveregs=__saveregs"
    , "_based=__based"
    , "_self=__self"
    , "_segname=__segname"
    , "_segment=__segment"
    , "_syscall=__syscall"
    , "_inline=__inline"
    , "_stdcall=__stdcall"
    , "_fastcall=__fastcall"
    , "_asm=__asm"
    , "_emit=__emit"
    , NULL
    };

    const char  **mac;                  // - current macro

    for( mac = mac_table; *mac != NULL; ++ mac ) {
        PreDefineStringMacro( *mac );
    }
}

static void defineFeatureMacros( void )
{
    char        *end_watcom;
    char        *end_feature;
    const char  **p;
    char        buff[128];
    static const char *feature[] = {
        "BOOL",
        "MUTABLE",
        "RTTI",
        "EXPLICIT",
        "NAMESPACE",
        "NEW_CASTS",
        "INT64",
        "TYPENAME",
        NULL
    };

    end_watcom = stxpcpy( buff, "__WATCOM_" );
    for( p = feature; *p != NULL; ++p ) {
        end_feature = stxpcpy( end_watcom, *p );
        strcpy( end_feature, "__" );
        PreDefineStringMacro( buff );
    }
    defineStringMacro( "_PUSHPOP_SUPPORTED" );
}


void MiscMacroDefs(             // PREDEFINE MISCELLANEOUS MACROS
    void )
{
    if( CompFlags.inline_intrinsics ) {
        // this is the only place 'inline_intrinsics' is checked
        // the mechanism works as follows:
        // (1) user sets -ox or -oi
        // (2) macro __INLINE_FUNCTIONS__ is defined
        // (3) user includes <header.h>
        // (4) various prototypes are marked as "intrinsic"
        // (5) fns marked 'intrinsic' will be looked up in code burst tables
        // (6) code for fn will be generated inline
        defineStringMacro( "__INLINE_FUNCTIONS__" );
    }
    if( !CompFlags.extensions_enabled ) {
        defineStringMacro( "_NO_EXT_KEYS" );
        if( CompFlags.non_iso_compliant_names_enabled ) {
            defineStringMacro( "NO_EXT_KEYS" );
        }
    }
    if( CompFlags.non_iso_compliant_names_enabled ) {
        defineKeywordMacros();
    }
    defineStringMacro( "_WCHAR_T_DEFINED" );
    defineStringMacro( "_STDWCHAR_T_DEFINED" );
    if( CompFlags.signed_char ) {
        defineStringMacro( "__CHAR_SIGNED__" );              /* 20-apr-90 */
    }
    if( CompFlags.excs_enabled ) {
        defineStringMacro( "_CPPUNWIND" );
    }
    if( CompFlags.rtti_enabled ) {
        defineStringMacro( "_CPPRTTI" );
    }
    defineFeatureMacros();
    if( !CompFlags.no_alternative_tokens ) {
        DefineAlternativeTokens();
    }
    PreDefineStringMacro( "__WATCOMC__=" _MACROSTR( _BLDVER ) );
    PreDefineStringMacro( "__WATCOM_CPLUSPLUS__=" _MACROSTR( _BLDVER ) );
    // #if __WATCOM_REVISION__ >= 8
    PreDefineStringMacro( "__WATCOM_REVISION__=8" );
    if( !TOGGLE( check_stack ) ) {
        DefSwitchMacro( "S" );
    }
    RingNameFree( &undef_names );
}

void InitModInfo(               // INITIALIZE MODULE INFORMATION
    void )
{
    PackAmount = TARGET_PACKING;
    GblPackAmount = PackAmount;
    SrcFileSetTab( DEF_TAB_WIDTH );
    RingNameInit( &undef_names );
    DataThreshold = 32767;
    OptSize = 50;
    WholeFName = NULL;
    ObjectFileName = NULL;
    ModuleName = NULL;
    ErrorFileName = strsave( "*" );
    ErrLimit = 20;
    WngLevel = WLEVEL_DEFAULT;
    /* set CompFlags defaults */
    CompFlags.extensions_enabled = true;
    CompFlags.non_iso_compliant_names_enabled = true;
    CompFlags.emit_library_names = true;
    CompFlags.emit_dependencies = true;
    CompFlags.emit_targimp_symbols = true;
    CompFlags.check_truncated_fnames = true;
    CompFlags.inline_functions = true;

    memset( &PragmaToggles, 0, sizeof( PragmaToggles ) );
#ifndef NDEBUG
    memset( &PragmaDbgToggles, 0, sizeof( PragmaDbgToggles ) );
#endif
    TOGGLE( check_stack ) = true;
    TOGGLE( unreferenced ) = true;

    SetAuxWatcallInfo();

    CgBackSetInlineDepth( DEFAULT_INLINE_DEPTH );
    CgBackSetOeSize( 0 );
    CgBackSetInlineRecursion( false );
}

unsigned VerifyPackAmount( unsigned pack )
/****************************************/
{
    if( pack <= 1 ) {
        return( 1 );
    }
    if( pack <= 2 ) {
        return( 2 );
    }
    if( pack <= 4 ) {
        return( 4 );
    }
    if( pack <= 8 ) {
        return( 8 );
    }
    return( 16 );
}
