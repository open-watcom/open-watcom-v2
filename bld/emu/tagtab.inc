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


t_tag struc     ; NB this structure MUST be 10 bytes long!!!
    t_off       dw      0
    t_zero      dw      0
    t_valid     dw      0
    t_nan       dw      0
    t_empty     dw      0
t_tag ends

;                       off,    zero,   valid,  nan,    empty
tagtab  t_tag    <      0FFFCH, 00001H, 00000H, 00002H, 00003H  >       ; 00
        t_tag    <      0FFF3H, 00004H, 00000H, 00008H, 0000CH  >       ; 10
        t_tag    <      0FFCFH, 00010H, 00000H, 00020H, 00030H  >       ; 20
        t_tag    <      0FF3FH, 00040H, 00000H, 00080H, 000C0H  >       ; 30
        t_tag    <      0FCFFH, 00100H, 00000H, 00200H, 00300H  >       ; 40
        t_tag    <      0F3FFH, 00400H, 00000H, 00800H, 00C00H  >       ; 50
        t_tag    <      0CFFFH, 01000H, 00000H, 02000H, 03000H  >       ; 60
        t_tag    <      03FFFH, 04000H, 00000H, 08000H, 0C000H  >       ; 70
