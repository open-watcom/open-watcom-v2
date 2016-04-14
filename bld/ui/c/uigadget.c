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


#include <math.h>
#include "uidef.h"
#include "uigadget.h"

#define _ATTR                   (UIData->attrs[ ATTR_SCROLL_ICON ])
#define _ATTR_BAR               (UIData->attrs[ ATTR_SCROLL_BAR ])
#define _ATTR_SLIDER            (UIData->attrs[ ATTR_SCROLL_BAR ])

#define row( g, i )     (g->dir == HORIZONTAL ? g->anchor : i)
#define col( g, i )     (g->dir == HORIZONTAL ? i : g->anchor)

char            VertScrollFrame[2]  = SCROLL_FRAME;
char            HorzScrollFrame[2]  = SCROLL_FRAME;
char            SliderChar[2]       = SLIDER_CHAR;
char            LeftPoint[2]        = LEFT_POINT;
char            RightPoint[2]       = RIGHT_POINT;
char            UpPoint[2]          = UP_POINT;
char            DownPoint[2]        = DOWN_POINT;

static  p_gadget        Pressed         = NULL;   /* pointer to gadget where mouse pressed  */
static  bool            Drag            = false;
static  EVENT           RepeatEvent     = EV_NO_EVENT;
static  int             StartPos        = 0;

static void drawgadget( p_gadget g )
{
    int                 i;
    int                 length;

    uiunprotect( g->win );
    length = g->end - g->start - 1;
    for( i=g->start; i <= g->end; ++i ){
        if( g->dir == VERTICAL ) {
            uivtextput( g->win, row(g,i), col(g,i), _ATTR, VertScrollFrame, 0 );
        } else {
            uivtextput( g->win, row(g,i), col(g,i), _ATTR, HorzScrollFrame, 0 );
        }
    }
    /* don't draw scroll thumb in g->total_size <= page_size */
    if( ( g->total_size > g->page_size ) && ( length > 1 ) ) {
        if( g->dir == HORIZONTAL ) {
            uivtextput( g->win, g->anchor, g->linear, _ATTR_SLIDER, SliderChar, 0 );
        } else {
            uivtextput( g->win, g->linear, g->anchor, _ATTR_SLIDER, SliderChar, 0 );
        }
    }
    if( g->dir == HORIZONTAL ){
        uivtextput( g->win, g->anchor, g->start, _ATTR, LeftPoint, 0 );
        uivtextput( g->win, g->anchor, g->end, _ATTR, RightPoint, 0 );
    } else {
        uivtextput( g->win, g->start, g->anchor, _ATTR, UpPoint, 0 );
        uivtextput( g->win, g->end, g->anchor, _ATTR, DownPoint, 0 );
    }
    uiprotect( g->win );
}

static void setlinear( p_gadget g )
{
    if( g->pos <= 0 ) {
        g->linear = g->start + 1;
    } else if( g->pos >= ( g->total_size - g->page_size ) ) {
        g->linear = g->end - 1;
    } else {
        g->linear = g->start + 1 +
                    (int)((long)g->pos * (long)( g->end - g->start - 1 ) /
                          (long)( g->total_size - g->page_size ) );
    }
    if( g->linear > ( g->end - 1 ) ) {
        g->linear = g->end - 1;
    }
    if( g->linear < ( g->start + 1 ) ) {
        g->linear = g->start + 1;
    }
    if( ( g->linear == ( g->start + 1 ) ) && ( g->pos > 0 ) ) {
        g->linear++;
    }
    if( ( g->linear == ( g->end - 1 ) ) && ( g->pos < ( g->total_size - g->page_size ) ) ) {
        g->linear--;
    }
}

void uiinitgadget( p_gadget g )
{
    setlinear( g );
    drawgadget( g );
    /* do NOT uirefresh here please, it causes screen flashing */
}

void uidrawgadget( p_gadget g )
{
    drawgadget( g );
    uirefresh();
}

void uishowgadget( p_gadget g )
{
    drawgadget( g );
}

static void setgadget( p_gadget g, int pos, bool draw )
{
    g->pos = pos;
    setlinear( g );
    if( draw ) {
        drawgadget( g );
    }
}

void uisetgadget( p_gadget g, int pos )
{
    setgadget( g, pos, true );
}

void uisetgadgetnodraw( p_gadget g, int pos )
{
    setgadget( g, pos, false );
}

void uifinigadget( p_gadget g )
{
    g=g;
}

EVENT uigadgetfilter( EVENT ev, p_gadget g )
{
    int         m_anchor, m_linear;
    int         tmp;
    EVENT       newev;
    ORD         start;
    int         length;
    int         pos = 0;

    if( uimouseinstalled() ) {
        uiunprotect( g->win );
        uimousepos( g->win, &m_anchor, &m_linear );
        uiprotect( g->win );
        if( g->dir == VERTICAL ) {
            tmp = m_linear;
            m_linear = m_anchor;
            m_anchor = tmp;
        }
        if( ( ev == EV_MOUSE_PRESS ) || ( ev == EV_MOUSE_DCLICK ) ) {
            if( ( m_anchor != g->anchor ) || ( m_linear < g->start ) ||
                ( m_linear > g->end ) || ( Pressed != NULL ) ) {
                return( ev );
            } else {
                Pressed = g;
            }
        }
        /* ignore everything if the gadget was not pressed */
        if( Pressed != g ) return( ev );
        length = g->end - g->start - 1;
        /* don't send pagefoward followed by pagebackward, then forward */
        /* ignore non-mouse events */
        switch( ev ) {
        case EV_MOUSE_PRESS :
            StartPos = g->pos;
        case EV_MOUSE_DCLICK :
            RepeatEvent = EV_NO_EVENT;
        case EV_MOUSE_REPEAT :
            if( Drag ) {
                break;
            }
            if( m_linear == g->start ) {
                return( g->backward );
            }
            if( m_linear == g->end ) {
                return( g->forward );
            }
            /* don't do page up and page down when total size is less than
               or equal to the page size */
            if( g->total_size <= g->page_size ) break;
            start = g->linear; //CalcStart( g, g->pos, length );
            if( m_linear < start ) {
                if( RepeatEvent == g->pageforward ) {
                    return( EV_NO_EVENT );
                } else {
                    RepeatEvent = g->pagebackward;
                    return( g->pagebackward );
                }
            }
            if( m_linear > start ) {
                if( RepeatEvent == g->pagebackward ) {
                    return( EV_NO_EVENT );
                } else {
                    RepeatEvent = g->pageforward;
                    return( g->pageforward );
                }
            }
            break;
        case EV_MOUSE_DRAG :
            /* don't do draging if total_size is less than or equal to the
               page size or mouse is too far from gadget */
            if( ( m_anchor < ( g->anchor -1 ) ) || ( m_anchor > (g->anchor+1) ) ||
                ( g->total_size <= g->page_size ) ) {
                return( EV_NO_EVENT );
            } else {
                Drag = true; /* so we don't send page events on MOUSE_REPEAT */
                if( g->slider == EV_NO_EVENT ) {
                    return( EV_NO_EVENT );
                }
            }
        case EV_MOUSE_RELEASE :
            if( Pressed == NULL ) {
                break;
            }
            if( g->slider == EV_NO_EVENT ) {
                Drag = false;
            }
            if( Drag ) {
                if( ( m_anchor < ( g->anchor -1 ) ) || ( m_anchor > (g->anchor+1) ) ) {
                    /* note : must have got EV_MOUSE_RELEASE */
                    pos = StartPos;
                    setgadget( g, pos, false );
                    m_linear = g->linear;
                    Drag = false;
                } else {
                    /* mouse drag to first scroll character or further left,
                       so pos = 0 */
                    if( m_linear <= ( g->start + 1 ) ) {
                        m_linear = g->start + 1;
                        pos = 0;
                    } else {
                        /* mouse drag to last scroll character or further right,
                           so pos = total_size */
                        if( m_linear >= ( g->end - 1 ) ) {
                            m_linear = g->end - 1;
                            pos = g->total_size - g->page_size;
                         } else {
                            pos = (int)((long)( m_linear - g->start ) *
                                        (long)( g->total_size - g->page_size ) /
                                        (long)length);
                         }
                    }
                }
                g->linear = m_linear;
                uidrawgadget( g );
            }
            if( ( ev == EV_MOUSE_RELEASE ) || ( g->flags & GADGET_TRACK ) ) {
                if( Drag ) {
                    StartPos = pos;
                    g->pos = pos;
                    g->linear = m_linear;
                    setlinear( g );
                    if( g->linear < m_linear ) {
                        g->pos++;
                        setlinear( g );
                    }
                    if( g->linear > m_linear ) {
                        g->pos--;
                        setlinear( g );
                    }
                    newev = g->slider;
                } else {
                    newev = EV_NO_EVENT;
                }
                if( ev == EV_MOUSE_RELEASE ) {
                    Drag = false;
                    Pressed = NULL;
                }
            } else {
                newev = EV_NO_EVENT;
            }
            return( newev );
        case EV_MOUSE_HOLD :
            break;
        default :
            return( ev );
        }
    } else {
        return( ev );
    }
    return( EV_NO_EVENT );
}
