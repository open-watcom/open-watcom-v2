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


#include "wdeglbl.h"
#include "wrdll.h"
#include "wderesin.h"
#include "wdecsize.h"
#include "wde_wres.h"
#include "wdei2mem.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WdeDialogBoxHeader  *WdeMem2DialogBoxHeader( uint_8 **data, bool );
static WdeDialogBoxHeader  *WdeMem2DialogBoxExHeader( uint_8 **data );
static WdeDialogBoxControl *WdeMem2DialogBoxControl( uint_8 **data, bool, bool );
static ResNameOrOrdinal    *WdeMem2NameOrOrdinal( uint_8 **data, bool );
static ControlClass        *WdeMem2ControlClass( uint_8 **data, bool );
static char                *WdeMem2String( uint_8 **data, bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static int WdeStringToMem( char *string, uint_8 use_unicode, uint_8 *mem )
{
    int         len;
    uint_16     *data16;

    if( string == NULL ) {
        string = "";
    }


    if( use_unicode ) {
        len = 0;
        if( WRmbcs2unicode( string, NULL, &len ) ) {
            if( !WRmbcs2unicodeBuf( string, (char *)mem, len ) ) {
                len = 0;
            }
        }
        if( len == 0 ) {
            data16 = (uint_16 *)mem;
            data16[0] = 0;
            len = 2;
        }
    } else {
        len = strlen( string ) + 1;
        memcpy( mem, string, len );
    }

    return( len );
}

static int WdeNameOrOrdToMem( ResNameOrOrdinal *name, uint_8 use_unicode, uint_8 *mem )
{
    int         num;
    uint_16     *data16;

    if( name == NULL ) {
        num = WdeStringToMem( "", use_unicode, mem );
    } else {
        if( name->ord.fFlag == 0xff ) {
            if( use_unicode ) {
                data16 = (uint_16 *)mem;
                data16[0] = 0xffff;
                data16[1] = name->ord.wOrdinalID;
                num = sizeof( uint_16 ) * 2;
            } else {
                memcpy( mem, &name->ord.fFlag, sizeof( uint_8 ) + sizeof( uint_16 ) );
                num = sizeof( uint_8 ) + sizeof( uint_16 );
            }
        } else {
            num = WdeStringToMem( name->name, use_unicode, mem );
        }
    }

    return( num );
}

static int WdeDialogBoxHeaderToMem( WdeDialogBoxHeader *head, uint_8 *mem )
{
    bool                ok;
    int                 pos, size;
    uint_16             pointsize;
    DialogBoxHeader32   h32;
    DialogBoxHeader     h16;
    void                *d;

    ok = (head != NULL && mem != NULL);

    if( ok ) {
        if( head->is32bit ) {
            pos = offsetof( DialogBoxHeader32, MenuName );
            h32.Style = head->Style;
            h32.ExtendedStyle = head->ExtendedStyle;
            h32.NumOfItems = head->NumOfItems;
            h32.Size = head->Size;
            d = &h32;
        } else {
            pos = offsetof( DialogBoxHeader, MenuName );
            h16.Style = head->Style;
            h16.NumOfItems = head->NumOfItems;
            h16.Size = head->Size;
            d = &h16;
        }
        ok = (pos != 0);
    }

    if( ok ) {
        memcpy( mem, d, pos );
        size = WdeNameOrOrdToMem( GETHDR_MENUNAME( head ), head->is32bit, mem + pos );
        ok = (size != 0);
    }

    if( ok ) {
        pos += size;
        size = WdeNameOrOrdToMem( GETHDR_CLASSNAME( head ), head->is32bit, mem + pos );
        ok = (size != 0);
    }

    if( ok ) {
        pos += size;
        size = WdeStringToMem( GETHDR_CAPTION( head ), head->is32bit, mem + pos );
        ok = (size != 0);
    }

    if( ok && (GETHDR_STYLE( head ) & DS_SETFONT) ) {
        pos += size;
        pointsize = GETHDR_POINTSIZE( head );
        memcpy( mem + pos, &pointsize, sizeof( uint_16 ) );
        pos += sizeof( uint_16 );
        size = WdeStringToMem( GETHDR_FONTNAME( head ), head->is32bit, mem + pos );
        ok = (size != 0);
    }

    if( ok ) {
        pos += size;
    } else {
        pos = 0;
    }

    return( pos );
}

static int WdeDialogBoxExHeaderToMem( WdeDialogBoxHeader *dhd, uint_8 *mem )
{
    int       pos, size;
    uint_16   miscbytes[2] = { 0x0001, 0xFFFF };

    /* copy the miscellaneous two WORDs 01 00 FF FF */
    memcpy( mem, miscbytes, sizeof( miscbytes ) );
    pos = sizeof( miscbytes );

    memcpy( mem + pos, &dhd->HelpId, sizeof( uint_32 ) );
    pos += sizeof( uint_32 );

    memcpy( mem + pos, &dhd->ExtendedStyle, sizeof( uint_32 ) );
    pos += sizeof( uint_32 );

    memcpy( mem + pos, &dhd->Style, sizeof( uint_32 ) );
    pos += sizeof( uint_32 );

    memcpy( mem + pos, &dhd->NumOfItems, sizeof( uint_16 ) );
    pos += sizeof( uint_16 );

    memcpy( mem + pos, &dhd->Size, sizeof( DialogSizeInfo ) );
    pos += sizeof( DialogSizeInfo );

    size = WdeNameOrOrdToMem( GETHDR_MENUNAME( dhd ), dhd->is32bit, mem + pos );
    pos += size;

    size = WdeNameOrOrdToMem( GETHDR_CLASSNAME( dhd ), dhd->is32bit, mem + pos );
    pos += size;

    size = WdeStringToMem( GETHDR_CAPTION( dhd ), dhd->is32bit, mem + pos );
    pos += size;

    if( GETHDR_STYLE( dhd ) & DS_SETFONT ) {
        memcpy( mem + pos, &dhd->PointSize, sizeof( uint_16 ) );
        pos += sizeof( uint_16 );

        memcpy( mem + pos, &dhd->FontWeight, sizeof( uint_16 ) );
        pos += sizeof( uint_16 );

        memcpy( mem + pos, &dhd->FontItalic, sizeof( uint_16 ) );
        pos += sizeof( uint_16 );

        size = WdeStringToMem( dhd->FontName, dhd->is32bit, mem + pos );
        pos += size;
    }

    return( pos );
}

static int WdeDialogBoxControlToMem( WdeDialogBoxControl *control,
                                     uint_8 *mem, bool is32bit, bool is32bitEx )
{
    DialogBoxControl        *c16;
    DialogBoxControl32      *c32;
    DialogBoxExControl32    *c32ex;
    ControlClass            *cclass;
    int                     pos, size;
    uint_16                 *data16;
    bool                    ok;

    ok = (control != NULL && mem != NULL);

    if( ok ) {
        if( is32bit ) {
            if( is32bitEx ) {
                pos = offsetof( DialogBoxExControl32, ClassID );
                c32ex = (DialogBoxExControl32 *)mem;
                c32ex->HelpId = control->HelpId;
                c32ex->Style = control->Style;
                c32ex->ExtendedStyle = control->ExtendedStyle;
                c32ex->Size = control->Size;
                c32ex->ID = control->ID;
            } else {
                pos = offsetof( DialogBoxControl32, ClassID );
                c32 = (DialogBoxControl32 *)mem;
                c32->Style = control->Style;
                c32->ExtendedStyle = control->ExtendedStyle;
                c32->Size = control->Size;
                c32->ID = control->ID;
            }
        } else {
            pos = offsetof( DialogBoxControl, ClassID );
            c16 = (DialogBoxControl *)mem;
            c16->Size = control->Size;
            c16->ID = control->ID;
            c16->Style = control->Style;
        }

        cclass = GETCTL_CLASSID( control );
        if( cclass->Class & 0x80 ) {
            if( is32bit ) {
                data16 = (uint_16 *)(mem + pos);
                data16[0] = 0xffff;
                data16[1] = (uint_16)cclass->Class;
                size = sizeof( uint_16 ) * 2;
            } else {
                memcpy( mem + pos, &cclass->Class, sizeof( uint_8 ) );
                size = sizeof( uint_8 );
            }
        } else {
            size = WdeStringToMem( cclass->ClassName, is32bit, mem + pos );
            ok = (size != 0);
        }
    }

    if( ok ) {
        pos += size;
        size = WdeNameOrOrdToMem( GETCTL_TEXT( control ), is32bit, mem + pos );
        ok = (size != 0);
    }

    if( ok ) {
        pos += size;
        if( is32bit ) {
            memcpy( mem + pos, &control->ExtraBytes, sizeof( uint_16 ) );
            size = sizeof( uint_16 );
        } else {
            uint_8 eb = control->ExtraBytes;
            memcpy( mem + pos, &eb, sizeof( uint_8 ) );
            size = sizeof( uint_8 );
        }
    }

    if ( ok ) {
        pos += size;
    } else {
        pos = 0;
    }

    return( pos );
}


bool WdeDBI2Mem( WdeDialogBoxInfo *info, uint_8 **mem, uint_32 *size )
{
    bool                ok;
    uint_32             pos, memsize, sz, pad;
    LIST                *l;
    WdeDialogBoxControl *ci;
    bool                is32bit;
    bool                is32bitEx;

    is32bit = info->dialog_header->is32bit;
    is32bitEx = info->dialog_header->is32bitEx;

    ok = (info != NULL && mem != NULL && size != NULL);

    if( ok ) {
        *mem = NULL;
        memsize = (int)WdeCalcSizeOfWdeDialogBoxInfo( info );
        ok = (memsize != 0);
    }

    if( ok ) {
        *mem = WRMemAlloc( memsize );
        ok = (*mem != NULL);
    }

    if( ok ) {
        if( is32bit && is32bitEx ) {
            pos = WdeDialogBoxExHeaderToMem( info->dialog_header, *mem );
        } else {
            pos = WdeDialogBoxHeaderToMem( info->dialog_header, *mem );
        }
        ok = (pos != 0);
    }

    if( ok ) {
        for( l = info->control_list; l != NULL; l = ListNext( l ) ) {
            ci = ListElement( l );
            if( ci == NULL ) {
                ok = FALSE;
                break;
            }
            if( is32bit ) {
                pad = CALC_PAD( pos, sizeof( uint_32 ) );
                memset( *mem + pos, 0, pad );
                pos += pad;
            }
            sz = WdeDialogBoxControlToMem( ci, *mem + pos, is32bit, is32bitEx );
            if( sz == 0 ) {
                ok = FALSE;
                break;
            }
            pos += sz;
        }
    }

    if( is32bit ) {
        pad = CALC_PAD( pos, sizeof( uint_32 ) );
        memset( *mem + pos, 0, pad );
        pos += pad;
    }

    if( ok ) {
        ok = (pos == memsize);
        if( ok ) {
            *size = memsize;
        }
    } else {
        if( mem != NULL && *mem != NULL ) {
            WRMemFree( *mem );
            *mem = NULL;
        }
    }

    return( ok );
}

WdeDialogBoxInfo *WdeMem2DBI( uint_8 *data, uint_32 size, bool is32bit )
{
    WdeDialogBoxInfo    *dbi;
    WdeDialogBoxControl *control;
    LIST                *prev_control;
    int                 index, pad;
    uint_8              *d;
    bool                ok;
    bool                is32bitEx = FALSE;
    uint_16             signa[2];

    dbi = NULL;

    ok = (data != NULL && size != 0);

    if( ok ) {
        d = data;
        dbi = (WdeDialogBoxInfo *)WRMemAlloc( sizeof( WdeDialogBoxInfo ) );
        ok = (dbi != NULL);
    }


    if( ok ) {
        /* check if the dialog is extended by testing for the signature */
        memcpy( signa, data, sizeof( signa ) );
        is32bitEx = (signa[0] == 0x0001 && signa[1] == 0xFFFF);

        dbi->control_list = NULL;
        dbi->MemoryFlags = 0;
        if( is32bitEx ) {
            dbi->dialog_header = WdeMem2DialogBoxExHeader( &data );
        } else {
            dbi->dialog_header = WdeMem2DialogBoxHeader( &data, is32bit );
        }
        ok = (dbi->dialog_header != NULL);
    }

    if( ok ) {
        prev_control = NULL;
        for( index = 0; index < GETHDR_NUMITEMS( dbi->dialog_header ); index++ ) {
            if( is32bit ) {
                pad = CALC_PAD( data - d, sizeof( uint_32 ) );
                data += pad;
            }
            control = WdeMem2DialogBoxControl( &data, is32bit, is32bitEx );
            if( control == NULL ) {
                ok = FALSE;
                break;
            }
            if( prev_control == NULL ) {
                ListAddElt( &dbi->control_list, (void *)control );
                prev_control = dbi->control_list;
            } else {
                ListInsertElt( prev_control, (void *)control );
                prev_control = ListNext( prev_control );
            }
        }
    }

    if( ok ) {
        ok = (size >= data - d);
    }

    if( !ok ) {
        if( dbi != NULL ) {
            WdeFreeDialogBoxInfo( dbi );
            dbi = NULL;
        }
    }

    return( dbi );
}

WdeDialogBoxHeader *WdeMem2DialogBoxHeader( uint_8 **data, bool is32bit )
{
    DialogBoxHeader32   *h32;
    DialogBoxHeader     *h16;
    WdeDialogBoxHeader  *dbh;
    int                 fixedbytes;
    bool                ok;

    dbh = NULL;

    ok = (data != NULL && *data != NULL);

    if( ok ) {
        dbh = WdeAllocDialogBoxHeader();
        ok = (dbh != NULL);
    }

    if( ok ) {
        dbh->symbol = NULL;
        dbh->helpsymbol = NULL;
        dbh->is32bit = is32bit;
        dbh->is32bitEx = FALSE;
        if( is32bit ) {
            fixedbytes = offsetof( DialogBoxHeader32, MenuName );
            h32 = (DialogBoxHeader32 *)*data;
            dbh->Style = h32->Style;
            dbh->ExtendedStyle = h32->ExtendedStyle;
            dbh->NumOfItems = h32->NumOfItems;
            dbh->Size = h32->Size;
        } else {
            fixedbytes = offsetof( DialogBoxHeader, MenuName );
            h16 = (DialogBoxHeader *)*data;
            dbh->Style = h16->Style;
            dbh->NumOfItems = h16->NumOfItems;
            dbh->Size = h16->Size;
        }
        *data += fixedbytes;
        SETHDR_MENUNAME( dbh, WdeMem2NameOrOrdinal( data, is32bit ) );
        ok = (GETHDR_MENUNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CLASSNAME( dbh, WdeMem2NameOrOrdinal( data, is32bit ) );
        ok = (GETHDR_CLASSNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CAPTION( dbh, WdeMem2String( data, is32bit ) );
        ok = (GETHDR_CAPTION( dbh ) != NULL);
    }

    if( ok ) {
        if( GETHDR_STYLE( dbh ) & DS_SETFONT ) {
            SETHDR_POINTSIZE( dbh, *(uint_16 *)*data );
            *data += sizeof( uint_16 );
            SETHDR_FONTNAME( dbh, WdeMem2String( data, is32bit ) );
            ok = (GETHDR_FONTNAME( dbh ) != NULL);
        } else {
            SETHDR_POINTSIZE( dbh, 0 );
            SETHDR_FONTNAME( dbh, NULL );
        }
    }

    if( !ok ) {
        if( dbh != NULL ) {
            WdeFreeDialogBoxHeader( &dbh );
        }
    }

    return( dbh );
}

WdeDialogBoxHeader *WdeMem2DialogBoxExHeader( uint_8 **data )
{
    uint_16             miscbytes[2] = { 0x0001, 0xFFFF };
    WdeDialogBoxHeader  *dbh;
    bool                ok;

    dbh = NULL;

    if ( data == NULL || *data == NULL ) {
        return( NULL );
    }

    dbh = WdeAllocDialogBoxHeader();
    if ( dbh == NULL ) {
        return( NULL );
    }

    dbh->is32bit = TRUE;
    dbh->is32bitEx = TRUE;

    /* skip the miscellaneous two WORDs 01 00 FF FF */
    *data += sizeof( miscbytes );

    memcpy( &dbh->HelpId, *data, sizeof( uint_32 ) );
    *data += sizeof( uint_32 );

    memcpy( &dbh->ExtendedStyle, *data, sizeof( uint_32 ) );
    *data += sizeof( uint_32 );

    memcpy( &dbh->Style, *data, sizeof( uint_32 ) );
    *data += sizeof( uint_32 );

    memcpy( &dbh->NumOfItems, *data, sizeof( uint_16 ) );
    *data += sizeof( uint_16 );

    memcpy( &dbh->Size, *data, sizeof( DialogSizeInfo ) );
    *data += sizeof( DialogSizeInfo );

    SETHDR_MENUNAME( dbh, WdeMem2NameOrOrdinal( data, TRUE ) );
    ok = (GETHDR_MENUNAME( dbh ) != NULL);

    if( ok ) {
        SETHDR_CLASSNAME( dbh, WdeMem2NameOrOrdinal( data, TRUE ) );
        ok = (GETHDR_CLASSNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CAPTION( dbh, WdeMem2String( data, TRUE ) );
        ok = (GETHDR_CAPTION( dbh ) != NULL);
    }

    if( ok ) {
        if( GETHDR_STYLE( dbh ) & DS_SETFONT ) {
            dbh->FontWeightDefined = TRUE;
            dbh->FontItalicDefined = TRUE;
            SETHDR_POINTSIZE( dbh, *(uint_16 *)*data );
            *data += sizeof( uint_16 );
            SETHDR_FONTWEIGHT( dbh, *(uint_16 *)*data );
            *data += sizeof( uint_16 );
            SETHDR_FONTITALIC( dbh, *(uint_16 *)*data );
            *data += sizeof( uint_16 );
            SETHDR_FONTNAME( dbh, WdeMem2String( data, TRUE ) );
            ok = (GETHDR_FONTNAME( dbh ) != NULL);
        }
    }

    if( !ok ) {
        if( dbh != NULL ) {
            WdeFreeDialogBoxHeader( &dbh );
        }
    }

    return( dbh );
}

WdeDialogBoxControl *WdeMem2DialogBoxControl( uint_8 **data, bool is32bit, bool is32bitEx )
{
    DialogBoxControl            *c16;
    DialogBoxControl32          *c32;
    DialogBoxExControl32        *c32ex;
    WdeDialogBoxControl         *dbc;
    int                         fixedbytes;
    bool                        ok;

    dbc = NULL;

    ok = (data != NULL && *data != NULL);

    if( ok ) {
        dbc = WdeAllocDialogBoxControl();
        ok = (dbc != NULL);
    }

    if( ok ) {
        if( is32bit ) {
            if( is32bitEx ) {
                fixedbytes = offsetof( DialogBoxExControl32, ClassID );
                c32ex = (DialogBoxExControl32 *)*data;
                dbc->HelpId = c32ex->HelpId;
                dbc->Style = c32ex->Style;
                dbc->ExtendedStyle = c32ex->ExtendedStyle;
                dbc->Size = c32ex->Size;
                dbc->ID = c32ex->ID;
            } else {
                fixedbytes = offsetof( DialogBoxControl32, ClassID );
                c32 = (DialogBoxControl32 *)*data;
                dbc->Style = c32->Style;
                dbc->ExtendedStyle = c32->ExtendedStyle;
                dbc->Size = c32->Size;
                dbc->ID = c32->ID;
            }
        } else {
            fixedbytes = offsetof( DialogBoxControl, ClassID );
            c16 = (DialogBoxControl *)*data;
            dbc->Size = c16->Size;
            dbc->ID = c16->ID;
            dbc->Style = c16->Style;
        }

        *data += fixedbytes;
        SETCTL_CLASSID( dbc, WdeMem2ControlClass( data, is32bit ) );
        ok = (GETCTL_CLASSID( dbc ) != NULL);
    }

    if( ok ) {
        SETCTL_TEXT( dbc, WdeMem2NameOrOrdinal( data, is32bit ) );
        ok = (GETCTL_TEXT( dbc ) != NULL);
    }

    if( ok ) {
        if( is32bit ) {
            if( is32bitEx ) {
                dbc->ExtraBytes = *(uint_16 *)*data;
            } else {
                dbc->ExtraBytes = *(uint_16 *)*data;
            }
            *data += sizeof( uint_16 );
        } else {
            dbc->ExtraBytes = (uint_8)((*data)[0]);
            *data += sizeof( uint_8 );
        }
    } else {
        if( dbc != NULL ) {
            WdeFreeDialogBoxControl( &dbc );
        }
    }

    return( dbc );
}


ResNameOrOrdinal *WdeMem2NameOrOrdinal( uint_8 **data, bool is32bit )
{
    ResNameOrOrdinal    *new;
    int                 size;

    if( data == NULL || *data == NULL ) {
        return( NULL );
    }

    new = WRMem2NameOrOrdinal( *data, is32bit );
    if( new == NULL ) {
        return( NULL );
    }

    size = WdeCalcSizeOfResNameOrOrdinal( new, is32bit );
    if( size == 0 ) {
        return( NULL );
    }

    *data += size;

    return( new );
}

ControlClass *WdeMem2ControlClass( uint_8 **_data, bool is32bit )
{
    ControlClass        *new;
    uint_8              *data8;
    uint_16             *data16;
    int                 stringlen;
    int                 len;
    char                *data;

    if( _data == NULL || *_data == NULL ) {
        return( NULL );
    }
    data = (char *)*_data;
    stringlen = 0;
    len = sizeof( ControlClass );
    if( is32bit ) {
        data16 = (uint_16 *)data;
        if( *data16 != 0xffff ) {
            WRunicode2mbcs( data, NULL, &stringlen );
            len = stringlen;
        }
    } else {
        data8 = (uint_8 *)data;
        if( (*data8 & 0x80) == 0 ) {
            stringlen = strlen( data ) + 1;
            len = stringlen;
        }
    }

    new = (ControlClass *)WRMemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    if( stringlen == 0 ) {
        if( is32bit ) {
            new->Class = data16[1] & 0x00ff;
            len = sizeof( uint_16 ) * 2;
        } else {
            new->Class = data8[0];
        }
    } else {
        if( is32bit ) {
            WRunicode2mbcsBuf( data, (char *)new, len );
            len *= 2;
        } else {
            memcpy( new, data, len );
        }
    }

    *_data += len;

    return( new );
}

char *WdeMem2String( uint_8 **_data, bool is32bit )
{
    char        *new;
    int         len;
    char        *data;

    if( _data == NULL || *_data == NULL ) {
        return( NULL );
    }

    data = (char *)*_data;
    if( is32bit ) {
        WRunicode2mbcs( data, NULL, &len );
    } else {
        len = strlen( data ) + 1;
    }

    new = WRMemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    if( is32bit ) {
        WRunicode2mbcsBuf( data, (char *)new, len );
    } else {
        memcpy( new, data, len );
    }

    len = WRStrlen( data, is32bit ) + 1;
    if( is32bit ) {
        len++;
    }
    *_data += len;

    return( new );
}
