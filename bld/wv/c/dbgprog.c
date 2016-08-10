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
* Description:  Processing of the NEW command, program and symbol loading.
*
****************************************************************************/


#include <stdio.h>
#include <limits.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "spawn.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbghook.h"
#include "mad.h"
#include "dui.h"
#include "srcmgt.h"
#include "tistrail.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "filelcl.h"
#include "filermt.h"
#include "dbgsrc.h"
#include "dbgmain.h"
#include "dbgshow.h"
#include "dbgovl.h"
#include "dbgbrk.h"
#include "dbgparse.h"
#include "dbgdot.h"
#include "dbgprog.h"
#include "dbgtrace.h"
#include "remcore.h"
#include "remfile.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"
#include "addarith.h"
#include "dbgevent.h"
#include "dbgupdt.h"
#include "dbglkup.h"
#include "ntdbgpb.h"

#include "clibext.h"


extern void             StdInNew( void );
extern void             StdOutNew( void );
extern char             *GetCmdArg( int );
extern void             SetCmdArgStart( int, char * );
extern void             SetNoSectSeg( void );
extern void             SetLastExe( const char *name );
extern void             VarFreeScopes( void );
extern void             VarUnMapScopes( image_entry * );
extern void             VarReMapScopes( image_entry * );
//extern const char       *CheckForPowerBuilder( const char * );
extern void             WndSetCmdPmt(char *,char *,unsigned int ,void (*)(void));

extern bool             DownLoadTask;

const char              *RealFName( const char *name, open_access *loc );

static bool             CopyToRemote( const char *local, const char *remote, bool strip, void *cookie );

static char             *SymFileName;
static char             *TaskCmd;
static process_info     *CurrProcess; //NYI: multiple processes

char_ring               *LocalDebugInfo;

#define SYM_FILE_IND ':'

bool InitCmd( void )
{
    unsigned    argc;
    char        *curr;
    char        *ptr;
    char        *end;
    char        *parm;
    char        *last;
    unsigned    total;
    char        *start;
    char        c;

    curr = GetCmdArg( 0 );
    if( curr != NULL ) {
        while( *curr == ' ' || *curr == '\t' )
            ++curr;
        if( *curr == SYM_FILE_IND ) {
            ++curr;
            while( *curr == ' ' || *curr == '\t' )
                ++curr;
            start = curr;
            while( *curr != ' ' && *curr != '\t' && *curr != NULLCHAR )
                ++curr;
            _Alloc( parm, curr - start + 1 );
            if( parm == NULL )
                return( false );
            SymFileName = parm;
            while( start < curr )
                *parm++ = *start++;
            *parm = NULLCHAR;
            while( *curr == ' ' || *curr == '\t' )
                ++curr;
            argc = 0;
            if( *curr == NULLCHAR ) {
                curr = GetCmdArg( ++argc );
            }
            SetCmdArgStart( argc, curr );
        }
    }
    total = 0;
    for( argc = 0; (curr = GetCmdArg( argc )) != NULL; ++argc ) {
        while( *curr++ != NULLCHAR )
            ++total;
        ++total;
    }
    _Alloc( TaskCmd, total + 2 );
    if( TaskCmd == NULL )
        return( false );
    ptr = TaskCmd;
    for( argc = 0; (curr = GetCmdArg( argc )) != NULL; ++argc ) {
        while( (c = *curr++) != NULLCHAR ) {
            if( c == ARG_TERMINATE )
                c = ' ';
            *ptr++ = c;
        }
        *ptr++ = NULLCHAR;
    }
    *ptr = ARG_TERMINATE;
    last = ptr;
    RemoteSplitCmd( TaskCmd, &end, &parm );
    for( ptr = TaskCmd; ptr < end; ++ptr ) {
        if( *ptr == NULLCHAR ) {
            *ptr = ' ';
        }
    }
    memmove( ptr + 1, parm, last - parm + 1 );
    *ptr = NULLCHAR;
    ptr = TaskCmd;
    // If the program name was quoted, strip off the quotes
    if( *ptr == '"' ) {
        memmove( ptr, ptr + 1, end - ptr );
        memmove( end - 2, end, last - end + 1 );
    }
    return( true );
}

void FindLocalDebugInfo( const char *name )
{
    char        *buff, *symfile;
    size_t      len;
    file_handle fh;

    len = strlen( name );
    _AllocA( buff, len + 1 + 4 + 2 );
    _AllocA( symfile, len + 1 + 4 );
    strcpy( buff, "@l" );
    // If a .sym file is present, use it in preference to the executable
    fh = FullPathOpen( name, ExtPointer( name, OP_LOCAL ) - name, "sym", symfile, len + 4 );
    if( fh != NIL_HANDLE ) {
        strcat( buff, symfile );
        FileClose( fh );
    } else {
        strcat( buff, name );
    }
    InsertRing( RingEnd( &LocalDebugInfo ), buff, strlen( buff ), false );
}

static void DoDownLoadCode( void )
/********************************/
{
    file_handle     fh;

    if( !DownLoadTask )
        return;
    fh = FullPathOpen( TaskCmd, strlen( TaskCmd ), "exe", TxtBuff, TXT_LEN );
    if( fh == NIL_HANDLE ) {
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), TaskCmd );
    }
    FileClose( fh );
    FindLocalDebugInfo( TxtBuff );
    CopyToRemote( TxtBuff, SkipPathInfo( TxtBuff, OP_LOCAL ), true, NULL );
}


bool DownLoadCode( void )
/***********************/
{
    return( Spawn( DoDownLoadCode ) == 0 );
}

void FiniCmd( void )
{
    _Free( TaskCmd );
}

void InitLocalInfo( void )
{
    LocalDebugInfo = NULL;
}

void FiniLocalInfo( void )
{
    FreeRing( LocalDebugInfo );
    LocalDebugInfo = NULL;
}

image_entry *ImagePrimary( void )
{
    return( DbgImageList );
}

image_entry *ImageEntry( mod_handle mh )
{
    image_entry         **image_ptr;

    image_ptr = ImageExtra( mh );
    return( (image_ptr == NULL) ? NULL : *image_ptr );
}

address DefAddrSpaceForMod( mod_handle mh )
{
    image_entry *image;
    address     def_addr;

    image = ImageEntry( mh );
    if( image == NULL )
        image = ImagePrimary();
    if( image != NULL ) {
        return( image->def_addr_space );
    }
    def_addr = GetRegSP();
    def_addr.mach.offset = 0;
    return( def_addr );
}

address DefAddrSpaceForAddr( address addr )
{
    mod_handle  mod;

    if( DeAliasAddrMod( addr, &mod ) == SR_NONE )
        mod = NO_MOD;
    return( DefAddrSpaceForMod( mod ) );
}

OVL_EXTERN void MapAddrSystem( image_entry *image, addr_ptr *addr,
                        addr_off *lo_bound, addr_off *hi_bound )
{
    RemoteMapAddr( addr, lo_bound, hi_bound, image->system_handle );
}

static bool InMapEntry( map_entry *curr, addr_ptr *addr )
{
    if( addr->segment != curr->map_addr.segment )
        return( false );
    if( addr->offset < curr->map_valid_lo )
        return( false );
    if( addr->offset > curr->map_valid_hi )
        return( false );
    return( true );
}

void MapAddrForImage( image_entry *image, addr_ptr *addr )
{
    map_entry           **owner;
    map_entry           *curr;
    addr_ptr            map_addr;
    addr_off            lo_bound;
    addr_off            hi_bound;

    for( owner = &image->map_list; (curr = *owner) != NULL; owner = &curr->link ) {
        if( curr->pre_map || InMapEntry( curr, addr ) ) {
            curr->map_addr = *addr;
            curr->pre_map = false;
            addr->segment = curr->real_addr.segment;
            addr->offset += curr->real_addr.offset;
            return;
        }
    }
    map_addr = *addr;
    image->mapper( image, addr, &lo_bound, &hi_bound );
    _Alloc( curr, sizeof( *curr ) );
    if( curr != NULL ) {
        curr->link = NULL;
        *owner = curr;
        curr->map_valid_lo = lo_bound;
        curr->map_valid_hi = hi_bound;
        curr->map_addr = map_addr;
        curr->map_addr.offset = 0;
        curr->real_addr = *addr;
        curr->real_addr.offset -= map_addr.offset;
        curr->pre_map = false;
    }
}


bool UnMapAddress( mappable_addr *loc, image_entry *image )
{
    map_entry           *map;
    mod_handle          himage;

    if( image == NULL ) {
        if( DeAliasAddrMod( loc->addr, &himage ) == SR_NONE )
            return( false );
        image = ImageEntry( himage );
    }
    if( image == NULL )
        return( false );
    DbgFree( loc->image_name );
    loc->image_name = DupStr( image->image_name );
    for( map = image->map_list; map != NULL; map = map->link ) {
        if( map->real_addr.segment == loc->addr.mach.segment ) {
            loc->addr.mach.segment = map->map_addr.segment;
            loc->addr.mach.offset = loc->addr.mach.offset - map->real_addr.offset;
            return( true );
        }
    }
    return( false );
}


static void UnMapOnePoint( brkp *bp, image_entry *image )
{
    mod_handle          himage;

    if( bp->status.b.unmapped )
        return;
    if( image != NULL ) {
        if( DeAliasAddrMod( bp->loc.addr, &himage ) == SR_NONE )
            return;
        if( image != ImageEntry( himage ) ) {
            return;
        }
    }
    if( bp->image_name == NULL || bp->mod_name == NULL ) {
        bp->status.b.unmapped = UnMapAddress( &bp->loc, image );
    } else {
        bp->status.b.unmapped = true;
    }
}


void UnMapPoints( image_entry *image )
{
    brkp                *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        UnMapOnePoint( bp, image );
    }
    if( UserTmpBrk.status.b.has_address ) {
        UnMapOnePoint( &UserTmpBrk, image );
    }
}


void FreeImage( image_entry *image )
{
    image_entry         **owner;
    image_entry         *curr;
    map_entry           *head;
    map_entry           *next;

    for( owner = &DbgImageList; (curr = *owner) != NULL; owner = &curr->link ) {
        if( curr == image ) {
            if( curr == ImageEntry( ContextMod ) ) {
                ContextMod = NO_MOD;
            }
            if( curr == ImageEntry( CodeAddrMod ) ) {
                CodeAddrMod = NO_MOD;
            }
            VarUnMapScopes( curr );
            UnMapPoints( curr );
            *owner = curr->link;
            for( head = curr->map_list; head != NULL; head = next ) {
                next = head->link;
                _Free( head );
            }
            _Free( curr->symfile_name );
            _Free( curr );
            break;
        }
    }
}


static image_entry *DoCreateImage( const char *exe, const char *symfile )
{
    image_entry         *image;
    image_entry         **owner;
    size_t              len;


    len = ( exe == NULL ) ? 0 : strlen( exe );
    _ChkAlloc( image, sizeof( *image ) + len, LIT_ENG( ERR_NO_MEMORY_FOR_DEBUG ) );
    if( image == NULL )
        return( NULL );
    memset( image, 0, sizeof( *image ) );
    if( len != 0 )
        memcpy( image->image_name, exe, len + 1 );
    if( symfile != NULL ) {
        _Alloc( image->symfile_name, strlen( symfile ) + 1 );
        if( image->symfile_name == NULL ) {
            _Free( image );
            Error( ERR_NONE, LIT_ENG( ERR_NO_MEMORY_FOR_DEBUG ) );
            return( NULL );
        }
        strcpy( image->symfile_name, symfile );
    }
    image->mapper = MapAddrSystem;
    for( owner = &DbgImageList; *owner != NULL; owner = &(*owner)->link )
        ;
    *owner = image;
    return( image );
}

char *GetLastImageName( void )
{
    image_entry         *image;

    for( image = DbgImageList; image->link != NULL; image = image->link )
        ;
    return( image->image_name );
}

static image_entry *CreateImage( const char *exe, const char *symfile )
{
    image_entry         *image;
    bool                local;
    const char          *curr_name;
    unsigned            curr_len;
    const char          *this_name;
    unsigned            this_len;
    char_ring           *curr;
    open_access         ind;

    if( exe != NULL && symfile == NULL ) {
        local = false;
        this_name = SkipPathInfo( exe, OP_REMOTE );
        this_len = ExtPointer( exe, OP_REMOTE ) - exe;
        for( curr = LocalDebugInfo; curr != NULL; curr = curr->next ) {
            curr_name = SkipPathInfo( curr->name, OP_LOCAL );
            curr_name = RealFName( curr_name, &ind );
            if( curr_name[0] == '@' && curr_name[1] == 'l' )
                curr_name += 2;
            curr_len = ExtPointer( curr_name, OP_LOCAL ) - curr_name;
            local = ( this_len == curr_len && strnicmp( this_name, curr_name, this_len ) == 0 );
            if( local ) {
                symfile = curr->name;
                break;
            }
        }
    }

    _SwitchOn( SW_ERROR_RETURNS );
    image = DoCreateImage( exe, symfile );
    _SwitchOff( SW_ERROR_RETURNS );
    return( image );
}

static bool CheckLoadDebugInfo( image_entry *image, file_handle fh,
                        unsigned start, unsigned end )
{
    char        buff[TXT_LEN];
    char        *symfile;
    unsigned    prio;
    char        *endstr;

    prio = start;
    for( ;; ) {
        prio = DIPPriority( prio );
        if( prio == 0 || prio > end )
            return( false );
        DIPStatus = DS_OK;
        image->dip_handle = DIPLoadInfo( (dig_fhandle)fh, sizeof( image_entry * ), prio );
        if( image->dip_handle != NO_MOD )
            break;
        if( DIPStatus & DS_ERR ) {
            symfile = image->symfile_name;
            if( symfile == NULL )
                symfile = image->image_name;
            endstr = Format( buff, LIT_ENG( Sym_Info_Load_Failed ), symfile );
            *endstr++ = ' ';
            StrCopy( DIPMsgText( DIPStatus ), endstr );
            Warn( buff );
            return( false );
        }
    }
    *(image_entry **)ImageExtra( image->dip_handle ) = image;
    return( true );
}


/*
 * ProcImgSymInfo -- initialize symbolic information
 *
 * Note: This function should try to open files locally first, for two
 * reasons:
 * 1) If a local file is open as remote, then local caching may interfere with
 *    file operations (notably seeks with DIO_SEEK_CUR)
 * 2) Remote access goes through extra layer of indirection; this overhead
 *    is completely unnecessary for local debugging.
 */
static bool ProcImgSymInfo( image_entry *image )
{
    file_handle fh;
    unsigned    last;
    char        buff[TXT_LEN];
    char        *symfile_name;
    const char  *nopath;
    size_t      len;

    image->deferred_symbols = false;
    if( _IsOff( SW_LOAD_SYMS ) )
        return( NO_MOD );
    if( image->symfile_name != NULL ) {
        last = DIP_PRIOR_MAX;
        fh = PathOpen( image->symfile_name, strlen( image->symfile_name ), "sym" );
        if( fh == NIL_HANDLE ) {
            nopath = SkipPathInfo( image->symfile_name, OP_REMOTE );
            fh = PathOpen( nopath, strlen( nopath ), "sym" );
            if( fh == NIL_HANDLE ) {
                /* try the sym file without an added extension */
                fh = FileOpen( image->symfile_name, OP_READ );
            }
        }
    } else {
        last = DIP_PRIOR_EXPORTS - 1;
        fh = FileOpen( image->image_name, OP_READ );
        if( fh == NIL_HANDLE ) {
            fh = FileOpen( image->image_name, OP_READ | OP_REMOTE );
        }
    }
    if( fh != NIL_HANDLE ) {
        if( CheckLoadDebugInfo( image, fh, DIP_PRIOR_MIN, last ) ) {
            return( true );
        }
        FileClose( fh );
    }
    if( image->symfile_name != NULL )
        return( false );
    _AllocA( symfile_name, strlen( image->image_name ) + 1 );
    strcpy( symfile_name, image->image_name );
    symfile_name[ExtPointer( symfile_name, OP_REMOTE ) - symfile_name] = NULLCHAR;
    len = MakeFileName( buff, symfile_name, "sym", OP_REMOTE );
    _Alloc( image->symfile_name, len + 1 );
    if( image->symfile_name != NULL ) {
        memcpy( image->symfile_name, buff, len + 1 );
        fh = FileOpen( image->symfile_name, OP_READ );
        if( fh == NIL_HANDLE ) {
            fh = FileOpen( image->symfile_name, OP_READ | OP_REMOTE );
        }
        if( fh == NIL_HANDLE ) {
            fh = PathOpen( image->symfile_name, strlen( image->symfile_name ), "" );
        }
        if( fh != NIL_HANDLE ) {
            if( CheckLoadDebugInfo( image, fh, DIP_PRIOR_MIN, DIP_PRIOR_MAX ) ) {
                return( true );
            }
            FileClose( fh );
        }
        _Free( image->symfile_name );
    }
    image->symfile_name = NULL;
    if( _IsOff( SW_NO_EXPORT_SYMS ) ) {
        if( _IsOn( SW_DEFER_SYM_LOAD ) ) {
            image->deferred_symbols = true;
        } else {
            fh = FileOpen( image->image_name, OP_READ | OP_REMOTE );
            if( fh != NIL_HANDLE ) {
                if( CheckLoadDebugInfo( image, fh, DIP_PRIOR_EXPORTS - 1, DIP_PRIOR_MAX ) ) {
                    return( true );
                }
                FileClose( fh );
            }
        }
    }
    return( false );
}


void UnLoadImgSymInfo( image_entry *image, bool nofree )
{
    if( image->dip_handle != NO_MOD ) {
        image->nofree = nofree;
        DIPUnloadInfo( image->dip_handle );
        if( nofree ) {
            image->dip_handle = NO_MOD;
            image->nofree = false;
        }
        DbgUpdate( UP_SYMBOLS_LOST );
        FClearOpenSourceCache();
    }
}

bool ReLoadImgSymInfo( image_entry *image )
{
    if( ProcImgSymInfo( image ) ) {
        DIPMapInfo( image->dip_handle, image );
        DbgUpdate( UP_SYMBOLS_ADDED );
        return( true );
    }
    return( false );
}


remap_return ReMapImageAddress( mappable_addr *loc, image_entry *image )
{
    map_entry           *map;

    if( loc->image_name == NULL ) {
        return( REMAP_WRONG_IMAGE );
    }
    if( strcmp( image->image_name, loc->image_name ) != 0 ) {
        return( REMAP_WRONG_IMAGE );
    }
    for( map = image->map_list; map != NULL; map = map->link ) {
        if( map->map_addr.segment == loc->addr.mach.segment ) {
            loc->addr.mach.segment = map->real_addr.segment;
            loc->addr.mach.offset = loc->addr.mach.offset + map->real_addr.offset;
            AddrSection( &loc->addr, OVL_MAP_CURR );
            DbgFree( loc->image_name );
            loc->image_name = NULL;
            return( REMAP_REMAPPED );
        }
    }
    return( REMAP_ERROR );
}

bool ReMapAddress( mappable_addr *loc )
{
    image_entry         *image;
    for( image = DbgImageList; image != NULL; image = image->link ) {
        if( ReMapImageAddress( loc, image ) == REMAP_REMAPPED ) {
            return( true );
        }
    }
    return( false );
}

static remap_return ReMapOnePoint( brkp *bp, image_entry *image )
{
    mod_handle  himage, mod;
    bool        ok;
    address     addr;
    DIPHDL( cue, ch );
    DIPHDL( cue, ch2 );
    remap_return        rc = REMAP_REMAPPED;

    if( !bp->status.b.unmapped )
        return( REMAP_WRONG_IMAGE );
    if( bp->image_name == NULL || bp->mod_name == NULL ) {
        if( image == NULL ) {
            if( ReMapAddress( &bp->loc ) ) {
                rc = REMAP_REMAPPED;
            } else {
                rc = REMAP_ERROR;
            }
        } else {
            rc = ReMapImageAddress( &bp->loc, image );
        }
    } else {
        himage = LookupImageName( bp->image_name, strlen( bp->image_name ) );
        if( himage == NO_MOD )
            return( REMAP_ERROR );
        mod =  LookupModName( himage, bp->mod_name, strlen( bp->mod_name ) );
        if( mod == NO_MOD )
            return( REMAP_ERROR );
        ok = GetBPSymAddr( bp, &addr );
        if( !ok )
            return( REMAP_ERROR );
        if( bp->cue_diff != 0 ) {
            if( DeAliasAddrCue( mod, addr, ch ) != SR_EXACT )
                return( REMAP_ERROR );
            if( LineCue( mod, CueFileId( ch ), CueLine( ch ) + bp->cue_diff, 0, ch2 ) != SR_EXACT )
                return( REMAP_ERROR );
            addr = CueAddr( ch2 );
        }
        if( bp->addr_diff != 0 ) {
            addr.mach.offset += bp->addr_diff;
        }
        bp->loc.addr = addr;
        rc = REMAP_REMAPPED;
    }
    if( rc == REMAP_REMAPPED ) {
        bp->status.b.unmapped = false;
    }
    SetPointAddr( bp, bp->loc.addr );
    if( bp->status.b.activate_on_remap ) {
        ActPoint( bp, true );
    }
    return( rc );
}


void ReMapPoints( image_entry *image )
{
    brkp        *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        switch( ReMapOnePoint( bp, image ) ) {
        case REMAP_ERROR:
            ActPoint( bp, false );
            bp->status.b.activate_on_remap = true;
            break;
        case REMAP_REMAPPED:
            bp->countdown = bp->initial_countdown;
            bp->total_hits = 0;
            break;
        }
    }
    if( UserTmpBrk.status.b.has_address ) {
        switch( ReMapOnePoint( &UserTmpBrk, image ) ) {
        case REMAP_ERROR:
// nobody cares about this warning!!        Warn( LIT_ENG( WARN_Unable_To_Remap_Tmp ) );
            UserTmpBrk.status.b.active = false;
            break;
        }
    }
}


static void InitImageInfo( image_entry *image )
{
    if( !FindNullSym( image->dip_handle, &image->def_addr_space ) ) {
        image->def_addr_space = GetRegSP();
        image->def_addr_space.mach.offset = 0;
    }
    SetWDPresent( image->dip_handle );
    VarReMapScopes( image );
    ReMapPoints( image );
}


bool LoadDeferredSymbols( void )
{
    image_entry *image;
    bool        rc = false;
    bool        defer;

    defer = _IsOn( SW_DEFER_SYM_LOAD );
    _SwitchOff( SW_DEFER_SYM_LOAD );
    for( image = DbgImageList; image != NULL; image = image->link ) {
        if( image->deferred_symbols ) {
            if( ReLoadImgSymInfo( image ) ) {
                InitImageInfo( image );
                image->deferred_symbols = false;
                rc = true;
            }
        }
    }
    if( defer )
        _SwitchOn( SW_DEFER_SYM_LOAD );
    return( rc );
}


bool AddLibInfo( bool already_stopping, bool *force_stop )
{
    unsigned long       module;
    bool                added;
    bool                deleted;
    image_entry         *image;

    added = false;
    deleted = false;
    module = 0;
    while( (module = RemoteGetLibName( module, TxtBuff, TXT_LEN )) != 0 ) {
        if( TxtBuff[0] == NULLCHAR ) {
            deleted = true;
            for( image = DbgImageList; image != NULL; image = image->link ) {
                if( image->system_handle == module ) {
                    DUIImageLoaded( image, false, already_stopping, force_stop );
                    UnLoadImgSymInfo( image, false );
                    break;
                }
            }
        } else {
            added = true;
            image = CreateImage( TxtBuff, NULL );
            if( image != NULL ) {
                image->system_handle = module;
                if( ReLoadImgSymInfo( image ) ) {
                    InitImageInfo( image );
                }
                DUIImageLoaded( image, true, already_stopping, force_stop );
            }
        }
    }
    CheckSegAlias();
    if( deleted ) {
        HookNotify( true, HOOK_DLL_END );
    }
    if( added ) {
        HookNotify( true, HOOK_DLL_START );
    }
    return( added );
}

static bool ProgStartHook = true;

bool SetProgStartHook( bool new )
{
    bool        old;

    old = ProgStartHook;
    ProgStartHook = new;
    return( old );
}

static void WndNewProg( void )
{
    DUIWndDebug();
    CodeAddrMod = NO_MOD;
    ContextMod = NO_MOD;
    SetCodeDot( GetRegIP() );
    DbgUpdate( UP_NEW_SRC | UP_CSIP_CHANGE |
               UP_SYM_CHANGE |
               UP_REG_CHANGE | UP_MEM_CHANGE |
               UP_THREAD_STATE | UP_NEW_PROGRAM );
    if( ProgStartHook ) {
        HookNotify( false, HOOK_PROG_START );
    }
    HookNotify( false, HOOK_NEW_MODULE );
}

static int DoLoadProg( const char *task, const char *symfile, error_handle *errh )
{
    open_access         loc;
    const char          *name;
    size_t              len;
    static char         fullname[2048];
    image_entry         *image;
    file_handle         fh;
    unsigned long       system_handle;

    *errh = 0;
#ifdef __NT__
    task = CheckForPowerBuilder( task );
#endif
    if( task[0] == NULLCHAR )
        return( TASK_NONE );
    name = FileLoc( task, &loc );
    if( DownLoadTask ) {
        strcpy( fullname, name );
        fh = FullPathOpen( TaskCmd, strlen( TaskCmd ), "exe", TxtBuff, TXT_LEN );
        if( fh != NIL_HANDLE ) {
            strcpy( fullname, TxtBuff );
            FileClose( fh );
        }
    } else {
        len = RemoteStringToFullName( true, name, fullname, sizeof( fullname ) );
        fullname[len] = NULLCHAR;
    }
    image = CreateImage( fullname, symfile );
    if( image == NULL )
        return( TASK_NOT_LOADED );
    if( DownLoadTask ) {
        name = SkipPathInfo( name, OP_LOCAL );
    }
    *errh = DoLoad( name, &system_handle );
    if( *errh != 0 ) {
        FreeImage( image );
        return( TASK_NOT_LOADED );
    }
    CheckSegAlias();
    image->system_handle = system_handle;
    SetLastExe( fullname );
    ProcImgSymInfo( image );
    if( image->dip_handle != NO_MOD ) {
        DIPMapInfo( image->dip_handle, image );
    }
    InitImageInfo( image );
    return( TASK_NEW );
}

void LoadProg( void )
{
    error_handle        errh = 0;
    int                 ret;
    unsigned long       system_handle;
    static char         NullProg[] = { NULLCHAR, NULLCHAR, ARG_TERMINATE };
    bool                dummy;

    ClearMachState();
    CurrProcess = DIPCreateProcess(); //NYI: multiple processes
    if( !DownLoadCode() ) {
        ret =  TASK_NOT_LOADED;
    } else {
        ret = DoLoadProg( TaskCmd, SymFileName, &errh );
    }
    if( ret != TASK_NEW ) {
        CreateImage( NULL, NULL );
        DoLoad( NullProg, &system_handle );
    }
    /* need to do all these because we might be the QNX low level debugger */
    AddLibInfo( true, &dummy );
    SetupMachState();
    WndNewProg();
    RecordStart();
    ReportTask( ret, errh );
}

/*
 * ReleaseProgOvlay -- release segment that was allocated for the user program
 */

void ReleaseProgOvlay( bool free_sym )
{
    if( _IsOn( SW_PROC_ALREADY_STARTED ) ) {
        /* detaching from a running proc just lets it go */
        DUIStop();
    }
    if( CurrProcess != NULL ) {
        DIPDestroyProcess( CurrProcess );
        CurrProcess = NULL;
    }
    if( !KillProgOvlay() ) {
        Error( ERR_NONE, LIT_ENG( ERR_CANT_KILL_PROGRAM ) );
    }
    if( free_sym ) {
        _Free( SymFileName );
        SymFileName = NULL;
    }
    FreeAliasInfo();
    WndNewProg();
    while( DbgImageList != NULL ) {
        FreeImage( DbgImageList );
    }
}



OVL_EXTERN void BadNew( void )
{
    Error( ERR_LOC, LIT_ENG( ERR_BAD_OPTION ), "new" );
}


void InitMappableAddr( mappable_addr *loc )
{
    loc->image_name = NULL;
}

void FiniMappableAddr( mappable_addr *loc )
{
    if( loc->image_name != NULL ) {
        DbgFree( loc->image_name );
    }
}


size_t GetProgName( char *where, size_t len )
{
    size_t      l;

    /*
        Before, we did a:

            RemoteStringToFullName( true, TaskCmd, where, len );

        but that screws up when the user specified something other than
        just an executable on the command line. E.g. a PID to connect
        to a running process, or a NID specifier for QNX.
    */
    l = strlen( TaskCmd );
    if( l >= len )
        l = len - 1;
    memcpy( where, TaskCmd, l );
    where[l] = NULLCHAR;
    return( l );
}

static bool ArgNeedsQuotes( const char *src )
{
    char        ch;

    if( *src == NULLCHAR )
        return( true );
    for( ; (ch = *src) != NULLCHAR; ++src ) {
        if( ch == ' ' )
            return( true );
        if( ch == '\t' ) {
            return( true );
        }
    }
    return( false );
}

static void AddString( char **dstp, size_t *lenp, const char *src )
{
    size_t      len;

    len = strlen( src );
    if( len > *lenp )
        len = *lenp;
    memcpy( *dstp, src, len );
    *dstp += len;
    *lenp -= len;
}

static size_t PrepProgArgs( char *where, size_t len )
{
    char        *src;
    char        *dst;

    --len;      /* leave room for NULLCHAR */
    dst = where;
    for( src = TaskCmd + strlen( TaskCmd ) + 1; *src != ARG_TERMINATE; src += strlen( src ) + 1 ) {
        if( dst != where )
            AddString( &dst, &len, " " );
        if( _IsOn( SW_TRUE_ARGV ) && ArgNeedsQuotes( src ) ) {
            AddString( &dst, &len, "\"" );
            AddString( &dst, &len, src );
            AddString( &dst, &len, "\"" );
        } else {
            AddString( &dst, &len, src );
        }
    }
    *dst = NULLCHAR;
    return( dst - where );
}

size_t GetProgArgs( char *where, size_t len )
{
    len = PrepProgArgs( where, len );
    _SwitchOff( SW_TRUE_ARGV );
    return( len );
}

void SetSymFileName( const char *file )
{
    if( SymFileName != NULL )
        _Free( SymFileName );
    _Alloc( SymFileName, strlen( file ) + 1 );
    strcpy( SymFileName, file );
}


static void DoResNew( bool have_parms, const char *cmd,
                     size_t clen, const char *parms, size_t plen )
{
    char                *new;

    TraceKill();
    new = DbgMustAlloc( clen + plen + 2 );
    ReleaseProgOvlay( false );
    BPsUnHit();
    memcpy( new, cmd, clen );
    new[clen] = NULLCHAR;
    memcpy( new + clen + 1, parms, plen );
    new[clen + plen + 1] = ARG_TERMINATE;
    _Free( TaskCmd );
    TaskCmd = new;
    if( have_parms )
        _SwitchOff( SW_TRUE_ARGV );
    LoadProg();
}


extern void LoadNewProg( const char *cmd, const char *parms )
{
    size_t      clen, plen;
    char        prog[FILENAME_MAX];

    clen = strlen( cmd );
    plen = strlen( parms );
    GetProgName( prog, sizeof( prog ) );
    if( stricmp( cmd, prog ) == 0 ) {
        DoResNew( plen != 0, cmd, clen, parms, plen + 1 );
    } else {
        BPsDeac();
        DoResNew( plen != 0, cmd, clen, parms, plen + 1 );
        VarFreeScopes();
    }
}


static unsigned long SizeMinusDebugInfo( file_handle fh, bool strip )
/*******************************************************************/
{
    TISTrailer          trailer;
    unsigned long       copylen;

    copylen = SeekStream( fh, 0, DIO_SEEK_END );
    if( !strip )
        return( copylen );
    SeekStream( fh, -sizeof( trailer ), DIO_SEEK_END );
    if( ReadStream( fh, &trailer, sizeof( trailer ) ) != sizeof( trailer ) )
        return( copylen );
    if( trailer.signature != TIS_TRAILER_SIGNATURE )
        return( copylen );
    return( copylen - trailer.size );
}


static bool CopyToRemote( const char *local, const char *remote, bool strip, void *cookie )
/*****************************************************************************************/
{
    file_handle         fh_lcl;
    file_handle         fh_rem;
    size_t              read_len;
    char                *buff;
    unsigned            bsize;
    unsigned long       copylen;
    unsigned long       copied;
    long                remdate;
    long                lcldate;
    bool                delete_file;

#ifdef __NT__
    lcldate = LocalGetFileDate( local );
#else
    lcldate = -1;
#endif
    remdate = RemoteGetFileDate( remote );
    if( remdate != -1 && lcldate != -1 && remdate == lcldate )
        return( true );
    strip = strip; // nyi - strip debug info here
    fh_lcl = FileOpen( local, OP_READ );
    if( fh_lcl == NIL_HANDLE ) {
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), local );
        return( false );
    }
    fh_rem = FileOpen( remote, OP_REMOTE | OP_WRITE | OP_CREATE | OP_TRUNC | OP_EXEC );
    if( fh_rem == NIL_HANDLE ) {
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), remote );
        FileClose( fh_lcl );
        return( false );
    }
    bsize = 0x8000;
    _Alloc( buff, bsize );
    if( buff == NULL ) {
        bsize = 128;
        buff = DbgMustAlloc( bsize );
    }
    copylen = SizeMinusDebugInfo( fh_lcl, strip );
    DUICopySize( cookie, copylen );
    SeekStream( fh_lcl, 0, DIO_SEEK_ORG );
    delete_file = false;
    copied = 0;
    while( (read_len = ReadStream( fh_lcl, buff, bsize )) != 0 ) {
        if( read_len == ERR_RETURN )
            break;
        WriteStream( fh_rem, buff, read_len );
        DUICopyCopied( cookie, copied );
        copied += read_len;
        if( copied >= copylen )
            break;
        if( DUICopyCancelled( cookie ) ) {
            delete_file = true;
            break;
        }
    }
    FileClose( fh_lcl );
    FileClose( fh_rem );
    _Free( buff );
    if( delete_file ) {
        RemoteErase( remote );
        return( false );
    } else {
        RemoteSetFileDate( remote, lcldate );
        return( true );
    }
}


static unsigned ArgLen( const char *p )
{
    const char  *start;

    start = p;
    while( *p != ARG_TERMINATE )
        ++p;
    return( p - start );
}


static void DoReStart( bool have_parms, size_t clen, const char *start, size_t len )
{
    DoResNew( have_parms, TaskCmd, clen, start, len );
}


static void ResNew( void )
{
    const char          *start;
    size_t              len;
    size_t              clen;
    bool                have_parms;

    clen = strlen( TaskCmd );
    if( ScanItem( false, &start, &len ) ) {
        memcpy( TxtBuff, start, len );
        TxtBuff[len++] = NULLCHAR;
        have_parms = true;
    } else {
        start = TaskCmd + clen + 1;
        len = ArgLen( start );
        have_parms = false;
    }
    ReqEOC();
    if( _IsOff( SW_PROC_ALREADY_STARTED ) && _IsOff( SW_POWERBUILDER ) ) {
        DoReStart( have_parms, clen, start, len );
    } else {
        Error( ERR_NONE, LIT_ENG( ERR_CANT_RESTART ) );
    }
}

void ReStart( void )
{
    char                prog[FILENAME_MAX];
    char                args[UTIL_LEN];

    if( _IsOff( SW_PROC_ALREADY_STARTED ) && _IsOff( SW_POWERBUILDER ) ) {
        GetProgName( prog, sizeof( prog ) );
        GetProgArgs( args, sizeof( args ) );
        LoadNewProg( prog, args );
    } else {
        Error( ERR_NONE, LIT_ENG( ERR_CANT_RESTART ) );
    }
}

/*
 *
 */

#define SKIP_SPACES     while( *start == ' ' && len != 0 ) { ++start; --len; }

static const char NogoTab[] = {
    "NOgo\0"
};



static void ProgNew( void )
{
    const char  *start;
    char        *cmd;
    char        *parm;
    char        *end;
    char        *new;
    const char  *symfile;
    size_t      len;
    size_t      clen;
    size_t      plen;
    bool        have_parms;
    bool        old;
    bool        progstarthook;

    progstarthook = true;
    if( CurrToken == T_DIV ) {
        Scan();
        if( ScanCmd( NogoTab ) != 0 ) {
            Error( ERR_LOC, LIT_ENG( ERR_BAD_OPTION ), "new" );
        }
        progstarthook = false;
    }
    old = SetProgStartHook( progstarthook );
    if( ScanItem( false, &start, &len ) ) {
        _Free( SymFileName );
        SymFileName = NULL;
        SKIP_SPACES;
        if( len > 1 && *start == SYM_FILE_IND ) {
            ++start;
            --len;
            SKIP_SPACES;
            if( len != 0 ) {
                symfile = start;
                while( len != 0 && *start != ' ' ) {
                    ++start;
                    --len;
                }
                new = DbgMustAlloc( start - symfile + 1 );
                SymFileName = new;
                memcpy( new, symfile, start - symfile );
                new[start - symfile] = NULLCHAR;
                SKIP_SPACES;
            }
        }
        cmd = TxtBuff;
        memcpy( cmd, start, len );
        cmd[len++] = NULLCHAR;
        cmd[len] = ARG_TERMINATE;
        RemoteSplitCmd( cmd, &end, &parm );
        clen = end - cmd;
        plen = len - (parm - cmd);
        have_parms = true;
    } else {
        cmd = TaskCmd;
        clen = strlen( TaskCmd );
        parm = TaskCmd + clen + 1;
        plen = ArgLen( parm );
        have_parms = false;
    }
    BPsDeac();
    ReqEOC();
    DoResNew( have_parms, cmd, clen, parm, plen );
    VarFreeScopes();
    SetProgStartHook( old );
}

#define NO_SEG          0

static void PostProcMapExpr( address *addr )
{
    addr_off            off;

    off = addr->mach.offset;
    if( _IsOff( SW_HAVE_SEGMENTS ) ) {
        *addr = GetRegSP();
        addr->mach.offset = off;
    } else if( addr->mach.segment == NO_SEG ) {
        addr->mach.segment = (addr_seg) off;
        addr->mach.offset = 0;
    }
}

static void EvalMapExpr( address *addr )
{
    addr->mach.segment = NO_SEG;
    ReqMemAddr( EXPR_GIVEN, addr );
    PostProcMapExpr( addr );
}

/*
 * MapAddrUser - have the user supply address mapping information
 */

OVL_EXTERN void MapAddrUser( image_entry *image, addr_ptr *addr,
                        addr_off *lo_bound, addr_off *hi_bound )
{
    address     mapped;
    addr_off    offset   = addr->offset;

    //NYI: what about bounds under Netware?
    *lo_bound = 0;
    *hi_bound = ~(addr_off)0;
    if( image->map_list != NULL && !image->map_list->pre_map
      && MADAddrMap( addr, &image->map_list->map_addr,
                &image->map_list->real_addr, &DbgRegs->mr ) == MS_OK ) {
        return;
    }
    for( ;; ) {
        switch( addr->segment ) {
        case MAP_FLAT_CODE_SELECTOR:
            Format( TxtBuff, LIT_ENG( Map_Named_Selector ), "Flat Code", image->symfile_name );
            break;
        case MAP_FLAT_DATA_SELECTOR:
            Format( TxtBuff, LIT_ENG( Map_Named_Selector ), "Flat Data", image->symfile_name );
            break;
        default:
            Format( TxtBuff, LIT_ENG( Map_Selector ), addr->segment, image->symfile_name );
        }
        mapped.mach.segment = NO_SEG;
        mapped.mach.offset = 0;
        if( DUIDlgGivenAddr( TxtBuff, &mapped ) ) {
            PostProcMapExpr( &mapped );
            mapped.mach.offset += offset;   // add offset back!
            *addr = mapped.mach;
            break;
        }
    }
}


/*
 * SymFileNew - process a new symbolic file request
 */

OVL_EXTERN void SymFileNew( void )
{
    const char  *fname;
    size_t      fname_len;
    image_entry *image;
    address     addr;
    map_entry   **owner;
    map_entry   *curr;
    const char  *temp;
    addr_off    dummy;

    if( ! ScanItem( true, &fname, &fname_len ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_WANT_FILENAME ) );
    }
    temp = ScanPos();
    while( !ScanEOC() ) {
        ChkExpr();
        if( CurrToken == T_COMMA ) {
            Scan();
        }
    }
    ReScan( temp );
    memcpy( TxtBuff, fname, fname_len );
    TxtBuff[fname_len] = NULLCHAR;
    image = DoCreateImage( NULL, TxtBuff );
    image->mapper = MapAddrUser;
    if( !ProcImgSymInfo( image ) ) {
        FreeImage( image );
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), TxtBuff );
    }
    owner = &image->map_list;
    while( !ScanEOC() ) {
        EvalMapExpr( &addr );
        _Alloc( curr, sizeof( *curr ) );
        if( curr == NULL ) {
            DIPUnloadInfo( image->dip_handle );
            Error( ERR_NONE, LIT_ENG( ERR_NO_MEMORY_FOR_DEBUG ) );
        }
        *owner = curr;
        owner = &curr->link;
        curr->link = NULL;
        curr->pre_map = true;
        curr->real_addr = addr.mach;
        curr->map_valid_lo = 0;
        curr->map_valid_hi = ~(addr_off)0;
        curr->map_addr.offset  = 0;
        curr->map_addr.segment = 0;
        if( CurrToken == T_COMMA ) {
            Scan();
        }
    }
    DIPMapInfo( image->dip_handle, image );
    curr = image->map_list->link;
    if( _IsOn( SW_HAVE_SEGMENTS )
      && (MADAddrFlat( &DbgRegs->mr ) == MS_OK)
      && (curr == NULL || curr->pre_map) ) {
        /* FLAT model program */
        if( curr == NULL ) {
            MapAddrUser( image, &addr.mach, &dummy, &dummy );
        } else {
            addr.mach = curr->real_addr;
        }
        AddAliasInfo( image->map_list->real_addr.segment, addr.mach.segment );
    }
    DbgUpdate( UP_SYMBOLS_ADDED );
    InitImageInfo( image );
}


/*
 * MapAddrUsrMod - simple address mapping for user loaded modules
 */

OVL_EXTERN void MapAddrUsrMod( image_entry *image, addr_ptr *addr,
                        addr_off *lo_bound, addr_off *hi_bound )
{
    address     mapped;
    addr_off    offset   = addr->offset;

    *lo_bound = 0;
    *hi_bound = ~(addr_off)0;
    if( image->map_list != NULL && !image->map_list->pre_map
      && MADAddrMap( addr, &image->map_list->map_addr,
                &image->map_list->real_addr, &DbgRegs->mr ) == MS_OK ) {
        return;
    }
    mapped.mach.segment = NO_SEG;
    mapped.mach.offset  = 0;

    // Assumes flat model images with single base address
    if( image->map_list != NULL ) {
        mapped.mach = image->map_list->real_addr;

        PostProcMapExpr( &mapped );
        mapped.mach.offset += offset;   // add offset back!
        *addr = mapped.mach;
    }
}


/*
 * SymUserModLoad - process symbol information for user loaded module
 *                  NB: assumes flat model
 */

bool SymUserModLoad( const char *fname, address *loadaddr )
{
    size_t      fname_len;
    image_entry *image;
    map_entry   **owner;
    map_entry   *curr;

    if( !fname )
        return( true );

    if( ( fname_len = strlen( fname ) ) == 0 )
        return( true );

    image = DoCreateImage( fname, fname );
    image->mapper = MapAddrUsrMod;
    if( !ProcImgSymInfo( image ) ) {
        FreeImage( image );
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), fname );
    }
    owner = &image->map_list;

    _Alloc( curr, sizeof( *curr ) );
    if( curr == NULL ) {
        DIPUnloadInfo( image->dip_handle );
        Error( ERR_NONE, LIT_ENG( ERR_NO_MEMORY_FOR_DEBUG ) );
    }

    // Save off the load address in a map entry
    *owner = curr;
    owner = &curr->link;
    curr->link    = NULL;
    curr->pre_map = true;
    curr->map_valid_lo = 0;
    curr->map_valid_hi = ~(addr_off)0;
    curr->real_addr    = loadaddr->mach;
    curr->map_addr.offset  = 0;
    curr->map_addr.segment = 0;

    DIPMapInfo( image->dip_handle, image );
    curr = image->map_list->link;
    DbgUpdate( UP_SYMBOLS_ADDED );
    InitImageInfo( image );
    return( false );
}


/*
 * SymUserModUnload - unload symbol information for user loaded module
 */

bool SymUserModUnload( char *fname )
{
    image_entry *image;

    if( fname != NULL ) {
        for( image = ImagePrimary(); image != NULL; image = image->link ) {
            if( image->symfile_name != NULL && ( strcmp( image->symfile_name, fname ) == 0 ) ) {
                UnLoadImgSymInfo( image, false );
                return( false );
            }
        }
    }
    return( true );
}

static const char NewNameTab[] = {
    "Program\0"
    "Restart\0"
    "STDIn\0"
    "STDOut\0"
    "SYmbol\0"
};


static void (* const NewJmpTab[])( void ) = {
    &ProgNew,
    &ResNew,
    &StdInNew,
    &StdOutNew,
    &SymFileNew
};


/*
 *
 */

void ProcNew( void )
{
    int     cmd;

    if( CurrToken == T_DIV ) {
        Scan();
        cmd = ScanCmd( NewNameTab );
        if( cmd < 0 ) {
            BadNew();
        } else {
            (*NewJmpTab[cmd])();
        }
    } else {
        ResNew();
    }
    HookPendingPush();
}

void RecordNewProg( void )
{
    char        buff[40];
    char        *p;

    GetCmdEntry( NewNameTab, 0, buff );
    p = Format( TxtBuff, "%s/%s", GetCmdName( CMD_NEW ), buff );
    if( !ProgStartHook ) {
        GetCmdEntry( NogoTab, 0, buff );
        p = Format( p, "/%s", buff );
    }
    *p++ = ' ';
    p += GetProgName( p, TXT_LEN - ( p - TxtBuff ) );
    *p++ = ' ';
    PrepProgArgs( p, TXT_LEN - ( p - TxtBuff ) );
    RecordEvent( TxtBuff );
}
