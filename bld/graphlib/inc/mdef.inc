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


_SMALL_CODE     =       00h
_BIG_CODE       =       01h
_SMALL_DATA     =       00h
_BIG_DATA       =       02h
_HUGE_DATA      =       04h
_LONG_INTS      =       08h

_SMALL          =       ( _SMALL_CODE or _SMALL_DATA )
_COMPACT        =       ( _SMALL_CODE or _BIG_DATA )
_MEDIUM         =       ( _BIG_CODE   or _SMALL_DATA )
_LARGE          =       ( _BIG_CODE   or _BIG_DATA )
_HUGE           =       ( _BIG_CODE   or _HUGE_DATA )


ifdef _386

modstart        macro   modname
                name    modname
_TEXT           segment word public 'CODE'
                assume  cs:_TEXT
                endm

endmod          macro   modname
_TEXT           ends
                endm

else

modstart        macro   modname
                name    modname
GRAPH_TEXT      segment word public 'CODE'
                assume  cs:GRAPH_TEXT
                endm

endmod          macro   modname
GRAPH_TEXT      ends
                endm

endif


xdefp           macro   xsym
                public  xsym
                endm

endproc         macro   dsym
         dsym   endp
                endm

xref            macro   dsym
                extrn   dsym : near
                endm

docall          macro   dest
                call    dest
                endm


if _MODEL and _BIG_CODE

defp            macro   dsym
         dsym   proc    far
                endm

else

defp            macro   dsym
         dsym   proc    near
                endm

endif
