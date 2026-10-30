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
#include "wdesdup.h"
#include "wdeobjid.h"
#include "wdecctl.h"
#include "wde_wres.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static const char   *WdeGetClassNameFromOrdinal( uint_16 );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    uint_16     ordinal;
    const char  *name;
} WdeControlClassItems;

typedef struct {
    OBJ_ID      id;
    char        *name;
} WdeCommonControlItems;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WdeControlClassItems WdeControlClasses[] = {
    { CLASS_BUTTON,     "BUTTON"    },
    { CLASS_EDIT,       "EDIT"      },
    { CLASS_STATIC,     "STATIC"    },
    { CLASS_LISTBOX,    "LISTBOX"   },
    { CLASS_SCROLLBAR,  "SCROLLBAR" },
    { CLASS_COMBOBOX,   "COMBOBOX"  },
    { 0,                NULL        }
};

static WdeCommonControlItems WdeCommonControlClasses[] = {
    { SBAR_OBJ,         STATUSCLASSNAME       },
    { LVIEW_OBJ,        WC_LISTVIEW           },
    { TVIEW_OBJ,        WC_TREEVIEW           },
    { TABCNTL_OBJ,      WC_TABCONTROL         },
    { ANIMATE_OBJ,      ANIMATE_CLASS         },
    { UPDOWN_OBJ,       UPDOWN_CLASS          },
    { TRACKBAR_OBJ,     TRACKBAR_CLASS        },
    { PROGRESS_OBJ,     PROGRESS_CLASS        },
    { HOTKEY_OBJ,       HOTKEY_CLASS          },
    { HEADER_OBJ,       WC_HEADER             },
    { 0,                NULL                  }
};


ResNameOrOrdinal *WdeStrToResNameOrOrdinal( const char *str )
{
    ResNameOrOrdinal    *rp;
    uint_16             ordinal;
    unsigned long       ul;
    char                *ep;

    if( str == NULL ) {
        return( NULL );
    }

    if( str[0] == '\0' ) {
        return( ResStrToNameOrOrdinal( str ) );
    }

    ul = strtoul( str, &ep, 0 );
    if( *ep == '\0' && ul <= 0xffff ) {
        ordinal = (uint_16)ul;
        rp = ResNumToNameOrOrdinal( ordinal );
    } else {
        rp = ResStrToNameOrOrdinal( str );
    }

    return( rp );
}

char *WdeResNameOrOrdinalToStr( const ResNameOrOrdinal *name, int base )
{
    char    temp[15];
    char    *str;

    str = NULL;

    if( name != NULL ) {
        if( name->ord.fFlag == 0xff ) {
            sprintf( temp, ( base == 10 ) ? "%u" : "%x", name->ord.wOrdinalID );
            str = WdeStrdup( temp );
        } else {
            str = (char *)WdeStrdup( name->name );
        }
    }

    return( str );
}

ResNameOrOrdinal *WdeStrToControlClass( const char *str )
{
    ResNameOrOrdinal    *cclass;
    size_t              slen;

    slen = strlen( str );

    cclass = MemAlloc( sizeof( ResNameOrOrdinal ) + slen );

    if( cclass != NULL ) {
        memcpy( cclass->name, str, slen + 1 );
    }

    return( cclass );
}

char *WdeControlClassToStr( const ResNameOrOrdinal *cclass )
{
    const char  *name;
    char        *str;
    char        temp[35];

    str = NULL;

    if( cclass != NULL ) {
        if( cclass->ord.fFlag == 0xff ) {
            name = WdeGetClassNameFromOrdinal( cclass->ord.wOrdinalID );
            if( name == NULL ) {
                sprintf( temp, "%lu", cclass->ord.wOrdinalID );
                name = temp;
            }
        } else {
            name = cclass->name;
        }
        if( name != NULL && name[0] != '\0' ) {
            str = WdeStrdup( name );
        }
    }

    return( str );
}

WdeDialogBoxControl *WdeCopyDialogBoxControl( const WdeDialogBoxControl *src )
{
    WdeDialogBoxControl *dest;

    if( src == NULL ) {
        return( NULL );
    }

    dest = WdeAllocDialogBoxControl();
    if( dest == NULL ) {
        return( NULL );
    }

    memcpy( dest, src, sizeof( WdeDialogBoxControl ) );

    SETCTL_CLASSID( dest, WdeCopyResNameOrOrdinal( GETCTL_CLASSID( src ) ) );
    SETCTL_TEXT( dest, WdeCopyResNameOrOrdinal( GETCTL_TEXT( src ) ) );

    dest->symbol = WdeStrdup( src->symbol );
    dest->helpsymbol = WdeStrdup( src->helpsymbol );

    return( dest );
}

WdeDialogBoxHeader *WdeCopyDialogBoxHeader( const WdeDialogBoxHeader *src )
{
    WdeDialogBoxHeader *dest;

    if( src == NULL ) {
        return( NULL );
    }

    dest = WdeAllocDialogBoxHeader();
    if( dest == NULL ) {
        return( NULL );
    }

    memcpy( dest, src, sizeof( WdeDialogBoxHeader ) );

    SETHDR_MENUNAME( dest, WdeCopyResNameOrOrdinal( GETHDR_MENUNAME( src ) ) );
    SETHDR_CLASSNAME( dest, WdeCopyResNameOrOrdinal( GETHDR_CLASSNAME( src ) ) );
    SETHDR_CAPTION( dest, WdeStrdup( GETHDR_CAPTION( src ) ) );
    SETHDR_FONTFACENAME( dest,  WdeStrdup( GETHDR_FONTFACENAME( src ) ) );

    dest->symbol = WdeStrdup( src->symbol );
    dest->helpsymbol = WdeStrdup( src->helpsymbol );

    return( dest );
}


WResID *WdeCopyWResID( const WResID *src )
{
    WResID  *dest;
    size_t  len;

    if( src == NULL ) {
        return ( NULL );
    }

    len = sizeof( WResID );

    if( src->IsName ) {
        len += src->ID.Name.NumChars - 1;
    }

    dest = MemAlloc( len );

    if( dest != NULL ) {
        memcpy( dest, src, len );
    }

    return( dest );
}

WResHelpID *WdeCopyWResHelpID( const WResHelpID *src )
{
    WResHelpID  *dest;
    size_t      len;

    if( src == NULL ) {
        return( NULL );
    }

    len = sizeof( WResHelpID );

    if( src->IsName ) {
        len += src->ID.Name.NumChars - 1;
    }

    dest = MemAlloc( len );

    if( dest != NULL ) {
        memcpy ( dest, src, len );
    }

    return( dest );
}

ResNameOrOrdinal *WdeCopyResNameOrOrdinal( const ResNameOrOrdinal *src )
{
    ResNameOrOrdinal *dest;

    if( src == NULL ) {
        return( NULL );
    }

    if( src->ord.fFlag == 0xff ) {
        dest = ResNumToNameOrOrdinal( src->ord.wOrdinalID );
    } else {
        dest = ResStrToNameOrOrdinal( src->name );
    }

    return( dest );
}

void WdeFreeDialogBoxControl( WdeDialogBoxControl **pcontrol )
{
    if( pcontrol != NULL && *pcontrol != NULL ) {
        if( GETCTL_CLASSID( *pcontrol ) ) {
            MemFree( GETCTL_CLASSID( *pcontrol ) );
        }
        if( GETCTL_TEXT( *pcontrol ) ) {
            MemFree( GETCTL_TEXT( *pcontrol ) );
        }
        if( (*pcontrol)->symbol ) {
            MemFree( (*pcontrol)->symbol );
        }
        if( (*pcontrol)->helpsymbol ) {
            MemFree( (*pcontrol)->helpsymbol );
        }
        MemFree( *pcontrol );
        *pcontrol = NULL;
    }
}

WdeDialogBoxControl *WdeAllocDialogBoxControl( void )
{
    WdeDialogBoxControl *control;

    control = MemAlloc( sizeof( WdeDialogBoxControl ) );
    if( control == NULL ) {
        return( NULL );
    }
    memset( control, 0, sizeof( WdeDialogBoxControl ) );

    return( control );
}

void WdeFreeDialogBoxHeader( WdeDialogBoxHeader **pheader )
{
    if( pheader != NULL && *pheader != NULL ) {
        if( GETHDR_MENUNAME( *pheader ) ) {
            MemFree( GETHDR_MENUNAME( *pheader ) );
        }
        if( GETHDR_CLASSNAME( *pheader ) ) {
            MemFree( GETHDR_CLASSNAME( *pheader ) );
        }
        if( GETHDR_CAPTION( *pheader ) ) {
            MemFree( GETHDR_CAPTION( *pheader ) );
        }
        if( GETHDR_FONTFACENAME( *pheader ) ) {
            MemFree( GETHDR_FONTFACENAME( *pheader ) );
        }
        if( (*pheader)->symbol ) {
            MemFree( (*pheader)->symbol );
        }
        MemFree( *pheader );
        *pheader = NULL;
    }
}

WdeDialogBoxHeader *WdeAllocDialogBoxHeader( void )
{
    WdeDialogBoxHeader *header;

    header = MemAlloc( sizeof( WdeDialogBoxHeader ) );
    if( header == NULL ) {
        return( NULL );
    }

    memset( header, 0, sizeof( WdeDialogBoxHeader ) );

    return( header );
}

const char *WdeGetClassNameFromOrdinal( uint_16 ordinal )
{
    int i;

    for( i = 0; WdeControlClasses[i].ordinal != 0; i++ ) {
        if( WdeControlClasses[i].ordinal == ordinal ) {
            return( WdeControlClasses[i].name );
        }
    }
    return ( NULL );
}

uint_16 WdeGetClassOrdinalFromName( const char *name )
{
    int i;

    for( i = 0; WdeControlClasses[i].ordinal != 0; i++ ) {
        if( stricmp( WdeControlClasses[i].name, name ) == 0 ) {
            return( WdeControlClasses[i].ordinal );
        }
    }

    return( 0 );
}

OBJ_ID WdeGetCommonControlClassFromClassName( const char *name )
{
    int i;

    for( i = 0; WdeCommonControlClasses[i].id != 0; i++ ) {
        if( stricmp( WdeCommonControlClasses[i].name, name ) == 0 ) {
            return( WdeCommonControlClasses[i].id );
        }
    }

    return( 0 );
}
