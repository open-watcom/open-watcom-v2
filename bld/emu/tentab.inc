;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


t_table struc           ; NB: this structure MUST be 10 bytes long
t_next   dw     0
t_prev   dw     0
t_stktop dw     0       ; stack top # in correct position for status word
t_regnum dw     0       ; register number of stack
         dw     0
t_table ends

;                       next    prev    stktop    regnum
tentab  t_table <       10,     70,     00 shl 11, 0    >       ; 00
        t_table <       20,     00,     01 shl 11, 1    >       ; 10
        t_table <       30,     10,     02 shl 11, 2    >       ; 20
        t_table <       40,     20,     03 shl 11, 3    >       ; 30
        t_table <       50,     30,     04 shl 11, 4    >       ; 40
        t_table <       60,     40,     05 shl 11, 5    >       ; 50
        t_table <       70,     50,     06 shl 11, 6    >       ; 60
        t_table <       00,     60,     07 shl 11, 7    >       ; 70


