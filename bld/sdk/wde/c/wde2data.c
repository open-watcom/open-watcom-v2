/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
static ControlClass        *WdeControlClassFromData( const char **pdata, bool );

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

static size_t WdeDataFromDialogBoxHeader( WdeDialogBoxHeader *head, char *data )
{
    bool                ok;
    size_t              size;
    char                *start;

    ok = (head != NULL && data != NULL);
    start = NULL;

    if( ok ) {
        start = data;
        if( head->is32bitEx ) {
            /* copy the miscellaneous two WORDs 0x0001, 0xFFFF */
            VALU16( data ) = 0x0001;
            INCU16( data );
            VALU16( data ) = 0xFFFF;
            INCU16( data );
            VALU32( data ) = GETHDR_HELPID( head );
            INCU32( data );
            VALU32( data ) = GETHDR_EXSTYLE( head );
            INCU32( data );
            VALU32( data ) = GETHDR_STYLE( head );
            INCU32( data );
        } else if( head->is32bit ) {
            VALU32( data ) = GETHDR_STYLE( head );
            INCU32( data );
            VALU32( data ) = GETHDR_EXSTYLE( head );
            INCU32( data );
        } else {
            VALU32( data ) = GETHDR_STYLE( head );
            INCU32( data );
        }
        if( head->is32bit ) {
            VALU16( data ) = GETHDR_NUMITEMS( head );
            INCU16( data );
        } else {
            VALU8( data ) = GETHDR_NUMITEMS( head );
            INCU8( data );
        }
        VALU16( data ) = GETHDR_SIZEX( head );
        INCU16( data );
        VALU16( data ) = GETHDR_SIZEY( head );
        INCU16( data );
        VALU16( data ) = GETHDR_SIZEW( head );
        INCU16( data );
        VALU16( data ) = GETHDR_SIZEH( head );
        INCU16( data );
    }

    if( ok ) {
        size = WdeDataFromNameOrOrdinal( GETHDR_MENUNAME( head ), head->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WdeDataFromNameOrOrdinal( GETHDR_CLASSNAME( head ), head->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        size = WRDataFromString( GETHDR_CAPTION( head ), head->is32bit, data );
        data += size;
        ok = (size != 0);
    }

    if( ok ) {
        if( GETHDR_STYLE( head ) & DS_SETFONT ) {
            VALU16( data ) = GETHDR_FONTPOINTSIZE( head );
            INCU16( data );
            if( head->is32bitEx ) {
                VALU16( data ) = GETHDR_FONTWEIGHT( head );
                INCU16( data );
                VALU8( data ) = GETHDR_FONTITALIC( head );
                INCU8( data );
                VALU8( data ) = GETHDR_FONTCHARSET( head );
                INCU8( data );
            }
            size = WRDataFromString( GETHDR_FONTFACENAME( head ), head->is32bit, data );
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
    ControlClass            *cclass;
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
        if( cclass->Class & 0x80 ) {
            if( is32bit ) {
                VALU16( data ) = (uint_16)-1;
                INCU16( data );
                VALU16( data ) = cclass->Class;
                INCU16( data );
            } else {
                VALU8( data ) = cclass->Class;
                INCU8( data );
            }
        } else {
            size = WRDataFromString( cclass->ClassName, is32bit, data );
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
    WdeDialogBoxControl *ci;
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
        start = data = WRMemAlloc( memsize );
        ok = (data != NULL);
    }

    if( ok ) {
        size = WdeDataFromDialogBoxHeader( info->dialog_header, data );
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
            size = WdeDataFromDialogBoxControl( ci, data, is32bit, is32bitEx );
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
            WRMemFree( start );
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
        dbi = (WdeDialogBoxInfo *)WRMemAlloc( sizeof( WdeDialogBoxInfo ) );
        ok = (dbi != NULL);
    }


    if( ok ) {
        /* check if the dialog is extended by testing for the signature */
        sign0 = VALU16( data );
        sign1 = VALU16( data + SIZEU16 );
        is32bitEx = (sign0 == 0x0001 && sign1 == 0xFFFF);

        dbi->control_list = NULL;
        dbi->MemoryFlags = 0;
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
    WdeDialogBoxHeader  *dbh;
    bool                ok;
    const char          *data;

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
            INCU32( data );
            GETHDR_HELPID( dbh ) = VALU32( data );
            INCU32( data );
            GETHDR_EXSTYLE( dbh ) = VALU32( data );
            INCU32( data );
            GETHDR_STYLE( dbh ) = VALU32( data );
            INCU32( data );
        } else if( is32bit ) {
            GETHDR_STYLE( dbh ) = VALU32( data );
            INCU32( data );
            GETHDR_EXSTYLE( dbh ) = VALU32( data );
            INCU32( data );
        } else {
            GETHDR_STYLE( dbh ) = VALU32( data );
            INCU32( data );
        }
        if( is32bit ) {
            GETHDR_NUMITEMS( dbh ) = VALU16( data );
            INCU16( data );
        } else {
            GETHDR_NUMITEMS( dbh ) = VALU8( data );
            INCU8( data );
        }
        GETHDR_SIZEX( dbh ) = VALU16( data );
        INCU16( data );
        GETHDR_SIZEY( dbh ) = VALU16( data );
        INCU16( data );
        GETHDR_SIZEW( dbh ) = VALU16( data );
        INCU16( data );
        GETHDR_SIZEH( dbh ) = VALU16( data );
        INCU16( data );

        SETHDR_MENUNAME( dbh, WdeNameOrOrdinalFromData( &data, is32bit ) );
        ok = (GETHDR_MENUNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CLASSNAME( dbh, WdeNameOrOrdinalFromData( &data, is32bit ) );
        ok = (GETHDR_CLASSNAME( dbh ) != NULL);
    }

    if( ok ) {
        SETHDR_CAPTION( dbh, WRStringFromData( &data, is32bit ) );
        ok = (GETHDR_CAPTION( dbh ) != NULL);
    }

    if( ok ) {
        if( GETHDR_STYLE( dbh ) & DS_SETFONT ) {
            GETHDR_FONTPOINTSIZE( dbh ) = VALU16( data );
            INCU16( data );
            if( is32bitEx ) {
                GETHDR_FONTWEIGHT( dbh ) = VALU16( data );
                INCU16( data );
                GETHDR_FONTITALIC( dbh ) = VALU8( data );
                INCU8( data );
                GETHDR_FONTCHARSET( dbh ) = VALU8( data );
                INCU8( data );
            }
            SETHDR_FONTFACENAME( dbh, WRStringFromData( &data, is32bit ) );
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

WdeDialogBoxControl *WdeDialogBoxControlFromData( const char **pdata, bool is32bit, bool is32bitEx )
{
    WdeDialogBoxControl         *dbc;
    bool                        ok;
    const char                  *data;

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
            GETCTL_HELPID( dbc ) = VALU32( data );
            INCU32( data );
            GETCTL_EXSTYLE( dbc ) = VALU32( data );
            INCU32( data );
            GETCTL_STYLE( dbc ) = VALU32( data );
            INCU32( data );
        } else if( is32bit ) {
            GETCTL_STYLE( dbc ) = VALU32( data );
            INCU32( data );
            GETCTL_EXSTYLE( dbc ) = VALU32( data );
            INCU32( data );
        }
        GETCTL_SIZEX( dbc ) = VALU16( data );
        INCU16( data );
        GETCTL_SIZEY( dbc ) = VALU16( data );
        INCU16( data );
        GETCTL_SIZEW( dbc ) = VALU16( data );
        INCU16( data );
        GETCTL_SIZEH( dbc ) = VALU16( data );
        INCU16( data );
        if( is32bitEx ) {
            GETCTL_ID( dbc ) = VALU32( data );
            INCU32( data );
        } else {
            GETCTL_ID( dbc ) = VALU16( data );
            INCU16( data );
        }
        if( !is32bit ) {
            GETCTL_STYLE( dbc ) = VALU32( data );
            INCU32( data );
        }
        SETCTL_CLASSID( dbc, WdeControlClassFromData( &data, is32bit ) );
        ok = (GETCTL_CLASSID( dbc ) != NULL);
    }

    if( ok ) {
        SETCTL_TEXT( dbc, WdeNameOrOrdinalFromData( &data, is32bit ) );
        ok = (GETCTL_TEXT( dbc ) != NULL);
    }

    if( ok ) {
        if( is32bit ) {
            GETCTL_EXTRABYTES( dbc ) = VALU16( data );
            INCU16( data );
        } else {
            GETCTL_EXTRABYTES( dbc ) = VALU8( data );
            INCU8( data );
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


ResNameOrOrdinal *WdeNameOrOrdinalFromData( const char **pdata, bool is32bit )
{
    ResNameOrOrdinal    *new;
    size_t              size;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }

    new = WRNameOrOrdinalFromData( *pdata, is32bit );
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

ControlClass *WdeControlClassFromData( const char **pdata, bool is32bit )
{
    ControlClass        *new;
    const char          *data;

    if( pdata == NULL || *pdata == NULL ) {
        return( NULL );
    }
    data = *pdata;
    if( ( is32bit && VALU16( data ) != 0xffff )
      || ( !is32bit && (VALU8( data ) & 0x80) == 0 ) ) {
        return( (ControlClass *)WRStringFromData( pdata, is32bit ) );
    }

    new = (ControlClass *)WRMemAlloc( sizeof( ControlClass ) );
    if( new == NULL ) {
        return( NULL );
    }

    if( is32bit ) {
        INCU16( data );
        new->Class = VALU16( data ) & 0x00ff;
        INCU16( data );
    } else {
        new->Class = VALU8( data );
        INCU8( data );
    }
    *pdata = data;

    return( new );
}
