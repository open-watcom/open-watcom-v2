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


;
;       Get all the segments to come out in the right order
;
.386p

dilbuffer segment byte public 'DILBUF'
dilbuffer ends

dilcode segment byte public 'DILCOD'
dilcode ends

_TEXT segment byte public 'CODE'
_TEXT ends

TEXT segment byte public 'CODE'
TEXT ends

BEGTEXT segment byte public 'CODE'
BEGTEXT ends


_NULL segment byte public 'BEGDATA'
_NULL ends

_AFTERNULL segment byte public 'BEGDATA'
_AFTERNULL ends

_DATA segment byte public 'DATA'
_DATA ends

_BSS segment byte public 'BSS'
_BSS ends

STACK segment byte stack 'STACK'
STACK ends

DGROUP group _NULL, _AFTERNULL, _DATA, _BSS, STACK, dilbuffer, dilcode, _TEXT, TEXT, BEGTEXT

        END
