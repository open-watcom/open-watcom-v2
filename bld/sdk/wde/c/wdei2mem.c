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

#define PADDING_WRITE(m,s) { \
    size_t pad = CALC_PAD( (m) - (s), sizeof( uint_32 ) ); \
    memset( m, 0, pad ); \
    m += pad; \
}

#define PADDING_SET(m,s) { \
    size_t pad = CALC_PAD( (m) - (s), sizeof( uint_32 ) ); \
    m += pad; \
}

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WdeDialogBoxHeader  *WdeMem2DialogBoxHeader( const uint_8 **pdata, bool, bool );
static WdeDialogBoxControl *WdeMem2DialogBoxControl( const uint_8 **pdata, bool, bool );
static ResNameOrOrdinal    *WdeMem2NameOrOrdinal( const uint_8 **pdata, bool );
static ControlClass        *WdeMem2ControlClass( const uint_8 **pdata, bool );
static char                *WdeMem2String( const uint_8 **pdata, bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static size_t WdeString2Mem( const char *string, uint_8 use_unicode, uint_8 *data )
{
    size_t      size;

    if( string == NULL ) {
        string = "";
    }

    if( use_unicode ) {
        size = 0;
        if( WRmbcs2unicode( string, NULL, &size ) ) {
            if( !WRmbcs2unicodeBuf( string, (char *)data, size ) ) {
                size = 0;
            }
        }
        if( size == 0 ) {
            U16ToMem( data, 0 );
            size = sizeof( uint_16 );
        }
    } else {
        size = strlen( string ) + 1;
        memcpy( data, string, size );
    }

    return( size );
}

static size_t WdeNameOrOrd2Mem( ResNameOrOrdinal *name, uint_8 use_unicode, uint_8 *data )
{
    size_t      size;

    if( name == NULL ) {
        size = WdeString2Mem( "", use_unicode, data );
    } else {
        if( name->ord.fFlag == 0xff ) {
            if( use_unicode ) {
                U16ToMem( data, (uint_16)-1 );
                U16ToMem( data, name->ord.wOrdinalID );
                size = 2 * sizeof( uint_16 );
            } else {
                memcpy( data, &name->ord.fFlag, sizeof( uint_8 ) + sizeof( uint_16 ) );
                size = sizeof( uint_8 ) + sizeof( uint_16 );
            }
        } else {
            size = WdeString2Mem( name->name, use_unicode, data );
        }
    }
    return( size );
}

static size_t WdeDialogBoxHeader2Mem( WdeDialogBoxHeader *head, uint_8 *data )
{
    bool                ok;
    size_t              size;
    uint_8              *start;

    ok = (head != NULL && data != NULL);
    start = NULL;

    if( ok ) {
        start = data;
        if( head->is32bitEx ) {
            /* copy the miscellaneous two WORDs 0x0001, 0xFFFF */
            U16ToMem( data, 0x0001 );
            U16ToMem( data, 0xFFFF );
            U32ToMem( data, GETHDR_HELPID( head ) );
            U32ToMem( data, GETHDR_EXSTYLE( head ) );
            U32ToMem( data, GETHDR_STYLE( head ) );
        } else if( head->is32bit ) {
            U32ToMem( data, GETHDR_STYLE( head ) );
            U32ToMem( data, GETHDR_EXSTYLE( head ) );
        } else {
            U32ToMem( data, GETHDR_STYLE( head ) );
        }
        if( head->is32bit ) {
            U16ToMem( data, GETHDR_NUMITEMS( head ) );
        } else {
            U8ToMem( data, GETHDR_NUMITEMS( head ) );
        }
        U16ToMem( data, GETHDR_SIZEX( head ) );
        U16ToMem( data, GETHDR_SIZEY( head ) );
        U16ToMem( data, GETHDR_SIZEW( head ) );
        U16ToMem( data, GETHDR_SIZEH( head ) );
    }

    if( ok ) {
        size = WdeNameOrOrd2Mem( GETHDR_MENUNAME( head ), head->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WdeNameOrOrd2Mem( GETHDR_CLASSNAME( head ), head->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WdeString2Mem( GETHDR_CAPTION( head ), head->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( GETHDR_STYLE( head ) & DS_SETFONT ) {
            U16ToMem( data, GETHDR_FONTPOINTSIZE( head ) );
            if( head->is32bitEx ) {
                U16ToMem( data, GETHDR_FONTWEIGHT( head ) );
                U8ToMem( data, GETHDR_FONTITALIC( head ) );
                U8ToMem( data, GETHDR_FONTCHARSET( head ) );
            }
            size = WdeString2Mem( GETHDR_FONTFACENAME( head ), head->is32bit, data );
            data += size;
            ok = (size != 0);
        }
    }

    if( ok )
        return( data - start );
    return( 0 );
}

static size_t WdeDialogBoxControl2Mem( WdeDialogBoxControl *control,
                                     uint_8 *data, bool is32bit, bool is32bitEx )
{
    ControlClass            *cclass;
    size_t                  size;
    bool                    ok;
    uint_8                  *start;

    ok = (control != NULL && data != NULL);
    start = NULL;

    if( ok ) {
        start = data;
        if( is32bitEx ) {
            U32ToMem( data, GETCTL_HELPID( control ) );
            U32ToMem( data, GETCTL_EXSTYLE( control ) );
            U32ToMem( data, GETCTL_STYLE( control ) );
        } else if( is32bit ) {
            U32ToMem( data, GETCTL_STYLE( control ) );
            U32ToMem( data, GETCTL_EXSTYLE( control ) );
        }
        U16ToMem( data, GETCTL_SIZEX( control ) );
        U16ToMem( data, GETCTL_SIZEY( control ) );
        U16ToMem( data, GETCTL_SIZEW( control ) );
        U16ToMem( data, GETCTL_SIZEH( control ) );
        if( is32bitEx ) {
            U32ToMem( data, GETCTL_ID( control ) );
        } else {
            U16ToMem( data, GETCTL_ID( control ) );
        }
        if( !is32bit ) {
            U32ToMem( data, GETCTL_STYLE( control ) );
        }
        cclass = GETCTL_CLASSID( control );
        if( cclass->Class & 0x80 ) {
            if( is32bit ) {
                U16ToMem( data, (uint_16)-1 );
                U16ToMem( data, cclass->Class );
            } else {
                U8ToMem( data, cclass->Class );
            }
        } else {
            size = WdeString2Mem( cclass->ClassName, is32bit, data );
            data += size;
            ok = (size != 0);
        }
    }

    if( ok ) {
        size = WdeNameOrOrd2Mem( GETCTL_TEXT( control ), is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( is32bit ) {
            U16ToMem( data, GETCTL_EXTRABYTES( control ) );
        } else {
            U8ToMem( data, GETCTL_EXTRABYTES( control ) );
        }
    }

    if ( ok )
        return( data - start );
    return( 0 );
}


bool WdeDBI2Mem( WdeDialogBoxInfo *info, uint_8 **pdata, size_t *psize )
{
    bool                ok;
    size_t              size, memsize;
    LIST                *l;
    WdeDialogBoxControl *ci;
    bool                is32bit;
    bool                is32bitEx;
    uint_8              *data;
    uint_8              *start;

    if( info == NULL || pdata == NULL || psize == NULL )
        return( false );

    start = NULL;
    is32bit = info->dialog_header->is32bit;
    is32bitEx = info->dialog_header->is32bitEx;
    memsize = WdeCalcSizeOfWdeDialogBoxInfo( info );
    ok = (memsize != 0);

    if( ok ) {
        start = WRMemAlloc( memsize );
        ok = (start != NULL);
    }

    if( ok ) {
        data = start;
        size = WdeDialogBoxHeader2Mem( info->dialog_header, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        for( l = info->control_list; l != NULL; l = ListNext( l ) ) {
            ci = (WdeDialogBoxControl *)ListElement( l );
            if( ci == NULL ) {
                ok = false;
                break;
            }
            if( is32bit ) {
                PADDING_WRITE( data, start );
            }
            size = WdeDialogBoxControl2Mem( ci, data, is32bit, is32bitEx );
            data += size;
            if( size == 0 ) {
                ok = false;
                break;
            }
        }
    }

    if( ok ) {
        if( is32bit ) {
            PADDING_WRITE( data, start );
        }
    }

    *psize = 0;
    *pdata = NULL;
    if( ok ) {
        ok = (( data - start ) == memsize);
        if( ok ) {
            *psize = memsize;
            *pdata = start;
        }
    }
    if( !ok && start != NULL ) {
        WRMemFree( start );
    }
    return( ok );
}

WdeDialogBoxInfo *WdeMem2DBI( const uint_8 *data, size_t size, bool is32bit )
{
    WdeDialogBoxInfo    *dbi;
    WdeDialogBoxControl *control;
    LIST                *prev_control;
    int                 index;
    const uint_8        *start;
    bool                ok;
    bool                is32bitEx = false;
    uint_16             sign0;
    uint_16             sign1;

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
        U16FromMem( data, sign0 );
        U16FromMem( data, sign1 );
        is32bitEx = (sign0 == 0x0001 && sign1 == 0xFFFF);

        data = start;
        dbi->control_list = NULL;
        dbi->MemoryFlags = 0;
        dbi->dialog_header = WdeMem2DialogBoxHeader( &data, is32bit, is32bitEx );
        ok = (dbi->dialog_header != NULL);
    }

    if( ok ) {
        prev_control = NULL;
        for( index = 0; index < GETHDR_NUMITEMS( dbi->dialog_header ); index++ ) {
            if( is32bit ) {
                PADDING_SET( data, start );
            }
            control = WdeMem2DialogBoxControl( &data, is32bit, is32bitEx );
            if( control == NULL ) {
                ok = false;
                break;
            }
            if( prev_control == NULL ) {
                ListAddElt( &dbi->control_list, (OBJPTR)control );
                prev_control = dbi->control_list;
            } else {
                ListInsertElt( prev_control, (OBJPTR)control );
                prev_control = ListNext( prev_control );
            }
        }
    }

    if( ok ) {
        ok = ( size >= data - start );
    }

    if( !ok ) {
        if( dbi != NULL ) {
            WdeFreeDialogBoxInfo( dbi );
            dbi = NULL;
        }
    }

    return( dbi );
}

WdeDialogBoxHeader *WdeMem2DialogBoxHeader( const uint_8 **pdata, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxHeader  *dbh;
    bool                ok;
    const uint_8        *data;

    dbh = NULL;
    data = NULL;

    ok = (pdata != NULL && *pdata != NULL);

    if( ok ) {
        dbh = WdeAllocDialogBoxHeader();
        ok = (dbh != NULL);
    }

    if( ok ) {
        data = *pdata;
        dbh->symbol = NULL;
        dbh->helpsymbol = NULL;
        dbh->is32bit = is32bit;
        dbh->is32bitEx = is32bitEx;
        if( is32bitEx ) {
            /* skip the miscellaneous two WORDs 0x0001, 0xFFFF */
            data += 2 * sizeof( uint_16 );
            U32FromMem( data, GETHDR_HELPID( dbh ) );
            U32FromMem( data, GETHDR_EXSTYLE( dbh ) );
            U32FromMem( data, GETHDR_STYLE( dbh ) );
        } else if( is32bit ) {
            U32FromMem( data, GETHDR_STYLE( dbh ) );
            U32FromMem( data, GETHDR_EXSTYLE( dbh ) );
        } else {
            U32FromMem( data, GETHDR_STYLE( dbh ) );
        }
        if( is32bit ) {
            U16FromMem( data, GETHDR_NUMITEMS( dbh ) );
        } else {
            U8FromMem( data, GETHDR_NUMITEMS( dbh ) );
        }
        U16FromMem( data, GETHDR_SIZEX( dbh ) );
        U16FromMem( data, GETHDR_SIZEY( dbh ) );
        U16FromMem( data, GETHDR_SIZEW( dbh ) );
        U16FromMem( data, GETHDR_SIZEH( dbh ) );

        SETHDR_MENUNAME( dbh, WdeMem2NameOrOrdinal( &data, is32bit ) );
        ok = (GETHDR_MENUNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CLASSNAME( dbh, WdeMem2NameOrOrdinal( &data, is32bit ) );
        ok = (GETHDR_CLASSNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CAPTION( dbh, WdeMem2String( &data, is32bit ) );
        ok = (GETHDR_CAPTION( dbh ) != NULL);
    }

    if( ok ) {
        if( GETHDR_STYLE( dbh ) & DS_SETFONT ) {
            U16FromMem( data, GETHDR_FONTPOINTSIZE( dbh ) );
            if( is32bitEx ) {
                U16FromMem( data, GETHDR_FONTWEIGHT( dbh ) );
                U8FromMem( data, GETHDR_FONTITALIC( dbh ) );
                U8FromMem( data, GETHDR_FONTCHARSET( dbh ) );
            }
            SETHDR_FONTFACENAME( dbh, WdeMem2String( &data, is32bit ) );
            ok = (GETHDR_FONTFACENAME( dbh ) != NULL);
        } else {
            SETHDR_FONTPOINTSIZE( dbh, 0 );
            SETHDR_FONTFACENAME( dbh, NULL );
        }
    }

    if( dbh != NULL ) {
        *pdata = data;
        if( !ok ) {
            WdeFreeDialogBoxHeader( &dbh );
        }
    }
    return( dbh );
}

WdeDialogBoxControl *WdeMem2DialogBoxControl( const uint_8 **pdata, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxControl         *dbc;
    bool                        ok;
    const uint_8                *data;

    dbc = NULL;
    data = NULL;

    ok = (pdata != NULL && *pdata != NULL);

    if( ok ) {
        dbc = WdeAllocDialogBoxControl();
        ok = (dbc != NULL);
    }

    if( ok ) {
        data = *pdata;
        if( is32bitEx ) {
            U32FromMem( data, GETCTL_HELPID( dbc ) );
            U32FromMem( data, GETCTL_EXSTYLE( dbc ) );
            U32FromMem( data, GETCTL_STYLE( dbc ) );
        } else if( is32bit ) {
            U32FromMem( data, GETCTL_STYLE( dbc ) );
            U32FromMem( data, GETCTL_EXSTYLE( dbc ) );
        }
        U16FromMem( data, GETCTL_SIZEX( dbc ) );
        U16FromMem( data, GETCTL_SIZEY( dbc ) );
        U16FromMem( data, GETCTL_SIZEW( dbc ) );
        U16FromMem( data, GETCTL_SIZEH( dbc ) );
        if( is32bitEx ) {
            U32FromMem( data, GETCTL_ID( dbc ) );
        } else {
            U16FromMem( data, GETCTL_ID( dbc ) );
        }
        if( !is32bit ) {
            U32FromMem( data, GETCTL_STYLE( dbc ) );
        }
        SETCTL_CLASSID( dbc, WdeMem2ControlClass( &data, is32bit ) );
        ok = (GETCTL_CLASSID( dbc ) != NULL);
    }

    if( ok ) {
        SETCTL_TEXT( dbc, WdeMem2NameOrOrdinal( &data, is32bit ) );
        ok = (GETCTL_TEXT( dbc ) != NULL);
    }

    if( ok ) {
        if( is32bit ) {
            U16FromMem( data, GETCTL_EXTRABYTES( dbc ) );
        } else {
            U8FromMem( data, GETCTL_EXTRABYTES( dbc ) );
        }
    }

    if( dbc != NULL ) {
        *pdata = data;
        if( !ok ) {
            WdeFreeDialogBoxControl( &dbc );
        }
    }
    return( dbc );
}


ResNameOrOrdinal *WdeMem2NameOrOrdinal( const uint_8 **pdata, bool is32bit )
{
    ResNameOrOrdinal    *new;
    size_t              size;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }

    new = WRMem2NameOrOrdinal( *pdata, is32bit );
    if( new == NULL ) {
        return( NULL );
    }

    size = WdeCalcSizeOfResNameOrOrdinal( new, is32bit );
    if( size == 0 ) {
        return( NULL );
    }

    *pdata += size;

    return( new );
}

ControlClass *WdeMem2ControlClass( const uint_8 **pdata, bool is32bit )
{
    ControlClass        *new;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }
    if( ( is32bit && GetU16( *pdata ) != 0xffff )
      || ( !is32bit && (GetU8( *pdata ) & 0x80) == 0 ) ) {
        return( (ControlClass *)WdeMem2String( pdata, is32bit ) );
    }

    new = (ControlClass *)WRMemAlloc( sizeof( ControlClass ) );
    if( new == NULL ) {
        return( NULL );
    }

    if( is32bit ) {
        new->Class = GetU16( *pdata + sizeof( uint_16 ) ) & 0x00ff;
        *pdata += 2 * sizeof( uint_16 );
    } else {
        new->Class = GetU8( *pdata );
        *pdata += 1;
    }

    return( new );
}

char *WdeMem2String( const uint_8 **pdata, bool is32bit )
{
    char        *new;
    size_t      size;
    const char  *data;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }

    data = (const char *)*pdata;
    if( is32bit ) {
        WRunicode2mbcs( data, NULL, &size );
    } else {
        size = strlen( data ) + 1;
    }

    new = WRMemAlloc( size );
    if( new == NULL ) {
        return( NULL );
    }

    if( is32bit ) {
        WRunicode2mbcsBuf( data, (char *)new, size );
        size = WRStrlen32( data ) + sizeof( uint_16 );
    } else {
        memcpy( new, data, size );
    }

    *pdata += size;

    return( new );
}
