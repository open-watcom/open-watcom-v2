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


#ifndef _FORMOPT_H
/*
 * Formatting options
 */

typedef enum {
    FORM_REG_UPPER      = 0x0001,       /* display register names in u.c. */
    FORM_NAME_UPPER     = 0x0002,       /* display instruction names in u.c. */
    FORM_INDEX_IN       = 0x0004,       /* display index inside ( [BP-2] ) */
    FORM_ASSEMBLER      = 0x0008,       /* assembler format */
    FORM_DO_WTK         = 0x0010,       /* dismacro weitek code */
    FORM_UNIX           = 0x0020,       /* UNIX 386asm assembler format */
} form_option;

#define DO_UNIX         ( Options & FORM_UNIX )

#define _FORMOPT_H
#endif
