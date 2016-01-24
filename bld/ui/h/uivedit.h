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


#ifndef _UIVEDIT_H
#define _UIVEDIT_H

typedef struct veditline {
        ORD             row;
        ORD             col;         /* position of field on vscreen         */
        int             fldlen;      /* length of field on vscreen           */
        int             scroll;      /* index of first visible character     */
        int             length;      /* length of buffer                     */
        char      _FARD *buffer;     /* buffer of characters editted         */
        int             index;       /* cursor position in buffer            */
        ATTR            attr;        /* attribute for output to vscreen      */
        unsigned        dirty:1;     /* boolean: user changed buffer         */
        unsigned        update:1;    /* boolean: application has changed     */
        unsigned        auto_clear:1;/* clear contents when user types       */
        unsigned        invisible:1; /* characters are invisible             */
        unsigned        marking:1;   /* boolean: are we marking?             */
        unsigned        mark_anchor; /* marking anchor position              */
        ATTR            mark_attr;   /* marking attribute                    */
} VEDITLINE;
/*                                                                         */
/* the veditline update flag must be set to true to start editting or      */
/* whenever the application alters any information in the veditline struct */
/* other than simply resetting the dirty flag                              */
/*                                                                         */

#ifdef __cplusplus
    extern "C" {
#endif

extern EVENT    uiveditevent( VSCREEN *, VEDITLINE *, EVENT );
extern EVENT    uiveditline( VSCREEN *, VEDITLINE * );
extern bool     uiveditinit(VSCREEN *, VEDITLINE *, char *, int, ORD, ORD, int );
extern bool     uiveditfini(VSCREEN *, VEDITLINE *);

#ifdef __cplusplus
}
#endif

#endif
