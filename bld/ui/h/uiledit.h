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


#ifndef _UILEDIT_H
#define _UILEDIT_H

typedef struct a_ui_edit {
        char                    *edit_buffer;
        VSCREEN                 *edit_screen;
        VEDITLINE               edit_eline;
        unsigned                edit_maxlen;
        struct a_ui_edit        *next;
} a_ui_edit;

#ifdef __cplusplus
    extern "C" {
#endif

extern a_ui_edit *uibegedit( VSCREEN *, ORD,
            ORD, ORD, ATTR, char *, unsigned,
            unsigned int, unsigned int, bool, unsigned int, bool );
extern void uieditpushlist(void);
extern void uieditpoplist(void);
/* temp kludge - modify client source to use uiledit() instead */
#undef uiedit
#define uiedit(a)       uiledit(a)
extern int uiledit( int );
extern void uieditinsert( char *, unsigned );
extern unsigned int uiendedit( void );
extern bool uieditisdirty( void );
extern bool uieditautoclear( void );
extern void uieditdirty( void );
extern void uieditcursor( unsigned );
extern void uieditclean( void );
extern void uiedittrim( char * );
extern void uieditmarking( bool, unsigned );

#ifdef __cplusplus
}
#endif

#endif
