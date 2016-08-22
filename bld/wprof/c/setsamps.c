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


#include <string.h>
#include <stdio.h>
#include "walloca.h"
#include "wio.h"
#include "common.h"
#include "aui.h"
#include "dip.h"
#include "mad.h"
#include "msg.h"
#include "myassert.h"
#include "sampinfo.h"
#include "memutil.h"
#include "madinter.h"
#include "setsamps.h"
#include "support.h"
#include "utils.h"

#include "clibext.h"


extern void         ClearMassaged(sio_data *curr_sio);
extern void         ClearModuleInfo(image_info *curr_image);
extern void         ClearFileInfo(mod_info *curr_mod);
extern void         ClearRoutineInfo(file_info *curr_file);
extern process_info *WPDipProc(void);
extern void         WPDipDestroyProc(process_info *dip_proc);
extern void         WPDipSetProc(process_info *dip_proc);
extern mod_handle   WPDipLoadInfo(dig_fhandle dfh,char *f_name,void *image,int image_size,unsigned int dip_start,unsigned int dip_end);

extern sio_data     *SIOData;
extern sio_data     *CurrSIOData;

static char         FNameBuff[_MAX_PATH2];

STATIC file_info    *loadFileInfo( mod_info *, sym_handle * );



STATIC mod_info *findCurrMod( image_info *curr_image, mod_handle mh )
/*******************************************************************/
{
    mod_info            *curr_mod;
    int                 mod_count;

    mod_count = 0;
    while( mod_count < curr_image->mod_count ) {
        curr_mod = curr_image->module[mod_count];
        if( curr_mod->mh != 0 && curr_mod->mh == mh ) {
            return( curr_mod );
        }
        mod_count++;
    }
    return( NULL );
}



STATIC rtn_info *findCurrRtn( mod_info *curr_mod, sym_handle *sh )
/****************************************************************/
{
    file_info           *curr_file;
    rtn_info            *curr_rtn;
    int                 file_count;
    int                 rtn_count;

    file_count = 0;
    while( file_count < curr_mod->file_count ) {
        curr_file = curr_mod->mod_file[file_count];
        rtn_count = 0;
        while( rtn_count < curr_file->rtn_count ) {
            curr_rtn = curr_file->routine[rtn_count];
            if( curr_rtn->sh != NULL && DIPSymCmp( curr_rtn->sh, sh ) == 0 ) {
                return( curr_rtn );
            }
            rtn_count++;
        }
        file_count++;
    }
    return( NULL );
}



STATIC void initRoutineInfo( file_info *curr_file )
/*************************************************/
{
    rtn_info        *new_rtn;
    int             name_len;

    ClearRoutineInfo( curr_file );
    name_len = strlen( LIT( Unknown_Routine ) ) + 1;
    new_rtn = ProfCAlloc( sizeof( rtn_info ) + name_len );
    memcpy( new_rtn->name, LIT( Unknown_Routine ), name_len );
    new_rtn->unknown_routine = true;
    curr_file->routine = ProfCAlloc( 2 * sizeof( pointer ) );
    curr_file->routine[0] = new_rtn;
    name_len = strlen( LIT( Gathered_Routines ) ) + 1;
    new_rtn = ProfCAlloc( sizeof( rtn_info ) + name_len );
    memcpy( new_rtn->name, LIT( Gathered_Routines ), name_len );
    new_rtn->ignore_gather = true;
    new_rtn->gather_routine = true;
    curr_file->routine[1] = new_rtn;
    curr_file->rtn_count = 2;
}



STATIC void initFileInfo( mod_info *curr_mod )
/********************************************/
{
    file_info       *new_file;
    int             file_len;

    ClearFileInfo( curr_mod );
    file_len = strlen( LIT( Unknown_File ) ) + 1;
    new_file = ProfCAlloc( sizeof( file_info ) + file_len );
    memcpy( new_file->name, LIT( Unknown_File ), file_len );
    new_file->unknown_file = true;
    initRoutineInfo( new_file );
    curr_mod->mod_file = ProfCAlloc( 2 * sizeof( pointer ) );
    curr_mod->mod_file[0] = new_file;
    file_len = strlen( LIT( Gathered_Files ) ) + 1;
    new_file = ProfCAlloc( sizeof( file_info )+file_len );
    memcpy( new_file->name, LIT( Gathered_Files ), file_len );
    new_file->ignore_gather = true;
    new_file->gather_file = true;
    initRoutineInfo( new_file );
    curr_mod->mod_file[1] = new_file;
    curr_mod->file_count = 2;
}



STATIC void initModuleInfo( image_info *curr_image )
/**************************************************/
{
    mod_info        *new_mod;
    int             name_len;

    ClearModuleInfo( curr_image );
    name_len = strlen( LIT( Unknown_Module ) ) + 1;
    new_mod = ProfCAlloc( sizeof( mod_info ) + name_len );
    memcpy( new_mod->name, LIT( Unknown_Module ), name_len );
    new_mod->unknown_module = true;
    initFileInfo( new_mod );
    curr_image->module = ProfCAlloc( 2 * sizeof( pointer ) );
    curr_image->module[0] = new_mod;
    name_len = strlen( LIT( Gathered_Modules ) ) + 1;
    new_mod = ProfCAlloc( sizeof( mod_info ) + name_len );
    memcpy( new_mod->name, LIT( Gathered_Modules ), name_len );
    new_mod->ignore_gather = true;
    new_mod->gather_module = true;
    initFileInfo( new_mod );
    curr_image->module[1] = new_mod;
    curr_image->mod_count = 2;
}



STATIC walk_result loadRoutineInfo( sym_walk_info swi, sym_handle *sym,
                                                      void *_new_mod )
/*********************************************************************/
{
    mod_info        *new_mod = _new_mod;
    sym_info        sinfo;
    file_info       *sym_file;
    rtn_info        *new_rtn;
    int             rtn_count;
    int             name_len;
    int             sym_size;
    int             demangle_type;

    if( swi != SWI_SYMBOL ) {
        return( WR_CONTINUE );
    }
    DIPSymInfo( sym, NULL, &sinfo );
    if( sinfo.kind != SK_CODE && sinfo.kind != SK_PROCEDURE ) {
        return( WR_CONTINUE );
    }
    sym_file = loadFileInfo( new_mod, sym );
    name_len = DIPSymName( sym, NULL, SN_DEMANGLED, NULL, 0 );
    if( name_len == 0 ) {
        name_len = DIPSymName( sym, NULL, SN_SOURCE, NULL, 0 );
        demangle_type = SN_SOURCE;
    } else {
        demangle_type = SN_DEMANGLED;
    }
    new_rtn = ProfCAlloc( sizeof( rtn_info ) + name_len );
    DIPSymName( sym, NULL, demangle_type, new_rtn->name, name_len + 1 );
    sym_size = DIPHandleSize( HK_SYM, false );
    new_rtn->sh = ProfAlloc( sym_size );
    memcpy( new_rtn->sh, sym, sym_size );
    rtn_count = sym_file->rtn_count;
    sym_file->rtn_count++;
    sym_file->routine = ProfRealloc( sym_file->routine, sym_file->rtn_count * sizeof( pointer ) );
    sym_file->routine[rtn_count] = new_rtn;
    return( WR_CONTINUE );
}



STATIC walk_result loadModuleInfo( mod_handle mh, void *_curr_image )
/*******************************************************************/
{
    image_info      *curr_image = _curr_image;
    mod_info        *new_mod;
    int             mod_count;
    int             name_len;

    name_len = DIPModName( mh, NULL, 0 );
    new_mod = ProfCAlloc( sizeof( mod_info ) + name_len );
    DIPModName( mh, new_mod->name, name_len + 1 );
    new_mod->mh = mh;
    mod_count = curr_image->mod_count;
    curr_image->mod_count++;
    curr_image->module = ProfRealloc( curr_image->module,
                                      curr_image->mod_count * sizeof( pointer ) );
    curr_image->module[mod_count] = new_mod;
    initFileInfo( new_mod );
    DIPWalkSymList( SS_MODULE, &mh, &loadRoutineInfo, new_mod );
    return( WR_CONTINUE );
}



STATIC file_info  *loadFileInfo( mod_info *curr_mod, sym_handle *sym )
/********************************************************************/
{
    file_info       *sym_file;
    cue_handle      *ch;
    cue_fileid      fid;
    int             file_count;
    int             count;
    location_list   ll;

    if( DIPSymLocation( sym, NULL, &ll ) != DS_OK ) {
        return( curr_mod->mod_file[0] );
    }
    ch = alloca( DIPHandleSize( HK_CUE, false ) );
    switch( DIPAddrCue( curr_mod->mh, ll.e[0].u.addr, ch ) ) {
    case SR_NONE:
    case SR_FAIL:
        return( curr_mod->mod_file[0] );
    }
    fid = DIPCueFileId( ch );
    file_count = curr_mod->file_count;
    count = 0;
    while( count < file_count ) {
        sym_file = curr_mod->mod_file[count];
        if( sym_file->fid == fid ) {
            return( sym_file );
        }
        count++;
    }
    curr_mod->file_count++;
    curr_mod->mod_file = ProfRealloc( curr_mod->mod_file,
                             curr_mod->file_count * sizeof( pointer ) );
    count = DIPCueFile( ch, NULL, 0 ) + 1;
    sym_file = ProfCAlloc( sizeof( file_info ) + count );
    sym_file->fid = fid;
    DIPCueFile( ch, sym_file->name, count );
    initRoutineInfo( sym_file );
    curr_mod->mod_file[file_count] = sym_file;
    return( sym_file );
}



int AddrCmp( address *addr1, address *addr2 )
/*******************************************/
{
    if( addr1->sect_id > addr2->sect_id ) return( +1 );
    if( addr1->sect_id < addr2->sect_id ) return( -1 );
    return( MADAddrComp( addr1, addr2, MAF_FULL ) );
}



STATIC int rawSampCmp( const void *_d1, const void *_d2 )
/*******************************************************/
{
    const pointer   *d1 = _d1;
    const pointer   *d2 = _d2;
    address         *data1;
    address         *data2;

    data1 = *d1;
    data2 = *d2;
    return( AddrCmp( data1, data2 ) );
}



void GatherSetAll( sio_data * curr_sio, bool gather_active )
/**********************************************************/
{
    image_info          *curr_image;
    mod_info            *curr_mod;
    long int            count;
    int                 count2;
    int                 count3;

    count = 0;
    while( count < curr_sio->image_count ) {
        curr_image = curr_sio->images[count];
        count2 = 0;
        while( count2 < curr_image->mod_count ) {
            curr_mod = curr_image->module[count2];
            count3 = 0;
            while( count3 < curr_mod->file_count ) {
                curr_mod->mod_file[count3]->gather_active = gather_active;
                count3++;
            }
            curr_mod->gather_active = gather_active;
            count2++;
        }
        curr_image->gather_active = gather_active;
        count++;
    }
    curr_sio->gather_active = gather_active;
}



void AbsSetAll( sio_data *curr_sio, bool abs_bar )
/************************************************/
{
    image_info          *curr_image;
    mod_info            *curr_mod;
    file_info           *curr_file;
    long int            count;
    int                 count2;
    int                 count3;
    int                 count4;

    count = 0;
    while( count < curr_sio->image_count ) {
        curr_image = curr_sio->images[count];
        count2 = 0;
        while( count2 < curr_image->mod_count ) {
            curr_mod = curr_image->module[count2];
            count3 = 0;
            while( count3 < curr_mod->file_count ) {
                curr_file = curr_mod->mod_file[count3];
                count4 = 0;
                while( count4 < curr_file->rtn_count ) {
                    curr_file->routine[count4]->abs_bar = abs_bar;
                    count4++;
                }
                curr_file->abs_bar = abs_bar;
                count3++;
            }
            curr_mod->abs_bar = abs_bar;
            count2++;
        }
        curr_image->abs_bar = abs_bar;
        count++;
    }
    curr_sio->abs_bar = abs_bar;
    curr_sio->asm_src_info.abs_bar = abs_bar;
}



void RelSetAll( sio_data *curr_sio, bool rel_bar )
/************************************************/
{
    image_info          *curr_image;
    mod_info            *curr_mod;
    file_info           *curr_file;
    long int            count;
    int                 count2;
    int                 count3;
    int                 count4;

    count = 0;
    while( count < curr_sio->image_count ) {
        curr_image = curr_sio->images[count];
        count2 = 0;
        while( count2 < curr_image->mod_count ) {
            curr_mod = curr_image->module[count2];
            count3 = 0;
            while( count3 < curr_mod->file_count ) {
                curr_file = curr_mod->mod_file[count3];
                count4 = 0;
                while( count4 < curr_file->rtn_count ) {
                    curr_file->routine[count4]->rel_bar = rel_bar;
                    count4++;
                }
                curr_file->rel_bar = rel_bar;
                count3++;
            }
            curr_mod->rel_bar = rel_bar;
            count2++;
        }
        curr_image->rel_bar = rel_bar;
        count++;
    }
    curr_sio->rel_bar = rel_bar;
    curr_sio->asm_src_info.rel_bar = rel_bar;
}



void StretchSetAll( sio_data *curr_sio, bool bar_max )
/****************************************************/
{
    image_info          *curr_image;
    mod_info            *curr_mod;
    file_info           *curr_file;
    long int            count;
    int                 count2;
    int                 count3;
    int                 count4;

    count = 0;
    while( count < curr_sio->image_count ) {
        curr_image = curr_sio->images[count];
        count2 = 0;
        while( count2 < curr_image->mod_count ) {
            curr_mod = curr_image->module[count2];
            count3 = 0;
            while( count3 < curr_mod->file_count ) {
                curr_file = curr_mod->mod_file[count3];
                count4 = 0;
                while( count4 < curr_file->rtn_count ) {
                    curr_file->routine[count4]->bar_max = bar_max;
                    count4++;
                }
                curr_file->bar_max = bar_max;
                count3++;
            }
            curr_mod->bar_max = bar_max;
            count2++;
        }
        curr_image->bar_max = bar_max;
        count++;
    }
    curr_sio->bar_max = bar_max;
    curr_sio->asm_src_info.bar_max = bar_max;
}



void SortSetAll( sio_data *curr_sio, int sort_type )
/**************************************************/
{
    image_info          *curr_image;
    mod_info            *curr_mod;
    file_info           *curr_file;
    long int            count;
    int                 count2;
    int                 count3;

    count = 0;
    while( count < curr_sio->image_count ) {
        curr_image = curr_sio->images[count];
        count2 = 0;
        while( count2 < curr_image->mod_count ) {
            curr_mod = curr_image->module[count2];
            count3 = 0;
            while( count3 < curr_mod->file_count ) {
                curr_file = curr_mod->mod_file[count3];
                curr_file->sort_type = sort_type;
                curr_file->sort_needed = true;
                count3++;
            }
            curr_mod->sort_type = sort_type;
            curr_mod->sort_needed = true;
            count2++;
        }
        curr_image->sort_type = sort_type;
        curr_image->sort_needed = true;
        count++;
    }
    curr_sio->sort_type = sort_type;
    curr_sio->sort_needed = true;
}



STATIC void resolveImageSamples( void )
/*************************************/
{
    image_info          *curr_image;
    massgd_sample_addr  **massgd_data;
    mod_info            *curr_mod;
    file_info           *curr_file;
    rtn_info            *curr_rtn;
    address             *addr;
    sample_index_t      tick_index;
    mod_handle          mh;
    sym_handle          *sh;
    long int            count;
    int                 count2;
    int                 count3;
    int                 count4;
    int                 index;
    int                 index2;

    sh = alloca( DIPHandleSize( HK_SYM, false ) );
    massgd_data = CurrSIOData->massaged_sample;
    tick_index = 1;
    index = 0;
    index2 = 0;
    for( count = 0; count < CurrSIOData->number_massaged; ++count, ++index2 ) {
        if( index2 > MAX_MASSGD_BUCKET_INDEX ) {
            ++index;
            index2 = 0;
        }
        addr = massgd_data[index][index2].raw;
        if( DIPAddrMod( *addr, &mh ) == SR_NONE ) {
            curr_image = AddrImage( addr );
            if( curr_image == NULL ) {
                curr_image = CurrSIOData->images[0];
            }
            curr_mod = curr_image->module[0];
            curr_rtn = curr_mod->mod_file[0]->routine[0];
        } else {
            curr_image = *(image_info **)DIPImageExtra( mh );
            curr_mod = findCurrMod( curr_image, mh );
            if( DIPAddrSym( mh, *addr, sh ) == SR_NONE ) {
                curr_rtn = curr_mod->mod_file[0]->routine[0];
            } else {
                curr_rtn = findCurrRtn( curr_mod, sh );
/**/            myassert( curr_rtn != NULL );
            }
        }
        if( curr_rtn != NULL ) {
            curr_rtn->tick_count += massgd_data[index][index2].hits;
            if( curr_rtn->first_tick_index == 0 ) {
                curr_rtn->first_tick_index = tick_index;
                if( curr_mod->first_tick_index == 0
                 || curr_mod->first_tick_index > tick_index ) {
                    curr_mod->first_tick_index = tick_index;
                }
            }
            curr_rtn->last_tick_index = tick_index;
        }
        tick_index++;
    }
    CurrSIOData->max_time = 0;
    count = 0;
    while( count < CurrSIOData->image_count ) {
        curr_image = CurrSIOData->images[count];
        curr_image->max_time = 0;
        count2 = 0;
        while( count2 < curr_image->mod_count ) {
            curr_mod = curr_image->module[count2];
            curr_mod->max_time = 0;
            count3 = 0;
            while( count3 < curr_mod->file_count ) {
                curr_file = curr_mod->mod_file[count3];
                curr_rtn = curr_file->routine[0];
                if( curr_rtn->unknown_routine && curr_rtn->tick_count == 0 ) {
                    curr_rtn->ignore_unknown_rtn = true;
                    curr_file->ignore_unknown_rtn = true;
                }
                curr_file->max_time = 0;
                count4 = 0;
                while( count4 < curr_file->rtn_count ) {
                    curr_rtn = curr_file->routine[count4];
                    curr_file->agg_count += curr_rtn->tick_count;
                    if( curr_rtn->tick_count > curr_file->max_time ) {
                        curr_file->max_time = curr_rtn->tick_count;
                    }
                    count4++;
                }
                curr_mod->agg_count += curr_file->agg_count;
                if( curr_file->agg_count > curr_mod->max_time ) {
                    curr_mod->max_time = curr_file->agg_count;
                }
                count3++;
            }
            curr_file = curr_mod->mod_file[0];
            if( curr_file->unknown_file && curr_file->agg_count == 0 ) {
                curr_file->ignore_unknown_file = true;
                curr_mod->ignore_unknown_file = true;
            }
            curr_image->agg_count += curr_mod->agg_count;
            if( curr_mod->agg_count > curr_image->max_time ) {
                curr_image->max_time = curr_mod->agg_count;
            }
            count2++;
        }
        curr_mod = curr_image->module[0];
        if( curr_mod->unknown_module && curr_mod->agg_count == 0 ) {
            curr_mod->ignore_unknown_mod = true;
            curr_image->ignore_unknown_mod = true;
        }
        if( curr_image->agg_count > CurrSIOData->max_time ) {
            CurrSIOData->max_time = curr_image->agg_count;
        }
        count++;
    }
    curr_image = CurrSIOData->images[0];
    if( curr_image->unknown_image && curr_image->agg_count == 0 ) {
        curr_image->ignore_unknown_image = true;
    }
}



STATIC void loadImageInfo( image_info * curr_image )
/**************************************************/
{
    int             name_len;
    int             object_file;
    int             sym_file;
    struct stat     file_status;

    sym_file = -1;
    object_file = -1;
    curr_image->dip_handle = NO_MOD;
    if( curr_image->sym_deleted ) {
    } else if( curr_image->sym_name != NULL ) {
        sym_file = open( curr_image->sym_name, O_RDONLY|O_BINARY );
        if( sym_file != -1 ) {
            curr_image->dip_handle = WPDipLoadInfo( PH2DFH( sym_file ),
                                       curr_image->sym_name, curr_image,
                                       sizeof(image_info), DIP_PRIOR_MIN, DIP_PRIOR_MAX );
        }
    } else {
        name_len = strlen( curr_image->name ) + 1;
        memcpy( FNameBuff, curr_image->name, name_len );
        ReplaceExt( FNameBuff, ".sym" );
        name_len = strlen( FNameBuff ) + 1;
        curr_image->sym_name = ProfAlloc( name_len );
        memcpy( curr_image->sym_name, FNameBuff, name_len );
        sym_file = open( curr_image->sym_name, O_RDONLY|O_BINARY );
        if( sym_file != -1 ) {
            curr_image->dip_handle = WPDipLoadInfo( PH2DFH( sym_file ),
                                      curr_image->sym_name, curr_image,
                                      sizeof(image_info), DIP_PRIOR_MIN, DIP_PRIOR_MAX );
        }
        if( curr_image->dip_handle == NO_MOD ) {
            ProfFree( curr_image->sym_name );
            curr_image->sym_name = NULL;
        }
    }
    object_file = open( curr_image->name, O_RDONLY|O_BINARY );
    if( object_file == -1 ) {
        curr_image->exe_not_found = true;
        if( curr_image->main_load ) {
            ErrorMsg( LIT( Exe_Not_Found ), curr_image->name );
        }
    } else if( curr_image->time_stamp == 0 ) {
        /*
           If sample timestamp is 0, the sampler couldn't figure out
           the right value. Assume it's OK.
        */
    } else if( fstat( object_file, &file_status ) == 0 ) {
        /* QNX creation dates and time stamps tend to be 1 */
        /* unit different, so do not test for equality */
        if( file_status.st_mtime - curr_image->time_stamp > 1 ) {
            curr_image->exe_changed = true;
            if( curr_image->main_load ) {
                ErrorMsg( LIT( Exe_Has_Changed ), curr_image->name );
            }
        }
    }
    if( curr_image->dip_handle == NO_MOD && !curr_image->sym_deleted
     && object_file != -1 ) {
        curr_image->dip_handle = WPDipLoadInfo( PH2DFH( object_file ),
                                   curr_image->name, curr_image,
                                   sizeof(image_info), DIP_PRIOR_MIN, DIP_PRIOR_MAX );
    }
    if( curr_image->dip_handle == NO_MOD ) {
        if( sym_file != -1 ) {
            close( sym_file );
        }
        if( object_file != -1 ) {
            close( object_file );
        }
    }
    initModuleInfo( curr_image );
    if( curr_image->dip_handle != NO_MOD ) {
        DIPWalkModList( curr_image->dip_handle, &loadModuleInfo, curr_image );
    }
}



STATIC void loadSampleImages( void )
/**********************************/
{
    image_info          *curr_image;
    int                 image_count;

    CurrSIOData->dip_process = WPDipProc();
    WPDipSetProc( CurrSIOData->dip_process );
    image_count = 0;
    while( image_count < CurrSIOData->image_count ) {
        if( CurrSIOData->images[image_count]->unknown_image ) break;
        image_count++;
    }
    if( image_count != 0 ) {
        curr_image = CurrSIOData->images[image_count];
        CurrSIOData->images[image_count] = CurrSIOData->images[0];
        CurrSIOData->images[0] = curr_image;
    }
    image_count = 0;
    while( image_count < CurrSIOData->image_count ) {
        curr_image = CurrSIOData->images[image_count];
        curr_image->agg_count = 0;
        if( curr_image->unknown_image ) {
            initModuleInfo( curr_image );
        } else {
            loadImageInfo( curr_image );
        }
        image_count++;
    }
}



STATIC void calcAggregates( void )
/********************************/
{
    unsigned            index;
    unsigned            index2;
    int                 cmp_result;
    unsigned            *sorted_idx;
    address             ***sorted_vect;
    massgd_sample_addr  **massgd_data;
//    unsigned            mbuckets;
    unsigned            curr_mbucket;
    unsigned            curr_midx;
    thread_data         *thd;
    unsigned            buckets;
    unsigned            base;
    unsigned            best;
    unsigned            end;
    massgd_sample_addr  *curr;

    ClearMassaged( CurrSIOData );
    buckets = 0;
    for( thd = CurrSIOData->samples; thd != NULL; thd = thd->next ) {
        buckets += RAW_BUCKET_IDX( thd->end_time - thd->start_time ) + 1;
    }
    sorted_idx = ProfCAlloc( buckets * sizeof( *sorted_idx ) );
    sorted_vect = ProfAlloc( buckets * sizeof(*thd->raw_bucket) );
    base = 0;
    for( thd = CurrSIOData->samples; thd != NULL; thd = thd->next ) {
        end = RAW_BUCKET_IDX( thd->end_time - thd->start_time ) + 1;
        for( index = 0; index < end; ++index, ++base ) {
            sorted_vect[base] = ProfAlloc( MAX_RAW_BUCKET_INDEX * sizeof( **sorted_vect ) );
            for( index2 = 0; index2 < MAX_RAW_BUCKET_INDEX; ++index2 ) {
                sorted_vect[base][index2]
                    = &thd->raw_bucket[index][index2];
            }
            qsort( sorted_vect[base], MAX_RAW_BUCKET_INDEX, sizeof(**sorted_vect), rawSampCmp );
        }
    }
    /* skip over all the 0:0 samples */
    for( index = 0; index < buckets; ++index ) {
        index2 = 0;
        for( ;; ) {
            if( index2 >= MAX_RAW_BUCKET_INDEX ) break;
            if( !(sorted_vect[index][index2]->mach.segment == 0
             && sorted_vect[index][index2]->mach.offset == 0) ) break;
            ++index2;
        }
        sorted_idx[index] = index2;
    }
    curr = NULL;
    curr_mbucket = 0;
    curr_midx = -1;
//    mbuckets = 1;
    massgd_data = ProfAlloc( sizeof( *massgd_data ) );
    massgd_data[0] = ProfCAlloc( MAX_MASSGD_BUCKET_SIZE );
    for( ;; ) {
        best = -1U;
        for( index = 0; index < buckets; ++index ) {
            index2 = sorted_idx[index];
            if( index2 >= MAX_RAW_BUCKET_INDEX ) continue;
            if( best == -1U ) best = index;
            cmp_result = AddrCmp( sorted_vect[index][index2],
                                  sorted_vect[best][sorted_idx[best]] );
            if( cmp_result < 0 ) best = index;
        }
        if( best == -1U ) break;
        if( curr == NULL || AddrCmp( sorted_vect[best][sorted_idx[best]], curr->raw ) != 0 ) {
            if( ++curr_midx >= MAX_MASSGD_BUCKET_INDEX ) {
                ++curr_mbucket;
                massgd_data = ProfRealloc( massgd_data, (curr_mbucket+1) * sizeof(*massgd_data) );
                massgd_data[curr_mbucket] = ProfCAlloc( MAX_MASSGD_BUCKET_SIZE );
                curr_midx = 0;
            }
            curr = &massgd_data[curr_mbucket][curr_midx];
            curr->raw = sorted_vect[best][sorted_idx[best]];
        }
        curr->hits++;
        sorted_idx[best]++;
    }

    CurrSIOData->massaged_sample = massgd_data;
    CurrSIOData->number_massaged = 1 + curr_midx
                + (curr_mbucket * (unsigned long)MAX_MASSGD_BUCKET_INDEX);
    CurrSIOData->massaged_mapped = true;
    for( index = 0; index < buckets; ++index ) {
        ProfFree( sorted_vect[index] );
    }
    ProfFree( sorted_vect );
    ProfFree( sorted_idx );
}



void SetSampleInfo( sio_data *curr_sio )
/**************************************/
{
    void            *cursor_type;

    cursor_type = WndHourGlass( NULL );
    if( curr_sio->dip_process != NULL ) {
        WPDipDestroyProc( curr_sio->dip_process );
        curr_sio->dip_process = NULL;
    }
    CurrSIOData = curr_sio;
    SetCurrentMAD( CurrSIOData->config.mad );
    if( CurrSIOData->samples != NULL ) {
        if( !CurrSIOData->massaged_mapped || CurrSIOData->massaged_sample == NULL ) {
            calcAggregates();
        }
    }
    loadSampleImages();
    resolveImageSamples();
    GatherSetAll( curr_sio, false );
    AbsSetAll( curr_sio, true );
    RelSetAll( curr_sio, true );
    SortSetAll( curr_sio, SORT_COUNT );
    WndHourGlass( cursor_type );
}
