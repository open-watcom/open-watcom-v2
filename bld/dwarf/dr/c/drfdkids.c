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
* Description:  Search children of a given debug information entry.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"

typedef struct {
    void        *data;  /* caller wants this back */
    DRCLSSRCH   callback;
    dr_handle   parent;
    dr_handle   inheritance;
} BaseInfo;

typedef struct {
    DRCLSSRCH   callback;
    void        *data;
} FriendInfo;

void DRKidsSearch( dr_handle clhandle, dr_search search, void *data, DRCLSSRCH callback )
/***************************************************************************************/
// search the children of a given entry for particular tags ..
// callback is called with a found entry's type, handle,
// name, parent, and data.  If it returns FALSE, searching stops.
{
    dr_sym_type     symtype;
    dr_handle       prt = clhandle;
    dr_handle       tmp_entry = clhandle;
    dr_handle       newentry;
    dr_handle       abbrev;
    dr_abbrev_idx   abbrev_idx;
    dw_tagnum       tag;
    unsigned_8      has_kids;
    int             index;
    char            *name;

    abbrev_idx = DWRVMReadULEB128( &tmp_entry );
    if( abbrev_idx == 0 ) {
        return;
    }

    abbrev = DWRLookupAbbrev( tmp_entry, abbrev_idx );
    tag = DWRVMReadULEB128( &abbrev );
    has_kids = DWRVMReadByte( abbrev );
    abbrev += sizeof( unsigned_8 );
    if( !has_kids ) {
        return;
    }

    DWRSkipAttribs( abbrev, &tmp_entry );

    for( ;; ) {
        newentry = tmp_entry;                   /* alway point to start of die */
        abbrev_idx = DWRVMReadULEB128( &tmp_entry );
        if( abbrev_idx == 0 ) {
            break;
        }
        abbrev = DWRLookupAbbrev( tmp_entry, abbrev_idx );
        tag = DWRVMReadULEB128( &abbrev );
        has_kids = DWRVMReadByte( abbrev );
        abbrev += sizeof( unsigned_8 );
        if( DWRSearchArray( SearchTypes[search], tag ) ) {
            symtype = DR_SYM_NOT_SYM;
            for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
                if( DWRSearchArray( SearchTags[index], tag ) ) {
                    symtype = index;
                    break;
                }
            }
            name = DWRGetName( abbrev, tmp_entry );
            if( !callback( symtype, newentry, name, prt, data ) ) {
                break;
            }
        }

        /* NYI - DW_TAG_lexical_block only one? */

        if( has_kids && tag != DW_TAG_lexical_block ) {
            DWRSkipChildren( &abbrev, &tmp_entry );
        } else {
            DWRSkipAttribs( abbrev, &tmp_entry );
        }
    }
}


static bool baseHook( dr_sym_type notused1, dr_handle handle,
                     char *name, dr_handle notused2, void *info )
/***************************************************************/
{
    BaseInfo        *binfo = (BaseInfo *)info;
    dr_handle       abbrev;
    dr_abbrev_idx   abbrev_idx;
    dr_handle       basehandle;
    dr_handle       tmp_entry;
    dw_tagnum       tag;
    char            *basename;
    dr_sym_type     symtype;
    int             index;

    notused1 = notused1;
    notused2 = notused2;

    if( name != NULL ) {
        DWRFREE( name );
    }

    tmp_entry = handle;
    abbrev_idx = DWRVMReadULEB128( &tmp_entry );
    if( abbrev_idx == 0 )
        return( TRUE );
    abbrev = DWRLookupAbbrev( tmp_entry, abbrev_idx );
    tag = DWRVMReadULEB128( &abbrev );
    abbrev += sizeof( unsigned_8 );

    if( DWRScanForAttrib( &abbrev, &tmp_entry, DW_AT_type ) ) {
        basehandle = DWRReadReference( abbrev, tmp_entry );
        tmp_entry = basehandle;
        abbrev_idx = DWRVMReadULEB128( &tmp_entry );
        if( abbrev_idx == 0 )
            return( TRUE );
        abbrev = DWRLookupAbbrev( tmp_entry, abbrev_idx );
        tag = DWRVMReadULEB128( &abbrev );
        abbrev += sizeof( unsigned_8 );

        for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
            symtype = DR_SYM_NOT_SYM;
            if( DWRSearchArray( SearchTags[index], tag ) ) {
                symtype = index;
                break;
            }
        }
        basename = DWRGetName( abbrev, tmp_entry );
        return( binfo->callback( symtype, basehandle, basename, handle, binfo->data ) );
    }
    return( TRUE );
}

void DRBaseSearch( dr_handle clhandle, void * data, DRCLSSRCH callback )
/**********************************************************************/
{
    BaseInfo binfo;
    binfo.data = data;
    binfo.callback = callback;
    DRKidsSearch( clhandle, DR_SEARCH_BASE, &binfo, baseHook );
}

static bool CheckEntry( dr_handle abbrev, dr_handle handle, mod_scan_info *minfo, void *data )
/********************************************************************************************/
{
    dr_handle       ref;
    BaseInfo        *sinfo = (BaseInfo *)data;
    dr_sym_type     symtype;
    int             index;
    dr_handle       tmp_abbrev = abbrev;
    dr_abbrev_idx   abbrev_idx;
    dr_handle       tmp_entry = handle;
    dw_tagnum       tag;

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) ) {
        ref = DWRReadReference( tmp_abbrev, tmp_entry );
        if( ref == sinfo->parent ) {
            tmp_entry = minfo->context->classhdl;
            abbrev_idx = DWRVMReadULEB128( &tmp_entry );
            if( abbrev_idx == 0 )
                DWREXCEPT( DREXCEP_BAD_DBG_INFO );
            tmp_abbrev = DWRLookupAbbrev( tmp_entry, abbrev_idx );
            tag = DWRVMReadULEB128( &tmp_abbrev );
            tmp_abbrev += sizeof( unsigned_8 );

            for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
                symtype = DR_SYM_NOT_SYM;
                if( DWRSearchArray( SearchTags[index], tag ) ) {
                    symtype = index;
                    break;
                }
            }
            sinfo->callback( symtype, minfo->context->classhdl,
                             DWRGetName( tmp_abbrev, tmp_entry ),
                             minfo->handle, sinfo->data );
        }
    }
    return( TRUE );
}

void DRDerivedSearch( dr_handle handle, void *data, DRCLSSRCH callback )
/**********************************************************************/
// Warning!! KLUDGE! this searches the compile unit for entries that have a
// DW_TAG_inheritance child that references handle -- not that fast.
// also, only checks the compile unit the base class is in.
{
    BaseInfo            info;
    compunit_info       *compunit;
    const dw_tagnum     inh_lst[] = { DW_TAG_inheritance, 0 };
    dr_search_context   ctxt;

    info.callback = callback;
    info.data = data;
    info.parent = handle;

    compunit = DWRFindCompileInfo( handle );
    ctxt.compunit = compunit;
    ctxt.start = compunit->start;
    ctxt.end = compunit->start + DWRVMReadDWord( compunit->start );
    ctxt.start += COMPILE_UNIT_HDR_SIZE;
    ctxt.classhdl    = 0;
    ctxt.functionhdl = 0;
    ctxt.stack.size  = 0;
    ctxt.stack.free  = 0;
    ctxt.stack.stack = NULL;

    DWRScanCompileUnit( &ctxt, CheckEntry, inh_lst, DR_DEPTH_CLASSES, &info );
    DWRFreeContextStack( &ctxt.stack );
}

static bool friendHook( dr_sym_type st, dr_handle handle, char *name,
                                            dr_handle prt, void *data )
/*************************************************************************/
// the 'st' field will be DR_SYM_NOT_SYM as it is the symbol type of
// the 'DW_TAG_friend'.  The correct symbol type must be found for
// the friend.  Likewise the name is meaningless, but the 'prt' is correct.
{
    dr_handle   abbrev;
    dr_handle   entry;
    dr_handle   friend_han;
    FriendInfo  *info;

    info = (FriendInfo *)data;
    DWRFREE( name );

    entry = handle;
    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_friend ) ) {
        friend_han = DWRReadReference( abbrev, entry );
        name = DRGetName( friend_han );
        st = DRGetSymType( friend_han );

        info->callback( st, friend_han, name, prt, info->data );
    }
    return( TRUE );
}

void DRFriendsSearch( dr_handle handle, void *data, DRCLSSRCH callback )
/**********************************************************************/
{
    FriendInfo info;

    info.callback = callback;
    info.data = data;
    DRKidsSearch( handle, DR_SEARCH_FRIENDS, &info, friendHook );
}

dr_sym_type DRGetSymType( dr_handle entry )
/*****************************************/
{
    dr_sym_type     symtype = DR_SYM_NOT_SYM;
    int             index;
    dr_handle       abbrev;
    dr_abbrev_idx   abbrev_idx;
    dw_tagnum       tag;

    abbrev_idx = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev_idx );
    tag = DWRVMReadULEB128( &abbrev );
    for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
        if( DWRSearchArray( SearchTags[index], tag ) ) {
            symtype = index;
            break;
        }
    }
    return( symtype );
}
