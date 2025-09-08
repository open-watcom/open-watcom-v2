/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
    drmem_hdl   parent;
    drmem_hdl   inheritance;
} BaseInfo;

typedef struct {
    DRCLSSRCH   callback;
    void        *data;
} FriendInfo;

void DRENTRY DRKidsSearch( drmem_hdl clhandle, dr_search search, void *data, DRCLSSRCH callback )
/************************************************************************************************
 * search the children of a given entry for particular tags ..
 * callback is called with a found entry's type, handle,
 * name, parent, and data.  If it returns false, searching stops.
 */
{
    dr_sym_type     symtype;
    drmem_hdl       prt = clhandle;
    drmem_hdl       tmp_entry = clhandle;
    drmem_hdl       newentry;
    drmem_hdl       abbrev;
    dw_tagnum       tag;
    unsigned_8      has_kids;
    int             index;
    char            *name;

    if( DR_ReadTagEnd( &tmp_entry, &abbrev, &tag ) ) {
        return;
    }
    has_kids = DR_VMReadByte( abbrev );
    abbrev++;
    if( !has_kids ) {
        return;
    }

    DR_SkipAttribs( abbrev, &tmp_entry );

    newentry = tmp_entry;                       /* alway point to start of die */
    while( !DR_ReadTagEnd( &tmp_entry, &abbrev, &tag ) ) {
        has_kids = DR_VMReadByte( abbrev );
        abbrev++;
        if( DR_SearchArray( SearchTypes[search], tag ) ) {
            symtype = DR_SYM_NOT_SYM;
            for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
                if( DR_SearchArray( SearchTags[index], tag ) ) {
                    symtype = index;
                    break;
                }
            }
            name = DR_GetName( abbrev, tmp_entry );
            if( !callback( symtype, newentry, name, prt, data ) ) {
                break;
            }
        }

        /* NYI - DW_TAG_lexical_block only one? */

        if( has_kids && tag != DW_TAG_lexical_block ) {
            DR_SkipChildren( &abbrev, &tmp_entry );
        } else {
            DR_SkipAttribs( abbrev, &tmp_entry );
        }
        newentry = tmp_entry;                   /* alway point to start of die */
    }
}


static bool baseHook( dr_sym_type notused1, drmem_hdl handle,
                     char *name, drmem_hdl notused2, void *info )
/***************************************************************/
{
    BaseInfo        *binfo = (BaseInfo *)info;
    drmem_hdl       abbrev;
    drmem_hdl       basehandle;
    drmem_hdl       tmp_entry;
    dw_tagnum       tag;
    char            *basename;
    dr_sym_type     symtype;
    int             index;

    /* unused parameters */ (void)notused1; (void)notused2;

    if( name != NULL ) {
        DR_FREE( name );
    }

    tmp_entry = handle;
    if( DR_ReadTagEnd( &tmp_entry, &abbrev, &tag ) )
        return( true );
    abbrev++;   /* skip child flag */

    if( DR_ScanForAttrib( &abbrev, &tmp_entry, DW_AT_type ) ) {
        basehandle = DR_ReadReference( abbrev, tmp_entry );
        tmp_entry = basehandle;
        if( DR_ReadTagEnd( &tmp_entry, &abbrev, &tag ) )
            return( true );
        abbrev++;   /* skip child flag */

        symtype = DR_SYM_NOT_SYM;
        for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
            if( DR_SearchArray( SearchTags[index], tag ) ) {
                symtype = index;
                break;
            }
        }
        basename = DR_GetName( abbrev, tmp_entry );
        return( binfo->callback( symtype, basehandle, basename, handle, binfo->data ) );
    }
    return( true );
}

void DRENTRY DRBaseSearch( drmem_hdl clhandle, void * data, DRCLSSRCH callback )
/******************************************************************************/
{
    BaseInfo binfo;
    binfo.data = data;
    binfo.callback = callback;
    DRKidsSearch( clhandle, DR_SEARCH_BASE, &binfo, baseHook );
}

static bool CheckEntry( drmem_hdl abbrev, drmem_hdl handle, mod_scan_info *minfo, void *data )
/********************************************************************************************/
{
    drmem_hdl       ref;
    BaseInfo        *sinfo = (BaseInfo *)data;
    dr_sym_type     symtype;
    int             index;
    drmem_hdl       tmp_abbrev = abbrev;
    drmem_hdl       tmp_entry = handle;
    dw_tagnum       tag;

    if( DR_ScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) ) {
        ref = DR_ReadReference( tmp_abbrev, tmp_entry );
        if( ref == sinfo->parent ) {
            tmp_entry = minfo->context->classhdl;
            if( DR_ReadTagEnd( &tmp_entry, &tmp_abbrev, &tag ) )
                DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
            tmp_abbrev++;   /* skip child flag */

            symtype = DR_SYM_NOT_SYM;
            for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
                if( DR_SearchArray( SearchTags[index], tag ) ) {
                    symtype = index;
                    break;
                }
            }
            sinfo->callback( symtype, minfo->context->classhdl,
                             DR_GetName( tmp_abbrev, tmp_entry ),
                             minfo->handle, sinfo->data );
        }
    }
    return( true );
}

void DRENTRY DRDerivedSearch( drmem_hdl handle, void *data, DRCLSSRCH callback )
/*******************************************************************************
 * Warning!! KLUDGE! this searches the compile unit for entries that have a
 * DW_TAG_inheritance child that references handle -- not that fast.
 * also, only checks the compile unit the base class is in.
 */
{
    BaseInfo            info;
    dr_cu_handle        compunit;
    const dw_tagnum     inh_lst[] = { DW_TAG_inheritance, 0 };
    dr_search_context   ctxt;

    info.callback = callback;
    info.data = data;
    info.parent = handle;

    compunit = DR_FindCompileInfo( handle );
    ctxt.compunit = compunit;
    ctxt.start = compunit->start;
    ctxt.end = compunit->start + DR_VMReadDWord( compunit->start );
    ctxt.start += sizeof( compuhdr_prologue );
    ctxt.classhdl    = DRMEM_HDL_NULL;
    ctxt.functionhdl = DRMEM_HDL_NULL;
    ctxt.stack.size  = 0;
    ctxt.stack.free  = 0;
    ctxt.stack.stack = NULL;

    DR_ScanCompileUnit( &ctxt, CheckEntry, inh_lst, DR_DEPTH_CLASSES, &info );
    DR_FreeContextStack( &ctxt.stack );
}

static bool friendHook( dr_sym_type st, drmem_hdl handle, char *name,
                                            drmem_hdl prt, void *data )
/*************************************************************************/
// the 'st' field will be DR_SYM_NOT_SYM as it is the symbol type of
// the 'DW_TAG_friend'.  The correct symbol type must be found for
// the friend.  Likewise the name is meaningless, but the 'prt' is correct.
{
    drmem_hdl   abbrev;
    drmem_hdl   entry;
    drmem_hdl   friend_han;
    FriendInfo  *info;

    info = (FriendInfo *)data;
    DR_FREE( name );

    entry = handle;
    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_friend ) ) {
        friend_han = DR_ReadReference( abbrev, entry );
        name = DRGetName( friend_han );
        st = DRGetSymType( friend_han );

        info->callback( st, friend_han, name, prt, info->data );
    }
    return( true );
}

void DRENTRY DRFriendsSearch( drmem_hdl handle, void *data, DRCLSSRCH callback )
/******************************************************************************/
{
    FriendInfo info;

    info.callback = callback;
    info.data = data;
    DRKidsSearch( handle, DR_SEARCH_FRIENDS, &info, friendHook );
}

dr_sym_type DRENTRY DRGetSymType( drmem_hdl entry )
/*************************************************/
{
    dr_sym_type     symtype = DR_SYM_NOT_SYM;
    int             index;
    dw_tagnum       tag;

    tag = DR_GetTag( entry );
    for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
        if( DR_SearchArray( SearchTags[index], tag ) ) {
            symtype = index;
            break;
        }
    }
    return( symtype );
}
