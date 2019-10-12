/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _UIVEDIT_H
#define _UIVEDIT_H

typedef struct veditline {
    ORD             row;            /* position of field on vscreen     */
    ORD             col;            /* ...                              */
    unsigned        fldlen;         /* length of field on vscreen       */
    int             scroll;         /* index of first visible character */
    unsigned        length;         /* length of buffer                 */
    char      _FARD *buffer;        /* buffer of characters editted     */
    unsigned        index;          /* cursor position in buffer        */
    ATTR            attr;           /* attribute for output to vscreen  */
    boolbit         dirty       :1; /* boolean: user changed buffer     */
    boolbit         update      :1; /* boolean: application has changed */
    boolbit         auto_clear  :1; /* clear contents when user types   */
    boolbit         invisible   :1; /* characters are invisible         */
    boolbit         marking     :1; /* boolean: are we marking?         */
    unsigned        mark_anchor;    /* marking anchor position          */
    ATTR            mark_attr;      /* marking attribute                */
} VEDITLINE;
/*                                                                         */
/* the veditline update flag must be set to true to start editting or      */
/* whenever the application alters any information in the veditline struct */
/* other than simply resetting the dirty flag                              */
/*                                                                         */

#ifdef __cplusplus
    extern "C" {
#endif

extern ui_event     UIAPI uiveditevent( VSCREEN *, VEDITLINE *, ui_event );
extern ui_event     UIAPI uiveditline( VSCREEN *, VEDITLINE * );
extern bool         UIAPI uiveditinit( VSCREEN *, VEDITLINE *, char *, unsigned, ORD, ORD, unsigned );
extern bool         UIAPI uiveditfini( VSCREEN *, VEDITLINE * );

#ifdef __cplusplus
}
#endif

#endif
