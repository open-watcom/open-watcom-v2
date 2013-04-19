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


#include "precomp.h"
#include <limits.h>
#include <string.h>
#include <ddeml.h>
#include "watcom.h"
#include "wreglbl.h"
#include "wremain.h"
#include "wregcres.h"
#include "wre_wres.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wremem.h"
#include "wredel.h"
#include "wrenames.h"
#include "wredde.h"
#include "wreimg.h"
#include "wreimage.h"
#include "wreseted.h"
#include "wreftype.h"
#include "wrectl3d.h"
#include "wrerenam.h"
#include "wrenames.h"
#include "wrenew.h"
#include "wreres.h"
#include "wreclip.h"
#include "wre_rc.h"
#include "wrdll.h"
#include "wrbitmap.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WREResPasteProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WREClipFormat {
    UINT        fmt;
    char        *fmt_name;
    uint_16     type;
} WREClipFormat;

typedef struct WREClipData {
    uint_32     clip_size;
    uint_32     data_size;
    uint_32     data_offset;
    uint_16     type;
    uint_16     memflags;
    Bool        is32bit;
    BYTE        name[1];
} WREClipData;

typedef struct WREPasteData {
    uint_16     type;
    WResID      *name;
    int         ret;
} WREPasteData;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WREClipData  *WRECreateClipData( WRECurrentResInfo *curr );
static Bool         WREGetClipData( WREClipFormat *fmt, void **data, uint_32 *dsize );
static Bool         WREClipBitmap( WRECurrentResInfo *curr, HWND main );
static Bool         WREClipResource( WRECurrentResInfo *curr, HWND main, UINT fmt );
static Bool         WREQueryPasteReplace( WResID *name, uint_16 type, Bool *replace );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WREClipFormat WREClipFormats[] = {
    { 0,            WR_CLIPBD_ACCEL,    (uint_16)RT_ACCELERATOR     },
    { 0,            WR_CLIPBD_MENU,     (uint_16)RT_MENU            },
    { 0,            WR_CLIPBD_STRING,   (uint_16)RT_STRING          },
    { 0,            WR_CLIPBD_CURSOR,   (uint_16)RT_GROUP_CURSOR    },
    { 0,            WR_CLIPBD_ICON,     (uint_16)RT_GROUP_ICON      },
    { 0,            WR_CLIPBD_DIALOG,   (uint_16)RT_DIALOG          },
    { 0,            WR_CLIPBD_FONT,     (uint_16)RT_FONT            },
    { 0,            WR_CLIPBD_RCDATA,   (uint_16)RT_RCDATA          },
    { 0,            WR_CLIPBD_BITMAP,   (uint_16)RT_BITMAP          },
    { CF_BITMAP,    NULL,               (uint_16)RT_BITMAP          },
    { CF_DIB,       NULL,               (uint_16)RT_BITMAP          },
    { 0,            NULL,               0                           }
    // last entry is a sentinel
};

static HBITMAP WPrivateFormat       = NULL;

Bool WREGetClipData( WREClipFormat *fmt, void **data, uint_32 *dsize )
{
    Bool        ok;
    HANDLE      hclipdata;
    void        *mem;

    hclipdata = (HANDLE)NULL;
    mem = NULL;
    ok = (fmt != NULL && fmt->fmt != 0 && data != NULL && dsize != 0);

    if( ok ) {
        hclipdata = GetClipboardData( fmt->fmt );
        ok = (hclipdata != NULL);
    }

    if( ok ) {
        mem = GlobalLock( hclipdata );
        ok = (mem != NULL);
    }

    if( ok ) {
        *dsize = (uint_32)GlobalSize( hclipdata );
        ok = (*dsize != 0);
    }

    if( ok ) {
        if( *dsize >= INT_MAX ) {
            WREDisplayErrorMsg( WRE_RESTOOLARGETOPASTE );
            ok = FALSE;
        }
    }

    if( ok ) {
        *data = WREMemAlloc( *dsize );
        ok = (*data != NULL);
    }

    if( ok ) {
        memcpy( *data, mem, *dsize );
    }

    if( !ok ) {
        if( *data ) {
            WREMemFree( *data );
            *data = NULL;
            *dsize = 0;
        }
    }

    if( mem != NULL ) {
        GlobalUnlock( hclipdata );
    }

    return( ok );
}

static WREClipFormat *WREFindClipFormatFromType( uint_16 type )
{
    int         i;

    for( i = 0; WREClipFormats[i].type != 0; i++ ) {
        if( WREClipFormats[i].type == type ) {
            return( &WREClipFormats[i] );
        }
    }

    return( NULL );
}

// assumes clipboard is already open
static WREClipFormat *WREGetClipFormat( void )
{
    int         i;

    for( i = 0; WREClipFormats[i].type != 0; i++ ) {
        if( IsClipboardFormatAvailable( WREClipFormats[i].fmt ) ) {
            return( &WREClipFormats[i] );
        }
    }

    return( NULL );
}

static WResID *WREGetClipDataName( WREClipData *clip_data )
{
    WResID      *name;

    name = NULL;
    if( clip_data != NULL ) {
        name = WRMem2WResID( &clip_data->name[0], clip_data->is32bit );
    }
    return( name );
}

static Bool WREHandleClipDataNames( WREResInfo *info, WResID *type,
                                    WResID **name, Bool *replace )
{
    WRECurrentResInfo   curr;
    WREResRenameInfo    ren_info;
    uint_16             t;
    WResLangType        lang;
    Bool                exists;
    Bool                ok;

    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    ok = (info != NULL && info->info != NULL && type != NULL && name != NULL &&
          *name != NULL && replace != NULL);

    if( ok ) {
        t = 0;
        if( !type->IsName ) {
            t = type->ID.Num;
        }
        ok = (t != 0);
    }

    if( ok ) {
        exists = WRDoesNameExist( info->info->dir, type, *name );
        while( exists ) {
            ok = WREQueryPasteReplace( *name, t, replace );
            if( !ok ) {
                break;
            }
            if( *replace ) {
                curr.info = info;
                curr.type = WREFindTypeNodeFromWResID( info->info->dir, type );
                curr.res = WREFindResNodeFromWResID( curr.type, *name );
                curr.lang = WREFindLangNodeFromLangType( curr.res, &lang );
                ok = WREDeleteResource( &curr, TRUE );
                if( !ok ) {
                    break;
                }
            } else {
                ren_info.old_name = *name;
                ren_info.new_name = NULL;
                ok = (WREGetNewName( &ren_info ) && ren_info.new_name != NULL);
                if( ok ) {
                    WREMemFree( *name );
                    *name = ren_info.new_name;
                }
            }
            exists = WRDoesNameExist( info->info->dir, type, *name );
            if( exists && *replace ) {
                ok = FALSE;
                break;
            }
        }
    }

    return( ok );
}

static Bool WREGetAndPasteResource( WREClipFormat *fmt )
{
    WRETypeName         *tn;
    WRECurrentResInfo   curr;
    WREClipData         *cdata;
    WResLangType        lang;
    WResID              *ctype;
    WResID              *cname;
    void                *data;
    uint_32             dsize;
    int                 dup;
    Bool                new_type;
    Bool                replace;
    Bool                ok;

    cdata = NULL;
    cname = NULL;
    ctype = NULL;
    new_type = TRUE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = (fmt != NULL);

    if( ok ) {
        tn = WREGetTypeNameFromRT( fmt->type );
        ok = (tn != NULL);
    }

    if( ok ) {
        ctype = WResIDFromNum( fmt->type );
        ok = (ctype != NULL);
    }

    if( ok ) {
        ok = WREGetClipData( fmt, &data, &dsize );
    }

    if( ok ) {
        cdata = (WREClipData *)data;
        data = NULL;
        ok = (cdata != NULL);
    }

    if( ok ) {
        data = WREMemAlloc( cdata->data_size );
        ok = (data != NULL);
    }

    if( ok ) {
        memcpy( data, (BYTE *)cdata + cdata->data_offset, cdata->data_size );
        cname = WREGetClipDataName( cdata );
        ok = (cname != NULL);
    }

    if( ok ) {
        WREGetCurrentResource( &curr );
        if( curr.info == NULL ) {
            curr.info = WRECreateNewResource( NULL );
            ok = (curr.info != NULL);
        }
    }

    if( ok ) {
        ok = WREHandleClipDataNames( curr.info, ctype, &cname, &replace );
    }

    if( ok ) {
        if( curr.info != NULL ) {
            if( curr.info->info->dir ) {
                new_type = (WREFindTypeNodeFromWResID( curr.info->info->dir,
                                                       ctype ) == NULL);
            }
        }
        ok = WRENewResource( &curr, ctype, cname, cdata->memflags, 0,
                             cdata->data_size, &lang, &dup, tn->type,
                             new_type ) && !dup;
    }

    if( ok ) {
        curr.lang->data = data;
        WRESetResModified( curr.info, TRUE );
    }

    if( cdata != NULL ) {
        WREMemFree( cdata );
    }

    if( cname != NULL ) {
        WREMemFree( cname );
    }

    if( ctype != NULL ) {
        WREMemFree( ctype );
    }

    if( !ok ) {
        if( data != NULL ) {
            WREMemFree( data );
        }
    }

    return( ok );
}

static Bool WREGetAndPasteIconOrCursor( WREClipFormat *fmt )
{
    WRECurrentResInfo   curr;
    WREClipData         *cdata;
    WResLangType        lang;
    WResID              *ctype;
    WResID              *cname;
    void                *data;
    uint_32             dsize;
    int                 dup;
    Bool                new_type;
    Bool                replace;
    Bool                ok;

    cdata = NULL;
    cname = NULL;
    ctype = NULL;
    new_type = TRUE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = (fmt != NULL);

    if( ok ) {
        ctype = WResIDFromNum( fmt->type );
        ok = (ctype != NULL);
    }

    if( ok ) {
        ok = WREGetClipData( fmt, &data, &dsize );
    }

    if( ok ) {
        cdata = (WREClipData *)data;
        data = NULL;
        ok = (cdata != NULL);
    }

    if( ok ) {
        data = WREMemAlloc( cdata->data_size );
        ok = (data != NULL);
    }

    if( ok ) {
        memcpy( data, (BYTE *)cdata + cdata->data_offset, cdata->data_size );
        cname = WREGetClipDataName( cdata );
        ok = (cname != NULL);
    }

    if( ok ) {
        WREGetCurrentResource( &curr );
        if( curr.info == NULL ) {
            curr.info = WRECreateNewResource( NULL );
            ok = (curr.info != NULL);
        }
    }

    if( ok ) {
        ok = WREHandleClipDataNames( curr.info, ctype, &cname, &replace );
    }

    if( ok ) {
        if( curr.info != NULL ) {
            if( curr.info->info->dir ) {
                new_type = (WREFindTypeNodeFromWResID( curr.info->info->dir,
                                                       ctype ) == NULL );
            }
        }
        ok = WRENewResource( &curr, ctype, cname, cdata->memflags, 0,
                             cdata->data_size, &lang, &dup, fmt->type,
                             new_type ) && !dup;
    }

    if( ok ) {
        if( fmt->type == (uint_16)RT_GROUP_ICON ) {
            ok = WRECreateIconEntries( &curr, data, dsize );
        } else if( fmt->type == (uint_16)RT_GROUP_CURSOR ) {
            ok = WRECreateCursorEntries( &curr, data, dsize );
        } else {
            ok = FALSE;
        }
    }

    if( ok ) {
        WRESetResModified( curr.info, TRUE );
    }

    if( data != NULL ) {
        WREMemFree( data );
    }

    if( cdata != NULL ) {
        WREMemFree( cdata );
    }

    if( cname != NULL ) {
        WREMemFree( cname );
    }

    if( ctype != NULL ) {
        WREMemFree( ctype );
    }

    if( !ok ) {
        if( data != NULL ) {
            WREMemFree( data );
        }
    }

    return( ok );
}

static Bool WREGetAndPasteBitmap( WREClipFormat *fmt, void *data, uint_32 dsize )
{
    WRECurrentResInfo   curr;
    WResLangType        lang;
    WResID              *ctype;
    WResID              *cname;
    int                 dup;
    Bool                new_type;
    Bool                replace;
    Bool                ok;

    cname = NULL;
    ctype = NULL;
    new_type = TRUE;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;

    ok = (fmt != NULL && data != NULL && dsize != 0);

    if( ok ) {
        ctype = WResIDFromNum( fmt->type );
        ok = (ctype != NULL);
    }

    if( ok ) {
        cname = WRRecallBitmapName();
        if( cname == NULL ) {
            cname = WRECreateImageTitle( (uint_16)RT_BITMAP );
        }
        ok = (cname != NULL);
    }

    if( ok ) {
        WREGetCurrentResource( &curr );
        if( curr.info == NULL ) {
            curr.info = WRECreateNewResource( NULL );
            ok = (curr.info != NULL);
        }
    }

    if( ok ) {
        ok = WREHandleClipDataNames( curr.info, ctype, &cname, &replace );
    }

    if( ok ) {
        if( curr.info != NULL ) {
            if( curr.info->info->dir ) {
                new_type = (WREFindTypeNodeFromWResID( curr.info->info->dir,
                                                       ctype ) == NULL );
            }
        }
        ok = WRENewResource( &curr, ctype, cname, DEF_MEMFLAGS, 0,
                             dsize, &lang, &dup, (uint_16)RT_BITMAP,
                             new_type ) && !dup;
    }

    if( ok ) {
        curr.lang->data = data;
        WRESetResModified( curr.info, TRUE );
    }

    if( cname != NULL ) {
        WREMemFree( cname );
    }

    if( ctype != NULL ) {
        WREMemFree( ctype );
    }

    return( ok );
}

static Bool WREGetAndPasteDIB( WREClipFormat *fmt )
{
    void                *data;
    uint_32             dsize;
    Bool                ok;

    data = NULL;

    ok = (fmt != NULL);

    if( ok ) {
        ok = WREGetClipData( fmt, &data, &dsize );
    }

    if( ok ) {
        ok = WREGetAndPasteBitmap( fmt, data, dsize );
    }

    if( !ok ) {
        if( data != NULL ) {
            WREMemFree( data );
        }
    }

    return( ok );
}

static Bool WREGetAndPasteHBITMAP( WREClipFormat *fmt )
{
    HBITMAP             hbitmap;
    void                *data;
    uint_32             dsize;
    Bool                ok;

    data = NULL;

    ok = (fmt != NULL);

    if( ok ) {
        hbitmap = (HBITMAP)GetClipboardData( fmt->fmt );
        ok = (hbitmap != (HBITMAP)NULL);
    }

    if( ok ) {
        ok = WRWriteBitmapToData( hbitmap, (BYTE **)&data, &dsize );
    }

    if( ok ) {
        ok = WRStripBitmapFileHeader( (BYTE **)&data, &dsize );
    }

    if( ok ) {
        ok = WREGetAndPasteBitmap( fmt, data, dsize );
    }

    if( !ok ) {
        if( data != NULL ) {
            WREMemFree( data );
        }
    }

    return( ok );
}

void WREFiniClipboard( void )
{
    if( WPrivateFormat != (HBITMAP)NULL ) {
        DeleteObject( WPrivateFormat );
    }
    WRForgetBitmapName();
}

Bool WRERegisterClipFormats( HINSTANCE inst )
{
    Bool        ok;
    int         i;

    WPrivateFormat = LoadBitmap( inst, "PrivateFmt" );
    ok = (WPrivateFormat != (HBITMAP)NULL);

    for( i = 0; ok && WREClipFormats[i].fmt_name != NULL; i++ ) {
        WREClipFormats[i].fmt = RegisterClipboardFormat( WREClipFormats[i].fmt_name );
        ok = (WREClipFormats[i].fmt != 0);
    }

    return( ok );
}

void WRESetCopyMenuItem( HWND main )
{
    int                 enable;
    WRECurrentResInfo   curr;
    HMENU               hmenu;

    hmenu = GetMenu( main );
    WREGetCurrentResource( &curr );
    enable = MF_GRAYED;
    if( curr.info != NULL ) {
        if( curr.info->current_type != 0 && curr.info->current_type != (uint_16)RT_STRING ) {
            enable = MF_ENABLED;
        }
    }
    EnableMenuItem( hmenu, IDM_CUT, enable );
    EnableMenuItem( hmenu, IDM_COPY, enable );
}

void WRESetPasteMenuItem( HWND main )
{
    HMENU       hmenu;
    int         enable;
    int         i;

    hmenu = GetMenu( main );
    enable = MF_GRAYED;

    if( OpenClipboard( main ) ) {
        for( i = 0; WREClipFormats[i].type != NULL; i++ ) {
            if( IsClipboardFormatAvailable( WREClipFormats[i].fmt ) ) {
                enable = MF_ENABLED;
                break;
            }
        }
        CloseClipboard();
    }

    EnableMenuItem( hmenu, IDM_PASTE, enable );
}

WREClipData *WRECreateClipData( WRECurrentResInfo *curr )
{
    WREClipData *cdata;
    uint_32     cdata_size;
    BYTE        *rdata;
    uint_32     rdata_size;
    void        *name;
    uint_32     name_size;
    uint_16     type;
    Bool        ok;

    cdata = NULL;
    rdata = NULL;
    name = NULL;

    ok = (curr != NULL && curr->type != NULL && curr->res != NULL && curr->lang != NULL);

    if( ok ) {
        type = 0;
        if( !curr->type->Info.TypeName.IsName ) {
            type = curr->type->Info.TypeName.ID.Num;
        }
        ok = (type != 0);
    }

    if( ok ) {
        ok = WRWResID2Mem( &curr->res->Info.ResName, &name, &name_size,
                           curr->info->is32bit );
    }

    if( ok ) {
        if( type == (long)RT_GROUP_ICON ) {
            ok = WRECreateIconDataFromGroup( curr, &rdata, &rdata_size );
        } else if( type == (long)RT_GROUP_CURSOR ) {
            ok = WRECreateCursorDataFromGroup( curr, &rdata, &rdata_size );
        } else {
            rdata = WREGetCurrentResData( curr );
            rdata_size = curr->lang->Info.Length;
            ok = (rdata != NULL && rdata_size != 0);
        }
    }

    if( ok ) {
        cdata_size = sizeof( WREClipData ) + name_size + rdata_size - 1;
        cdata = (WREClipData *)WREMemAlloc( cdata_size );
        ok = (cdata != NULL);
    }

    if( ok ) {
        cdata->clip_size = cdata_size;
        cdata->data_size = rdata_size;
        cdata->data_offset = cdata_size - rdata_size;
        cdata->type = type;
        cdata->memflags = curr->lang->Info.MemoryFlags;
        cdata->is32bit = curr->info->is32bit;
        memcpy( &cdata->name[0], name, name_size );
        memcpy( &cdata->name[name_size], rdata, rdata_size );
    } else {
        if( cdata != NULL ) {
            WREMemFree( cdata );
            cdata = NULL;
        }
    }

    if( rdata != NULL ) {
        WREMemFree( rdata );
    }

    if( name != NULL ) {
        WREMemFree( name );
    }

    return( cdata );
}

Bool WREClipBitmap( WRECurrentResInfo *curr, HWND main )
{
    HBITMAP     hbitmap;
    BYTE        *data;
    uint_32     dsize;
    Bool        ok;

    data = NULL;
    hbitmap = (HBITMAP)NULL;

    ok = (curr != NULL && curr->type != NULL && curr->res != NULL && curr->lang != NULL);

    if( ok ) {
        data = (BYTE *)WREGetCurrentResData( curr );
        ok = (data != NULL);
    }

    if( ok ) {
        dsize = curr->lang->Info.Length;
        ok = WREAddBitmapFileHeader( &data, &dsize );
    }

    if( ok ) {
        hbitmap = WRBitmapFromData( data, NULL );
        ok = (hbitmap != (HBITMAP)NULL);
    }

    if( ok ) {
        ok = OpenClipboard( main );
    }

    if( ok ) {
        EmptyClipboard();
        SetClipboardData( CF_BITMAP, hbitmap );
        CloseClipboard();
        hbitmap = (HBITMAP)NULL;
    }

    if( ok ) {
        WRRememberBitmapName( &curr->res->Info.ResName );
    }

    if( data != NULL ) {
        WREMemFree( data );
    }

    return( ok );
}

Bool WREClipResource( WRECurrentResInfo *curr, HWND main, UINT fmt )
{
    WREClipData *cdata;
    HGLOBAL     hmem;
    BYTE        *mem;
    Bool        ok;
    HINSTANCE   inst;

    cdata = NULL;
    mem = NULL;
    hmem = (HGLOBAL)NULL;
    ok = (curr != NULL && fmt != 0);

    if( ok ) {
        cdata = WRECreateClipData( curr );
        ok = (cdata != NULL);
    }

    if( ok ) {
        hmem = GlobalAlloc( GMEM_MOVEABLE, cdata->clip_size );
        ok = (hmem != (HGLOBAL)NULL);
    }

    if( ok ) {
        mem = GlobalLock( hmem );
        ok = (mem != NULL);
    }

    if( ok ) {
        memcpy( mem, cdata, cdata->clip_size );
        GlobalUnlock( hmem );
        mem = NULL;
        ok = OpenClipboard( main );
    }

    if( ok ) {
        EmptyClipboard();
        SetClipboardData( fmt, hmem );
        SetClipboardData( CF_DSPBITMAP, WPrivateFormat );
        inst = WREGetAppInstance();
        WPrivateFormat = LoadBitmap( inst, "PrivateFmt" );
        CloseClipboard();
        hmem = (HGLOBAL)NULL;
        ok = (WPrivateFormat != (HBITMAP)NULL);
    }

    if( hmem != (HGLOBAL)NULL ) {
        GlobalFree( hmem );
    }

    if( cdata != NULL ) {
        WREMemFree( cdata );
    }

    return( ok );
}

Bool WREClipCurrentResource( HWND main, Bool cut )
{
    WRECurrentResInfo   curr;
    WREClipFormat       *fmt;
    uint_16             type;
    Bool                ok;

    WREGetCurrentResource( &curr );

    ok = (curr.info != NULL && curr.type != NULL);

    if( ok ) {
        type = 0;
        if( !curr.type->Info.TypeName.IsName ) {
            type = curr.type->Info.TypeName.ID.Num;
        }
        ok = (type != 0);
    }

    if( ok ) {
        fmt = WREFindClipFormatFromType( type );
        ok = (fmt != NULL);
    }

    if( ok ) {
        if( curr.info->current_type == (uint_16)RT_BITMAP ) {
            ok = WREClipBitmap( &curr, main );
        } else if( curr.info->current_type == (uint_16)RT_STRING ||
                   curr.info->current_type == 0 ) {
            ok = FALSE;
        } else {
            ok = WREClipResource( &curr, main, fmt->fmt );
        }
    }

    if( ok ) {
        if( cut ) {
            ok = WREDeleteCurrResource( TRUE );
        }
    }

    return( ok );
}

Bool WREPasteResource( HWND main )
{
    uint_16             type;
    WREClipFormat       *fmt;
    Bool                clipbd_open;
    Bool                ok;

    type = 0;
    clipbd_open = FALSE;
    ok = OpenClipboard( main );

    if( ok ) {
        clipbd_open = TRUE;
        fmt = WREGetClipFormat();
        ok = (fmt != NULL);
    }

    if( ok ) {
        if( fmt->fmt == CF_BITMAP ) {
            ok = WREGetAndPasteHBITMAP( fmt );
        } else if( fmt->fmt == CF_DIB ) {
            ok = WREGetAndPasteDIB( fmt );
        } else if( fmt->type == (long)RT_GROUP_ICON ) {
            ok = WREGetAndPasteIconOrCursor( fmt );
        } else if( fmt->type == (long)RT_GROUP_CURSOR ) {
            ok = WREGetAndPasteIconOrCursor( fmt );
        } else {
            ok = WREGetAndPasteResource( fmt );
        }
    }

    if( clipbd_open ) {
        CloseClipboard();
    }

    return( ok );
}

Bool WREQueryPasteReplace( WResID *name, uint_16 type, Bool *replace )
{
    WREPasteData        pdata;
    HWND                dialog_owner;
    DLGPROC             proc_inst;
    HINSTANCE           inst;
    int                 ret;

    if( name == NULL || type == 0 || replace == NULL ) {
        return( FALSE );
    }

    pdata.ret = 0;
    pdata.type = type;
    pdata.name = name;
    *replace = FALSE;
    dialog_owner  = WREGetMainWindowHandle();
    inst = WREGetAppInstance();
    proc_inst = (DLGPROC)MakeProcInstance( (FARPROC)WREResPasteProc, inst );

    ret = JDialogBoxParam( inst, "WREPaste", dialog_owner, proc_inst, (LPARAM)&pdata );

    FreeProcInstance( (FARPROC)proc_inst );

    if( ret == -1 || ret == IDCANCEL ) {
        return( FALSE );
    }

    if( ret == IDM_PASTE_REPLACE ) {
        *replace = TRUE;
    }

    return( TRUE );
}

static void WRESetPasteInfo( HWND hDlg, WREPasteData *pdata )
{
    WRETypeName *tn;
    char        *text;

    tn = WREGetTypeNameFromRT( pdata->type );
    if( tn != NULL ) {
        text = WREAllocRCString( tn->name );
        WRESetEditWithStr( GetDlgItem( hDlg, IDM_PASTE_TYPE ), text );
        if( text != NULL ) {
            WREFreeRCString( text );
        }
    }
    WRESetEditWithWResID( GetDlgItem( hDlg, IDM_PASTE_NAME ), pdata->name );
}

BOOL CALLBACK WREResPasteProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WREPasteData        *pdata;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {
    case WM_INITDIALOG:
        pdata = (WREPasteData *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)pdata );
        WRESetPasteInfo( hDlg, pdata );
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRECtl3dColorChange();
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDM_PASTE_RENAME:
        case IDM_PASTE_REPLACE:
            pdata = (WREPasteData *)GET_DLGDATA( hDlg );
            EndDialog( hDlg, LOWORD( wParam ) );
            ret = TRUE;
            break;

        case IDCANCEL:
            EndDialog( hDlg, IDCANCEL );
            ret = TRUE;
            break;
        }
    }

    return( ret );
}
