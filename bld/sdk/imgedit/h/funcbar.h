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


#ifndef __OS2_PM__
typedef struct {
    char        *name;
    int         id;
    BOOL        has_down;
    char        *downname;
    HBITMAP     hbmp;
    HBITMAP     downbmp;
} button;

#define NONE                    ""
#ifdef __NT__
#define FUNC_BUTTON_WIDTH       26      // add 4 because it's the magic number
#else
#define FUNC_BUTTON_WIDTH       25      // add 3 because it's the magic number
#endif
#ifdef __NT__
#define FUNC_BUTTON_HEIGHT      22      // ditto
#else
#define FUNC_BUTTON_HEIGHT      21      // ditto
#endif

#else
typedef struct {
    int         name;
    int         id;
    BOOL        has_down;
    int         downname;
    HBITMAP     hbmp;
    HBITMAP     downbmp;
} button;

#define NONE                    0
#define FUNC_BUTTON_WIDTH       26
#define FUNC_BUTTON_HEIGHT      22

#endif

#define NUMBER_OF_FUNCTIONS     20
#define FUNC_BORDER_X           2
#define FUNC_BORDER_Y           1
#define MIN_WIDTH               (35+NUMBER_OF_FUNCTIONS*FUNC_BUTTON_WIDTH)
