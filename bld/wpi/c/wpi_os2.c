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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_SPL
#define INCL_SPLFSE
#define INCL_SPLDOSPRINT
#define INCL_BASE
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES
#define INCL_DOSSIGNALS
#define INCL_DOSSEMAPHORES
#define INCL_DOSMISC
#define INCL_DOSDEVICES
#define INCL_DOSNLS
#include <os2.h>
#include "wpi.h"

#define PATBRUSHID 200   /* This is the constant id for pattern brushes */
static char sys_font_facename[] = "System Proportional";
static BOOL GotSysFont = FALSE;
static FATTRS SysFont;
#define NUM_SYS_FONTS 5
#define IDEAL_SYSFONT_HEIGHT 10
static int SysFontHeight = 0;
static int SysFontWidth = 0;

#define PI 3.14159265

        /*---------------------------------------------------------
         * The following functions are static.
         *---------------------------------------------------------*/
static HWND _wpi_getscrollhwnd( HWND parent, int scroll )
{
    HWND        scroll_bar;

    if( parent == (HWND)NULL ) {
        return( (HWND)NULL );
    }

    if( scroll == SB_HORZ ) {
        scroll_bar = WinWindowFromID( parent, FID_HORZSCROLL );
    } else {
        scroll_bar = WinWindowFromID( parent, FID_VERTSCROLL );
    }

    return( scroll_bar );
}

static BOOL _wpi_setmenuitemattr( HMENU hmenu, unsigned id,
                                  unsigned mask, unsigned attr )
{
    return( (BOOL) WinSendMsg( hmenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE),
                               MPFROM2SHORT(mask,attr) ) );
}

void _wpi_menutext2win( char *text )
{
    if( text ) {
        while( *text ) {
            if( *text == '~' ) {
                *text = '&';
            }
            text++;
        }
    }
}

#if 0
static unsigned _wpi_getmenuitemposfromid( HMENU hmenu, unsigned id )
{
    MRESULT     result;

    result = WinSendMsg( hmenu, MM_ITEMPOSITIONFROMID,
                         MPFROM2SHORT(id, TRUE), NULL );
    if( result != MIT_NONE ) {
        return( (unsigned)result );
    }
    return( -1 );
}
#endif

static unsigned _wpi_getmenuitemidfrompos( HMENU hmenu, unsigned pos )
{
    MRESULT     result;

    result = WinSendMsg( hmenu, MM_ITEMIDFROMPOSITION, (WPI_PARAM1)pos, NULL );
    if( result != (MRESULT)MIT_ERROR ) {
        return( (unsigned)result );
    }
    return( -1 );
}

static BOOL _wpi_getmenuparentoffset( HMENU hmenu, unsigned id,
                                      HMENU *parent, unsigned *offset )
{
    int         num;
    int         i;
    unsigned    item_id;
    MENUITEM    mi;
    WPI_MRESULT result;

    if( hmenu == NULLHANDLE ) {
        return( FALSE );
    }

    num = (int) _wpi_getmenuitemcount( hmenu );
    for( i = 0; i < num; i++ ) {
        item_id = _wpi_getmenuitemidfrompos( hmenu, i );
        if( item_id == -1 ) {
            return( FALSE );
        }
        if( item_id == id ) {
            if( parent != NULL ) {
                *parent = hmenu;
            }
            if( offset != NULL ) {
                *offset = i;
            }
            return( TRUE );
        }
    }

    for( i = 0; i < num; i++ ) {
        item_id = _wpi_getmenuitemidfrompos( hmenu, i );
        if( item_id == -1 ) {
            return( FALSE );
        }
        result = WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT(item_id, FALSE), MPFROMP(&mi) );
        if( (BOOL)result && (mi.afStyle & MF_POPUP) && (mi.hwndSubMenu != NULLHANDLE) ) {
            if( _wpi_getmenuparentoffset( mi.hwndSubMenu, id, parent, offset ) ) {
                return( TRUE );
            }
        }
    }

    return( FALSE );
}

static FIXED calc_angle( WPI_POINT *cen, int x, int y )
/*****************************************************/
/* This function is used to calculate angles for _wpi_arc and _wpi_pie */
{
    float               dx, dy;
    float               angle;
    SHORT               whole;
//    USHORT            fract;

    dx = (float) x - cen->x;
    dy = (float) y - cen->y;

    angle = asin( dy / (sqrt( dx * dx + dy * dy )) ) * 180.0 / 3.14159265;
    if( dx < 0.0 ) {
        angle = 180.0 - angle;
    }

    if( angle < 0.0 ) {
        angle += 360.0;
    }

    whole = (SHORT) angle;
    /* Fractional part doesn't seem to give us any great advantage */
    /* In fact, it seems to cause more off by one errors */
//    fract = (USHORT) ((float) (angle - (float) whole) * 65536);

    return( MAKEFIXED( whole, 0 ) );
}

static void _OldBrush( WPI_PRES pres, WPI_OBJECT* brush )
/**************************************/
/* The function is used to determine the type of and retrieve the
   old brush. */
{
    GpiQueryAttrs( pres, PRIM_AREA,
                   ABB_COLOR | ABB_SYMBOL | ABB_MIX_MODE | ABB_SET,
                   &(brush->brush.info) );

    if( brush->brush.info.usSet > 0 ) {
        /* if the pattern is not the default pattern
           then it is a pattern brush*/
        brush->type = WPI_PATBRUSH_OBJ;
    } else {
        /* Otherwise, it only a regular brush */
        brush->type = WPI_BRUSH_OBJ;
    }
}

        /*---------------------------------------------------------
         * The following functions have Windows equivalents
         *---------------------------------------------------------*/

void GetWindowRect( HWND hwnd, WPI_RECT *rect )
/**********************************************************************/
{
    WinQueryWindowRect( hwnd, rect );
    WinMapWindowPoints( hwnd, HWND_DESKTOP, (WPI_POINT *)rect, 2L );
} /* GetWindowRect */

void GetClientRect( HWND hwnd, WPI_RECT *prect )
{
    WPI_RECT    screen;
    BOOL        ret;

    WinQueryWindowRect( hwnd, prect );
    WinMapWindowPoints( hwnd, HWND_DESKTOP, (POINTL *)prect, 2 );
    screen = *prect;
    ret = WinCalcFrameRect( hwnd, prect, TRUE );
    prect->xRight   = prect->xRight - prect->xLeft;
    prect->yTop     = prect->yTop - prect->yBottom;
    if ( ret ) {
        prect->xLeft    = prect->xLeft - screen.xLeft;
        prect->yBottom  = prect->yBottom - screen.yBottom;
        prect->xRight  += prect->xLeft;
        prect->yTop    += prect->yBottom;
    } else {
        prect->xLeft    = 0;
        prect->yBottom  = 0;
    }

}

        /*---------------------------------------------------------
         * The following functions have the WPI naming convention.
         *---------------------------------------------------------*/

BOOL _wpi_showwindow( HWND hwnd, int state )
/******************************************/
{
    BOOL                ret = TRUE;
    if( state == SW_HIDE ) {
        ret = ret && WinShowWindow( hwnd, FALSE );
    } else {
        ret = ret && WinShowWindow( hwnd, TRUE );
    }
    return( ret && WinSetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, state ) );
}

HINI _wpi_openinifile( WPI_INST inst, char *name )
/************************************************/
{
    char                *ptr;
    char                *end;
    PRFPROFILE          prof;
    HINI                ret;

    ptr = name;
    if( strchr( name, '\\' ) == NULL && strchr( name, ':' ) == NULL ) {
        /* no path; use the system INI directory to prevent OS/2
           from creating multiple INI files for each program */

        prof.cchUserName = 0;
        prof.cchSysName = 0;
        prof.pszSysName = NULL;
        prof.pszUserName = NULL;
        if( PrfQueryProfile( inst.hab, &prof ) ) {
            ptr = _wpi_malloc( prof.cchSysName + strlen( name ) + 1 );
            prof.pszSysName = ptr;
            prof.pszUserName = _wpi_malloc( prof.cchUserName );
            PrfQueryProfile( inst.hab, &prof );
            end = strrchr( ptr, '\\' );
            if( end != NULL ) {
                strcpy( end + 1, name );
            } else {
                strcpy( ptr, name );
            }
        }
    }

    ret = PrfOpenProfile( inst.hab, ptr );

    if( prof.pszSysName != NULL ) {
        _wpi_free( prof.pszSysName );
        _wpi_free( prof.pszUserName );
    }

    return( ret );
}

void _wpi_arc( WPI_PRES pres, int xx1, int yy1, int xx2, int yy2,
                                    int xx3, int yy3, int xx4, int yy4 )
/**********************************************************************/
{
    WPI_POINT           cen;
    WPI_POINT           vec;
    WPI_POINT           pt;
    ARCPARAMS           arcp;
    FIXED               start, end;
    unsigned long       a, b;           /* Length of axis of the ellipse */
    float               val;
    float               ratio;

    a = abs( xx2 - xx1 ) / 2;
    b = abs( yy2 - yy1 ) / 2;

    arcp.lP = a;
    arcp.lQ = b;
    arcp.lR = 0;
    arcp.lS = 0;
    GpiSetArcParams( pres, &arcp );

    cen.x = xx1 + (xx2 - xx1) / 2;
    cen.y = yy1 + (yy2 - yy1) / 2;

    start = calc_angle( &cen, xx3, yy3 );
    end = calc_angle( &cen, xx4, yy4 );

    /* Calculate the swing angle */
#if 0
    /* Using the fractional part seems to give us no advantage */
    /* In fact, it seems to cause more off by one errors */
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    }
#else
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ), 0 );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ), 0 );
    }
#endif

    /* We want a^2 and b^2 from now on */
    a *= a;
    b *= b;
    vec.x = xx3 - cen.x;
    vec.y = yy3 - cen.y;

    val = (float)(a * b) / (float)((unsigned) (b * vec.x * vec.x) +
                                           (unsigned) (a * vec.y * vec.y) );
    ratio = sqrt( val );
    pt.x = (ratio * vec.x) + cen.x;
    pt.y = (ratio * vec.y) + cen.y;

    GpiSetCurrentPosition( pres, &pt );
    GpiPartialArc( pres, &cen, MAKEFIXED( 1, 0 ), start, end );
} /* _wpi_arc */

void _wpi_bitblt( WPI_PRES dest, int x_dest, int y_dest, int cx, int cy,
                        WPI_PRES source, int x_src, int y_src, LONG format )
/**********************************************************************/
/* The x and y values are platform dependent.  ie. (0,0) is top left  */
/* for windows and bottom left for PM.                                */
{
    POINTL      pts[3];

    pts[0].x = x_dest;
    pts[0].y = y_dest;
    pts[1].x = x_dest + cx;
    pts[1].y = y_dest + cy;
    pts[2].x = x_src;
    pts[2].y = y_src;
    GpiBitBlt( dest, source, 3, pts, format, BBO_IGNORE );
} /* _wpi_bitblt */

WPI_HANDLE _wpi_createcompatiblebitmap( WPI_PRES pres, int width, int height )
/**********************************************************************/
/* Note that the bitmap returned is of type WPI_OBJECT.  So we declare*/
/* the routine to be void*.                                           */
{
    WPI_BITMAP          bmih;
    LONG                formats[24];
    WPI_OBJECT          *obj;

    memset( &bmih, 0, sizeof(WPI_BITMAP) );
    GpiQueryDeviceBitmapFormats( pres, 24L, formats );
    bmih.cbFix = sizeof( WPI_BITMAP );
    bmih.cx = width;
    bmih.cy = height;
    bmih.cPlanes = (USHORT) formats[0];
    bmih.cBitCount = (USHORT) formats[1];

    obj = _wpi_malloc( sizeof(WPI_OBJECT) );
    obj->type = WPI_BITMAP_OBJ;
    obj->bitmap = GpiCreateBitmap( pres, &bmih, 0L, NULL, NULL );

    return( (WPI_HANDLE)obj );
}

WPI_PRES _wpi_createcompatiblepres( WPI_PRES pres, WPI_INST inst, HDC *hdc )
/**********************************************************************/
{
    WPI_PRES            mempres;
    SIZEL               sizl = { 0, 0 };
    DEVOPENSTRUC        dop = { 0L, "DISPLAY", NULL, 0L,
                                0L, 0L, 0L, 0L, 0L };

    pres = pres;                        // PM doesn't use this variable
    *hdc = DevOpenDC( inst.hab, OD_MEMORY, "*", 5L,
                                        (PDEVOPENDATA)&dop, NULLHANDLE );
    mempres = GpiCreatePS( inst.hab, *hdc, &sizl, PU_PELS | GPIA_ASSOC );
    return( mempres );
} /* _wpi_createcompatiblepres */

WPI_PRES _wpi_createos2normpres( WPI_INST inst, HWND hwnd )
/**********************************************************************/
{
    SIZEL               sizel = { 0, 0 };
    WPI_PRES            hps;

    hps = GpiCreatePS( inst.hab, WinOpenWindowDC( hwnd ), &sizel,
                            PU_PELS | GPIF_LONG | GPIT_NORMAL | GPIA_ASSOC );
    return( hps );
} /* _wpi_createos2normpres */

void _wpi_deletecompatiblepres( WPI_PRES pres, HDC hdc )
/**********************************************************************/
{
    GpiDestroyPS( pres );
    DevCloseDC( hdc );
} /* _wpi_deletecompatiblepres */

void _wpi_deleteos2normpres( WPI_PRES pres )
/**********************************************************************/
{
    GpiAssociate( pres, NULLHANDLE );
    GpiDestroyPS( pres );
} /* _wpi_deleteos2normpres */

void _wpi_deletepres( WPI_PRES pres, HDC hdc )
/**********************************************************************/
{
    GpiDestroyPS( pres );
    DevCloseDC( hdc );
} /* _wpi_deletepres */

int _wpi_dialogbox( HWND parent, WPI_PROC proc, WPI_INST inst, int res_id,
                                                                void *data )
/**********************************************************************/
{
    HWND                new_dlg;
    int                 ret;

    new_dlg = WinLoadDlg( HWND_DESKTOP, parent, proc, inst.mod_handle,
                                            (ULONG)res_id, (PVOID)data );
    if( new_dlg == (HWND)NULL ) {
        return( -1 );
    }
    ret = WinProcessDlg( new_dlg );
    WinDestroyWindow( new_dlg );
    return( ret );
}

void _wpi_drawfocusrect( WPI_PRES pres, WPI_RECT *rect )
/**********************************************************************/
{
    POINTL      pt;
    LONG        old_mix;
    LONG        old_back_mix;

    pt.x = rect->xLeft;
    pt.y = rect->yTop;

    GpiSetCurrentPosition( pres, &pt );
    pt.x = rect->xRight - 1;
    pt.y = rect->yBottom + 1;

    old_mix = GpiQueryMix( pres );
    old_back_mix = GpiQueryBackMix( pres );
    GpiSetMix( pres, FM_XOR );
    GpiSetBackMix( pres, FM_LEAVEALONE );
    GpiBox( pres, DRO_OUTLINEFILL, &pt, 0L, 0L );
    GpiSetMix( pres, old_mix );
    GpiSetBackMix( pres, old_back_mix );
} /* _wpi_drawfocusrect */

BOOL _wpi_ellipse( WPI_PRES pres, int left, int top, int right, int bottom )
/*******************************************************************/
{
    POINTL      pt;
    LONG        width;
    LONG        height;
    BOOL        ret = TRUE;

    width = abs(right - left);
    height = abs(top - bottom);
    pt.x = left;
    pt.y = top;
    if ( !GpiSetCurrentPosition(pres, &pt) ) ret = FALSE;
    pt.x = right - 1;
    pt.y = bottom + 1;

    if ( !GpiBox(pres, DRO_OUTLINEFILL, &pt, width, height) ) ret = FALSE;

    return( ret );
} /* _wpi_ellipse */

void _wpi_fillrect( WPI_PRES pres, WPI_RECT *rect, WPI_COLOUR colour, HBRUSH brush )
/*******************************************************************/
{
    RECTL       newrect;

    brush = brush;                      // PM doesn't use this
    newrect.xLeft = rect->xLeft;
    newrect.xRight = rect->xRight;
    newrect.yTop = rect->yTop;
    newrect.yBottom = rect->yBottom;
    WinFillRect( pres, (PRECTL)&newrect, colour );
} /* _wpi_fillrect */

void _wpi_getbitmapdim( WPI_HANDLE hbmp, int *pwidth, int *pheight )
/*******************************************************************/
{
    BITMAPINFOHEADER    bih;
    WPI_OBJECT          *obj;

    obj = (WPI_OBJECT *)hbmp;
    bih.cbFix = sizeof( BITMAPINFOHEADER );
    GpiQueryBitmapParameters( obj->bitmap, &bih );
    *pwidth = bih.cx;
    *pheight = bih.cy;
} /* _wpi_getbitmapdim */

BOOL _wpi_getclassinfo( WPI_INST inst, PSZ name, WPI_WNDCLASS *info )
/*******************************************************************/
/* Since there is no WNDCLASS structure in OS/2, we made one. But this
   function must then be defined */
{
    CLASSINFO           ci;
    BOOL                ret;

    ret = WinQueryClassInfo( inst.hab, name, &ci );
    if( ret ) {
        info->style = ci.flClassStyle;
        info->lpfnWndProc = ci.pfnWindowProc;
        info->cbWndExtra = ci.cbWindowData;
    }

    return( ret );
}

#ifdef __FLAT__
WPI_TASK _wpi_getcurrenttask( void )
/**********************************************************************/
{
    PTIB                pt;
    PPIB                ppib;

    DosGetInfoBlocks( &pt, &ppib );
    return( pt->tib_ptib2->tib2_ultid );
} /* _wpi_getcurrenttask */
#endif

int _wpi_getdlgitemint( HWND hwnd, int item, BOOL *retcode, BOOL issigned )
/**********************************************************************/
{
    char                buffer[20];
    char                dummy[2];
    int                 result = 0;

    issigned = issigned;                // PM doesn't use this

    *retcode = TRUE;
    if( WinQueryDlgItemText( hwnd, (ULONG)item, 20, buffer ) != 0 ) {
        if( sscanf( buffer, "%d%1s", &result, dummy ) != 1) {
            result = 0;
            *retcode = FALSE;
        }
    } else {
        *retcode = FALSE;
    }

    return( result );
} /* _wpi_getdlgitemint */

void _wpi_getpaintrect( PAINTSTRUCT *ps, WPI_RECT *rect )
/**********************************************************************/
{
    rect->xLeft = ps->xLeft;
    rect->xRight =  ps->xRight;
    rect->yBottom =  ps->yTop;
    rect->yTop =  ps->yBottom;
} /* _wpi_getpaintrect */

WPI_COLOUR _wpi_getpixel( WPI_PRES hps, int x, int y )
/**********************************************************************/
{
    POINTL      pt;

    pt.x = x;
    pt.y = y;
    return( GpiQueryPel(hps, &pt) );
}

WPI_PRES _wpi_getpres( HWND hwnd )
/**********************************************************************/
{
    HDC         wpi_hdc;
    WPI_PRES    pres;

    pres = WinGetPS( hwnd );
    wpi_hdc = GpiQueryDevice( pres );
    GpiAssociate( pres, wpi_hdc );
    return( pres );
} /* _wpi_getpres */

void _wpi_getwindowrect( HWND hwnd, WPI_RECT *rect )
/**********************************************************************/
{
    GetWindowRect( hwnd, rect );
} /* _wpi_getwindowrect */

void _wpi_hitrect( int xc, int yc, int ytop, int ybottom, int xleft,
                                            int xright, int mxp, int myp )
/**********************************************************************/
{
    float               y_fract = 0;
    float               x_fract = 0;

    y_fract = abs( myp - yc ) / ( ytop - ybottom );
    x_fract = abs( mxp - xc ) / ( xright - xleft );

    if( y_fract >= x_fract ) {
        if( myp > yc ) {
            mxp += ( ytop - yc ) / ( myp - yc ) * ( xc - mxp );
            myp = ytop;
        } else if( myp < yc ) {
            mxp += ( yc - ybottom ) / ( yc - myp ) * ( xc - mxp );
            myp = ybottom;
        }
    } else {
        if( mxp > xc ) {
            myp += ( xright - xc ) / ( mxp - xc ) * ( myp - yc );
            mxp = xright;
        } else if( mxp < xc ) {
            myp += ( xc - xright ) / ( xc - mxp ) * ( myp - yc );
            mxp = xleft;
        }
    }
} /* _wpi_hitrect */

BOOL _wpi_iszoomed( HWND hwnd )
/***********************************************************************/
{
    SWP         swp;

    WinQueryWindowPos( hwnd, &swp );

    if (PM1632SWP_FLAG( swp ) & SWP_MAXIMIZE) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

BOOL _wpi_isiconic( HWND hwnd )
/***********************************************************************/
{
    SWP         swp;

    WinQueryWindowPos( hwnd, &swp );

    if (PM1632SWP_FLAG( swp ) & SWP_MINIMIZE) {
        return( TRUE );
    } else {
        return( FALSE );
    }
 } /* _wpi_isiconic */

void _wpi_patblt( WPI_PRES dest, int x_pos, int y_pos, int cx, int cy, LONG format )
/***********************************************************************/
/* NOTE: for PM version, y_pos is taken to be the lower left corner,   */
/* not the upper left as it would be for the windows version.          */
{
    HPS         hps;
    POINTL      pts[3];

    hps = WinGetScreenPS( HWND_DESKTOP );
    _wpi_torgbmode( hps );
    pts[0].x = x_pos;
    pts[0].y = y_pos;
    pts[1].x = x_pos + cx;
    pts[1].y = y_pos + cy;
    pts[2].x = 0;
    pts[2].y = 0;

    GpiBitBlt( dest, hps, 3, pts, format, BBO_IGNORE );
    WinReleasePS( hps );
} /* _wpi_patblt */

void _wpi_pie( WPI_PRES pres, int xx1, int yy1, int xx2, int yy2,
                                     int xx3, int yy3, int xx4, int yy4 )
/***********************************************************************/
{
    WPI_POINT           cen;
    ARCPARAMS           arcp;
    FIXED               start, end;

    cen.x = xx1 + ( xx2 - xx1 ) / 2;
    cen.y = yy1 + ( yy2 - yy1 ) / 2;

    start = calc_angle( &cen, xx3, yy3 );
    end = calc_angle( &cen, xx4, yy4 );

    /* Calculate the swing angle */
#if 0
    /* Using the fractional part seems to give us no advantage */
    /* In fact, it seems to cause more off by one errors */
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    }
#else
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ), 0 );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ), 0 );
    }
#endif

    arcp.lP = (xx2 - xx1) / 2;
    arcp.lQ = (yy1 - yy2) / 2;
    arcp.lR = 0;
    arcp.lS = 0;
    GpiSetArcParams( pres, &arcp );

    GpiSetCurrentPosition( pres, &cen );

    GpiBeginArea( pres, BA_BOUNDARY | BA_ALTERNATE );

    GpiPartialArc( pres, &cen, MAKEFIXED( 1, 0 ), start, end );

    GpiEndArea( pres );
} /* _wpi_pie */

#ifdef __FLAT__
BOOL _wpi_polygon( WPI_PRES pres, WPI_POINT *pts, int num_pts )
/**********************************************************************/
{
    POLYGON             poly[1];
    BOOL                ret;

    poly[0].aPointl = pts;
    poly[0].ulPoints = num_pts;

    GpiSetCurrentPosition( pres, &pts[0] );
    ret = ( GpiPolygons(pres, 1, poly, POLYGON_BOUNDARY, POLYGON_INCL) ==
                                                                    GPI_OK );
    return( ret );
} /* _wpi_polygon */
#endif

void _wpi_preparemono( WPI_PRES hps, WPI_COLOUR colour, WPI_COLOUR back_colour )
/**********************************************************************/
/* This is necessary for bltting a monochrome bitmap.  The colour     */
/* parameter should be the colour of the 1's on the bitmap (normally  */
/* white).  The back_colour parameter should be the colour of the 0's */
/* on the bitmap (normally black) Also, hps needs to be in RGB mode.  */
{
    IMAGEBUNDLE         imb;

    GpiQueryAttrs( hps, PRIM_IMAGE, IBB_BACK_COLOR | IBB_COLOR, &imb );
    imb.lBackColor = colour;
    imb.lColor = back_colour;
    GpiSetAttrs(hps, PRIM_IMAGE, IBB_COLOR | IBB_BACK_COLOR, 0, &imb);
} /* _wpi_preparemono */

BOOL _wpi_rectangle( WPI_PRES pres, int left, int top, int right, int bottom )
/**********************************************************************/
/* The coordinates are assumed to be in the order of the parameter    */
/* list.  The right and bottom are adjusted to be consisten with Win  */
{
    POINTL              pt;
    BOOL                ret;

    ret = TRUE;

    pt.x = (LONG)left;
    pt.y = (LONG)top;
    if ( !GpiSetCurrentPosition(pres, &pt) ) ret = FALSE;

    pt.x = right - 1;
    pt.y = bottom + 1;
    if ( !GpiBox(pres, DRO_OUTLINEFILL, &pt, 0L, 0L) ) ret = FALSE;

    return( ret );
} /* _wpi_rectangle */

void _wpi_releasepres( HWND hwnd, WPI_PRES pres )
/**********************************************************************/
{
    hwnd = hwnd;
    GpiAssociate( pres, NULLHANDLE );
    WinReleasePS( pres );
} /* _wpi_releasepres */

WPI_HANDLE _wpi_selectbitmap( WPI_PRES pres, WPI_HANDLE bitmap )
/**********************************************************************/
{
    WPI_OBJECT  *obj;
    WPI_OBJECT  *old_obj;

    obj = (WPI_OBJECT *)bitmap;

    if( obj != NULL ) {
        old_obj = _wpi_malloc( sizeof(WPI_OBJECT) );
        old_obj->type = WPI_BITMAP_OBJ;
        old_obj->bitmap = GpiSetBitmap( pres, obj->bitmap );
    }

    return( (WPI_HANDLE)old_obj );
} /* _wpi_selectbitmap */

void _wpi_getoldbitmap( WPI_PRES pres, WPI_HANDLE oldobj )
/********************************************************/
{
    WPI_OBJECT  *oldbitmap;

    oldbitmap = (WPI_OBJECT *)oldobj;

    if( oldbitmap && oldbitmap->type == WPI_BITMAP_OBJ ) {
        GpiSetBitmap( pres, oldbitmap->bitmap );
        _wpi_free( oldbitmap );
    }
} /* _wpi_getoldbitmap */


void _wpi_deletebitmap( WPI_HANDLE bmp )
/**********************************************************************/
{
    WPI_OBJECT  *obj;

    obj = (WPI_OBJECT *)bmp;

    if( obj != NULL ) {
        if( obj->bitmap != (HBITMAP)NULL ) {
            GpiDeleteBitmap( obj->bitmap );
        }
        _wpi_free( obj );
    }
} /* _wpi_deletebitmap */

#ifdef __FLAT__
void _wpi_setmodhandle( char *name, WPI_INST *inst )
/**********************************************************************/
{
    if( DosQueryModuleHandle( name, &(inst->mod_handle) ) != 0 ) {
        inst->mod_handle = NULLHANDLE;
    }
} /* _wpi_setmodhandle */
#endif

WPI_COLOUR _wpi_setpixel( WPI_PRES hps, int x, int y, WPI_COLOUR clr )
/**********************************************************************/
{
    LONG        oldclr;
    WPI_COLOUR  setclr;
    POINTL      pt;

    setclr = GpiQueryNearestColor( hps, 0L, clr );
    oldclr = GpiQueryColor( hps );
    if ( !oldclr ) setclr = -1;
    if ( !GpiSetColor(hps, clr) ) setclr = -1;

    pt.x = (LONG)x;
    pt.y = (LONG)y;
    if ( !GpiSetPel(hps, &pt) ) setclr = -1;
    if ( !GpiSetColor(hps, oldclr) ) setclr = -1;
    return( setclr );
} /* _wpi_setpixel */

void _wpi_setpoint( WPI_POINT *pt, int x, int y )
/**********************************************************************/
{
    pt->x = (LONG)x;
    pt->y = (LONG)y;
} /* _wpi_setpoint */

#if 0
WPI_INST _wpi_setwpiinst( HINSTANCE inst, HMODULE handle )
/**********************************************************************/
{
    WPI_INST    wpi_instance;

    wpi_instance->hab = inst;
    wpi_instance->mod_handle = handle;
} /* _wpi_setwpiinst */
#endif

void _wpi_stretchblt( WPI_PRES dest, int x_dest, int y_dest, int cx_dest,
    int cy_dest, WPI_PRES src, int x_src, int y_src, int cx_src, int cy_src, LONG rop )
/**********************************************************************/
/* NOTE:  the coordinates to this function must be in PM units AND    */
/* in PM convention.                                                  */
{
    POINTL              pts[4];

    pts[0].x = x_dest;
    pts[0].y = y_dest;
    pts[1].x = x_dest + cx_dest;
    pts[1].y = y_dest + cy_dest;
    pts[2].x = x_src;
    pts[2].y = y_src;
    pts[3].x = x_src + cx_src;
    pts[3].y = y_src + cy_src;
    GpiBitBlt(dest, src, 4L, pts, rop, BBO_IGNORE);
} /* _wpi_stretchblt */


BOOL _wpi_ptinrect( WPI_RECT *prect, WPI_POINT pt )
{
    return( ( pt.x >= prect->xLeft ) && ( pt.x < prect->xRight ) &&
            ( pt.y <= prect->yTop ) && ( pt.y > prect->yBottom ) );
}

BOOL _wpi_insertmenu( HMENU hmenu, unsigned pos, unsigned menu_flags,
                      unsigned attr_flags, unsigned id,
                      HMENU popup, char *text, BOOL by_position )
{
    MENUITEM    mi;
    MRESULT     result;
    char        *new_text;
    char        *t;
    HMENU       parent;
    unsigned    pos_in_parent;

    if( hmenu == NULLHANDLE ) {
        return( FALSE );
    }

    if( by_position ) {
        if( pos == -1 ) {
            pos = MIT_END;
        }
        parent = hmenu;
        pos_in_parent = pos;
    } else {
        if( !_wpi_getmenuparentoffset( hmenu, pos, &parent, &pos_in_parent ) ) {
            return( FALSE );
        }
    }

    mi.iPosition   = pos_in_parent;
    mi.afStyle     = menu_flags;
    mi.afAttribute = attr_flags;
    mi.id          = id;
    mi.hwndSubMenu = popup;
    mi.hItem       = NULL;

    new_text = _wpi_menutext2pm( text );

    t = NULL;
    if( new_text && *new_text ) {
        t = new_text;
    }
    result = WinSendMsg( parent, MM_INSERTITEM, MPFROMP(&mi), MPFROMP(t) );

    if( new_text ) {
        _wpi_free( new_text );
    }

    return( ( result != (MRESULT)MIT_MEMERROR ) &&
            ( result != (MRESULT)MIT_ERROR ) );
}

BOOL _wpi_appendmenu( HMENU hmenu, unsigned menu_flags,
                      unsigned attr_flags, unsigned id,
                      HMENU popup, char *text )
{
    return( _wpi_insertmenu( hmenu, -1, menu_flags, attr_flags, id, popup, text, TRUE ) );
}

BOOL _wpi_getmenustate( HMENU hmenu, unsigned id, WPI_MENUSTATE *state,
                        BOOL by_position )
{
    if( !hmenu || !state ) {
        return( FALSE );
    }
    if( by_position ) {
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    }
    if( id == -1 ) {
        return( FALSE );
    }
    return( (BOOL) WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT(id, TRUE), MPFROMP(state) ) );
}

void _wpi_getmenuflagsfromstate( WPI_MENUSTATE *state, unsigned *menu_flags,
                                 unsigned *attr_flags )
{
    *menu_flags = state->afStyle;
    *attr_flags = state->afAttribute;
}

BOOL _wpi_modifymenu( HMENU hmenu, unsigned id, unsigned menu_flags,
                      unsigned attr_flags, unsigned new_id,
                      HMENU new_popup, char *new_text, BOOL by_position )
{
    HMENU               parent;
    unsigned            pos;

    if( hmenu == NULLHANDLE ) {
        return( FALSE );
    }

    if( by_position ) {
        parent = hmenu;
        pos = id;
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    } else {
        if( !_wpi_getmenuparentoffset( hmenu, id, &parent, &pos ) ) {
            return( FALSE );
        }
    }

    _wpi_deletemenu( hmenu, id, FALSE );

    return( _wpi_insertmenu( parent, pos, menu_flags, attr_flags, new_id, new_popup, new_text, TRUE ) );
}

HMENU _wpi_getsubmenu( HMENU hmenu, unsigned id )
{
    MENUITEM    mi;

    id = _wpi_getmenuitemidfrompos( hmenu, id );
    if( id == -1 ) {
        return( NULL );
    }
    if( WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT(id, TRUE), MPFROMP(&mi) ) ) {
        if( mi.afStyle & MIS_SUBMENU ) {
            return( mi.hwndSubMenu );
        }
    }
    return( NULL );
}

HMENU _wpi_getsystemmenu( HWND hwnd )
{
    HWND        syshwnd;
    MENUITEM    mi;

    syshwnd = WinWindowFromID( hwnd, FID_SYSMENU );
    if( syshwnd && WinSendMsg( syshwnd, MM_QUERYITEM,
                               MPFROM2SHORT(SC_SYSMENU, TRUE),
                               MPFROMP((PSZ)&mi) ) ) {
        return( mi.hwndSubMenu );
    }
    return( NULL );
}

BOOL _wpi_setmenu( HWND hwnd, HMENU hmenu )
{
    HWND        old_menu;

    if( !hwnd ) {
        return( FALSE );
    }

    old_menu = WinWindowFromID( hwnd, FID_MENU );
    if( old_menu ) {
        WinSetWindowUShort( old_menu, QWS_ID, 0 );
        WinDestroyWindow ( old_menu );
    }
    if( hmenu ) {
        WinSetWindowUShort( hmenu, QWS_ID, FID_MENU );
        WinSetWindowULong( hmenu, QWL_STYLE, WS_VISIBLE | WS_SYNCPAINT |
                                             MS_ACTIONBAR | 0x00000008 );
        WinSetParent( hmenu, hwnd, FALSE );
        WinSetOwner( hmenu, hwnd );

        _wpi_drawmenubar( hwnd );
    }

    return( TRUE );
}

BOOL _wpi_deletemenu( HMENU hmenu, unsigned id, BOOL by_position )
{
    if( !hmenu ) {
        return( FALSE );
    }
    if( by_position ) {
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    }
    if( id == -1 ) {
        return( FALSE );
    }
    return( (BOOL) WinSendMsg( hmenu, MM_DELETEITEM, MPFROM2SHORT(id, TRUE), NULL ) );
}

BOOL _wpi_checkmenuitem( HMENU hmenu, unsigned id,
                         BOOL fchecked, BOOL by_position )
{
    if( by_position ) {
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    }
    if( id == -1 ) {
        return( FALSE );
    }
    return ( _wpi_setmenuitemattr( hmenu, id, MIA_CHECKED, (fchecked)? MIA_CHECKED : ~MIA_CHECKED ) );
}

BOOL _wpi_enablemenuitem( HMENU hmenu, unsigned id,
                         BOOL fenabled, BOOL by_position )
{
    if( by_position ) {
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    }
    if( id == -1 ) {
        return( FALSE );
    }
    return ( _wpi_setmenuitemattr( hmenu, id, MIA_DISABLED, (fenabled)? ~MIA_DISABLED : MIA_DISABLED ) );
}

BOOL _wpi_setmenutext( HMENU hmenu, unsigned id, char *text, BOOL by_position )
{
    BOOL        ret;
    char        *new_text;

    if( !hmenu ) {
        return( FALSE );
    }

    if( by_position ) {
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    }
    if( id == -1 ) {
        return( FALSE );
    }

    new_text = _wpi_menutext2pm( text );

    ret = (BOOL) WinSendMsg( hmenu, MM_SETITEMTEXT, (WPI_PARAM1)id, MPFROMP(new_text) );

    if( new_text ) {
        _wpi_free( new_text );
    }

    return( ret );
}

BOOL _wpi_getmenutext( HMENU hmenu, unsigned id, char *text, int ctext,
                       BOOL by_position )
{
    BOOL        ret;

    if( !hmenu || !text ) {
        return( FALSE );
    }
    text[0] = '\0';
    if( by_position ) {
        id = _wpi_getmenuitemidfrompos( hmenu, id );
    }
    if( id == -1 ) {
        return( FALSE );
    }
    ret = (BOOL) WinSendMsg( hmenu, MM_QUERYITEMTEXT, MPFROM2SHORT(id, ctext), MPFROMP(text) );
    if ( ret ) {
        _wpi_menutext2win( text );
    }
    return( ret );
}

UINT _wpi_getmenuitemid( HMENU hmenu, unsigned pos )
{
    return( _wpi_getmenuitemidfrompos( hmenu, pos ) );
}

WPI_FONT _wpi_selectfont( WPI_PRES hps, WPI_FONT wfont )
{
    FATTRS              *oldwfont;
    FONTMETRICS         fm;
    SIZEF               box;
    WPI_FONT            tmp_wfont;
    WPI_FONT            tmp_font;
    int                 pix_per_inch;
    int                 pix_per_point;
    LONG                num_fonts;
    LONG                ltemp = 0L;
    PFONTMETRICS        pfm;
    int                 i, diff;
    int                 closest = -1;
    int                 selected = -1;
    int                 point_size_x, point_size_y;
//    LONG              matched;

    GpiQueryFontMetrics( hps, sizeof(FONTMETRICS), &fm );
    oldwfont = (FATTRS *) _wpi_malloc( sizeof(FATTRS) );
    if( oldwfont ) {
        _wpi_getfontattrs( &fm, oldwfont );
    }

    _wpi_malloc2( tmp_wfont, 1 );
    memcpy( tmp_wfont, wfont, sizeof( FATTRS ) );
    GpiSetCharSet( hps, 0L );
    GpiDeleteSetId( hps, 1L );
    if( (tmp_wfont)->szFacename == NULL
                    || strlen( (tmp_wfont)->szFacename ) < 1 ) {

        /* get default font */

        tmp_font = _wpi_getsystemfont();
        memcpy( tmp_wfont, tmp_font, sizeof( FATTRS ) );
        tmp_wfont->lMaxBaselineExt = SysFontHeight;
        tmp_wfont->lAveCharWidth = SysFontWidth;
        if( GpiCreateLogFont( (WPI_PRES) hps, (PSTR8) NULL, 1L,
                            (PFATTRS) tmp_wfont ) == FONT_MATCH ) {
            goto found;
        }
        GpiDeleteSetId( hps, 1L );
        goto notfound;
    }

    /* Conceed defeat, look for it manually */
    num_fonts = GpiQueryFonts( (HPS) hps,
                (ULONG) QF_PUBLIC | QF_PRIVATE,
                (PSZ) (wfont)->szFacename,
                (PLONG) &ltemp, (LONG) sizeof( FONTMETRICS ),
                (PFONTMETRICS) NULL );
    if( num_fonts != 0 && num_fonts != GPI_ALTERROR ) {
        _wpi_malloc2( pfm, num_fonts );
        ltemp = GpiQueryFonts( (HPS) hps,
                    (ULONG) QF_PUBLIC | QF_PRIVATE,
                    (PSZ) (wfont)->szFacename,
                    (PLONG) &num_fonts,
                    (LONG) sizeof( FONTMETRICS ),
                    (PFONTMETRICS) pfm );
        for( i = 0; i < num_fonts; i++ ) {
            if( pfm[i].fsDefn & FM_DEFN_OUTLINE
                        && pfm[i].fsDefn & FM_DEFN_GENERIC ) {
#if 0
                _wpi_getfontattrs( &pfm[i], tmp_wfont );
                (tmp_wfont)->fsFontUse = FATTR_FONTUSE_OUTLINE;
                (tmp_wfont)->fsType = 0;
#endif
                memset( tmp_wfont, 0, sizeof( FATTRS ) );
                (tmp_wfont)->usRecordLength = sizeof( FATTRS );
                (tmp_wfont)->fsFontUse = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_NOMIX;
                (tmp_wfont)->idRegistry = pfm[i].idRegistry;
                strcpy( (tmp_wfont)->szFacename, pfm[i].szFacename );
                (tmp_wfont)->lMatch = pfm[i].lMatch;
                (tmp_wfont)->fsSelection = (wfont)->fsSelection;
                (tmp_wfont)->usCodePage = pfm[i].usCodePage;
                (tmp_wfont)->fsType = 0;
                (tmp_wfont)->lMaxBaselineExt = pfm[i].lMaxBaselineExt;
                (tmp_wfont)->lAveCharWidth = pfm[i].lAveCharWidth;
                if( GpiCreateLogFont( (WPI_PRES) hps, (PSTR8) NULL, (LONG) 1L,
                        (PFATTRS) tmp_wfont ) == FONT_MATCH ) {
                    _wpi_free( pfm );
                    goto found;
                }
                GpiDeleteSetId( hps, 1L );
            }
            if( closest != 0 ) {
                diff = abs( pfm[i].sNominalPointSize / 10 - (wfont)->lMaxBaselineExt );
                if( diff < closest || closest == -1 ) {
                    closest = diff;
                    selected = i;
                }
            }
            if( pfm[i].lMatch == (wfont)->lMatch ) {
                selected = i;
                i = num_fonts;
                break;
            }
        }
        if( selected > 0 ) {
            _wpi_getfontattrs( &pfm[selected], tmp_wfont );
            (tmp_wfont)->fsType = 0;
            (tmp_wfont)->fsSelection = (wfont)->fsSelection;
            if( GpiCreateLogFont( (WPI_PRES) hps, (PSTR8) NULL, (LONG) 1L,
                                    (PFATTRS) tmp_wfont ) == FONT_MATCH ) {
                _wpi_free( pfm );
                goto found;
            }
        }
        _wpi_free( pfm );
        goto notfound;
    }

 notfound:

    GpiSetCharSet( hps, LCID_DEFAULT );
    _wpi_free( tmp_wfont );
    return( (WPI_FONT)oldwfont );
 found:

    _wpi_free( tmp_wfont );

    GpiSetCharSet( hps, 1L );

    /* Critical for printing                            */
    pix_per_inch = min( _wpi_devicecapableinch( hps, LOGPIXELSX ),
                    _wpi_devicecapableinch( hps, LOGPIXELSY ) );
    pix_per_point = ( pix_per_inch + 71 ) / 72;

    /* Set the font size ( for outline/sizeable fonts ) */
    point_size_x = ( wfont->lAveCharWidth ) * pix_per_point;
    point_size_y = ( wfont->lMaxBaselineExt ) * pix_per_point;
    box.cx = MAKEFIXED( point_size_x, 0 );
    box.cy = MAKEFIXED( point_size_y, 0 );
    GpiSetCharBox( hps, &box );

    return( (WPI_FONT)oldwfont );
}

void _wpi_getfontattrs( FONTMETRICS *fm, WPI_FONT attr )
{
    attr->usRecordLength = sizeof( FATTRS );
    attr->fsSelection = fm->fsSelection;
    attr->lMatch = fm->lMatch;
    if( fm->szFacename != NULL ) {
        strcpy( attr->szFacename, fm->szFacename );
    } else {
        attr->szFacename[0] = '\0';
    }
    attr->idRegistry = fm->idRegistry;
    attr->usCodePage = fm->usCodePage;
    attr->fsType = fm->fsType;
    attr->lMaxBaselineExt = fm->lMaxBaselineExt;
    attr->lAveCharWidth = fm->lAveCharWidth;
    if( fm->fsDefn & FM_DEFN_OUTLINE ) {
        attr->fsFontUse = FATTR_FONTUSE_OUTLINE;
    } else {
        attr->fsFontUse = 0;
    }
}

BOOL _wpi_setscrollpos( HWND parent, int scroll, int pos, BOOL redraw )
{
    HWND        scroll_bar;

    redraw = redraw; // scroll bar always redrawn

    scroll_bar =  _wpi_getscrollhwnd( parent, scroll );

    if( scroll_bar == NULLHANDLE ) {
        scroll_bar = parent;
    }
    return( (BOOL)(WinSendMsg(scroll_bar, SBM_SETPOS, MPFROMSHORT( pos ), 0)) );
}

int _wpi_getscrollpos( HWND parent, int scroll )
{
    HWND        scroll_bar;

    scroll_bar =  _wpi_getscrollhwnd( parent, scroll );
    if( scroll_bar == NULLHANDLE ) {
        scroll_bar = parent;
    }
    return( (int) WinSendMsg( scroll_bar, SBM_QUERYPOS, 0, 0 ) );
}

BOOL _wpi_setscrollrange( HWND parent, int scroll, int min, int max, BOOL redraw )
{
    HWND        scroll_bar;
    SHORT       pos;

    redraw = redraw; // scroll bar always redrawn

    scroll_bar =  _wpi_getscrollhwnd( parent, scroll );
    if( scroll_bar == NULLHANDLE ) {
        scroll_bar = parent;
    }
    pos = (SHORT) WinSendMsg( scroll_bar, SBM_QUERYPOS, 0, 0 );
    return( (BOOL) WinSendMsg( scroll_bar, SBM_SETSCROLLBAR, MPFROMSHORT(pos),
                               MPFROM2SHORT( min, max ) ) );
}

BOOL _wpi_getscrollrange( HWND parent, int scroll, int *min, int *max )
{
    MRESULT     ret;
    HWND        scroll_bar;

    scroll_bar =  _wpi_getscrollhwnd( parent, scroll );
    if( scroll_bar == NULLHANDLE ) {
        scroll_bar = parent;
    }
    ret = WinSendMsg( scroll_bar, SBM_QUERYRANGE, 0, 0 );
    *min = SHORT1FROMMP( ret );
    *max = SHORT2FROMMP( ret );
    return( TRUE );
}

BOOL _wpi_drawtext( WPI_PRES hps, char *text, int len,
                    WPI_RECT *rect, unsigned format )
{
    WinDrawText( hps, len, text, rect, 0, 0, (ULONG)(format | DT_TEXTATTRS) );
    return( TRUE );
}

#define GETSYSFONT_DIST(i) (abs( fonts[i].lEmHeight - 10 ) )
WPI_FONT _wpi_getsystemfont( void )
{
    WPI_PRES    pres;
    LONG        num_fonts;
    FONTMETRICS fonts[NUM_SYS_FONTS];
    int         i;
    int         selected_font;
    int         closest_match;

    if( !GotSysFont ) {
        pres = _wpi_getpres( HWND_DESKTOP );
        num_fonts = NUM_SYS_FONTS;
        selected_font = -1;
        closest_match = 0;

        GpiQueryFonts( pres, QF_PUBLIC, sys_font_facename, &num_fonts,
                       sizeof( FONTMETRICS ), fonts );
        _wpi_releasepres( HWND_DESKTOP, pres );

        for( i = 0; i < num_fonts; ++i ) {
            if( fonts[i].lEmHeight == IDEAL_SYSFONT_HEIGHT ) {
                selected_font = i;
                break;
            } else {
                if( GETSYSFONT_DIST(closest_match) > GETSYSFONT_DIST(i) ) {
                    closest_match = i;
                }
            }
        }

        if( selected_font == -1 ) {
            if( num_fonts ) {
                selected_font = closest_match;
            }
        }

        if( selected_font != -1 ) {
            _wpi_getfontattrs( &fonts[selected_font], &SysFont );
            SysFont.fsType = 0;
            SysFont.fsSelection = 0;
            SysFontHeight = SysFont.lMaxBaselineExt;
            SysFontWidth = SysFont.lAveCharWidth;
            SysFont.lMaxBaselineExt = IDEAL_SYSFONT_HEIGHT;
            SysFont.lAveCharWidth = IDEAL_SYSFONT_HEIGHT;
            GotSysFont = TRUE;
        } else {
            return( NULL );
        }

    }

    return( &SysFont );
}

void _wpi_drawmenubar( HWND hwnd )
{
    WinSendMsg( hwnd, WM_UPDATEFRAME, (WPI_PARAM1)FCF_MENU, NULL );
}

// NOTE: This function will return HWND_DESKTOP if the parent is the desktop
//       and NOT the window handle of the desktop!! Most PM functions accept
//       HWND_DESKTOP instead the desktop HWND.
//       This change is not duplicated for _wpi_getowner!!
HWND _wpi_getparent( HWND hwnd )
{
    HWND        parent;
    HAB         ab;

    parent = PM1632WinQueryWindow( hwnd, QW_PARENT );

    ab = WinQueryAnchorBlock( hwnd );
    if( ab != NULLHANDLE ) {
        if( WinQueryDesktopWindow( ab, NULLHANDLE ) == parent ) {
            parent = HWND_DESKTOP;
        }
    }

    return( parent );
}

HCURSOR _wpi_setcursor( HCURSOR newcursor )
/****************************************************************/
{
    HCURSOR     oldcursor;

    oldcursor = WinQueryPointer( HWND_DESKTOP );
    WinSetPointer( HWND_DESKTOP, newcursor );

    return( oldcursor );
} /* _wpi_setcursor */

void _wpi_enddialog( HWND hwnd, ULONG result )
{
    WinDismissDlg( hwnd, result );
}

HBRUSH _wpi_createnullbrush( void )
/**********************************************************************/
{
    WPI_OBJECT  *null_brush;

    null_brush = _wpi_malloc( sizeof(WPI_OBJECT) );
    memset( null_brush, 0, sizeof(WPI_OBJECT) );
    null_brush->type = WPI_NULLBRUSH_OBJ;
    null_brush->brush.info.usSymbol = PATSYM_BLANK;
    null_brush->brush.info.usMixMode = FM_LEAVEALONE;
    return( (HBRUSH)null_brush );
} /* _wpi_createnullbrush */

HBRUSH _wpi_createpatternbrush( WPI_HANDLE bitmap )
/**********************************************************************/
{
    WPI_OBJECT          *obj;
    WPI_OBJECT          *brush;

    obj = (WPI_OBJECT *)bitmap;
    brush = _wpi_malloc( sizeof(WPI_OBJECT) );
    brush->type = WPI_PATBRUSH_OBJ;
    brush->brush.info.usMixMode = FM_OVERPAINT;
    brush->brush.bitmap = obj->bitmap;
    brush->brush.info.usBackMixMode = BM_OVERPAINT;
    brush->brush.info.usSet = PATBRUSHID;

    return( (HBRUSH)brush );
} /* _wpi_createpatternbrush */


HBRUSH _wpi_createsolidbrush( WPI_COLOUR colour )
/**********************************************************************/
{
    WPI_OBJECT  *new_brush;

    new_brush = _wpi_malloc( sizeof(WPI_OBJECT) );
    memset( new_brush, 0, sizeof(WPI_OBJECT) );

    new_brush->type = WPI_BRUSH_OBJ;
    new_brush->brush.info.usSymbol = PATSYM_SOLID;
    new_brush->brush.info.lColor = colour;
    new_brush->brush.info.usMixMode = FM_OVERPAINT;

    return( (HBRUSH)new_brush );
} /* _wpi_createsolidbrush */

void _wpi_getoldbrush( WPI_PRES pres, HBRUSH oldobj )
/***************************************************/
{
    WPI_OBJECT  *oldbrush;

    oldbrush = (WPI_OBJECT *)oldobj;
    if( oldbrush->type == WPI_PATBRUSH_OBJ ) {
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_SET,
                                                0L, &(oldbrush->brush.info) );
    } else {
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_SYMBOL | ABB_MIX_MODE,
                                                0L, &(oldbrush->brush.info) );
    }
    _wpi_free( oldbrush );
} /* _wpi_getoldbrush */

void _wpi_setlogbrushsolid( LOGBRUSH *brush )
/*******************************************/
{
    brush->usMixMode = FM_OVERPAINT;
    brush->usSymbol = PATSYM_SOLID;
} /* _wpi_setlogbrushsolid */

void _wpi_setlogbrushnull( LOGBRUSH *brush )
/******************************************/
{
    brush->usMixMode = FM_LEAVEALONE;
#if 0
    // This should explicitly null ou the background
    brush->usSymbol = PATSYM_SOLID;
#else
    // this probably leaves the background area alone
    brush->usSymbol = PATSYM_BLANK;
#endif
} /* _wpi_setlogbrushnull */

void _wpi_setlogbrushhollow( LOGBRUSH *brush )
/********************************************/
{
    brush->usMixMode = FM_OVERPAINT;
    brush->usSymbol = PATSYM_NOSHADE;
} /* _wpi_setlogbrushhollow */

HBRUSH _wpi_createbrush( LOGBRUSH *log_brush )
/********************************************/

{
    WPI_OBJECT          *brush;

    brush = (WPI_OBJECT *)_wpi_malloc( sizeof( WPI_OBJECT ) );
    brush->brush.info = *log_brush;

    if( log_brush->usMixMode == FM_LEAVEALONE ) {
        brush->type = WPI_NULLBRUSH_OBJ;
    } else {
        switch( log_brush->usSymbol ) {

        case PATSYM_NOSHADE:
            /* OS/2 has no concept of a hollow brush like Windows. The best
               equivalent is to use a pattern brush, drawn in the background
               colour. I tried a solid brush, and it didn't work. See
               'selectbrush/object' for the other changes nesc. djp */
            brush->type = WPI_HLWBRUSH_OBJ;
            brush->brush.info.usSymbol = BS_HATCHED;
            break;

        case PATSYM_BLANK:
            brush->type = WPI_NULLBRUSH_OBJ;
            break;

        default:
            /*
            case PATSYM_HATCH:
            case PATSYM_SOLID:
            case PATSYM_DEFAULT:
            */
            brush->type = WPI_BRUSH_OBJ;
            break;
        }
    }

    return( (HBRUSH) brush );
}

HBRUSH _wpi_selectbrush( WPI_PRES pres, HBRUSH obj )
/**************************************************/
{
    WPI_OBJECT  *brush;
    WPI_OBJECT  *old_brush;

    brush = (WPI_OBJECT *)obj;
    old_brush = _wpi_malloc( sizeof(WPI_OBJECT) );
    _OldBrush( pres, old_brush );

    switch( brush->type ) {

    case WPI_HLWBRUSH_OBJ:
        _wpi_setlogbrushcolour( &(brush->brush.info), GetBkColor( pres ) );
        /**** FALL THROUGH INTO WPI_BRUSH_OBJ ****/

    case WPI_BRUSH_OBJ:         /* Solid brushes */
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_SYMBOL,
                                                0L, &(brush->brush.info) );
        break;

    case WPI_PATBRUSH_OBJ:      /* Pattern brushes */
        GpiSetPatternSet( pres, 0 ); /* Reset the selected pattern to default */
        /* set background and foreground colors */
        brush->brush.info.lColor = GpiQueryColor( pres );
        brush->brush.info.lBackColor = GpiQueryBackColor( pres );
        /* give the bitmap an id */
        GpiSetBitmapId(pres, brush->brush.bitmap, brush->brush.info.usSet );
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_BACK_COLOR |
                           ABB_MIX_MODE | ABB_BACK_MIX_MODE | ABB_SET,
                                                  0L, &(brush->brush.info) );
        break;

    case WPI_NULLBRUSH_OBJ:     /* NULL brushes */
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_MIX_MODE,
                                                0L, &(brush->brush.info) );
        break;
    default :
        return( ( HBRUSH ) NULL );
    }

    return( (HBRUSH)old_brush );
} /* _wpi_selectbrush */


WPI_HANDLE __wpi_selectobject( WPI_PRES pres, WPI_HANDLE v_obj, void *v_old_obj )
/**********************************************************************/
{
    WPI_OBJECT  *obj;
    WPI_OBJECT  *old_obj;

    obj = (WPI_OBJECT *)v_obj;
    old_obj = (WPI_OBJECT *)v_old_obj;

    switch( obj->type ) {
    case WPI_PEN_OBJ:
        old_obj->type = WPI_PEN_OBJ;
        GpiQueryAttrs( pres, PRIM_LINE, LBB_COLOR | LBB_MIX_MODE |
                                LBB_WIDTH | LBB_TYPE, &(old_obj->pen) );
        GpiSetAttrs( pres, PRIM_LINE, LBB_COLOR | LBB_WIDTH |
                                LBB_TYPE, 0L, &(obj->pen) );
        break;

    case WPI_NULLPEN_OBJ:
        old_obj->type = WPI_PEN_OBJ;
        GpiQueryAttrs( pres, PRIM_LINE, LBB_COLOR | LBB_MIX_MODE |
                                LBB_WIDTH | LBB_TYPE, &(old_obj->pen) );
        GpiSetAttrs( pres, PRIM_LINE, LBB_MIX_MODE, 0L, &(obj->pen) );
        break;

    case WPI_HLWBRUSH_OBJ:
        _wpi_setlogbrushcolour( &(obj->brush.info), GetBkColor( pres ) );
        /**** FALL THROUGH INTO WPI_BRUSH_OBJ ****/

    case WPI_BRUSH_OBJ:
        _OldBrush( pres, old_obj );
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_SYMBOL,
                                                0L, &(obj->brush.info) );
        break;

    case WPI_PATBRUSH_OBJ:
        _OldBrush( pres, old_obj );
        GpiSetPatternSet( pres, 0 ); /* Reset the selected pattern to default */
        /* Set background and foreground colors */
        obj->brush.info.lColor = GpiQueryColor( pres );
        obj->brush.info.lBackColor = GpiQueryBackColor( pres );
        /* give the bitmap an id */
        GpiSetBitmapId(pres, obj->brush.bitmap, obj->brush.info.usSet );
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_BACK_COLOR |
                           ABB_MIX_MODE | ABB_BACK_MIX_MODE | ABB_SET,
                                                  0L, &(obj->brush.info) );
        break;

    case WPI_NULLBRUSH_OBJ:
        _OldBrush( pres, old_obj );
        GpiSetAttrs( pres, PRIM_AREA, ABB_COLOR | ABB_MIX_MODE,
                                                  0L, &(obj->brush.info) );
        break;

    case WPI_BITMAP_OBJ:
        old_obj->type = WPI_BITMAP_OBJ;
        old_obj->bitmap = GpiSetBitmap( pres, obj->bitmap );
        break;

    default:
        break;
    }
    return( (WPI_HANDLE)old_obj );
} /* __wpi_selectobject */

void _wpi_deleteobject( WPI_HANDLE object )
/**********************************************************************/
{
    WPI_OBJECT  *obj;

    obj = (WPI_OBJECT *)object;

    if( obj->type == WPI_BITMAP_OBJ ) {
        GpiDeleteBitmap( obj->bitmap );
    }
    _wpi_free( obj );
} /* _wpi_deleteobject */

HPEN _wpi_createnullpen( void )
/**********************************************************************/
{
    WPI_OBJECT  *nullpen;

    nullpen = _wpi_malloc( sizeof(WPI_OBJECT) );

    memset( nullpen, 0, sizeof(WPI_OBJECT) );
    nullpen->type = WPI_NULLPEN_OBJ;
    nullpen->pen.usType = LINETYPE_INVISIBLE;
#ifdef __FLAT__
    nullpen->pen.usBackMixMode = BM_LEAVEALONE;
#endif
    nullpen->pen.usMixMode = FM_LEAVEALONE;
    return( (HPEN)nullpen );
} /* _wpi_createnullpen */

HPEN _wpi_createpen( USHORT type, short width, WPI_COLOUR colour )
/**********************************************************************/
{
    WPI_OBJECT  *new_pen;

    new_pen = _wpi_malloc( sizeof(WPI_OBJECT) );

    new_pen->type = WPI_PEN_OBJ;
    new_pen->pen.usMixMode = FM_OVERPAINT;
    new_pen->pen.lColor = (COLOR)colour;
    new_pen->pen.usType = type;
    if( width <= 0 ) {
        new_pen->pen.fxWidth = (FIXED)1;
    } else {
        new_pen->pen.fxWidth = (FIXED)width;
    }
    return( (HPEN)new_pen );
} /* _wpi_createpen */

HPEN _wpi_selectpen( WPI_PRES pres, HPEN obj )
/**********************************************************************/
{
    WPI_OBJECT  *oldpen;
    WPI_OBJECT  *pen;

    oldpen = _wpi_malloc( sizeof(WPI_OBJECT) );
    pen = (WPI_OBJECT *)obj;
    oldpen->type = WPI_PEN_OBJ;

    GpiQueryAttrs( pres, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, &(oldpen->pen) );
    if( pen->type == WPI_PEN_OBJ ) {
        GpiSetAttrs( pres, PRIM_LINE,
                        LBB_COLOR | LBB_WIDTH | LBB_TYPE | LBB_MIX_MODE,
                                                            0L, &(pen->pen) );
    } else if( pen->type == WPI_NULLPEN_OBJ ) {
        GpiSetAttrs( pres, PRIM_LINE, LBB_MIX_MODE, 0L, &(pen->pen) );
    } else {
        return( (HPEN)NULL );
    }
    return( (HPEN)oldpen );
} /* _wpi_selectpen */

void _wpi_getoldpen( WPI_PRES pres, HPEN oldobj )
/**********************************************************************/
{
    WPI_OBJECT  *oldpen;

    oldpen = (WPI_OBJECT *)oldobj;
    GpiSetAttrs( pres, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                            LBB_MIX_MODE, 0L, &(oldpen->pen) );
    _wpi_free( oldpen );
} /* _wpi_getoldpen */

void _wpi_enumfonts( WPI_PRES pres, char *facename, WPI_ENUMFONTPROC proc,
                                                                char *data )
/************************************************************************/
/* This routine closely approximates the enumerate routine for Windows  */
{
    PFONTMETRICS        pfm;
    LONG                ltemp = 0L;
    LONG                num_fonts;
    int                 i;
    BOOL                ret;

    facename = facename;                // not used in PM version

    num_fonts = GpiQueryFonts( pres, (ULONG)QF_PUBLIC | QF_PRIVATE, (PSZ)NULL,
                                        &ltemp, (LONG)sizeof(FONTMETRICS),
                                        (PFONTMETRICS)NULL );
    if( num_fonts != 0 && num_fonts != GPI_ALTERROR ) {
        _wpi_malloc2( pfm, num_fonts );
        ltemp = GpiQueryFonts( pres, (ULONG)QF_PUBLIC | QF_PRIVATE, (PSZ)NULL,
                                        &num_fonts, (LONG)sizeof(FONTMETRICS),
                                        (PFONTMETRICS)pfm );
        for( i = 0; i < num_fonts; i++ ) {
            ret = proc( (WPI_LOGFONT *) &pfm[i], (WPI_TEXTMETRIC *) &pfm[i],
                                        (short)pfm[i].fsDefn, (LPSTR)data );
            if (!ret) {
                break;
            }
        }
        _wpi_free( pfm );
    }
} /* _wpi_enumfonts */

void _wpi_enumchildwindows( HWND hwnd, WPI_ENUMPROC proc, LONG data )
/************************************************************************/
/* the hwnd sent to the enum proc is that of the frame window! use      */
/* _wpi_getclient if you need the client window handle                  */
{
    HENUM       henum;
    int         ret;
    HWND        hnext;

    henum = WinBeginEnumWindows( hwnd );
    ret = TRUE;

    hnext = WinGetNextWindow( henum );
    while( hnext && ret ) {
        ret = proc( (HWND)hnext, (LONG)data );
        hnext = WinGetNextWindow( henum );
    }
    WinEndEnumWindows( henum );
} /* _wpi_enumchildwindows */

void _wpi_checkradiobutton( HWND hwnd, int start_id, int end_id, int check_id )
/************************************************************/
{
    USHORT              i;
    HWND                button;
    USHORT              action;

    for( i= start_id; i<= end_id; i++ ) {
        button = WinWindowFromID( hwnd, i);
        if( button != (HWND)NULL ) {
            if( i== check_id ) {
                action = TRUE;
            } else {
                action = FALSE;
            }
            WinSendMsg( button, BM_SETCHECK, (WPI_PARAM1) action, NULL );
        }
    }
} /* _wpi_checkradiobutton */

char *_wpi_menutext2pm( char *text )
/************************************************************/
{
    char        *new;
    int         len;

    new = NULL;

    if( text ) {
        len = strlen( text ) + 1;
        new = (char *)_wpi_malloc( len );
        if( new ) {
            memcpy( new, text, len );
            text = new;
            while( *text ) {
                if( *text == '&' ) {
                    *text = '~';
                }
                text++;
            }
        }
    }

    return( new );
}

LONG _wpi_getbitmapbits( WPI_HANDLE hbitmap, int size, BYTE *bits )
/****************************************************************/
/* The bitmap may NOT be selected into an HPS when calling      */
/* this function.                                               */
{
    BITMAPINFOHEADER    ih;             // Not BITMAPINFOHEADER2 so it will
    BITMAPINFO          *bmi;           // work for 16 bit.
    LONG                slcount;
    LONG                ret;
    HPS                 memhps;
    HDC                 hdc;
    HAB                 hab;
    WPI_OBJECT          *obj;
    HBITMAP             oldbitmap;
    int                 bitsize;
    SIZEL               sizl = { 0, 0 };
    DEVOPENSTRUC        dop = { 0L, "DISPLAY", NULL, 0L,
                                0L, 0L, 0L, 0L, 0L };

    obj = (WPI_OBJECT *)hbitmap;
    ih.cbFix = sizeof( BITMAPINFOHEADER );
    GpiQueryBitmapParameters( obj->bitmap, &ih );

    bitsize = sizeof(BITMAPINFO) + sizeof(RGB) * (1 << ih.cBitCount);
    bmi = _wpi_malloc( bitsize );
    if (!bmi) return( 0L );

    memset( bmi, 0, bitsize );
    memcpy( bmi, &ih, sizeof(BITMAPINFOHEADER) );

    hab = WinQueryAnchorBlock( HWND_DESKTOP );
    hdc = DevOpenDC( hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE );
    memhps = GpiCreatePS( hab, hdc, &sizl, PU_PELS | GPIA_ASSOC );

    oldbitmap = GpiSetBitmap( memhps, obj->bitmap );
    slcount = min( ih.cy, (32 * size) / (4 * ih.cx * ih.cBitCount) );
    ret = GpiQueryBitmapBits( memhps, 0L, slcount, bits, (WPI_BITMAPINFO *)bmi);

    GpiSetBitmap( memhps, oldbitmap );
    GpiDestroyPS( memhps );
    DevCloseDC( hdc );

    _wpi_free( bmi );
    return( ret );
} /* _wpi_getbitmapbits */

LONG _wpi_setbitmapbits( WPI_HANDLE hbitmap, int size, BYTE *bits )
{
    BITMAPINFO          *bmi;
    BITMAPINFOHEADER    ih;
    WPI_OBJECT          *obj;
    HBITMAP             oldbitmap;
    LONG                ret;
    HPS                 memhps;
    HDC                 hdc;
    HAB                 hab;
    LONG                slcount;
    SIZEL               sizl = { 0, 0 };
    DEVOPENSTRUC        dop = { 0L, "DISPLAY", NULL, 0L,
                                0L, 0L, 0L, 0L, 0L };

    obj = (WPI_OBJECT *)hbitmap;
    ih.cbFix = sizeof( BITMAPINFOHEADER );
    GpiQueryBitmapParameters( obj->bitmap, &ih );
    bmi = _wpi_malloc( sizeof(BITMAPINFO) + sizeof(RGB) * (1<<ih.cBitCount) );
    memcpy( bmi, &ih, sizeof(BITMAPINFOHEADER) );

    hab = WinQueryAnchorBlock( HWND_DESKTOP );
    hdc = DevOpenDC( hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE );
    memhps = GpiCreatePS( hab, hdc, &sizl, PU_PELS | GPIA_ASSOC );

    oldbitmap = GpiSetBitmap( memhps, obj->bitmap );
    slcount = min( ih.cy, (32 * size) / (4 * ih.cx * ih.cBitCount) );
    GpiQueryBitmapBits( memhps, 0L, (LONG)slcount, NULL, (WPI_BITMAPINFO *)bmi);
    ret = GpiSetBitmapBits( memhps, 0L, (LONG)slcount, bits, (WPI_BITMAPINFO *)bmi );

    GpiSetBitmap( memhps, oldbitmap );
    GpiDestroyPS( memhps );
    DevCloseDC( hdc );

    _wpi_free( bmi );
    return( ret );
} /* _wpi_setbitmapbits */

BOOL _wpi_movewindow( HWND hwnd, int x, int y, int width, int height, BOOL repaint )
/****************************************************************/
{
    BOOL        ret;

    if ( repaint ) {
        ret = WinSetWindowPos( hwnd, HWND_TOP, x, y, width, height,
                               SWP_MOVE | SWP_SIZE );
    } else {
        ret = WinSetWindowPos( hwnd, HWND_TOP, x, y, width, height,
                               SWP_MOVE | SWP_SIZE | SWP_NOREDRAW );
    }
    return( ret );
} /* _wpi_movewindow */

void _wpi_recttowpirect( RECT *src_rc, WPI_RECT *dest_rc)
/****************************************************************/
/* This routine converts a RECT (ie. the windows form) to a     */
/* WPI_RECT (ie. in this case, a RECTL)                         */
{
    dest_rc->xLeft = src_rc->left;
    dest_rc->xRight = src_rc->right;
    dest_rc->yTop = src_rc->top;
    dest_rc->yBottom = src_rc->bottom;
} /* _wpi_recttowpirect */

void _wpi_wpirecttorect( WPI_RECT *src_rc, RECT *dest_rc )
/****************************************************************/
/* This routine assigns a WPI_RECT (ie. a RECTL) to a RECT      */
/* (ie. the windows form).                                      */
{
    dest_rc->left = src_rc->xLeft;
    dest_rc->right = src_rc->xRight;
    dest_rc->top = src_rc->yTop;
    dest_rc->bottom = src_rc->yBottom;
} /* _wpi_wpirecttorect */

int _wpi_setrop2( WPI_PRES pres, int mode )
/****************************************************************/
{
    int         prevmode;

    prevmode = GpiQueryMix( pres );
    GpiSetMix( pres, mode );
    return( prevmode );
} /* _wpi_setrop2 */

WPI_HANDLE _wpi_loadbitmap( WPI_INST inst, int id )
/***********************************************************/
{
    WPI_OBJECT  *obj;
    WPI_PRES    pres;

    obj = _wpi_malloc( sizeof(WPI_OBJECT) );
    obj->type = WPI_BITMAP_OBJ;
    pres = _wpi_getpres( HWND_DESKTOP );
    obj->bitmap = GpiLoadBitmap( pres, inst.mod_handle, (ULONG)id, 0, 0 );
    _wpi_releasepres( HWND_DESKTOP, pres );

    return( (WPI_HANDLE)obj );
} /* _wpi_loadbitmap */

WPI_HANDLE _wpi_createbitmap( int width, int height, int planes, int bitcount,
                                                                BYTE *bits )
/**************************************************************************/
{
    WPI_BITMAPINFOHEADER        bmih;
    WPI_BITMAPINFO              bitsinfo;
    HPS                         hps;
    WPI_OBJECT                  *obj;

    memset( &bitsinfo, 0, sizeof(WPI_BITMAPINFO) );
    (bitsinfo).cbFix = sizeof(WPI_BITMAPINFO);
    (bitsinfo).cx = width;
    (bitsinfo).cy = height;
#ifdef __FLAT__
    (bitsinfo).ulCompression = BCA_UNCOMP;
    (bitsinfo).usUnits = BRU_METRIC;
    (bitsinfo).usRendering = BRH_NOTHALFTONED;
    (bitsinfo).cSize1 = width;
    (bitsinfo).cSize2 = height;
    (bitsinfo).ulColorEncoding = BCE_RGB;
#endif

    memset( &bmih, 0, sizeof(WPI_BITMAPINFOHEADER) );
    (bmih).cbFix = sizeof( WPI_BITMAPINFOHEADER );
    (bmih).cx = width;
    (bmih).cy = height;
    (bmih).cPlanes = (USHORT)planes;
    /*
     * the windows CreateBitmap takes the bitspixel but PM takes bitcount
     */
    (bmih).cBitCount = (USHORT)bitcount;

    obj = _wpi_malloc( sizeof(WPI_OBJECT) );
    obj->type = WPI_BITMAP_OBJ;

    hps = WinGetScreenPS( HWND_DESKTOP );
    if( bits ) {
        obj->bitmap = GpiCreateBitmap( hps, &bmih, CBM_INIT, bits, &bitsinfo );
    } else {
        obj->bitmap = GpiCreateBitmap( hps, &bmih, 0L, NULL, NULL );
    }
    WinReleasePS( hps );
    return( (WPI_HANDLE)obj );
} /* _wpi_createbitmap */

WPI_HANDLE _wpi_createdibitmap( WPI_PRES pres, WPI_BITMAP *info, ULONG opt,
                                BYTE *data, WPI_BITMAPINFO *table, int opt2 )
{
    WPI_OBJECT  *obj;

    obj = _wpi_malloc( sizeof(WPI_OBJECT) );
    if (!obj) return ( (WPI_HANDLE)NULL );

    obj->type = WPI_BITMAP_OBJ;
    obj->bitmap = GpiCreateBitmap( pres, (WPI_BITMAP *)info, opt, data,
                                                (WPI_BITMAPINFO *)table );
    opt2 = opt2;                                // not used in PM

    return( (WPI_HANDLE)obj );
} /* _wpi_createdibitmap */

#ifdef __FLAT__
void _wpi_setclipboarddata( WPI_INST inst, UINT format, WPI_HANDLE data,
                                                                BOOL is_bitmap )
/*****************************************************************************/
{
    WPI_OBJECT  *obj;

    if( is_bitmap ) {
        obj = (WPI_OBJECT *)data;
        WinSetClipbrdData( inst.hab, (ULONG)obj->bitmap, (ULONG)format,
                                                                CFI_HANDLE );
    } else if( format == CF_TEXT ) {
        PCHAR           tmp;

        /* has to be allocated with DosAllocSharedMem */
        if( DosAllocSharedMem( (PPVOID)&tmp, NULL, strlen( (char *)data ) + 100,
                            PAG_WRITE | PAG_READ | OBJ_GIVEABLE
                            | OBJ_GETTABLE | PAG_COMMIT ) == 0 ) {
            strcpy( tmp, (char *)data );
        }
        WinSetClipbrdData( inst.hab, (ULONG)tmp, (ULONG)format, CFI_POINTER );
    }
} /* _wpi_setclipboarddata */
#endif

#ifndef __FLAT__
static BOOL WinPopupMenu( HWND hwndParent, HWND hwndOwner, HWND hwndMenu,
                          LONG x, LONG y, LONG idItem, ULONG fs)
{
    WPI_RECT    mrect;
    WPI_RECT    crect;
    int         x_adjust, y_adjust;
    int         pos, item_count;
    unsigned    id;

    hwndParent = hwndParent;
    fs = fs;
    idItem = idItem;

    WinSetWindowULong( hwndMenu, QWL_STYLE, WS_SYNCPAINT | WS_SAVEBITS | 0x00000008 );
    WinSetParent( hwndMenu, HWND_DESKTOP, FALSE );
    WinSetOwner( hwndMenu, hwndOwner );
    // dimensions of 100x100 are arbitrary - I just wanted to force the menu
    // to size itself. Apparently menus will choose their own dimensions
    WinSetWindowPos( hwndMenu, HWND_TOP, x, y, 100, 100,
                     SWP_MOVE | SWP_SIZE | SWP_ZORDER );

    // Forgive the scary nature of the following code. One must remember
    // it is the result of bitter, not without vitriolic rejoiner, hacking
    // at the midnight hour. Fortunately, however, I have not lost my keen
    // wit and ability to turn a phrase.
    //                          Wes "Paint problems be damned" Nelson

    //move the y position down by the height of the menu
    WinQueryWindowRect( hwndMenu, &mrect );
    y -= ( mrect.yTop - mrect.yBottom );
    WinSetWindowPos( hwndMenu, NULL, x, y, 0, 0, SWP_MOVE );

    // adjust the position of the menu window if it can't fit in the
    // parent window
    x_adjust = 0;
    y_adjust = 0;
    _wpi_getwindowrect( HWND_DESKTOP, &crect );
    _wpi_getwindowrect( hwndMenu, &mrect );
    mrect.yTop -= y_adjust;
    mrect.yBottom -= y_adjust;
    if( mrect.xRight > crect.xRight ) {
        x_adjust = crect.xRight - mrect.xRight - 1;
    } else if( mrect.xLeft < crect.xLeft ) {
        x_adjust = crect.xLeft - mrect.xLeft + 1;
    }
    if( mrect.yTop > crect.yTop ) {
        y_adjust = crect.yTop - mrect.yTop - 1;
    } else if( mrect.yBottom < crect.yBottom ) {
        y_adjust = crect.yBottom - mrect.yBottom + 1;
    }
    if( y_adjust || x_adjust ) {
        x += x_adjust;
        y += y_adjust;
    }
    WinSetWindowPos( hwndMenu, NULL, x, y, 0, 0, SWP_MOVE | SWP_SHOW );

    // set the initially selected item to be the first selectable one
    item_count = (int) WinSendMsg( hwndMenu, MM_QUERYITEMCOUNT, NULL, NULL );
    pos = 0;
    while ( pos <= item_count ) {
        id = _wpi_getmenuitemidfrompos( hwndMenu, pos );
        if( (BOOL) WinSendMsg( hwndMenu, MM_ISITEMVALID,
                               MPFROM2SHORT(id, FALSE), NULL ) ) {
            WinSendMsg( hwndMenu, MM_SELECTITEM, MPFROM2SHORT(id, FALSE),
                        NULL );
            break;
        }
        pos++;
    }

    // if no selectable item was found then set the initially selected
    // item to be the first one
    if( pos > item_count ) {
        id = _wpi_getmenuitemidfrompos( hwndMenu, 0 );
        WinSendMsg( hwndMenu, MM_SELECTITEM, MPFROM2SHORT(id, FALSE), NULL );
    }

    return( TRUE );
}
#endif

BOOL _wpi_trackpopupmenu( HMENU hmenu, ULONG flags, LONG x, LONG y,
                          HWND parent )
{
    WPI_QMSG    msg;
    HAB         ab;
    BOOL        quit_loop;
    BOOL        discard_msg;
    char        class_name[10];
    WPI_POINT   pt;
    WPI_RECT    rect;

    pt.x = x;
    pt.y = y;
#ifdef __FLAT__
    _wpi_mapwindowpoints( HWND_DESKTOP, parent, &pt, 1 );
#endif

    ab = WinQueryAnchorBlock( parent );
    if( ab == NULLHANDLE ) {
        return( FALSE );
    }

#ifdef __FLAT__
    flags |= ( PU_VCONSTRAIN | PU_HCONSTRAIN );
#endif
    if( !WinPopupMenu( parent, parent, hmenu, pt.x, pt.y, 0, flags ) ) {
        return( FALSE );
    }

    WinSetCapture( HWND_DESKTOP, hmenu );

    quit_loop = FALSE;

    while( !quit_loop ) {
        discard_msg = FALSE;
        if( !WinPeekMsg( ab, &msg, NULLHANDLE, 0, 0, PM_NOREMOVE ) ) {
            if( !WinWaitMsg( ab, 0, 0 ) ) {
                return( FALSE );
            }
            WinPeekMsg( ab, &msg, NULLHANDLE, 0, 0, PM_NOREMOVE );
        }
        switch ( msg.msg ) {
        case WM_MOUSEMOVE:
            // a mousemove in the menu is ok
            if( ( msg.hwnd == hmenu ) ) {
                break;
            }
            // a mousemove in a window that is not the menu AND not a
            // child of the desktop means we chuck the msg
            if( _wpi_getparent( msg.hwnd ) != HWND_DESKTOP ) {
                discard_msg = TRUE;
            } else {
                // lets make sure this child of the desktop is a menu!!
                WinQueryClassName( msg.hwnd, 9, class_name );
                if( memcmp( class_name, "#4", 2 ) ) {
                    discard_msg = TRUE;
                }
            }
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // clicking in the menu is ok
            // the following code is commented out 'cause the menu
            // window will set mouse capture. As a result button down
            // messages outside the window will still come here -- Wes
            //if( ( msg.hwnd == hmenu ) ) {
                //break;
            //}

            // clicking in a window that is not the menu AND not a
            // child of the desktop means a non popup menu was clicked
            if( _wpi_getparent( msg.hwnd ) != HWND_DESKTOP ) {
                quit_loop = TRUE;
            } else {
                // lets make sure this child of the desktop is a menu!!
                WinQueryClassName( msg.hwnd, 9, class_name );
                if( memcmp( class_name, "#4", 2 ) ) {
                    // the window is not a menu
                    quit_loop = TRUE;
                } else {
                    // the window is a menu
                    pt.x = (LONG) SHORT1FROMMP( msg.mp1 );
                    pt.y = (LONG) SHORT2FROMMP( msg.mp1 );
                    WinQueryWindowRect( msg.hwnd, &rect );
                    if( !WinPtInRect( ab, &rect, &pt ) ) {
                        quit_loop = TRUE;
                    }
                }
            }
            break;
        case WM_DESTROY :
            if( ( msg.hwnd == hmenu ) ) {
                quit_loop = TRUE;
            }
            break;
        case WM_MENUEND :
            if( ( msg.hwnd == hmenu ) ) {
                if( (HMENU)msg.mp2 == hmenu ) {
                    quit_loop = TRUE;
                }
            }
            break;
        case WM_COMMAND :
            if( msg.hwnd == parent ) {
                quit_loop = TRUE;
            }
            break;
        case WM_CHAR :
            if( ( SHORT1FROMMP( msg.mp1 ) & KC_VIRTUALKEY ) &&
                ( SHORT2FROMMP( msg.mp2 ) == VK_ESC ) ) {
                quit_loop = TRUE;
            }
            break;
        }
        if( quit_loop ) {
            //WinShowWindow( hmenu, FALSE );
        } else {
            WinGetMsg( ab, &msg, NULLHANDLE, 0, 0 );
            if( !discard_msg ) {
                WinDispatchMsg( ab, &msg );
            }
        }
    }

    WinSetCapture( HWND_DESKTOP, (HWND)NULL );

    return( TRUE );
}

#ifdef __FLAT__
WPI_HANDLE _wpi_getclipboarddata( WPI_INST inst, UINT format )
/************************************************************/
/* Currently this only gets bitmaps from the clipboard.     */
/* Added capability for personally defined stuff            */
{
    WPI_OBJECT  *obj;

    if( format == CF_BITMAP ) {
        obj = _wpi_malloc( sizeof(WPI_OBJECT) );
        obj->type = WPI_BITMAP_OBJ;
        obj->bitmap = WinQueryClipbrdData( inst.hab, (ULONG)format );
    } else {
        /* Your own defined format */
        return( WinQueryClipbrdData( inst.hab, (ULONG)format ) );
    }
    return( (WPI_HANDLE)obj );

} /* _wpi_getclipboarddata */
#endif

BOOL _wpi_iswindow( WPI_INST inst, HWND hwnd )
/****************************************************************/
{
    ERRORID             eid;
    BOOL                ret;

    WinFindAtom( 0L, "RESET ERROR" );
    WinIsWindowEnabled( hwnd );
    eid = WinGetLastError( inst.hab );

    if( LOWORD( eid ) == PMERR_INVALID_HWND ) {
        ret = FALSE;
    } else {
        ret = TRUE;
    }
    WinFindAtom( 0L, "RESET ERROR" );
    return( ret );
} /* _wpi_iswindow */

#ifdef __FLAT__
BOOL _wpi_extfloodfill( WPI_PRES hps, int x, int y, WPI_COLOUR clr, UINT mode )
/*****************************************************************************/
{
    POINTL              pt;
    LONG                ret;

    pt.x = x;
    pt.y = y;
    GpiSetCurrentPosition( hps, &pt );
    ret = GpiFloodFill( hps, (LONG)mode, clr );

    if( ret == GPI_OK ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
} /* _wpi_extfloodfill */
#endif

void _wpi_getbitmapparms( WPI_HANDLE hbitmap, int *width, int *height,
                                    int *planes, int *notused1, int *bitcount )
/****************************************************************/
/* This routine is used when particular fields are wanted.      */
{
    BITMAPINFOHEADER    bih;
    WPI_OBJECT          *obj;

    obj = (WPI_OBJECT *)hbitmap;
    bih.cbFix = sizeof( BITMAPINFOHEADER );
    GpiQueryBitmapParameters( obj->bitmap, &bih );

    if( width ) *width = bih.cx;
    if( height ) *height = bih.cy;
    if( planes ) *planes = bih.cPlanes;
    if ( notused1 ) *notused1 = 0;      // this variable not used for PM
    /*
     * this parameter is bitspixel (usually 1) in Windows but bitcount in
     * PM
     */
    if ( bitcount ) *bitcount = bih.cBitCount;
} /* _wpi_getbitmapparms */

LONG _wpi_getbitmapstruct( WPI_HANDLE bitmap, WPI_BITMAP *info )
{
    WPI_OBJECT  *obj;

    obj = (WPI_OBJECT *)bitmap;
    info->cbFix = sizeof( WPI_BITMAP );
#ifdef __FLAT__
    GpiQueryBitmapInfoHeader( obj->bitmap, info );
#else
    GpiQueryBitmapParameters( obj->bitmap, info );
#endif
    return( sizeof( WPI_BITMAP ) );
} /* _wpi_getbitmapstruct */

void _wpi_setqmsgvalues( WPI_QMSG *qmsg, HWND hwnd, WPI_MSG message,
                        WPI_PARAM1 parm1, WPI_PARAM2 parm2, ULONG wpi_time,
                        WPI_POINT wpi_pt )
/*************************************************************************/
{
    qmsg->hwnd = hwnd;
    qmsg->msg = message;
    qmsg->mp1 = parm1;
    qmsg->mp2 = parm2;
    qmsg->time = wpi_time;
    qmsg->ptl.x = wpi_pt.x;
    qmsg->ptl.y = wpi_pt.y;
} /* _wpi_setqmsgvalues */

void _wpi_getqmsgvalues( WPI_QMSG qmsg, HWND *hwnd, WPI_MSG *message,
                        WPI_PARAM1 *parm1, WPI_PARAM2 *parm2, ULONG *wpi_time,
                        WPI_POINT *wpi_pt )
/*************************************************************************/
{
    if( hwnd ) {
        *hwnd = qmsg.hwnd;
    }
    if( message ) {
        *message = qmsg.msg;
    }
    if( parm1 ) {
        *parm1 = qmsg.mp1;
    }
    if( parm2 ) {
        *parm2 = qmsg.mp2;
    }
    if( wpi_time ) {
        *wpi_time = qmsg.time;
    }
    if( wpi_pt ) {
        wpi_pt->x = qmsg.ptl.x;
        wpi_pt->y = qmsg.ptl.y;
    }
} /* _wpi_getqmsgvalues */


void _wpi_setrectvalues( WPI_RECT *rect, WPI_RECTDIM in_left, WPI_RECTDIM
                        in_top, WPI_RECTDIM in_right, WPI_RECTDIM in_bottom )
/***************************************************************************/
/* Note that for this routine yTop is assigned to in_bottom.  This is for  */
/* Gpi drawing, to ensure that if bottom > top for Windows (which is       */
/* normal) then top > bottom for PM (which is normal).                     */
{
    rect->xLeft = in_left;
    rect->xRight = in_right;
    rect->yTop = in_bottom;
    rect->yBottom = in_top;
} /* _wpi_setrectvalues */

void _wpi_getrectvalues( WPI_RECT rect, WPI_RECTDIM *left,
        WPI_RECTDIM *top, WPI_RECTDIM *right, WPI_RECTDIM *bottom )
/***************************************************************************/
/* Note that this routine assigns top to yBottom.  This routine should be  */
/* used with _wpi_setrectvalues.                                           */
{
    if( left ) *left = rect.xLeft;
    if( right ) *right = rect.xRight;
    if( top ) *top = rect.yBottom;
    if( bottom ) *bottom = rect.yTop;
} /* _wpi_getrectvalues */

void _wpi_setwrectvalues( WPI_RECT *rect, WPI_RECTDIM in_left, WPI_RECTDIM
                        in_top, WPI_RECTDIM in_right, WPI_RECTDIM in_bottom )
/***************************************************************************/
/* This routine can be used when one wants yTop to be the value of top.    */
{
    rect->xLeft = in_left;
    rect->xRight = in_right;
    rect->yTop = in_top;
    rect->yBottom = in_bottom;
} /* _wpi_setwrectvalues */

void _wpi_getwrectvalues( WPI_RECT rect, WPI_RECTDIM *left,
                WPI_RECTDIM *top, WPI_RECTDIM *right, WPI_RECTDIM *bottom)
/***************************************************************************/
/* Note that this routine assigns bottom to yBottom.  It should be used    */
/* _wpi_setwrectvalues.                                                    */
{
    if( left ) *left = rect.xLeft;
    if( right ) *right = rect.xRight;
    if( top ) *top = rect.yTop;
    if( bottom ) *bottom = rect.yBottom;
} /* _wpi_getwrectvalues */

void _wpi_setintrectvalues( WPI_RECT *rect, int in_left, int in_top,
                                                int in_right, int in_bottom )
/***************************************************************************/
/* Note that for this routine yTop is assigned to in_bottom.  This is for  */
/* Gpi drawing, to ensure that if bottom > top for Windows (which is       */
/* normal) then top > bottom for PM (which is normal).                     */
{
    rect->xLeft = (LONG)in_left;
    rect->xRight = (LONG)in_right;
    rect->yTop = (LONG)in_bottom;
    rect->yBottom = (LONG)in_top;
} /* _wpi_setintrectvalues */

void _wpi_getintrectvalues( WPI_RECT rect, int *left, int *top, int *right,
                                                                int *bottom )
/***************************************************************************/
/* Note that this routine assigns top to yBottom.  This routine should be  */
/* used with _wpi_setrectvalues.                                           */
{
    if( left ) *left = (int)rect.xLeft;
    if( right ) *right = (int)rect.xRight;
    if( top ) *top = (int)rect.yBottom;
    if( bottom ) *bottom = (int)rect.yTop;
} /* _wpi_getintrectvalues */

void _wpi_setintwrectvalues( WPI_RECT *rect, int in_left, int in_top,
                                                int in_right, int in_bottom )
/***************************************************************************/
/* This routine can be used when one wants yTop to be the value of top.    */
{
    rect->xLeft = (LONG)in_left;
    rect->xRight = (LONG)in_right;
    rect->yTop = (LONG)in_top;
    rect->yBottom = (LONG)in_bottom;
} /* _wpi_setintwrectvalues */

void _wpi_getintwrectvalues( WPI_RECT rect, int *left, int *top, int *right,
                                                                    int *bottom)
/***************************************************************************/
/* Note that this routine assigns bottom to yBottom.  It should be used    */
/* _wpi_setwrectvalues.                                                    */
{
    if( left ) *left = (int)rect.xLeft;
    if( right ) *right = (int)rect.xRight;
    if( top ) *top = (int)rect.yTop;
    if( bottom ) *bottom = (int)rect.yBottom;
} /* _wpi_getintwrectvalues */

BOOL _wpi_equalrect( WPI_PRECT prect1, WPI_PRECT prect2 )
/*******************************************************/
{
    return( ( prect1->xLeft == prect2->xLeft ) &&
            ( prect1->xRight == prect2->xRight ) &&
            ( prect1->yTop == prect2->yTop ) &&
            ( prect1->yBottom == prect2->yBottom ) );
} /* _wpi_equalrect */

void _wpi_suspendthread( UINT thread_id, WPI_QMSG *msg )
/******************************************************/
{
    *msg = *msg;
    DosSuspendThread( thread_id );
} /* _wpi_suspendthread */

int _wpi_getdevicecaps( WPI_PRES pres, int what )
/***********************************************/
{
    HDC         hdc;
    LONG        result;

    hdc = GpiQueryDevice( pres );

    DevQueryCaps( hdc, (LONG)what, 1L, &result );
    return( (int)result );
} /* _wpi_getdevicecaps */

int _wpi_devicecapableinch( WPI_PRES pres, int what )
/***************************************************/
{
    HDC                 dc;
    LONG                result;

    dc = GpiQueryDevice( pres );
    DevQueryCaps( dc, (LONG)what, 1L, &result );

    if( what != WPI_LOGPIXELSY_FONT && what != WPI_LOGPIXELSX_FONT ) {
        result *= 2.54;
        result /= 100.;
    }

    return( (int)result );
} /* _wpi_devicecapableinch */

void _wpi_setmaxtracksize( WPI_MINMAXINFO *info, int width, int height )
/**********************************************************************/
{
    info->ptlMaxTrackSize.x = (LONG)width;
    info->ptlMaxTrackSize.y = (LONG)height;
} /* _wpi_setmaxtracksize */

void _wpi_setmintracksize( WPI_MINMAXINFO *info, int width, int height )
/**********************************************************************/
{
    info->ptlMinTrackSize.x = (LONG)width;
    info->ptlMinTrackSize.y = (LONG)height;
} /* _wpi_setmintracksize */

void _wpi_setwpiinst( HINSTANCE hinst, HMODULE module, WPI_INST *inst )
/*********************************************************************/
{
    inst->hab = hinst;
    inst->mod_handle = module;
} /* _wpi_setwpiinst */

short _wpi_getdlgitemshort( HWND hwnd, int item, BOOL *trans, BOOL issigned )
/***************************************************************************/
{
    short   result;

    *trans = WinQueryDlgItemShort( hwnd, (ULONG)item, &result, issigned );
    return( result );
} /* _wpi_getdlgitemshort */

WPI_COLOUR _wpi_settextcolor( WPI_PRES pres, WPI_COLOUR colour )
/**************************************************************/
{
    CHARBUNDLE          cb;
    WPI_COLOUR          old_colour;

    old_colour = GpiQueryColor( pres );
    cb.lColor = colour;
    GpiSetAttrs( pres, PRIM_CHAR, CBB_COLOR, 0, &cb );
    return( old_colour );
}/* _wpi_settextcolor */

WPI_COLOUR _wpi_setbackcolour( WPI_PRES pres, WPI_COLOUR colour)
/**************************************************************/
{
    WPI_COLOUR          old_colour;

    old_colour = GpiQueryBackColor( pres );
    GpiSetBackColor( pres, (LONG) colour );
    return( old_colour );
}/* _wpi_setbackcolour */

WPI_COLOUR _wpi_palettergb( WPI_PRES pres, short red,
/*========================*/short green, short blue )
/***************************************************/
{
    WPI_COLOUR          colour;

    colour = _wpi_getrgb( red, green, blue );
    return( GpiQueryNearestColor( pres, 0, colour ) );
}

int _wpi_getdibits( WPI_PRES pres, WPI_HANDLE bitmap, UINT start, UINT count,
                            BYTE *buffer, WPI_BITMAPINFO *info, UINT notused )
/****************************************************************************/
/* According to windows, the bitmap should not be selected into the pres.   */
/* so I assume it isn't here either.                                        */
{
    HBITMAP             oldbitmap;
    HBITMAP             oldbitmap2;
    WPI_OBJECT          *obj;
    HDC                 hdc;
    HPS                 memhps;
    int                 ret;
    HAB                 hab;
    SIZEL               sizl = { 0L, 0L };
    DEVOPENSTRUC        dop = { 0L, "DISPLAY", NULL, 0L,
                                0L, 0L, 0L, 0L, 0L };
    ERRORID             err;

    notused = notused;

    obj = (WPI_OBJECT *)bitmap;
    if( obj->type != WPI_BITMAP_OBJ ) {
        return( 0 );
    }
    oldbitmap = GpiSetBitmap( pres, obj->bitmap );
    hab = WinQueryAnchorBlock( HWND_DESKTOP );
    ret = (int)GpiQueryBitmapBits(pres, (LONG)start, (LONG)count, buffer, info);
    err = WinGetLastError( hab );
    if( ret == -1 ) {
        hdc = DevOpenDC( hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE );
        memhps = GpiCreatePS( hab, hdc, &sizl, PU_PELS | GPIA_ASSOC );
        oldbitmap2 = GpiSetBitmap( memhps, obj->bitmap );
        ret = (int)GpiQueryBitmapBits( memhps, (LONG)start, (LONG)count, buffer, info);
        GpiSetBitmap( memhps, oldbitmap2 );
        GpiDestroyPS( memhps );
        DevCloseDC( hdc );
    }
    GpiSetBitmap( pres, oldbitmap );

    return( ret );
} /* _wpi_getdibits */

HWND _wpi_createwindow( LPSTR class, LPSTR name, ULONG frame_style,
                        ULONG create_flags, ULONG client_style, int x, int y,
                        int width, int height, HWND parent, HMENU menu,
                        WPI_INST inst, void *param, HWND *frame_hwnd )
/***************************************************************************/
{
    HWND    hwnd = 0;

    param = param;

    // Old _wpi_createwindow used HWND_DESKTOP instead of parent_hwnd
    if( parent == NULL ) {
        parent = HWND_DESKTOP;
    }
#ifdef __FLAT__
    *frame_hwnd = WinCreateStdWindow( parent, frame_style, &create_flags,
                    class, name, client_style, inst.mod_handle, menu, &hwnd );
#else
    *frame_hwnd = WinCreateStdWindow( parent, frame_style, &create_flags,
            class, name, client_style, inst.mod_handle, (USHORT)menu, &hwnd );
#endif
    WinSetWindowPos( *frame_hwnd, HWND_TOP, x, y, width, height, SWP_MOVE |
                                        SWP_SIZE | SWP_SHOW | SWP_ACTIVATE );
    return( hwnd );
} /* _wpi_createwindow */

HWND _wpi_createobjwindow( LPSTR class, LPSTR name, ULONG style, int x, int y,
                        int width, int height, HWND parent, HMENU menu,
                        WPI_INST inst, void *param, HWND *frame )
/****************************************************************************/
{
    HWND    hwnd;
    ULONG   flags = FCF_TITLEBAR | FCF_SIZEBORDER | FCF_SYSMENU |
                     FCF_SHELLPOSITION | FCF_MINMAX | FCF_MENU;

    parent = parent;
    param = param;

#ifdef __FLAT__
    *frame = WinCreateStdWindow( HWND_OBJECT, 0L, &flags, class, name,
                                        style, inst.mod_handle, menu, &hwnd );
#else
    *frame = WinCreateStdWindow( HWND_OBJECT, 0L, &flags, class, name,
                                style, inst.mod_handle, (USHORT)menu, &hwnd );
#endif
    WinSetWindowPos( *frame, HWND_TOP, x, y, width, height,
                                                        SWP_MOVE | SWP_SIZE );
    return( hwnd );
} /* _wpi_createobjwindow */

int _wpi_selectcliprgn( WPI_PRES pres, HRGN rgn )
/***********************************************/
{
    HRGN                old_rgn;
    LONG                ret;

    ret = GpiSetClipRegion( pres, rgn, &old_rgn );
    return( (int)ret );
} /* _wpi_selectcliprgn */

HFILE _wpi_fileopen( LPSTR filename, int format )
/***********************************************/
{
#if 0   // this definition seems to interfere with C lib IO
    PM1632_FILESIZETYPE         action;
    HFILE                       hfile;

    if( !(format & (OPEN_SHARE_DENYREAD | OPEN_SHARE_DENYWRITE
                        | OPEN_SHARE_DENYREADWRITE | OPEN_SHARE_DENYNONE)) ) {
        format |= OPEN_SHARE_DENYNONE;
    }

    if( DosOpen( (PSZ)filename, &hfile, &action, 0L, FILE_NORMAL,
                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                format, 0L ) != 0 ) {
        hfile = -1;
    }
#ifdef __FLAT__
    DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#else
    // I don't know what to do here!
    // DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#endif
    return( hfile );
#else
    char                *fmt;
    FILE                *f;

    switch( format ) {

    case OF_READ:
        fmt = "r";
        break;

    case OF_WRITE:
        fmt = "w";
        break;

    default:
        return( -1 );
    }
    f = fopen( filename, fmt );
    if( f == NULL ) {
        return( -1 );
    }

    /* an HFILE is a long, so this next line works */
    return( (HFILE) f );
#endif

} /* _wpi_fileopen */

HFILE _wpi_filecreate( LPSTR filename, int format )
/*************************************************/
{

#if 0   // this definition seems to interfere with C lib IO
    PM1632_FILESIZETYPE         action;
    HFILE                       hfile;

    if( !(format & (OPEN_SHARE_DENYREAD | OPEN_SHARE_DENYWRITE
                        | OPEN_SHARE_DENYREADWRITE | OPEN_SHARE_DENYNONE)) ) {
        format |= OPEN_SHARE_DENYNONE;
    }

    if( DosOpen( (PSZ)filename, &hfile, &action, 0L, FILE_NORMAL,
                OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                format, 0L ) != 0 ) {
        hfile = -1;
    }

#ifdef __FLAT__
    DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#else
    // I don't know what to do here!
    // DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#endif
    return( hfile );
#else
    return( _wpi_fileopen( filename, format ) );
#endif

} /* _wpi_filecreate */

PM1632_FILESIZETYPE _wpi_fileclose( HFILE hfile )
/***********************************************/
{
#if 0   // this definition seems to interfere with C lib IO
    PM1632_FILESIZETYPE     ret;

    ret = (PM1632_FILESIZETYPE)DosClose( hfile );
#ifdef __FLAT__
    DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#else
    // I don't know what to do here!
    // DosDevIOCtl( hfile, 0x08, 0x00, 0, 0, 0, 0, 512L, 0 );
#endif
    return( ret );
#else
    return( fclose( (void *)hfile ) );
#endif
} /* _wpi_fileclose */

PM1632_FILESIZETYPE _wpi_filewrite( HFILE hfile, void *buf,
                                                    PM1632_FILESIZETYPE size )
/****************************************************************************/
{
#if 0   // this definition seems to interfere with C lib IO
    PM1632_FILESIZETYPE len;
    PM1632_APIRET       ret;

    ret = DosWrite( hfile, (PVOID)buf, size, &len );
    if( !ret ) {
        len = 0;
    }
    return( len );
#else
    return( fwrite( buf, 1, size, (void *)hfile ) );
#endif
} /* _wpi_filewrite */

PM1632_FILESIZETYPE _wpi_fileread( HFILE hfile, void *buf,
                                                    PM1632_FILESIZETYPE size )
/****************************************************************************/
{
#if 0   // this definition seems to interfere with C lib IO
    PM1632_FILESIZETYPE     len;
    PM1632_APIRET           ret;

    ret = DosRead( hfile, (PVOID)buf, size, &len );
    if( !ret ) {
        len = 0;
    }
    return( len );
#else
    return( fread( buf, 1, size, (void *)hfile ) );
#endif
} /* _wpi_fileread */

BOOL _wpi_textout( WPI_PRES pres, int left, int top, LPSTR text, ULONG len )
/**************************************************************************/
{
    WPI_POINT   pt;
    BOOL        success;

    pt.x = (LONG)left;
    pt.y = (LONG)top;

    success = GpiCharStringAt( pres, &pt, (LONG)len, text );

    return( success );
} /* _wpi_textout */

BOOL _wpi_exttextout( WPI_PRES pres, int left, int top, UINT options,
                      WPI_RECT *rect, LPSTR text, ULONG len, LPINT spacing )
/**************************************************************************/
{
    WPI_POINT   pt;
    BOOL        success;

    pt.x = (LONG)left;
    pt.y = (LONG)top;

    success = GpiCharStringPosAt( pres, &pt, rect, options, (LONG)len, text,
                                                            (PLONG)spacing);
    return( success );
} /* _wpi_exttextout */

int _wpi_intersectcliprect( WPI_PRES pres, WPI_RECTDIM left, WPI_RECTDIM top,
                                        WPI_RECTDIM right, WPI_RECTDIM bottom )
/*****************************************************************************/
{
    RECTL               rect;

    rect.xLeft = left;
    rect.xRight = right;
    rect.yTop = bottom;
    rect.yBottom = top;

    return( GpiIntersectClipRectangle( pres, &rect ) );
} /* _wpi_intersectcliprect */

BOOL _wpi_setviewportorg( WPI_PRES pres, int x, int y )
/*****************************************************/
{
    RECTL               rect;

    GpiQueryPageViewport( pres, &rect );
    rect.xLeft = (LONG)x;
    rect.yTop += (LONG)(y - rect.yBottom);

    return( GpiSetPageViewport( pres, &rect ) );
} /* _wpi_setviewportorg */

void _wpi_getbmphdrvalues( WPI_BITMAPINFOHEADER bmih, ULONG *size,
        int *cx, int *cy, short *planes, short *bc, int *comp,
        int *size_image, int *xpels, int *ypels, int *used, int *important )
/**************************************************************************/
{
    if( size ) *size = bmih.cbFix;
    if( cx ) *cx = bmih.cx;
    if( cy ) *cy = bmih.cy;
    if( planes ) *planes = bmih.cPlanes;
    if( bc ) *bc = bmih.cBitCount;
#ifdef __FLAT__
    if( comp ) *comp = bmih.ulCompression;
    if( size_image ) *size_image = bmih.cbImage;
    if( xpels ) *xpels = bmih.cSize1;
    if( ypels ) *ypels = bmih.cSize2;
    if( used ) *used = bmih.cclrUsed;
    if( important ) *important = bmih.cclrImportant;
#else
    if( comp ) *comp = 0;
    if( size_image ) *size_image = 0;
    if( xpels ) *xpels = 0;
    if( ypels ) *ypels = 0;
    if( used ) *used = 0;
    if( important ) *important = 0;
#endif
} /* _wpi_getbmphdrvalues */

void _wpi_setbmphdrvalues( WPI_BITMAPINFOHEADER *bmih, ULONG size,
        int cx, int cy, short planes, short bc, int comp, int size_image,
        int xpels, int ypels, int used, int important )
/***********************************************************************/
{
    bmih->cbFix = size;
    bmih->cx = cx;
    bmih->cy = cy;
    bmih->cPlanes = planes;
    bmih->cBitCount = bc;
#ifdef __FLAT__
    bmih->ulCompression = comp;
    bmih->cbImage = size_image;
    bmih->cSize1 = xpels;
    bmih->cSize2 = ypels;
    bmih->cclrUsed = used;
    bmih->cclrImportant = important;
#else
    comp = comp;
    size_image = size_image;
    xpels = xpels;
    ypels = ypels;
    used = used;
    important = important;
#endif
} /* _wpi_setbmphdrvalues */

void _wpi_bmptorgb( WPI_BITMAPINFOHEADER *bmp_info )
/**************************************************/
{
    (bmp_info)->cbFix = sizeof( WPI_BITMAPINFOHEADER );
#ifdef __FLAT__
    (bmp_info)->usUnits = BRU_METRIC;
    (bmp_info)->usReserved = 0;
    (bmp_info)->usRecording = BRA_BOTTOMUP;
    (bmp_info)->usRendering = BRH_NOTHALFTONED;
    (bmp_info)->cSize1 = 0;
    (bmp_info)->cSize2 = 0;
    (bmp_info)->ulColorEncoding = BCE_RGB;
    (bmp_info)->ulIdentifier = 0L;
#endif
} /* _wpi_bmptorgb */

void _wpi_setimagemode( WPI_PRES pres, USHORT mode )
/**************************************************/
{
    IMAGEBUNDLE         p;

    p.usMixMode = mode;
    GpiSetAttrs( pres, PRIM_IMAGE, IBB_MIX_MODE, 0L, &p );
} /* _wpi_setimagemode */

void _wpi_setimagebackmode( WPI_PRES pres, USHORT mode )
/******************************************************/
{
    IMAGEBUNDLE         p;

    p.usBackMixMode = mode;
    GpiSetAttrs( pres, PRIM_IMAGE, IBB_BACK_MIX_MODE, 0L, &p );
} /* _wpi_setimagebackmode */

USHORT _wpi_imagemode( WPI_PRES pres )
/************************************/
{
    IMAGEBUNDLE         p;
    USHORT              mode;

    mode = GpiQueryAttrs( pres, PRIM_IMAGE, IBB_MIX_MODE, &p );
    return( mode );
} /* _wpi_imagemode */

USHORT _wpi_imagebackmode( WPI_PRES pres )
/****************************************/
{
    IMAGEBUNDLE         p;
    USHORT              mode;

    mode = GpiQueryAttrs( pres, PRIM_IMAGE, IBB_BACK_MIX_MODE, &p );
    return( mode );
} /* _wpi_imagebackmode */

void _wpi_deletesysmenupos( HMENU hmenu, SHORT pos )
/**************************************************/
{
    SHORT               id;
    MENUITEM            mi;
    HWND                newmenu;

    WinSendMsg(hmenu, MM_QUERYITEM, MPFROM2SHORT(SC_SYSMENU, FALSE),
                                                            MPFROMP((PSZ)&mi));
    newmenu = mi.hwndSubMenu;

    id = SHORT1FROMMR( WinSendMsg(newmenu, MM_ITEMIDFROMPOSITION,
                                    MPFROM2SHORT(pos, FALSE), MPFROMSHORT(0)) );

    WinSendMsg(newmenu, MM_DELETEITEM, MPFROM2SHORT(id, FALSE), MPFROMSHORT(0));
} /* _wpi_deletesysmenupos */

void _wpi_gettextextent( WPI_PRES pres, LPSTR string, int len_string,
                                                    int *width, int *height )
/***************************************************************************/
{
    WPI_POINT           pts[TXTBOX_COUNT];
    int                 t_max, t_min;

    GpiQueryTextBox( pres, (LONG)len_string, (PCH)string, TXTBOX_COUNT, pts );

    t_max = max( pts[TXTBOX_TOPLEFT].x, pts[TXTBOX_TOPRIGHT].x );
    t_max = max( t_max, pts[TXTBOX_BOTTOMLEFT].x );
    t_max = max( t_max, pts[TXTBOX_BOTTOMRIGHT].x );
    t_min = min( pts[TXTBOX_TOPLEFT].x, pts[TXTBOX_TOPRIGHT].x );
    t_min = min( t_min, pts[TXTBOX_BOTTOMLEFT].x );
    t_min = min( t_min, pts[TXTBOX_BOTTOMRIGHT].x );
    *width = abs( t_max - t_min );

    t_max = max( pts[TXTBOX_TOPLEFT].y, pts[TXTBOX_TOPRIGHT].y );
    t_max = max( t_max, pts[TXTBOX_BOTTOMLEFT].y );
    t_max = max( t_max, pts[TXTBOX_BOTTOMRIGHT].y );
    t_min = min( pts[TXTBOX_TOPLEFT].y, pts[TXTBOX_TOPRIGHT].y );
    t_min = min( t_min, pts[TXTBOX_BOTTOMLEFT].y );
    t_min = min( t_min, pts[TXTBOX_BOTTOMRIGHT].y );

    *height = abs( t_max - t_min );

} /* _wpi_gettextextent */

void _wpi_getinidirectory( WPI_INST inst, LPSTR dir_info, int size )
/******************************************************************/
{
    PRFPROFILE                  prof;
    int                         i;
    char                        c;
    int                         len;

    prof.cchUserName = 0L;
    prof.cchSysName = 0L;

    if( PrfQueryProfile( inst.hab, &prof ) ) {
        if( prof.cchSysName > 0 ) {
            _wpi_malloc2( prof.pszSysName, prof.cchSysName );
            _wpi_malloc2( prof.pszUserName, prof.cchUserName );
            PrfQueryProfile( inst.hab, &prof );
        }

        strcpy( dir_info, prof.pszSysName );
        if( prof.cchSysName <= size ) {
            len = prof.cchSysName;
        } else {
            len = size;
        }

        for( i = len - 1; i >= 0; i-- ) {
            c = dir_info[i];
            dir_info[i] = '\0';
            if( c == '\\' ) {
                break;
            }
        }
    } else {
        dir_info[0] = '\0';
    }
    _wpi_free( prof.pszUserName );
    _wpi_free( prof.pszSysName );
} /* _wpi_getinidirectory */

void _wpi_getrestoredrect( HWND hwnd, WPI_RECT *prect )
/*****************************************************/
{
    SWP swp;

    WinQueryWindowPos( hwnd, &swp );
    if( PM1632SWP_FLAG( swp ) & SWP_MINIMIZE ) {
        // this is seriously hokey so if anyone figures out a better
        // way to do this change this code - Wes

        // if the window is minimized lets restore it then minimize it
        _wpi_setredraw( hwnd, FALSE );
        _wpi_restorewindow( hwnd );
        WinQueryWindowPos( hwnd, &swp );
        prect->xLeft   = swp.x;
        prect->yBottom = swp.y;
        prect->xRight  = swp.x + swp.cx;
        prect->yTop    = swp.y + swp.cy;
        _wpi_minimizewindow( hwnd );
        _wpi_setredraw( hwnd, TRUE );
    } else {
        prect->xLeft   = swp.x;
        prect->yBottom = swp.y;
        prect->xRight  = swp.x + swp.cx;
        prect->yTop    = swp.y + swp.cy;
    }

} /* _wpi_getrestoredrect */

void _wpi_setrgbquadvalues( WPI_RGBQUAD *rgb, BYTE red, BYTE green,
                                                    BYTE blue, BYTE option )
/**************************************************************************/
{
    rgb->bRed = red;
    rgb->bGreen = green;
    rgb->bBlue = blue;
#ifdef __FLAT__
    rgb->fcOptions = option;
#else
    option = option;
#endif
} /* _wpi_setrgbquadvalues */

void _wpi_gettextface( WPI_PRES pres, int size, LPSTR buf )
/*********************************************************/
{
    WPI_TEXTMETRIC              metric;
    int                         min_size;

    GpiQueryFontMetrics( pres, sizeof( FONTMETRICS ), &metric );
    min_size = min( size, strlen( metric.szFacename ) + 1 );
    strncpy( buf, metric.szFacename, min_size );
} /* _wpi_gettextface */

void _wpi_cvth_rect( WPI_RECT *rect, LONG height )
/************************************************/
{
    rect->yBottom = height - rect->yBottom - 1L;
    rect->yTop = height - rect->yTop - 1L;
} /* _wpi_cvth_rect */

LONG _wpi_cvth_wanchor( LONG y, LONG window_height, LONG parent_height )
/**********************************************************************/
{
    LONG    ret;

    ret = y + window_height;
    ret = parent_height - ret - 1L;
    return( ret );
} /* _wpi_cvth_wanchor */

LONG _wpi_cvtc_y( HWND hwnd, LONG y )
/***********************************/
/* convert a point for the given   */
/* client window.                  */
{
    WPI_RECT    rect;
    LONG        ret;

    _wpi_getclientrect( hwnd, &rect );
    ret = _wpi_getheightrect( rect ) - y - 1L;
    return( ret );
} /* _wpi_cvtc_y */

void _wpi_cvtc_rect( HWND hwnd, WPI_RECT *rect )
/**********************************************/
/* convert a rectangle for the given client   */
/* window.                                    */
{
    WPI_RECT    r;
    int         h;

    _wpi_getclientrect( hwnd, &r );
    h = _wpi_getheightrect( r );

    rect->yBottom = h - rect->yBottom - 1L;
    rect->yTop = h - rect->yTop - 1L;
} /* _wpi_cvtc_rect */

LONG _wpi_cvts_wanchor( LONG y, LONG window_height )
/**************************************************/
{
    LONG    ret;

    ret = y + window_height;
    ret = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) - ret - 1L;
    return( ret );
} /* _wpi_cvts_wanchor */

LONG _wpi_getclipbox( WPI_PRES pres, WPI_PRECT rcl )
/**************************************************/
{
    LONG  rc;

    rc = GpiQueryClipBox( pres, rcl );
    rcl->yTop++;
    rcl->xRight++;
    return( rc );
}/* _wpi_getclipbox */

int _wpi_dlg_command( HWND dlg_hld, WPI_MSG *msg, WPI_PARAM1 *parm1,
                                                        WPI_PARAM2 *parm2 )
/*************************************************************************/
{
    dlg_hld = dlg_hld;

    if( *msg == WM_CLOSE ) {
        /* in Windows, this is automatically generated on a sysmenu
           close. But OS/2 is dumb, so we fake it */
        *msg = WM_COMMAND;
        *parm1 = (WPI_PARAM1)IDCANCEL;
        *parm2 = 0;
        return( TRUE );
    } else if( *msg == WM_COMMAND ) {
        if( _wpi_getid( *parm2 ) == CMDSRC_PUSHBUTTON ) {
            *parm1 = MPFROM2SHORT( _wpi_getid( *parm1 ), BN_CLICKED );
        }
        return( TRUE );
    } else if( *msg == WM_DLGCOMMAND &&
                                SHORT2FROMMP( *parm1 ) != BN_PAINT ) {
        return( TRUE );
    }

    return( FALSE );
} /* _wpi_dlg_command */

void _wpi_linedda( int x1, int y1, int x2, int y2,
                                    WPI_LINEDDAPROC line_proc, WPI_PARAM2 lp )
/****************************************************************************/
{
    float       m;
    float       b;
    int         x;
    int         y;

    // check for a verticle line
    if( x2 - x1 == 0 ) {

        // we want to draw from y1 to y2 ALWAYS
        if( y2 > y1 ) {
            for( y=y1; y < y2; ++y ) {
                line_proc( x1, y, lp );
            }
        } else {
            for( y=y1; y > y2; --y ) {
                line_proc( x1, y, lp );
            }
        }
    } else {
        m = (float)( ( (float)(y2 - y1) ) / ( (float)(x2 - x1) ) );
        b = (float)( (float)y1 - ( m * (float)x1 ) );

        if( abs( x2-x1 ) > abs( y2-y1 ) ) {
            // we want to draw from x1 to x2 ALWAYS!!
            if( x2 > x1 ) {
                for( x=x1; x < x2; ++x ) {
                    y = (int)( (m*(float)x) + b );
                    line_proc( x, y, lp );
                }
            } else {
                for( x=x1; x > x2; --x ) {
                    y = (int)( (m*(float)x) + b );
                    line_proc( x, y, lp );
                }
            }
        } else {
            // we want to draw from y1 to y2 ALWAYS!!
            if( y2 > y1 ) {
                for( y=y1; y < y2; ++y ) {
                    x = (int)( ((float)y - b) / m );
                    line_proc( x, y, lp );
                }
            } else {
                for( y=y1; y > y2; --y ) {
                    x = (int)( ((float)y - b) / m );
                    line_proc( x, y, lp );
                }
            }
        }
    }
} /* _wpi_linedda */

int _wpi_getmetricpointsize( WPI_PRES pres, WPI_TEXTMETRIC *textmetric,
                                                int *pix_size, int *match_num )
/*****************************************************************************/
{
    int     pointsize;

    pres = pres;
    pointsize = (int)textmetric->sNominalPointSize / 10;
    *match_num = (int)textmetric->lMatch;
    *pix_size = 0;
    return( pointsize );
} /* _wpi_getmetricpointsize */

DWORD _wpi_getmessagepos( WPI_INST inst )
/***************************************/
{
    WPI_POINT       pt;
    short           x, y;
    DWORD           ret;

    WinQueryMsgPos( inst.hab, &pt );
    x = (short)pt.x;
    y = (short)pt.y;
    ret = MAKEULONG( x, y );
    return( ret );
} /* _wpi_getmessagepos */

int _wpi_getprivateprofilestring( HINI hini, LPSTR app,
                LPSTR key, LPSTR def, LPSTR buf, int size, LPSTR dummy )
/**********************************************************************/
{
    ULONG                       len;

    dummy = dummy;

    len = PrfQueryProfileString( hini, app, key, def, buf, size );
    if( len != 0 && len < size && buf[len-1] != '\0' ) {
        buf[len] = '\0';
    }

    return( (int)len );
}

HMODULE _wpi_loadlibrary( WPI_INST inst, LPSTR name )
/***************************************************/

{
#if 0
    return( WinLoadLibrary( (inst).hab, name ) );
#else
    HMODULE                     module;

    inst = inst;

    DosLoadModule( 0, 0, name, &module );

    return( module );
#endif
}

void _wpi_freelibrary( WPI_INST inst, HMODULE module )
/****************************************************/


{
#if 0
    WinDeleteLibrary( (inst).hab, module );
#else

    inst = inst;
    DosFreeModule( module );
#endif
}

WPI_PROC _wpi_loadprocedure( WPI_INST inst, HMODULE module, LPSTR proc )
/**********************************************************************/

{
#ifdef __386__
    PFN         addr;

    inst = inst;
    DosQueryProcAddr( module, 0, proc, &addr );

    return( (WPI_PROC)addr );
#else
    return( WinLoadProcedure( (inst).hab, module, proc ) );
#endif
}

/***** _WPI_F_* Font Functions *****/
/* These functions are a set of replacement WPI font functions
   which should work. All the other functions are crap; this
   is an attempt to do it right. DJP */

#define FONT_SET_ID     1       // font index in the PRES which we use

void _wpi_f_setfontescapement( WPI_F_FONT *font, LONG angle )
/***********************************************************/
/* In Windows, the escapement is in tenths of degrees */

{
    /* The '50's below are important. On a Laserwriter PS printer,
       using 200 causes outline fonts to come out on an angle
       (i.e. yet another OS/2 bug). */
    font->bundle.ptlShear.x = 50 * sin( 2 * PI * angle / 3600 );
    font->bundle.ptlShear.y = 50 * cos( 2 * PI * angle / 3600 );
}

void _wpi_f_setfontorientation( WPI_F_FONT *font, LONG angle )
/************************************************************/
/* In Windows, the orientation is in tenths of degrees */

{
    /* The '50's below are important. On a Laserwriter PS printer,
       using 200 causes outline fonts to come out on an angle
       (i.e. yet another OS/2 bug). */
    font->bundle.ptlAngle.x = 50 * cos( 2 * PI * angle / 3600 );
    font->bundle.ptlAngle.y = 50 * sin( 2 * PI * angle / 3600 );
}

void _wpi_f_setfontfacename( WPI_F_FONT *font, PSZ name )
/*******************************************************/

{
    font->attr.szFacename[0] = '\0';
    if( name != NULL ) {
        if( strcmp( name, "MS Sans Serif" ) == 0 ) {
            strcpy( font->attr.szFacename, "Helvetica" );
        } else if( strcmp( name, "MS Serif" ) == 0 ) {
            strcpy( font->attr.szFacename, "Times New Roman" );
        } else if( strcmp( name, "Arial" ) == 0 ) {
            strcpy( font->attr.szFacename, "Helvetica" );
        } else if( strcmp( name, "Times" ) == 0 ) {
            strcpy( font->attr.szFacename, "Times New Roman" );
        } else {
            strcpy( font->attr.szFacename, name );
        }
    }
}

HFONT _wpi_f_createfont( WPI_F_FONT *font )
/*****************************************/

{
    /* In Windows, the CreateFont function takes a LOGFONT and builds
       a HFONT. With our WPI_F_FONT structure in WPI OS/2, we don't
       really need this. But, somebody may build a WPI_F_FONT,
       call this function, then return the result. Hence,
       we do have to allocates and copy it */

    WPI_F_FONT                  *copy_font;

    _wpi_malloc2( copy_font, 1 );

    memcpy( copy_font, font, sizeof( *font ) );

    return( (LONG)copy_font );
}

void _wpi_f_getoldfont( WPI_PRES pres, HFONT ofont )
/**************************************************/

{
    WPI_F_FONT                  *font;

    font = (void *) ofont;
    GpiSetCharSet( pres, LCID_DEFAULT );
    GpiDeleteSetId( pres, FONT_SET_ID );        // delete any leftover first
    GpiCreateLogFont( pres, NULL, FONT_SET_ID, &font->attr );
    GpiSetCharSet( pres, FONT_SET_ID );
    if( font->attr.fsFontUse & FATTR_FONTUSE_OUTLINE ) {
        /* set character attrs too */
        GpiSetAttrs( pres, PRIM_CHAR, CBB_BOX | CBB_ANGLE | CBB_SHEAR,
                                                        0, &font->bundle );
    }

    _wpi_free( font );
}

static WPI_F_FONT *get_f_attrs( WPI_PRES pres, WPI_F_FONT *font )
/***************************************************************/
/* get the current font information in the pres into the font structure */

{
    FONTMETRICS                 fm;
    HDC                         dc;
    LONG                        vert_res;

    GpiQueryFontMetrics( pres, sizeof(FONTMETRICS), &fm );

    font->attr.usRecordLength = sizeof( FATTRS );

    font->attr.fsSelection = fm.fsSelection;
    font->attr.lMatch = fm.lMatch;
    strcpy( font->attr.szFacename, fm.szFacename );
    font->attr.idRegistry = fm.idRegistry;
    font->attr.lMaxBaselineExt = fm.lMaxBaselineExt;
    font->attr.lAveCharWidth = fm.lAveCharWidth;
    if( fm.fsDefn & FM_DEFN_OUTLINE ) {
        font->attr.fsFontUse = FATTR_FONTUSE_OUTLINE;
        GpiQueryCharShear( pres, &font->bundle.ptlShear );
        GpiQueryCharAngle( pres, &font->bundle.ptlAngle );
        GpiQueryCharBox( pres, &font->bundle.sizfxCell );

        /* compute point size */
        dc = GpiQueryDevice( pres );
        DevQueryCaps( dc, CAPS_VERTICAL_FONT_RES, 1L, &vert_res );

        font->pt_size = font->bundle.sizfxCell.cy * 72 / vert_res;
    } else {
        font->pt_size = fm.sNominalPointSize / 10;
    }

    font->retrieved = TRUE;

    return( font );
}

static BOOL find_font( WPI_PRES pres, FATTRS *attr, WPI_F_FONT *font )
/********************************************************************/

{
    HDC                         dc;
    LONG                        horz_res;
    LONG                        vert_res;
    LONG                        num_fonts;
    FONTMETRICS                 *fonts;
    int                         i;
    int                         best_outline;
    int                         best_outline_diff;
    SHORT                       size;
    int                         size_diff;
    SIZEF                       sizef;
    BOOL                        found_match;

    dc = GpiQueryDevice( pres );

    DevQueryCaps( dc, CAPS_HORIZONTAL_FONT_RES, 1L, &horz_res );
    DevQueryCaps( dc, CAPS_VERTICAL_FONT_RES,   1L, &vert_res );

    memset( attr, 0, sizeof( *attr ) );
    attr->usRecordLength = sizeof( FATTRS );
    if( font->attr.szFacename[0] == '\0' ) {
        /* default font requested. The books claim that we can just
           set the facename to the null string, and it will select
           the default font for the DC */

        return( TRUE );
    }

    /* First, see if we can match the font with an image
       font based on the definition resolution and the size */

    num_fonts = 0;
    num_fonts = GpiQueryFonts( pres, QF_PUBLIC, font->attr.szFacename,
                                   &num_fonts, 0L, NULL);
    if( num_fonts == 0 ) {
        return( FALSE );
    }

    fonts = _wpi_malloc( num_fonts * sizeof( FONTMETRICS ) );
    GpiQueryFonts( pres, QF_PUBLIC, font->attr.szFacename,
                    &num_fonts, (LONG) sizeof( FONTMETRICS ), fonts );

    found_match = FALSE;
    best_outline = -1;
    size = font->pt_size * 10;
    for( i = 0; i < num_fonts; i++ ) {
        size_diff = abs( size - fonts[i].sNominalPointSize );
        if( fonts[i].fsDefn & FM_DEFN_OUTLINE ) {
            /* outline font. Record it if it is a good fit */
            if( best_outline == -1 ||
                ( best_outline_diff < size_diff &&
                ( font->attr.usCodePage == 0 ||
                fonts[i].usCodePage == 0 ||
                ( font->attr.usCodePage == fonts[i].usCodePage ) ) ) ) {
                best_outline = i;
                best_outline_diff = size_diff;
            }
        } else {
            if( fonts[i].sXDeviceRes == (SHORT) horz_res &&
                fonts[i].sYDeviceRes == (SHORT) vert_res &&
                fonts[i].sNominalPointSize == (SHORT) size &&
                ( font->attr.usCodePage == 0 ||
                fonts[i].usCodePage == 0 ||
                ( font->attr.usCodePage == fonts[i].usCodePage ) ) ) {
                /* this is a good image font match. Select it */
                break;
            }
        }
    }

    size = font->pt_size;
    if( i == num_fonts ) {
        if( best_outline != -1 ) {
            /* use an outline font */
            found_match = TRUE;
            i = best_outline;
            attr->fsFontUse |= FATTR_FONTUSE_OUTLINE;
            attr->lMatch = 0;

            /* the size is unusual for OS/2. The conversion to
               logical units has to be by you, not the OS.
               So this is what that does */
            sizef.cx = MAKEFIXED( size * horz_res / 72, 0 );
            sizef.cy = MAKEFIXED( size * vert_res / 72, 0 );
            font->bundle.sizfxCell = sizef;
            attr->lMaxBaselineExt = 0;
            attr->lAveCharWidth = 0;
        }
    } else {
        found_match = TRUE;
        attr->fsFontUse &= ~FATTR_FONTUSE_OUTLINE;
        attr->lMatch = fonts[i].lMatch;
        attr->lMaxBaselineExt = fonts[i].lMaxBaselineExt;
        attr->lAveCharWidth = fonts[i].lAveCharWidth;
    }

    if( found_match ) {
        strcpy( attr->szFacename, fonts[i].szFacename );
        attr->fsSelection = font->attr.fsSelection;
        attr->idRegistry = fonts[i].idRegistry;
        attr->usCodePage = fonts[i].usCodePage;
        //attr->fsType ... // doesn't need to be set
    }

    _wpi_free( fonts );

    return( found_match );
}


HFONT _wpi_f_selectfont( WPI_PRES pres, HFONT f )
/***********************************************/
/* this code mimics the behaviour of the OS/2 font palette program.
   Hence, corresponding fonts in the palette program and
   from this code will appear the same. */

{
    WPI_F_FONT                  *font;
    FATTRS                      attr;
    ULONG                       mask;
    char                        old_face[FACESIZE];
    char                        tmp_face[500];
    WPI_F_FONT                  *old_font;
    BOOL                        find_normal;
    CHARBUNDLE                  bundle;

    font = (void *)f;

    _wpi_malloc2( old_font, 1 );
    get_f_attrs( pres, old_font );

    /* The OS/2 font palette will choose a non-synthesized bold/italic
       font by title, if you ask for a 'bold' one */
    find_normal = TRUE;
    if( font->attr.fsSelection & FATTR_SEL_ITALIC ||
                                font->attr.fsSelection & FATTR_SEL_BOLD ) {
        strcpy( old_face, font->attr.szFacename );
        strcpy( tmp_face, font->attr.szFacename );
        if( font->attr.fsSelection & FATTR_SEL_BOLD ) {
            strcat( tmp_face, " Bold" );
        }
        if( font->attr.fsSelection & FATTR_SEL_ITALIC ) {
            strcat( tmp_face, " Italic" );
        }
        if( strlen( tmp_face ) < FACESIZE ) {
            strcpy( font->attr.szFacename, tmp_face );
            if( find_font( pres, &attr, font ) ) {
                find_normal = FALSE;
                attr.fsSelection &= ~FATTR_SEL_BOLD;
                attr.fsSelection &= ~FATTR_SEL_ITALIC;
            }
            strcpy( font->attr.szFacename, old_face );
        }
    }
    if( find_normal ) {
        find_font( pres, &attr, font );
    }

    /* select the font in the PS */
    GpiSetCharSet( pres, LCID_DEFAULT );
    GpiDeleteSetId( pres, FONT_SET_ID );        // delete any leftover first
    GpiCreateLogFont( pres, (PSTR8) NULL, FONT_SET_ID, &attr );
    GpiSetCharSet( pres, FONT_SET_ID );

    /* set the charbundle features */
    if( attr.fsFontUse & FATTR_FONTUSE_OUTLINE ) {
        mask = CBB_BOX;
        if( font->bundle.ptlAngle.x != 0 || font->bundle.ptlAngle.y != 0 ) {
            mask |= CBB_ANGLE;
        }
        if( font->bundle.ptlShear.x != 0 || font->bundle.ptlShear.y != 0 ) {
            mask |= CBB_SHEAR;
        }
        GpiSetAttrs( pres, PRIM_CHAR, mask, 0, &font->bundle );
    } else {
        /* we stil must set the attrs for image fonts. AND, DON'T TRY
           and set these to 'defaults'; the defaults are different for
           different devices! */
        bundle.ptlAngle.x = 1;
        bundle.ptlAngle.y = 0;
        bundle.ptlShear.x = 0;
        bundle.ptlShear.y = 1;
        GpiSetAttrs( pres, PRIM_CHAR,
                        CBB_ANGLE | CBB_SHEAR | CBB_BOX, CBB_BOX, &bundle );
    }

    return( (HFONT) old_font );
}

void _wpi_f_getsystemfont( WPI_PRES in_pres, WPI_F_FONT *font )
/*************************************************************/

{
    FATTRS                      attr;
    WPI_F_FONT                  old_font;
    WPI_PRES                    pres;

    if( in_pres == NULL ) {
        /* assume screen */
        pres = _wpi_getpres( HWND_DESKTOP );
    } else {
        pres = in_pres;
    }
    /* select a default font into the presentation space */
    memset( &attr, 0, sizeof( attr ) );
    attr.usRecordLength = sizeof( FATTRS );

    get_f_attrs( pres, &old_font );

    GpiSetCharSet( pres, LCID_DEFAULT );
    GpiDeleteSetId( pres, FONT_SET_ID );        // delete any leftover first
    GpiCreateLogFont( pres, (PSTR8) NULL, FONT_SET_ID, &attr );
    GpiSetCharSet( pres, FONT_SET_ID );

    get_f_attrs( pres, font );

    _wpi_f_deletefont( (WPI_F_FONT *)_wpi_f_selectfont( pres,
                                                (HFONT)&old_font ) );

    if( in_pres == NULL ) {
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
}

void _wpi_f_default( WPI_F_FONT *font )
/*************************************/
/* this is here, rather than wpi_os2.h, so that people don't have
   to recompile the world when we change WPI_F_FONT */

{
    memset( font, 0, sizeof( *font ) );
}

LONG _wpi_f_getfontsize( WPI_F_FONT *font )
/*****************************************/

{
    return( font->pt_size );
}

/***** end of _WPI_F_* Font Functions *****/


BOOL _wpi_is_dbcs( void )
/***********************/

{
    COUNTRYCODE                 code;
    UCHAR                       dbcs_env[12];

    code.country = 0;
    code.codepage = 0;
    #ifdef __386__
        DosQueryDBCSEnv( 12, &code, dbcs_env );
    #else
        DosGetDBCSEv( 12, &code, dbcs_env );
    #endif

    return( dbcs_env[0] | dbcs_env[1] );
}

void _wpi_fix_dbcs( HWND dlg )
/****************************/

{
    /* stolen from David Brandow's code in VX-REXX. */
    #define DBCS_FONT   "10.Mincho"

    WinRemovePresParam( dlg, PP_FONTNAMESIZE );
    WinSetPresParam( dlg, PP_FONTNAMESIZE, sizeof(DBCS_FONT), DBCS_FONT );
}

