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


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> OVLDATA:    data common to all files in the overlay manager.
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

_TEXT   segment word '_OVLCODE' PUBLIC
        assume  CS:_TEXT

        public  __OVLCAUSE__
        public  __OVLPSP__
        public  __OVLMSGS__
        public  __OVLFILEPREV__
        public  __OVLHDLPREV__
        public  __OVLFLAGS__
        public  __OVLSHARE__
        public  __OVLNULLSTR__

__OVLPSP__      dw      ?
__OVLCAUSE__    dw      ?,?
__OVLHDLPREV__  dd      ?
__OVLFILEPREV__ dw      ?
__OVLFLAGS__    dw      ?
__OVLSHARE__    dw      0

__OVLNULLSTR__  db      0

; Messages used by overlay manager.

Msg0:       db      28, "error opening section file", 0dh, 0ah
Msg1:       db      33, "i/o error while reading overlay", 0dh, 0ah
Msg2:       db      30, "incompatible overlay manager", 0dh, 0ah
Msg3:       db      23, "out of overlay memory", 0dh, 0ah
ifdef OVL_DEBUG
Msg4:       db       8, "Section "
Msg5:       db       9, " loaded", 0dh, 0ah
Msg6:       db      11, " resident", 0dh, 0ah
Msg7:       db      11, " unloaded", 0dh, 0ah
Msg8:       db      8,  " moved", 0dh, 0ah
Msg9:       db      17, " return trapped", 0dh, 0ah
endif
__OVLMSGS__ dd      Msg0
            dd      Msg1
            dd      Msg2
            dd      Msg3
ifdef OVL_DEBUG
            dd      Msg4
            dd      Msg5
            dd      Msg6
            dd      Msg7
            dd      Msg8
            dd      Msg9
endif

_TEXT   ends

        end


