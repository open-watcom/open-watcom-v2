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

#define U8ToMem(m,v)        *(uint_8 *)(m) = (v);m+=sizeof(uint_8)
#define U16ToMem(m,v)       *(uint_16 *)(m) = (v);m+=sizeof(uint_16)
#define U32ToMem(m,v)       *(uint_32 *)(m) = (v);m+=sizeof(uint_32)

#define U8FromMem(m,v)      (v) = *(uint_8 *)(m);m+=sizeof(uint_8)
#define U16FromMem(m,v)     (v) = *(uint_16 *)(m);m+=sizeof(uint_16)
#define U32FromMem(m,v)     (v) = *(uint_32 *)(m);m+=sizeof(uint_32)

#define GetU8(m)            *(uint_8 *)(m)
#define GetU16(m)           *(uint_16 *)(m)
#define GetU32(m)           *(uint_32 *)(m)

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WdeDialogBoxHeader  *WdeMem2DialogBoxHeader( const uint_8 **data, bool, bool );
static WdeDialogBoxControl *WdeMem2DialogBoxControl( const uint_8 **data, bool, bool );
static ResNameOrOrdinal    *WdeMem2NameOrOrdinal( const uint_8 **data, bool );
static ControlClass        *WdeMem2ControlClass( const uint_8 **data, bool );
static char                *WdeMem2String( const uint_8 **data, bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static size_t WdeStringToMem( const char *string, uint_8 use_unicode, uint_8 *mem )
{
    size_t      size;

    if( string == NULL ) {
        string = "";
    }

    if( use_unicode ) {
        size = 0;
        if( WRmbcs2unicode( string, NULL, &size ) ) {
            if( !WRmbcs2unicodeBuf( string, (char *)mem, size ) ) {
                size = 0;
            }
        }
        if( size == 0 ) {
            U16ToMem( mem, 0 );
            size = sizeof( uint_16 );
        }
    } else {
        size = strlen( string ) + 1;
        memcpy( mem, string, size );
    }

    return( size );
}

static size_t WdeNameOrOrdToMem( ResNameOrOrdinal *name, uint_8 use_unicode, uint_8 *mem )
{
    size_t      size;

    if( name == NULL ) {
        size = WdeStringToMem( "", use_unicode, mem );
    } else {
        if( name->ord.fFlag == 0xff ) {
            if( use_unicode ) {
                U16ToMem( mem, (uint_16)-1 );
                U16ToMem( mem, name->ord.wOrdinalID );
                size = 2 * sizeof( uint_16 );
            } else {
                memcpy( mem, &name->ord.fFlag, sizeof( uint_8 ) + sizeof( uint_16 ) );
                size = sizeof( uint_8 ) + sizeof( uint_16 );
            }
        } else {
            size = WdeStringToMem( name->name, use_unicode, mem );
        }
    }
    return( size );
}

static size_t WdeDialogBoxHeaderToMem( WdeDialogBoxHeader *head, uint_8 *mem )
{
    bool                ok;
    size_t              size;
    uint_8              *start;

    ok = (head != NULL && mem != NULL);
    start = NULL;

    if( ok ) {
        start = mem;
        if( head->is32bitEx ) {
            /* copy the miscellaneous two WORDs 01 00 FF FF */
            U16ToMem( mem, 1 );
            U16ToMem( mem, (uint_16)-1 );
            U32ToMem( mem, GETHDR_HELPID( head ) );
            U32ToMem( mem, GETHDR_EXSTYLE( head ) );
            U32ToMem( mem, GETHDR_STYLE( head ) );
        } else if( head->is32bit ) {
            U32ToMem( mem, GETHDR_STYLE( head ) );
            U32ToMem( mem, GETHDR_EXSTYLE( head ) );
        } else {
            U32ToMem( mem, GETHDR_STYLE( head ) );
        }
        if( head->is32bit ) {
            U16ToMem( mem, GETHDR_NUMITEMS( head ) );
        } else {
            U8ToMem( mem, GETHDR_NUMITEMS( head ) );
        }
        U16ToMem( mem, GETHDR_SIZEX( head ) );
        U16ToMem( mem, GETHDR_SIZEY( head ) );
        U16ToMem( mem, GETHDR_SIZEW( head ) );
        U16ToMem( mem, GETHDR_SIZEH( head ) );
    }

    if( ok ) {
        size = WdeNameOrOrdToMem( GETHDR_MENUNAME( head ), head->is32bit, mem );
        mem += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WdeNameOrOrdToMem( GETHDR_CLASSNAME( head ), head->is32bit, mem );
        mem += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WdeStringToMem( GETHDR_CAPTION( head ), head->is32bit, mem );
        mem += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( GETHDR_STYLE( head ) & DS_SETFONT ) {
            U16ToMem( mem, GETHDR_POINTSIZE( head ) );
            if( head->is32bitEx ) {
                U16ToMem( mem, GETHDR_FONTWEIGHT( head ) );
                U8ToMem( mem, GETHDR_FONTITALIC( head ) );
                U8ToMem( mem, GETHDR_FONTCHARSET( head ) );
            }
            size = WdeStringToMem( GETHDR_FONTNAME( head ), head->is32bit, mem );
            mem += size;
            ok = (size != 0);
        }
    }

    if( ok )
        return( mem - start );
    return( 0 );
}

static size_t WdeDialogBoxControlToMem( WdeDialogBoxControl *control,
                                     uint_8 *mem, bool is32bit, bool is32bitEx )
{
    ControlClass            *cclass;
    size_t                  size;
    bool                    ok;
    uint_8                  *start;

    ok = (control != NULL && mem != NULL);
    start = NULL;

    if( ok ) {
        start = mem;
        if( is32bitEx ) {
            U32ToMem( mem, GETCTL_HELPID( control ) );
            U32ToMem( mem, GETCTL_EXSTYLE( control ) );
            U32ToMem( mem, GETCTL_STYLE( control ) );
        } else if( is32bit ) {
            U32ToMem( mem, GETCTL_STYLE( control ) );
            U32ToMem( mem, GETCTL_EXSTYLE( control ) );
        }
        U16ToMem( mem, GETCTL_SIZEX( control ) );
        U16ToMem( mem, GETCTL_SIZEY( control ) );
        U16ToMem( mem, GETCTL_SIZEW( control ) );
        U16ToMem( mem, GETCTL_SIZEH( control ) );
        if( is32bitEx ) {
            U32ToMem( mem, GETCTL_ID( control ) );
        } else {
            U16ToMem( mem, GETCTL_ID( control ) );
        }
        if( !is32bit ) {
            U32ToMem( mem, GETCTL_STYLE( control ) );
        }
        cclass = GETCTL_CLASSID( control );
        if( cclass->Class & 0x80 ) {
            if( is32bit ) {
                U16ToMem( mem, (uint_16)-1 );
                U16ToMem( mem, cclass->Class );
            } else {
                U8ToMem( mem, cclass->Class );
            }
        } else {
            size = WdeStringToMem( cclass->ClassName, is32bit, mem );
            mem += size;
            ok = (size != 0);
        }
    }

    if( ok ) {
        size = WdeNameOrOrdToMem( GETCTL_TEXT( control ), is32bit, mem );
        mem += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( is32bit ) {
            U16ToMem( mem, GETCTL_EXTRABYTES( control ) );
        } else {
            U8ToMem( mem, GETCTL_EXTRABYTES( control ) );
        }
    }

    if ( ok )
        return( mem - start );
    return( 0 );
}


bool WdeDBI2Mem( WdeDialogBoxInfo *info, uint_8 **mem, uint_32 *size )
{
    bool                ok;
    size_t              pos, memsize, sz, pad;
    LIST                *l;
    WdeDialogBoxControl *ci;
    bool                is32bit;
    bool                is32bitEx;

    ok = (info != NULL && mem != NULL && size != NULL);
    pos = 0;
    memsize = 0;

    is32bit = info->dialog_header->is32bit;
    is32bitEx = info->dialog_header->is32bitEx;

    if( ok ) {
        *mem = NULL;
        memsize = WdeCalcSizeOfWdeDialogBoxInfo( info );
        ok = (memsize != 0);
    }

    if( ok ) {
        *mem = WRMemAlloc( memsize );
        ok = (*mem != NULL);
    }

    if( ok ) {
        pos = WdeDialogBoxHeaderToMem( info->dialog_header, *mem );
        ok = (pos != 0);
    }

    if( ok ) {
        for( l = info->control_list; l != NULL; l = ListNext( l ) ) {
            ci = ListElement( l );
            if( ci == NULL ) {
                ok = false;
                break;
            }
            if( is32bit ) {
                pad = CALC_PAD( pos, sizeof( uint_32 ) );
                memset( *mem + pos, 0, pad );
                pos += pad;
            }
            sz = WdeDialogBoxControlToMem( ci, *mem + pos, is32bit, is32bitEx );
            if( sz == 0 ) {
                ok = false;
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
            *size = (uint_32)memsize;
        }
    } else {
        if( mem != NULL && *mem != NULL ) {
            WRMemFree( *mem );
            *mem = NULL;
        }
    }

    return( ok );
}

WdeDialogBoxInfo *WdeMem2DBI( const uint_8 *data, uint_32 size, bool is32bit )
{
    WdeDialogBoxInfo    *dbi;
    WdeDialogBoxControl *control;
    LIST                *prev_control;
    int                 index, pad;
    const uint_8        *start;
    bool                ok;
    bool                is32bitEx = false;
    uint_16             signa[2];

    dbi = NULL;
    start = NULL;

    ok = (data != NULL && size != 0);

    if( ok ) {
        dbi = (WdeDialogBoxInfo *)WRMemAlloc( sizeof( WdeDialogBoxInfo ) );
        ok = (dbi != NULL);
    }


    if( ok ) {
        start = data;

        /* check if the dialog is extended by testing for the signature */
        memcpy( signa, data, sizeof( signa ) );
        is32bitEx = (signa[0] == 0x0001 && signa[1] == 0xFFFF);

        dbi->control_list = NULL;
        dbi->MemoryFlags = 0;
        dbi->dialog_header = WdeMem2DialogBoxHeader( &data, is32bit, is32bitEx );
        ok = (dbi->dialog_header != NULL);
    }

    if( ok ) {
        prev_control = NULL;
        for( index = 0; index < GETHDR_NUMITEMS( dbi->dialog_header ); index++ ) {
            if( is32bit ) {
                pad = CALC_PAD( data - start, sizeof( uint_32 ) );
                data += pad;
            }
            control = WdeMem2DialogBoxControl( &data, is32bit, is32bitEx );
            if( control == NULL ) {
                ok = false;
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
        ok = (size >= data - start);
    }

    if( !ok ) {
        if( dbi != NULL ) {
            WdeFreeDialogBoxInfo( dbi );
            dbi = NULL;
        }
    }

    return( dbi );
}

WdeDialogBoxHeader *WdeMem2DialogBoxHeader( const uint_8 **data, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxHeader  *dbh;
    bool                ok;
    const uint_8        *mem;

    dbh = NULL;
    mem = NULL;

    ok = (data != NULL && *data != NULL);

    if( ok ) {
        dbh = WdeAllocDialogBoxHeader();
        ok = (dbh != NULL);
    }

    if( ok ) {
        mem = *data;
        dbh->symbol = NULL;
        dbh->helpsymbol = NULL;
        dbh->is32bit = is32bit;
        dbh->is32bitEx = is32bitEx;
        if( is32bitEx ) {
            /* skip the miscellaneous two WORDs 01 00 FF FF */
            mem += 4;
            U32FromMem( mem, GETHDR_HELPID( dbh ) );
            U32FromMem( mem, GETHDR_EXSTYLE( dbh ) );
            U32FromMem( mem, GETHDR_STYLE( dbh ) );
        } else if( is32bit ) {
            U32FromMem( mem, GETHDR_STYLE( dbh ) );
            U32FromMem( mem, GETHDR_EXSTYLE( dbh ) );
        } else {
            U32FromMem( mem, GETHDR_STYLE( dbh ) );
        }
        if( is32bit ) {
            U16FromMem( mem, GETHDR_NUMITEMS( dbh ) );
        } else {
            U8FromMem( mem, GETHDR_NUMITEMS( dbh ) );
        }
        U16FromMem( mem, GETHDR_SIZEX( dbh ) );
        U16FromMem( mem, GETHDR_SIZEY( dbh ) );
        U16FromMem( mem, GETHDR_SIZEW( dbh ) );
        U16FromMem( mem, GETHDR_SIZEH( dbh ) );

        SETHDR_MENUNAME( dbh, WdeMem2NameOrOrdinal( &mem, is32bit ) );
        ok = (GETHDR_MENUNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CLASSNAME( dbh, WdeMem2NameOrOrdinal( &mem, is32bit ) );
        ok = (GETHDR_CLASSNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CAPTION( dbh, WdeMem2String( &mem, is32bit ) );
        ok = (GETHDR_CAPTION( dbh ) != NULL);
    }

    if( ok ) {
        if( GETHDR_STYLE( dbh ) & DS_SETFONT ) {
            U16FromMem( mem, GETHDR_POINTSIZE( dbh ) );
            if( is32bitEx ) {
                U16FromMem( mem, GETHDR_FONTWEIGHT( dbh ) );
                U8FromMem( mem, GETHDR_FONTITALIC( dbh ) );
                U8FromMem( mem, GETHDR_FONTCHARSET( dbh ) );
            }
            SETHDR_FONTNAME( dbh, WdeMem2String( &mem, is32bit ) );
            ok = (GETHDR_FONTNAME( dbh ) != NULL);
        } else {
            SETHDR_POINTSIZE( dbh, 0 );
            SETHDR_FONTNAME( dbh, NULL );
        }
    }

    if( dbh != NULL ) {
        *data = mem;
        if( !ok ) {
            WdeFreeDialogBoxHeader( &dbh );
        }
    }

    return( dbh );
}

WdeDialogBoxControl *WdeMem2DialogBoxControl( const uint_8 **data, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxControl         *dbc;
    bool                        ok;
    const uint_8                *mem;

    dbc = NULL;
    mem = NULL;

    ok = (data != NULL && *data != NULL);

    if( ok ) {
        dbc = WdeAllocDialogBoxControl();
        ok = (dbc != NULL);
    }

    if( ok ) {
        mem = *data;
        if( is32bitEx ) {
            U32FromMem( mem, GETCTL_HELPID( dbc ) );
            U32FromMem( mem, GETCTL_EXSTYLE( dbc ) );
            U32FromMem( mem, GETCTL_STYLE( dbc ) );
        } else if( is32bit ) {
            U32FromMem( mem, GETCTL_STYLE( dbc ) );
            U32FromMem( mem, GETCTL_EXSTYLE( dbc ) );
        }
        U16FromMem( mem, GETCTL_SIZEX( dbc ) );
        U16FromMem( mem, GETCTL_SIZEY( dbc ) );
        U16FromMem( mem, GETCTL_SIZEW( dbc ) );
        U16FromMem( mem, GETCTL_SIZEH( dbc ) );
        if( is32bitEx ) {
            U32FromMem( mem, GETCTL_ID( dbc ) );
        } else {
            U16FromMem( mem, GETCTL_ID( dbc ) );
        }
        if( !is32bit ) {
            U32FromMem( mem, GETCTL_STYLE( dbc ) );
        }
        SETCTL_CLASSID( dbc, WdeMem2ControlClass( &mem, is32bit ) );
        ok = (GETCTL_CLASSID( dbc ) != NULL);
    }

    if( ok ) {
        SETCTL_TEXT( dbc, WdeMem2NameOrOrdinal( &mem, is32bit ) );
        ok = (GETCTL_TEXT( dbc ) != NULL);
    }

    if( ok ) {
        if( is32bit ) {
            U16FromMem( mem, GETCTL_EXTRABYTES( dbc ) );
        } else {
            U8FromMem( mem, GETCTL_EXTRABYTES( dbc ) );
        }
    }

    if( dbc != NULL ) {
        *data = mem;
        if( !ok ) {
            WdeFreeDialogBoxControl( &dbc );
        }
    }

    return( dbc );
}


ResNameOrOrdinal *WdeMem2NameOrOrdinal( const uint_8 **data, bool is32bit )
{
    ResNameOrOrdinal    *new;
    size_t              size;

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

ControlClass *WdeMem2ControlClass( const uint_8 **data, bool is32bit )
{
    ControlClass        *new;

    if( data == NULL || *data == NULL ) {
        return( NULL );
    }
    if( ( is32bit && GetU16( *data ) != 0xffff )
      || ( !is32bit && (GetU8( *data ) & 0x80) == 0 ) ) {
        return( (ControlClass *)WdeMem2String( data, is32bit ) );
    }

    new = (ControlClass *)WRMemAlloc( sizeof( ControlClass ) );
    if( new == NULL ) {
        return( NULL );
    }

    if( is32bit ) {
        new->Class = GetU16( *data + sizeof( uint_16 ) ) & 0x00ff;
        *data += 2 * sizeof( uint_16 );
    } else {
        new->Class = GetU8( *data );
        *data += 1;
    }

    return( new );
}

char *WdeMem2String( const uint_8 **data, bool is32bit )
{
    char        *new;
    size_t      size;
    const char  *mem;

    if( data == NULL || *data == NULL ) {
        return( NULL );
    }

    mem = (const char *)*data;
    if( is32bit ) {
        WRunicode2mbcs( mem, NULL, &size );
    } else {
        size = strlen( mem ) + 1;
    }

    new = WRMemAlloc( size );
    if( new == NULL ) {
        return( NULL );
    }

    if( is32bit ) {
        WRunicode2mbcsBuf( mem, (char *)new, size );
        size = WRStrlen32( mem ) + sizeof( uint_16 );
    } else {
        memcpy( new, mem, size );
    }

    *data += size;

    return( new );
}
