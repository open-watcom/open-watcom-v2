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


fpbits  db              01H     ; C3 = 0, C2 = 0, C0 = 1  a < b
        db              40H     ; C3 = 1, C2 = 0, C0 = 0  a == b
        db              00H     ; C3 = 0, C2 = 0, C0 = 0  a > b
        db              45H     ; C3 = 1, C2 = 1, C0 = 1  one of opnds is NaN

;                                  C0 C3 C1  (order wanted by fprem)
ccbits  db              00H     ;   0  0  0
        db              02H     ;   0  0  1
        db              40H     ;   0  1  0
        db              42H     ;   0  1  1
        db              01H     ;   1  0  0
        db              03H     ;   1  0  1
        db              41H     ;   1  1  0
        db              43H     ;   1  1  1
