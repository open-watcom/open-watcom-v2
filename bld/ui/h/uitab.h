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


#ifndef _UITAB_H_
#define _UITAB_H_

typedef void            a_tab_field;

typedef struct vtab {
        unsigned          (*tab)();     /* is field in tab sequence ? */
        a_tab_field     * (*next)();    /* get next VFIELD */
        void            * parm;         /* parm for functions */
        void            * (*mousepos)();/* mouse position function */
        void            * mouseparm;    /* parm for mousepos function */
        a_tab_field     * other;        /* prev VFIELD or moused but no tab */
        a_tab_field     * curr;         /* current VFIELD */
        a_tab_field     * first;        /* first VFIELD */
        ORD               home;         /* home column for tab algorithm */
        unsigned          wrap:1;       /* wrap cursor top to bottom */
        unsigned          enter:1;      /* grab enter event as CR */
} VTAB;

EVENT uitabfilter( EVENT, VTAB * );
#endif
