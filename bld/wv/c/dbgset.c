/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "strutil.h"
#include "dbgscan.h"
#include "madinter.h"
#include "dbgmad.h"
#include "dbgutil.h"
#include "dbgsrc.h"
#include "trapglbl.h"
#include "dbgmain.h"
#include "dbginvk.h"
#include "dbgcall.h"
#include "dbgshow.h"
#include "dbgovl.h"
#include "dbgparse.h"
#include "dbgtrace.h"
#include "dbgset.h"
#include "dbglkup.h"
#include "dbgchopt.h"
#include "dbgsetfn.h"
#include "dbgsetfg.h"

#include "clibext.h"


extern void             WndUserAdd( char *, unsigned int );

extern margins          SrcMar;
extern margins          AsmMar;

static char_ring        *SupportRtns = NULL;

typedef struct pending_toggle_list      pending_toggle_list;

struct pending_toggle_list {
    pending_toggle_list *next;
    dig_arch            arch;
    char                toggle[1]; /* variable sized */
};

typedef enum {
    MWT_ASM,
    MWT_REG,
    MWT_FPU,
    MWT_MMX,
    MWT_XMM,
    MWT_LAST
} mad_window_toggles;

char    *Language = NULL;

static pending_toggle_list *PendToggleList[MWT_LAST];

static const char SetNameTab[] = {
    "AUtosave\0"
    "ASsembly\0"
    "Variable\0"
    "FUnctions\0"
    "GLobals\0"
    "MOdules\0"
    "REGister\0"
    "Fpu\0"
    "MMx\0"
    "XMm\0"
    "BEll\0"
    "Call\0"
    "Dclick\0"
    "Implicit\0"
    "INput\0"
    "Radix\0"
    "RECursion\0"
    "SEarch\0"
    "SOurce\0"
    "SYmbol\0"
    "TAb\0"
    "TYpes\0"
    "Level\0"
    "LAnguage\0"
    "MAcro\0"
    "SUpportroutine\0"
    "BReakonwrite\0"
    "DOntexpandhex\0"
};

static void (* const SetJmpTab[])( void ) = {
    AutoSet,
    AsmSet,
    VarSet,
    FuncSet,
    GlobSet,
    ModSet,
    RegSet,
    FPUSet,
    MMXSet,
    XMMSet,
    BellSet,
    CallSet,
    DClickSet,
    ImplicitSet,
    InputSet,
    RadixSet,
    RecursionSet,
    SearchSet,
    SourceSet,
    LookSet,
    TabSet,
    TypeSet,
    LevelSet,
    LangSet,
    MacroSet,
    SupportSet,
    BreakOnWriteSet,
    DontExpandHexStringSet,
};

static void (* SetConfJmpTab[])( void ) = {
    AutoConf,
    AsmConf,
    VarConf,
    FuncConf,
    GlobConf,
    ModConf,
    RegConf,
    FPUConf,
    MMXConf,
    XMMConf,
    BellConf,
    CallConf,
    DClickConf,
    ImplicitConf,
    InputConf,
    RadixConf,
    RecursionConf,
    SearchConf,
    SourceConf,
    LookConf,
    TabConf,
    TypeConf,
    LevelConf,
    LangConf,
    MacroConf,
    SupportConf,
    BreakOnWriteConf,
    DontExpandHexStringConf,
    NULL,
};


static void (* SetNotAllTab[])( void ) =
{
    CallConf,
    LevelConf,
    LangConf,
    InputConf,
    NULL,
};




bool SwitchOnOff( void )
{
    int     cmd;

    cmd = ScanCmd( "ON\0OFf\0" );
    if( cmd < 0 )
        Error( ERR_LOC, LIT_ENG( ERR_WANT_ON_OFF ) );
    ReqEOC();
    return( cmd == 0 );
}


void ShowSwitch( bool on )
{
    GetCmdEntry( "ON\0OFf\0", on ? 0 : 1, TxtBuff );
    ConfigLine( TxtBuff );
}


OVL_EXTERN void BadSet( void )
{
    Error( ERR_LOC, LIT_ENG( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
}


/*
 * ProcSet -- process set command
 */


void ProcSet( void )
{
    int     cmd;

    cmd = ScanCmd( SetNameTab );
    if( cmd < 0 ) {
        BadSet();
    } else {
        (*SetJmpTab[cmd])();
    }
}


void ConfigSet( void )
{
    DoConfig( GetCmdName( CMD_SET ), SetNameTab, SetConfJmpTab, SetNotAllTab );
}


/*
 * BellSet - set bell on/off processing
 */

OVL_EXTERN void BellSet( void )
{
    _SwitchSet( SW_BELL, SwitchOnOff() );
}

OVL_EXTERN void BellConf( void )
{
    ShowSwitch( _IsOn( SW_BELL ) );
}


/*
 * AutoSet - set autoconfig on/off processing
 */

OVL_EXTERN void AutoSet( void )
{
    _SwitchSet( SW_AUTO_SAVE_CONFIG, SwitchOnOff() );
}

OVL_EXTERN void AutoConf( void )
{
    ShowSwitch( _IsOn( SW_AUTO_SAVE_CONFIG ) );
}

/*
 *  - set break on write option
 */

OVL_EXTERN void BreakOnWriteSet( void )
{
    _SwitchSet( SW_BREAK_ON_WRITE, SwitchOnOff() );
    SetCapabilitiesExactBreakpointSupport( true, false );
}

OVL_EXTERN void BreakOnWriteConf( void )
{
    ShowSwitch( _IsOn( SW_BREAK_ON_WRITE ) );
}
/*
 *  - set hex expansion
 */

OVL_EXTERN void DontExpandHexStringSet( void )
{
    _SwitchSet( SW_DONT_EXPAND_HEX, SwitchOnOff() );
}

OVL_EXTERN void DontExpandHexStringConf( void )
{
    ShowSwitch( _IsOn( SW_DONT_EXPAND_HEX ) );
}

/*
 * RecursionSet - set recursion checking on/off processing
 */

OVL_EXTERN void RecursionSet( void )
{
    _SwitchSet( SW_RECURSE_CHECK, SwitchOnOff() );
}

OVL_EXTERN void RecursionConf( void )
{
    ShowSwitch( _IsOn( SW_RECURSE_CHECK ) );
}


bool LangSetInit( void )
{
    static char InitialLang[] = { "cpp" };

    LangInit();
    _Alloc( Language, sizeof( InitialLang ) + 1 );
    if( Language == NULL )
        return( false );
    StrCopy( InitialLang, Language );
    return( LangLoad( Language, strlen( Language ) ) );
}

void LangSetFini( void )
{
    _Free( Language );
    LangFini();
}




/*
 * NewLang -- load a new expression language, if different from current one
 */

void NewLang( const char *lang )
{
    char       *new_lang;
    size_t     len;

    if( lang == NULL )
        return;
    len = strlen( lang );
    new_lang = DbgMustAlloc( len + 1 );
    memcpy( new_lang, lang, len );
    new_lang[len] = NULLCHAR;
    strlwr( new_lang );
    if( ( len != strlen( Language ) ) || memcmp( new_lang, Language, len ) != 0 ) {
        if( LangLoad( new_lang, len ) ) {
            _Free( Language );
            Language = new_lang;
            return;
        }
        LangLoad( Language, strlen( Language ) );
        Error( ERR_NONE, LIT_ENG( ERR_NO_LANG ) );
    }
    _Free( new_lang );
}


OVL_EXTERN void LangSet( void )
{
    const char  *start;
    size_t      len;

    ScanItem( true, &start, &len );
    ReqEOC();
    NewLang( start );
}

OVL_EXTERN void LangConf( void )
{
    ConfigLine( Language );
}


/*
        Individual Window Settings
*/
#define ONCE_FOR_ALL_SWITCHES \
    once( SW_ASM_SOURCE ) \
    once( SW_ASM_HEX ) \
    once( SW_VAR_WHOLE_EXPR ) \
    once( SW_VAR_SHOW_CODE ) \
    once( SW_VAR_SHOW_INHERIT ) \
    once( SW_VAR_SHOW_COMPILER ) \
    once( SW_VAR_SHOW_MEMBERS ) \
    once( SW_VAR_SHOW_PRIVATE ) \
    once( SW_VAR_SHOW_PROTECTED ) \
    once( SW_VAR_SHOW_STATIC ) \
    once( SW_FUNC_D2_ONLY ) \
    once( SW_GLOB_D2_ONLY ) \
    once( SW_MOD_ALL_MODULES )

enum {
    #define once( x ) x,
    ONCE_FOR_ALL_SWITCHES
    #undef once
};

typedef struct window_toggle {
    char        on;
    char        off;
    char        sw;
} window_toggle;

enum {
    ON,
    OFF
};


static void SwitchTwiddle( int which, int on )
{
    switch( which ) {
    #define once( x ) \
    case x: \
        _SwitchSet( x, on ); \
        break;
    ONCE_FOR_ALL_SWITCHES
    #undef once
    }
}

static int SwitchIsOn( int which )
{
    switch( which ) {
    #define once( x ) \
    case x: \
        return( _IsOn( x ) ); \
        break;
    ONCE_FOR_ALL_SWITCHES
    #undef once
    }
    return( 0 );
}

static const mad_toggle_strings *GetMADToggleList( const mad_reg_set_data *rsd )
{
    if( rsd == NULL )
        return( MADDisasmToggleList() );
    return( MADRegSetDisplayToggleList( rsd ) );
}

static unsigned DoMADToggle( const mad_reg_set_data *rsd, unsigned on, unsigned off )
{
    if( rsd == NULL )
        return( MADDisasmToggle( on, off ) );
    return( MADRegSetDisplayToggle( rsd, on, off ) );
}

static void PendingAdd( mad_window_toggles wt, dig_arch arch, const char *name, size_t len )
{
    pending_toggle_list **owner;
    pending_toggle_list *new;

    for( owner = &PendToggleList[wt]; (new = *owner) != NULL; owner = &new->next )
        ;
    new = DbgMustAlloc( sizeof( *new ) + len );
    *owner = new;
    new->next = NULL;
    new->arch = arch;
    memcpy( new->toggle, name, len );
    new->toggle[len] = NULLCHAR;
}


static bool DoOneToggle( mad_window_toggles wt )
{
    unsigned                    bit;
    const char                  *start;
    size_t                      len;
    const mad_toggle_strings    *toggles;
    const mad_reg_set_data      *rsd;

    if( !ScanItem( true, &start, &len ) )
        return( false );
    switch( wt ) {
    case MWT_FPU:
        RegFindData( MTK_FLOAT, &rsd );
        if( rsd == NULL ) {
            PendingAdd( SysConfig.arch, wt, start, len );
            return( true );
        }
        break;
    case MWT_REG:
        RegFindData( MTK_INTEGER, &rsd );
        if( rsd == NULL ) {
            PendingAdd( SysConfig.arch, wt, start, len );
            return( true );
        }
        break;
    case MWT_MMX:
        RegFindData( MTK_MMX, &rsd );
        if( rsd == NULL ) {
            PendingAdd( SysConfig.arch, wt, start, len );
            return( true );
        }
        break;
    case MWT_XMM:
        RegFindData( MTK_XMM, &rsd );
        if( rsd == NULL ) {
            PendingAdd( SysConfig.arch, wt, start, len );
            return( true );
        }
        break;
    default:
        rsd = NULL;
        break;
    }
    bit = 1;
    for( toggles = GetMADToggleList( rsd ); toggles->on != MAD_MSTR_NIL; ++toggles ) {
        GetMADNormalizedString( toggles->on, TxtBuff, TXT_LEN );
        if( TxtBuff[0] != NULLCHAR && strnicmp( start, TxtBuff, len ) == 0 ) {
            DoMADToggle( rsd, bit, 0 );
            return( true );
        }
        GetMADNormalizedString( toggles->off, TxtBuff, TXT_LEN );
        if( TxtBuff[0] != NULLCHAR && strnicmp( start, TxtBuff, len ) == 0 ) {
            DoMADToggle( rsd, 0, bit );
            return( true );
        }
        bit <<= 1;
    }
    return( false );
}

void PendingToggles( void )
{
    mad_window_toggles          wt;
    pending_toggle_list         **owner;
    pending_toggle_list         *curr;
    const char                  *scan;

    scan = ScanPos();
    for( wt = 0; wt < MWT_LAST; ++wt ) {
        for( owner = &PendToggleList[wt]; (curr = *owner) != NULL; ) {
            if( curr->arch == SysConfig.arch ) {
                ReScan( curr->toggle );
                DoOneToggle( wt );
                *owner = curr->next;
                _Free( curr );
            } else {
                owner = &curr->next;
            }
        }
    }
    ReScan( scan );
}

static bool OneToggle( mad_window_toggles wt )
{
    const char          *name;
    size_t              len;
    dig_arch            arch_old;
    dig_arch            arch_new;
    const char          *scan;
    bool                res;


    scan = ScanPos();
    if( DoOneToggle( wt ) )
        return( true );
    ReScan( scan );
    if( !ScanItem( true, &name, &len ) )
        return( false );
    for( scan = name; scan <= name + len; ++scan ) {
        if( *scan == '/' ) {
            len = scan - name;
            ReScan( scan );
            break;
        }
    }
    if( CurrToken != T_DIV )
        return( false );
    Scan();
    arch_new = FindMAD( name, len );
    if( arch_new == DIG_ARCH_NIL )
        return( false );
    if( MADLoaded( arch_new ) != MS_OK ) {
        /* put the toggle on the pending list */
        if( !ScanItem( true, &name, &len ) )
            return( false );
        PendingAdd( wt, arch_new, name, len );
        return( true );
    }
    arch_old = MADActiveSet( arch_new );
    res = DoOneToggle( wt );
    MADActiveSet( arch_old );
    return( res );
}

static void ToggleWindowSwitches( window_toggle *toggle, size_t len,
                                const char *settings, mad_window_toggles wt )
{
    int     idx;
    size_t  i;

    while( !ScanEOC() ) {
        if( settings != NULL ) {
            idx = ScanCmd( settings );
            for( i = 0; i < len; ++i ) {
                if( toggle[i].on == idx ) {
                    SwitchTwiddle( toggle[i].sw, 1 );
                    break;
                }
                if( toggle[i].off == idx ) {
                    SwitchTwiddle( toggle[i].sw, 0 );
                    break;
                }
            }
        }
        if( settings == NULL || i == len ) {
            if( wt >= MWT_LAST || !OneToggle( wt ) ) {
                Error( ERR_LOC, LIT_ENG( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
            }
        }
    }
}

static char *DumpAToggle( char *p, dig_arch arch, char *toggle )
{
    if( toggle[0] != NULLCHAR ) {
        MADNameDescription( arch, p, TXT_LEN - ( p - TxtBuff ) );
        for( ; *p != NULLCHAR; ++p ) {
            if( *p == ' ' ) {
                break;
            }
        }
        *p++ = '/';
        p = StrCopy( toggle, p );
        *p++ = ' ';
    }
    return( p );
}

struct dump_toggles {
    mad_window_toggles          wt;
    char                        *p;
};

OVL_EXTERN walk_result DumpToggles( dig_arch arch, void *d )
{
    struct dump_toggles         *td = d;
    const mad_toggle_strings    *toggles;
    unsigned                    bit;
    char                        buff[80];
    const mad_reg_set_data      *rsd;

    if( MADLoaded( arch ) != MS_OK )
        return( WR_CONTINUE );
    switch( td->wt ) {
    case MWT_FPU:
        RegFindData( MTK_FLOAT, &rsd );
        if( rsd == NULL )
            return( WR_CONTINUE );
        break;
    case MWT_REG:
        RegFindData( MTK_INTEGER, &rsd );
        if( rsd == NULL )
            return( WR_CONTINUE );
        break;
    case MWT_MMX:
        RegFindData( MTK_MMX, &rsd );
        if( rsd == NULL )
            return( WR_CONTINUE );
        break;
    case MWT_XMM:
        RegFindData( MTK_XMM, &rsd );
        if( rsd == NULL )
            return( WR_CONTINUE );
        break;
    default:
        rsd = NULL;
        break;
    }
    bit = DoMADToggle( rsd, 0, 0 );
    toggles = GetMADToggleList( rsd );
    while( toggles->menu != MAD_MSTR_NIL ) {
        if( bit & 1 ) {
            GetMADNormalizedString( toggles->on, buff, sizeof( buff ) );
        } else {
            GetMADNormalizedString( toggles->off, buff, sizeof( buff ) );
        }
        td->p = DumpAToggle( td->p, arch, buff );
        bit >>= 1;
        ++toggles;
    }
    return( WR_CONTINUE );
}

static void ConfWindowSwitches( window_toggle *toggle, int len, const char *settings, mad_window_toggles wt )
{
    struct dump_toggles data;
    pending_toggle_list *curr;
    char                *ptr;
    int                 i;

    ptr = TxtBuff;
    for( i = 0; i < len; ++i ) {
        ptr = GetCmdEntry( settings, SwitchIsOn( toggle[i].sw ) ? toggle[i].on : toggle[i].off, ptr );
        *ptr++= ' ';
    }
    if( wt < MWT_LAST ) {
        data.wt = wt;
        data.p = ptr;
        MADWalk( DumpToggles, &data );
        ptr = data.p;
        for( curr = PendToggleList[wt]; curr != NULL; curr = curr->next ) {
            ptr = DumpAToggle( ptr, curr->arch, curr->toggle );
        }
    }
    *ptr = NULLCHAR;
    ConfigLine( TxtBuff );
}


/*
        Assembly window
*/

static const char AsmSettings[] = {
    "Source\0"
    "NOSource\0"
    "Hexadecimal\0"
    "Decimal\0"
};

enum {
    ASM_SOURCE,
    ASM_NOSOURCE,
    ASM_HEX,
    ASM_DECIMAL,
};

static window_toggle    AsmToggle[] = {
    { ASM_SOURCE, ASM_NOSOURCE, SW_ASM_SOURCE },
    { ASM_HEX, ASM_DECIMAL, SW_ASM_HEX },
};

OVL_EXTERN void AsmSet( void )
{
    ToggleWindowSwitches( AsmToggle, ArraySize( AsmToggle ), AsmSettings, MWT_ASM );
    AsmChangeOptions();
}

OVL_EXTERN void AsmConf( void )
{
    ConfWindowSwitches( AsmToggle, ArraySize( AsmToggle ), AsmSettings, MWT_ASM );
}

/*
        FPU Window
*/

OVL_EXTERN void FPUSet( void )
{
    ToggleWindowSwitches( NULL, 0, NULL, MWT_FPU );
    FPUChangeOptions();
}

OVL_EXTERN void FPUConf( void )
{
    ConfWindowSwitches( NULL, 0, NULL, MWT_FPU );
}

/*
        Variables window
*/

static const char VarSettings[] = {
    "Entire\0"
    "Partial\0"
    "CODe\0"
    "NOCODe\0"
    "INherit\0"
    "NOINherit\0"
    "COMpiler\0"
    "NOCOMpiler\0"
    "PRIvate\0"
    "NOPRIvate\0"
    "PROtected\0"
    "NOPROTected\0"
    "STatic\0"
    "NOSTatic\0"
    "Members\0"
    "NOMembers\0"
};

enum {
    VAR_ENTIRE,
    VAR_PARTIAL,
    VAR_CODE,
    VAR_NOCODE,
    VAR_INHERIT,
    VAR_NOINHERIT,
    VAR_COMPILER,
    VAR_NOCOMPILER,
    VAR_PRIVATE,
    VAR_NOPRIVATE,
    VAR_PROTECTED,
    VAR_NOPROTECTED,
    VAR_STATIC,
    VAR_NOSTATIC,
    VAR_MEMBERS,
    VAR_NOMEMBERS,
};

static window_toggle VarToggle[] = {
    { VAR_ENTIRE, VAR_PARTIAL, SW_VAR_WHOLE_EXPR },
    { VAR_CODE, VAR_NOCODE, SW_VAR_SHOW_CODE },
    { VAR_INHERIT, VAR_NOINHERIT, SW_VAR_SHOW_INHERIT },
    { VAR_COMPILER, VAR_NOCOMPILER, SW_VAR_SHOW_COMPILER },
    { VAR_MEMBERS, VAR_NOMEMBERS, SW_VAR_SHOW_MEMBERS },
    { VAR_PRIVATE, VAR_NOPRIVATE, SW_VAR_SHOW_PRIVATE },
    { VAR_PROTECTED, VAR_NOPROTECTED, SW_VAR_SHOW_PROTECTED },
    { VAR_STATIC, VAR_NOSTATIC, SW_VAR_SHOW_STATIC },
};

OVL_EXTERN void VarSet( void )
{
    ToggleWindowSwitches( VarToggle, ArraySize( VarToggle ), VarSettings, MWT_LAST );
    VarChangeOptions();
}

OVL_EXTERN void VarConf( void )
{
    ConfWindowSwitches( VarToggle, ArraySize( VarToggle ), VarSettings, MWT_LAST );
}


static const char FuncSettings[] = {
    "Typed\0"
    "All\0"
};

enum {
    FUNC_TYPED,
    FUNC_ALL,
};

static window_toggle FuncToggle[] = {
    { FUNC_TYPED, FUNC_ALL, SW_FUNC_D2_ONLY },
};

OVL_EXTERN void FuncSet( void )
{
    ToggleWindowSwitches( FuncToggle, ArraySize( FuncToggle ), FuncSettings, MWT_LAST );
    FuncChangeOptions();
}

OVL_EXTERN void FuncConf( void )
{
    ConfWindowSwitches( FuncToggle, ArraySize( FuncToggle ), FuncSettings, MWT_LAST );
}


static window_toggle GlobToggle[] = {
    { FUNC_TYPED, FUNC_ALL, SW_GLOB_D2_ONLY },
};

OVL_EXTERN void GlobSet( void )
{
    ToggleWindowSwitches( GlobToggle, ArraySize( GlobToggle ), FuncSettings, MWT_LAST );
    GlobChangeOptions();
}

OVL_EXTERN void GlobConf( void )
{
    ConfWindowSwitches( GlobToggle, ArraySize( GlobToggle ), FuncSettings, MWT_LAST );
}


static window_toggle ModToggle[] = {
    { FUNC_ALL, FUNC_TYPED, SW_MOD_ALL_MODULES },
};

OVL_EXTERN void ModSet( void )
{
    ToggleWindowSwitches( ModToggle, ArraySize( ModToggle ), FuncSettings, MWT_LAST );
    ModChangeOptions();
}

OVL_EXTERN void ModConf( void )
{
    ConfWindowSwitches( ModToggle, ArraySize( ModToggle ), FuncSettings, MWT_LAST );
}


OVL_EXTERN void RegSet( void )
{
    ToggleWindowSwitches( NULL, 0, NULL, MWT_REG );
    RegChangeOptions();
}

OVL_EXTERN void RegConf( void )
{
    ConfWindowSwitches( NULL, 0, NULL, MWT_REG );
}


OVL_EXTERN void MMXSet( void )
{
    ToggleWindowSwitches( NULL, 0, NULL, MWT_MMX );
    MMXChangeOptions();
}

OVL_EXTERN void MMXConf( void )
{
    ConfWindowSwitches( NULL, 0, NULL, MWT_MMX );
}


OVL_EXTERN void XMMSet( void )
{
    ToggleWindowSwitches( NULL, 0, NULL, MWT_XMM );
    XMMChangeOptions();
}

OVL_EXTERN void XMMConf( void )
{
    ConfWindowSwitches( NULL, 0, NULL, MWT_XMM );
}


void SupportFini( void )
{
    char_ring   *curr, *junk;

    curr = SupportRtns;
    while( curr != NULL ) {
        junk = curr;
        curr = curr->next;
        _Free( junk );
    }
    SupportRtns = NULL;
}


static bool IsInSupportNames( const char *name, size_t len )
{
    char_ring   *curr;

    for( curr = SupportRtns; curr != NULL; curr = curr->next ) {
        if( strlen( curr->name ) == len && memcmp( curr->name, name, len ) == 0 ) {
            return( true );
        }
    }
    return( false );
}

OVL_EXTERN void SupportSet( void )
{
    char_ring   *new;
    const char  *start;
    size_t      len;
    unsigned    count;

    count = 0;
    while( ScanItemDelim( ";}", true, &start, &len ) ) {
        if( !IsInSupportNames( start, len ) ) {
            new = DbgMustAlloc( sizeof( *new ) + len );
            new->next = SupportRtns;
            SupportRtns = new;
            memcpy( new->name, start, len );
            new->name[len] = NULLCHAR;
        }
        ++count;
    }
    ReqEOC();
    // if no argument then clean all support names
    if( count == 0 ) {
        SupportFini();
    }
}

OVL_EXTERN void SupportConf( void )
{
    char_ring   *curr;
    char        *p;

    p = TxtBuff;
    for( curr = SupportRtns; curr != NULL; curr = curr->next ) {
        p = StrCopy( "}", StrCopy( curr->name, StrCopy( "{", p ) ) );
        if( p - TxtBuff > 50 ) {
            ConfigLine( TxtBuff );
            p = TxtBuff;
        }
    }
    if( p != TxtBuff ) {
        ConfigLine( TxtBuff );
    }
}

static bool SupportName( char *name, char *pattern )
{
    for( ; *name != NULLCHAR || *pattern != NULLCHAR; ++name, ++pattern ) {
        if( *pattern == '*' )
            return( true );
        if( *name != *pattern ) {
            return( false );
        }
    }
    return( true );
}

bool IsSupportRoutine( sym_handle *sym )
{
    char_ring                   *curr;
    char                        name[TXT_LEN];

    QualifiedSymName( sym, name, sizeof( name ), true );
    for( curr = SupportRtns; curr != NULL; curr = curr->next ) {
        if( SupportName( name, curr->name ) ) {
            return( true );
        }
    }
    return( false );
}
