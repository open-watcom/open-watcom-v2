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


#include <string.h>
#include "wwindow.hpp"
#include "wlines.hpp"
#include "wpoint.hpp"


void WEXPORT WWindow::fillRect( const WRect &r, Color colour ) {
/**************************************************************/

    gui_rect    rr;

    if( !isPainting() ) return;
    rr.x = r.x();
    rr.y = r.y();
    rr.width = r.w();
    rr.height = r.h();
    GUIFillRectRGB( handle(), &rr, colour );
}


void WEXPORT WWindow::fillRect( const WRect &r, WPaintAttr attr ) {
/*****************************************************************/

    gui_rect    rr;

    if( !isPainting() ) return;
    rr.x = r.x();
    rr.y = r.y();
    rr.width = r.w();
    rr.height = r.h();
    GUIFillRect( handle(), &rr, attr );
}


void WEXPORT WWindow::drawRect( const WRect &r, Color colour ) {
/**************************************************************/

    gui_rect    rr;

    if( !isPainting() ) return;
    rr.x = r.x();
    rr.y = r.y();
    rr.width = r.w();
    rr.height = r.h();
    GUIDrawRectRGB( handle(), &rr, colour );
}


void WEXPORT WWindow::drawRect( const WRect &r, WPaintAttr attr ) {
/*****************************************************************/

    gui_rect    rr;

    if( !isPainting() ) return;
    rr.x = r.x();
    rr.y = r.y();
    rr.width = r.w();
    rr.height = r.h();
    GUIDrawRect( handle(), &rr, attr );
}


void WEXPORT WWindow::drawLine( const WPoint &start, const WPoint &end,
                                WLineStyle ls, unsigned int thickness,
                                Color colour ) {
/**********************************************/

    gui_point   p;
    gui_point   q;

    if( !isPainting() ) return;
    p.x = start.x();
    p.y = start.y();
    q.x = end.x();
    q.y = end.y();
    GUIDrawLineRGB( handle(), &p, &q, ls, thickness, colour );
}


void WEXPORT WWindow::drawLine( const WPoint &start, const WPoint &end,
                                WLineStyle ls, unsigned int thickness,
                                WPaintAttr attr ) {
/*************************************************/

    gui_point   p;
    gui_point   q;

    if( !isPainting() ) return;
    p.x = start.x();
    p.y = start.y();
    q.x = end.x();
    q.y = end.y();
    GUIDrawLine( handle(), &p, &q, ls, thickness, attr );
}


void WEXPORT WWindow::drawLine( const WPoint &start, const WPoint &end, Color colour ) {
/**************************************************************************************/

    drawLine( start, end, LS_PEN_SOLID, 1, colour );
}


void WEXPORT WWindow::drawLine( const WPoint &start, const WPoint &end, WPaintAttr attr ) {
/*****************************************************************************************/

    drawLine( start, end, LS_PEN_SOLID, 1, attr );
}


void WEXPORT WWindow::drawText( const WPoint& p, const char *str, int len,
                                Color fg, Color bg ) {
/****************************************************/

    gui_coord   pos;

    if( !isPainting() ) return;
    pos.x = p.x();
    pos.y = p.y();
    GUIDrawTextPosRGB( handle(), (char *)str, len, &pos, fg, bg );
}


void WEXPORT WWindow::drawText( const WPoint& p, const char *str,
                                Color fg, Color bg ) {
/****************************************************/

    drawText( p, str, strlen( str ), fg, bg );
}


void WEXPORT WWindow::drawText( const WPoint& p, const char *str, int len,
                                WPaintAttr attr ) {
/*************************************************/

    gui_coord   pos;

    if( !isPainting() ) return;
    pos.x = p.x();
    pos.y = p.y();
    GUIDrawTextPos( handle(), (char *)str, len, &pos, attr );
}


void WEXPORT WWindow::drawText( const WPoint& p, const char *str,
                                WPaintAttr attr ) {
/*************************************************/

    drawText( p, str, strlen( str ), attr );
}


void WEXPORT WWindow::drawText( int row, int indent, const char *str, int len,
                                Color fg, Color bg ) {
/****************************************************/

    if( !isPainting() ) return;
    GUIDrawTextRGB( handle(), (char *)str, len, row, indent, fg, bg );
}


void WEXPORT WWindow::drawText( int row, int indent, const char *str, Color fg, Color bg ) {
/******************************************************************************************/

    drawText( row, indent, str, strlen( str ), fg, bg );
}


void WEXPORT WWindow::drawText( int row, int indent, const char *str, int len,
                                WPaintAttr attr ) {
/*************************************************/

    if( !isPainting() ) return;
    GUIDrawText( handle(), (char *)str, len, row, indent, attr );
}


void WEXPORT WWindow::drawText( int row, int indent, const char *str,
                                WPaintAttr attr ) {
/*************************************************/

    drawText( row, indent, str, strlen( str ), attr );
}


void WEXPORT WWindow::drawText( const WPoint& p, const char *str, int len ) {
/***************************************************************************/

    drawText( p, str, len, ColorBlack, ColorWhite );
}


void WEXPORT WWindow::drawText( const WPoint& p, const char *str ) {
/******************************************************************/

    drawText( p, str, ColorBlack, ColorWhite );
}


void WEXPORT WWindow::drawText( int row, int offset, const char *str, int len ) {
/*****************************************************************************/

    drawText( row, offset, str, len, GUI_MENU_PLAIN );
}


void WEXPORT WWindow::drawText( int row, int offset, const char *str ) {
/**********************************************************************/

    drawText( row, offset, str, GUI_MENU_PLAIN );
}


void WEXPORT WWindow::drawTextExtent( int row, int offset,
                                      const char *str, int len,
                                      Color fg, Color bg, int extent ) {
/**********************************************************************/

    if( !isPainting() ) return;
    GUIDrawTextExtentRGB( handle(), (char *)str, len, row, offset,
                          fg, bg, extent );
}


void WEXPORT WWindow::drawTextExtent( int row, int offset, const char *str,
                                      Color fg, Color bg, int extent ) {
/**********************************************************************/

    drawTextExtent( row, offset, str, strlen( str ), fg, bg, extent );
}


void WEXPORT WWindow::drawTextExtent( const WPoint &p,
                                      const char *str, int len,
                                      Color fg, Color bg, int extent ) {
/**********************************************************************/

    gui_coord   pos;

    if( !isPainting() ) return;
    pos.x = p.x();
    pos.y = p.y();
    GUIDrawTextExtentPosRGB( handle(), (char *)str, len, &pos, fg, bg, extent );
}


void WEXPORT WWindow::drawTextExtent( const WPoint &p, const char *str,
                                      Color fg, Color bg,
                                      int extent ) {
/**************************************************/

    drawTextExtent( p, str, strlen( str ), fg, bg, extent );
}


void WEXPORT WWindow::drawTextExtent( int row, int offset,
                                      const char *str, int len,
                                      WPaintAttr attr, int extent ) {
/*******************************************************************/

    if( !isPainting() ) return;
    GUIDrawTextExtent( handle(), (char *)str, len, row, offset, attr, extent );
}


void WEXPORT WWindow::drawTextExtent( int row, int offset, const char *str,
                                      WPaintAttr attr, int extent ) {
/*******************************************************************/

    drawTextExtent( row, offset, str, strlen( str ), attr, extent );
}


void WEXPORT WWindow::drawTextExtent( const WPoint &p,
                                      const char *str, int len,
                                      WPaintAttr attr, int extent ) {
/*******************************************************************/

    gui_coord   pos;

    if( !isPainting() ) return;
    pos.x = p.x();
    pos.y = p.y();
    GUIDrawTextExtentPos( handle(), (char *)str, len, &pos, attr, extent );
}


void WEXPORT WWindow::drawTextExtent( const WPoint &p, const char *str,
                                      WPaintAttr attr, int extent ) {
/*******************************************************************/

    drawTextExtent( p, str, strlen( str ), attr, extent );
}


void WEXPORT WWindow::drawTextExtent( const WPoint& p,
                                      const char *str, int len,
                                      int extent ) {
/**************************************************/

    drawTextExtent( p, str, len, ColorBlack, ColorWhite, extent );
}


void WEXPORT WWindow::drawTextExtent( const WPoint& p, const char *str,
                                      int extent ) {
/**************************************************/

    drawTextExtent( p, str, ColorBlack, ColorWhite, extent );
}


void WEXPORT WWindow::drawTextExtent( int row, int offset,
                                      const char *str, int len,
                                      int extent ) {
/**************************************************/

    drawTextExtent( row, offset, str, len, GUI_MENU_PLAIN, extent );
}


void WEXPORT WWindow::drawTextExtent( int row, int offset, const char *str,
                                      int extent ) {
/**************************************************/

    drawTextExtent( row, offset, str, GUI_MENU_PLAIN, extent );
}


void WEXPORT WWindow::invalidateRect( const WRect &r ) {
/******************************************************/

    gui_rect    rr;

    rr.x = r.x();
    rr.y = r.y();
    rr.width = r.w();
    rr.height = r.h();
    GUIWndDirtyRect( handle(), &rr );
}


void WEXPORT WWindow::getPaintRect( WRect &r ) {
/**********************************************/

    gui_rect    rr;

    GUIGetPaintRect( handle(), &rr );
    r.x( rr.x );
    r.y( rr.y );
    r.w( rr.width );
    r.h( rr.height );
}


void WEXPORT WWindow::drawHotSpot( int hot_spot, int row, int offset ) {
/**********************************************************************/

    if( !isPainting() ) return;
    GUIDrawHotSpot( handle(), hot_spot, row, offset, GUI_BACKGROUND );
}


Color WEXPORT WWindow::backgroundColour() {
/*****************************************/

    gui_colour_set      colour;
    Color               rgb;

    GUIGetWndColour( handle(), WPaintAttrBackground, &colour );
    GUIGetRGB( colour.back, &rgb );
    return( rgb );
}
