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


#ifndef _UIGADGET_H
#define _UIGADGET_H

#include "uichars.h"

#define UP_ARROW                {(char)PC_arrowup,0}
#define DOWN_ARROW              {(char)PC_arrowdown,0}
#define RIGHT_ARROW             {(char)PC_arrowright,0}
#define LEFT_ARROW              {(char)PC_arrowleft,0}
#define UP_POINT                {(char)PC_triangup,0}
#define DOWN_POINT              {(char)PC_triangdown,0}
#define RIGHT_POINT             {(char)PC_triangright,0}
#define LEFT_POINT              {(char)PC_triangleft,0}
#define SCROLL_FRAME            {(char)PC_sparseblock,0}
#define SLIDER_CHAR             {(char)PC_solid,0}

extern char VertScrollFrame[2];
extern char HorzScrollFrame[2];
extern char SliderChar[2];
extern char LeftPoint[2];
extern char RightPoint[2];
extern char UpPoint[2];
extern char DownPoint[2];

typedef enum a_gadget_direction {
    HORIZONTAL,
    VERTICAL
} a_gadget_direction;

#define GADGET_NONE     0x00
#define GADGET_TRACK    0x01

typedef struct a_gadget{
    VSCREEN             *win;
    a_gadget_direction  dir;
    ORD                 anchor;
    ORD                 start;
    ORD                 end;
    EVENT               forward;
    EVENT               backward;
    EVENT               pageforward;
    EVENT               pagebackward;
    EVENT               slider;
    int                 total_size;
    int                 page_size;
    int                 pos;            /* in users units */
    unsigned char       flags;
    ORD                 linear;         /* offset of displayed scroll thumb */
} a_gadget, *p_gadget;

#ifdef __cplusplus
    extern "C" {
#endif
extern void uiinitgadget(struct a_gadget *);
extern void uidrawgadget(struct a_gadget *);
extern void uishowgadget(struct a_gadget *);
extern void uisetgadget(struct a_gadget *,int);
extern void uisetgadgetnodraw(struct a_gadget *, int);
extern void uifinigadget(struct a_gadget *);
extern EVENT uigadgetfilter(EVENT ,struct a_gadget *);
#ifdef __cplusplus
}
#endif

#endif
