/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "wde2data.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#define PADDING_WRITE(m,s) { \
    size_t pad = CALC_PAD( (m) - (s), SIZEU32 ); \
    memset( m, 0, pad ); \
    m += pad; \
}

#define PADDING_SET(m,s) { \
    size_t pad = CALC_PAD( (m) - (s), SIZEU32 ); \
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
static WdeDialogBoxHeader  *WdeDialogBoxHeaderFromData( const char **pdata, bool, bool );
static WdeDialogBoxControl *WdeDialogBoxControlFromData( const char **pdata, bool, bool );
static ResNameOrOrdinal    *WdeNameOrOrdinalFromData( const char **pdata, bool );
static ResNameOrOrdinal    *WdeControlClassFromData( const char **pdata, bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static size_t WdeDataFromNameOrOrdinal( ResNameOrOrdinal *name, bool use_unicode, char *data )
{
    size_t      size;

    if( name == NULL ) {
        size = WRDataFromString( "", use_unicode, data );
    } else {
        if( name->ord.fFlag == 0xff ) {
            if( use_unicode ) {
                VALU16( data ) = (uint_16)-1;
                INCU16( data );
                VALU16( data ) = name->ord.wOrdinalID;
                INCU16( data );
                size = SIZEU16 + SIZEU16;
            } else {
                VALU8( data ) = name->ord.fFlag;
                INCU8( data );
                VALU16( data ) = name->ord.wOrdinalID;
                INCU16( data );
                size = SIZEU8 + SIZEU16;
            }
        } else {
            size = WRDataFromString( name->name, use_unicode, data );
        }
    }
    return( size );
}

static size_t WdeDataFromDialogBoxHeader( WdeDialogBoxHeader *header, char *data )
{
    bool                ok;
    size_t              size;
    char                *start;

    ok = (header != NULL && data != NULL);
    start = NULL;

    if( ok ) {
        start = data;
        if( header->is32bitEx ) {
            /* copy the miscellaneous two WORDs 0x0001, 0xFFFF */
            VALU16( data ) = 0x0001;
            INCU16( data );
            VALU16( data ) = 0xFFFF;
            INCU16( data );
            VALU32( data ) = GETHDR_HELPID( header );
            INCU32( data );
            VALU32( data ) = GETHDR_EXSTYLE( header );
            INCU32( data );
            VALU32( data ) = GETHDR_STYLE( header );
            INCU32( data );
        } else if( header->is32bit ) {
            VALU32( data ) = GETHDR_STYLE( header );
            INCU32( data );
            VALU32( data ) = GETHDR_EXSTYLE( header );
            INCU32( data );
        } else {
            VALU32( data ) = GETHDR_STYLE( header );
            INCU32( data );
        }
        if( header->is32bit ) {
            VALU16( data ) = GETHDR_NUMITEMS( header );
            INCU16( data );
        } else {
            VALU8( data ) = GETHDR_NUMITEMS( header );
            INCU8( data );
        }
        VALU16( data ) = GETHDR_SIZEX( header );
        INCU16( data );
        VALU16( data ) = GETHDR_SIZEY( header );
        INCU16( data );
        VALU16( data ) = GETHDR_SIZEW( header );
        INCU16( data );
        VALU16( data ) = GETHDR_SIZEH( header );
        INCU16( data );
    }

    if( ok ) {
        size = WdeDataFromNameOrOrdinal( GETHDR_MENUNAME( header ), header->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WdeDataFromNameOrOrdinal( GETHDR_CLASSNAME( header ), header->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WRDataFromString( GETHDR_CAPTION( header ), header->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( GETHDR_STYLE( header ) & DS_SETFONT ) {
            VALU16( data ) = GETHDR_FONTPOINTSIZE( header );
            INCU16( data );
            if( header->is32bitEx ) {
                VALU16( data ) = GETHDR_FONTWEIGHT( header );
                INCU16( data );
                VALU8( data ) = GETHDR_FONTITALIC( header );
                INCU8( data );
                VALU8( data ) = GETHDR_FONTCHARSET( header );
                INCU8( data );
            }
            size = WRDataFromString( GETHDR_FONTFACENAME( header ), header->is32bit, data );
            data += size;
            ok = (size != 0);
        }
    }

    if( ok )
        return( data - start );
    return( 0 );
}

static size_t WdeDataFromDialogBoxControl( WdeDialogBoxControl *control,
                                     char *data, bool is32bit, bool is32bitEx )
{
    ResNameOrOrdinal        *cclass;
    size_t                  size;
    bool                    ok;
    char                    *start;

    ok = (control != NULL && data != NULL);
    start = NULL;

    if( ok ) {
        start = data;
        if( is32bitEx ) {
            VALU32( data ) = GETCTL_HELPID( control );
            INCU32( data );
            VALU32( data ) = GETCTL_EXSTYLE( control );
            INCU32( data );
            VALU32( data ) = GETCTL_STYLE( control );
            INCU32( data );
        } else if( is32bit ) {
            VALU32( data ) = GETCTL_STYLE( control );
            INCU32( data );
            VALU32( data ) = GETCTL_EXSTYLE( control );
            INCU32( data );
        }
        VALU16( data ) = GETCTL_SIZEX( control );
        INCU16( data );
        VALU16( data ) = GETCTL_SIZEY( control );
        INCU16( data );
        VALU16( data ) = GETCTL_SIZEW( control );
        INCU16( data );
        VALU16( data ) = GETCTL_SIZEH( control );
        INCU16( data );
        if( is32bitEx ) {
            VALU32( data ) = GETCTL_ID( control );
            INCU32( data );
        } else {
            VALU16( data ) = GETCTL_ID( control );
            INCU16( data );
        }
        if( !is32bit ) {
            VALU32( data ) = GETCTL_STYLE( control );
            INCU32( data );
        }
        cclass = GETCTL_CLASSID( control );
        if( cclass->ord.fFlag == 0xff ) {
            if( is32bit ) {
                VALU16( data ) = 0xffff;
                INCU16( data );
                VALU16( data ) = cclass->ord.wOrdinalID;
                INCU16( data );
            } else {
                VALU8( data ) = cclass->ord.wOrdinalID;
                INCU8( data );
            }
        } else {
            size = WRDataFromString( cclass->name, is32bit, data );
            data += size;
            ok = (size != 0);
        }
    }

    if( ok ) {
        size = WdeDataFromNameOrOrdinal( GETCTL_TEXT( control ), is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( is32bit ) {
            VALU16( data ) = GETCTL_EXTRABYTES( control );
            INCU16( data );
        } else {
            VALU8( data ) = GETCTL_EXTRABYTES( control );
            INCU8( data );
        }
    }

    if ( ok )
        return( data - start );
    return( 0 );
}


bool WdeAllocDataFromDBI( WdeDialogBoxInfo *info, char **pdata, size_t *dsize )
{
    bool                ok;
    size_t              size;
    size_t              memsize;
    LIST                *l;
    WdeDialogBoxControl *control;
    bool                is32bit;
    bool                is32bitEx;
    char                *data;
    char                *start;

    if( info == NULL || pdata == NULL || dsize == NULL )
        return( false );

    start = NULL;
    is32bit = info->dialog_header->is32bit;
    is32bitEx = info->dialog_header->is32bitEx;
    memsize = WdeCalcSizeOfWdeDialogBoxInfo( info );
    ok = (memsize != 0);

    if( ok ) {
        start = data = MemAlloc( memsize );
        ok = (data != NULL);
    }

    if( ok ) {
        size = WdeDataFromDialogBoxHeader( info->dialog_header, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        for( l = info->control_list; l != NULL; l = ListNext( l ) ) {
            control = (WdeDialogBoxControl *)ListElement( l );
            if( control == NULL ) {
                ok = false;
                break;
            }
            if( is32bit ) {
                PADDING_WRITE( data, start );
            }
            size = WdeDataFromDialogBoxControl( control, data, is32bit, is32bitEx );
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

    if( ok ) {
        ok = ( data == start + memsize );
        if( ok ) {
            *dsize = memsize;
            *pdata = start;
        }
    }
    if( !ok ) {
        if( start != NULL ) {
            MemFree( start );
        }
        *dsize = 0;
        *pdata = NULL;
    }
    return( ok );
}

WdeDialogBoxInfo *WdeAllocDBIFromData( const char *data, size_t size, bool is32bit )
{
    WdeDialogBoxInfo    *dbi;
    WdeDialogBoxControl *control;
    LIST                *prev_control;
    int                 index;
    const char          *start;
    bool                ok;
    bool                is32bitEx = false;
    uint_16             sign0;
    uint_16             sign1;

    dbi = NULL;
    start = data;

    ok = (data != NULL && size != 0);

    if( ok ) {
        dbi = MemAlloc( sizeof( WdeDialogBoxInfo ) );
        ok = (dbi != NULL);
    }


    if( ok ) {
        /* check if the dialog is extended by testing for the signature */
        sign0 = VALU16( data );
        sign1 = VALU16( data + SIZEU16 );
        is32bitEx = (sign0 == 0x0001 && sign1 == 0xFFFF);

        dbi->control_list = NULL;
        dbi->res_flags = DEFAULT_FLAGS_NONE;
        dbi->dialog_header = WdeDialogBoxHeaderFromData( &data, is32bit, is32bitEx );
        ok = (dbi->dialog_header != NULL);
    }

    if( ok ) {
        prev_control = NULL;
        for( index = 0; index < GETHDR_NUMITEMS( dbi->dialog_header ); index++ ) {
            if( is32bit ) {
                PADDING_SET( data, start );
            }
            control = WdeDialogBoxControlFromData( &data, is32bit, is32bitEx );
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
        ok = ( start + size >= data );
    }

    if( !ok ) {
        if( dbi != NULL ) {
            WdeFreeDialogBoxInfo( dbi );
            dbi = NULL;
        }
    }

    return( dbi );
}

WdeDialogBoxHeader *WdeDialogBoxHeaderFromData( const char **pdata, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxHeader  *header;
    bool                ok;
    const char          *data;

    header = NULL;
    data = NULL;

    ok = (pdata != NULL && *pdata != NULL);

    if( ok ) {
        header = WdeAllocDialogBoxHeader();
        ok = (header != NULL);
    }

    if( ok ) {
        data = *pdata;
        header->symbol = NULL;
        header->helpsymbol = NULL;
        header->is32bit = is32bit;
        header->is32bitEx = is32bitEx;
        if( is32bitEx ) {
            /* skip the miscellaneous two WORDs 0x0001, 0xFFFF */
            INCU32( data );
            GETHDR_HELPID( header ) = VALU32( data );
            INCU32( data );
            GETHDR_EXSTYLE( header ) = VALU32( data );
            INCU32( data );
            GETHDR_STYLE( header ) = VALU32( data );
            INCU32( data );
        } else if( is32bit ) {
            GETHDR_STYLE( header ) = VALU32( data );
            INCU32( data );
            GETHDR_EXSTYLE( header ) = VALU32( data );
            INCU32( data );
        } else {
            GETHDR_STYLE( header ) = VALU32( data );
            INCU32( data );
        }
        if( is32bit ) {
            GETHDR_NUMITEMS( header ) = VALU16( data );
            INCU16( data );
        } else {
            GETHDR_NUMITEMS( header ) = VALU8( data );
            INCU8( data );
        }
        GETHDR_SIZEX( header ) = VALU16( data );
        INCU16( data );
        GETHDR_SIZEY( header ) = VALU16( data );
        INCU16( data );
        GETHDR_SIZEW( header ) = VALU16( data );
        INCU16( data );
        GETHDR_SIZEH( header ) = VALU16( data );
        INCU16( data );

        SETHDR_MENUNAME( header, WdeNameOrOrdinalFromData( &data, is32bit ) );
        ok = (GETHDR_MENUNAME( header ) != NULL);
    }

    if( ok ) {
        SETHDR_CLASSNAME( header, WdeNameOrOrdinalFromData( &data, is32bit ) );
        ok = (GETHDR_CLASSNAME( header ) != NULL);
    }

    if( ok ) {
        SETHDR_CAPTION( header, WRStringFromData( &data, is32bit ) );
        ok = (GETHDR_CAPTION( header ) != NULL);
    }

    if( ok ) {
        if( GETHDR_STYLE( header ) & DS_SETFONT ) {
            GETHDR_FONTPOINTSIZE( header ) = VALU16( data );
            INCU16( data );
            if( is32bitEx ) {
                GETHDR_FONTWEIGHT( header ) = VALU16( data );
                INCU16( data );
                GETHDR_FONTITALIC( header ) = VALU8( data );
                INCU8( data );
                GETHDR_FONTCHARSET( header ) = VALU8( data );
                INCU8( data );
            }
            SETHDR_FONTFACENAME( header, WRStringFromData( &data, is32bit ) );
            ok = (GETHDR_FONTFACENAME( header ) != NULL);
        } else {
            SETHDR_FONTPOINTSIZE( header, 0 );
            SETHDR_FONTFACENAME( header, NULL );
        }
    }

    if( header != NULL ) {
        *pdata = data;
        if( !ok ) {
            WdeFreeDialogBoxHeader( &header );
        }
    }
    return( header );
}

WdeDialogBoxControl *WdeDialogBoxControlFromData( const char **pdata, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxControl         *control;
    bool                        ok;
    const char                  *data;

    control = NULL;
    data = NULL;

    ok = (pdata != NULL && *pdata != NULL);

    if( ok ) {
        control = WdeAllocDialogBoxControl();
        ok = (control != NULL);
    }

    if( ok ) {
        data = *pdata;
        if( is32bitEx ) {
            GETCTL_HELPID( control ) = VALU32( data );
            INCU32( data );
            GETCTL_EXSTYLE( control ) = VALU32( data );
            INCU32( data );
            GETCTL_STYLE( control ) = VALU32( data );
            INCU32( data );
        } else if( is32bit ) {
            GETCTL_STYLE( control ) = VALU32( data );
            INCU32( data );
            GETCTL_EXSTYLE( control ) = VALU32( data );
            INCU32( data );
        }
        GETCTL_SIZEX( control ) = VALU16( data );
        INCU16( data );
        GETCTL_SIZEY( control ) = VALU16( data );
        INCU16( data );
        GETCTL_SIZEW( control ) = VALU16( data );
        INCU16( data );
        GETCTL_SIZEH( control ) = VALU16( data );
        INCU16( data );
        if( is32bitEx ) {
            GETCTL_ID( control ) = VALU32( data );
            INCU32( data );
        } else {
            GETCTL_ID( control ) = VALU16( data );
            INCU16( data );
        }
        if( !is32bit ) {
            GETCTL_STYLE( control ) = VALU32( data );
            INCU32( data );
        }
        SETCTL_CLASSID( control, WdeControlClassFromData( &data, is32bit ) );
        ok = (GETCTL_CLASSID( control ) != NULL);
    }

    if( ok ) {
        SETCTL_TEXT( control, WdeNameOrOrdinalFromData( &data, is32bit ) );
        ok = (GETCTL_TEXT( control ) != NULL);
    }

    if( ok ) {
        if( is32bit ) {
            GETCTL_EXTRABYTES( control ) = VALU16( data );
            INCU16( data );
        } else {
            GETCTL_EXTRABYTES( control ) = VALU8( data );
            INCU8( data );
        }
    }

    if( control != NULL ) {
        *pdata = data;
        if( !ok ) {
            WdeFreeDialogBoxControl( &control );
        }
    }
    return( control );
}


ResNameOrOrdinal *WdeNameOrOrdinalFromData( const char **pdata, bool is32bit )
{
    ResNameOrOrdinal    *cclass;
    size_t              size;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }

    cclass = WRNameOrOrdinalFromData( *pdata, is32bit );
    if( cclass == NULL ) {
        return( NULL );
    }

    size = WdeCalcSizeOfResNameOrOrdinal( cclass, is32bit );
    if( size == 0 ) {
        return( NULL );
    }

    *pdata += size;

    return( cclass );
}

ResNameOrOrdinal *WdeControlClassFromData( const char **pdata, bool is32bit )
{
    ResNameOrOrdinal    *cclass;
    const char          *data;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }
    data = *pdata;
    if( is32bit ) {
        if( VALU16( data ) != 0xffff ) {
            return( (ResNameOrOrdinal *)WRStringFromData( pdata, is32bit ) );
        }
        INCU16( data );
    } else {
        if( (VALU8( data ) & 0x80) == 0 ) {
            return( (ResNameOrOrdinal *)WRStringFromData( pdata, is32bit ) );
        }
    }

    cclass = MemAlloc( sizeof( ResNameOrOrdinal ) );
    if( cclass == NULL ) {
        return( NULL );
    }
    cclass->ord.fFlag = 0xff;
    if( is32bit ) {
        cclass->ord.wOrdinalID = VALU16( data );
        INCU16( data );
    } else {
        cclass->ord.wOrdinalID = VALU8( data );
        INCU8( data );
    }
    *pdata = data;

    return( cclass );
}
