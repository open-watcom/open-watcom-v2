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


#ifndef _UIMOUSE_H_
#define _UIMOUSE_H_

#define         MOUSE_ON                1
#define         MOUSE_OFF               2

#define         MOUSE_PRESS             0x0001
#define         MOUSE_PRESS_RIGHT       0x0002
#define         MOUSE_PRESS_MIDDLE      0x0004
#define         MOUSE_PRESS_ANY         (MOUSE_PRESS | \
                                        MOUSE_PRESS_RIGHT | \
                                        MOUSE_PRESS_MIDDLE)

#define         M_PRESS                 1
#define         M_RELEASE               2
#define         M_DCLICK                3
#define         M_HOLD                  4
#define         M_DRAG                  5
#define         M_REPEAT                6

#endif
