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


          .286

port      segment   word public 'CODE'
          assume    cs:port

public    TrapInit_, TrapFini_, TrapRequest_

TrapInit_ proc      far
          ret
TrapInit_ endp

TrapRequest_ proc      far
          ret
TrapRequest_ endp

TrapFini_ proc      far
          ret
TrapFini_ endp

          public    OUTPORT
OUTPORT   proc      far

          push bp
          mov  bp,sp
          push ax
          push dx

          mov  ax,[bp+6]
          mov  dx,[bp+8]
          out  dx,al

          pop  dx
          pop  ax
          pop  bp

          ret  4

OUTPORT   endp

          public    INPORT
INPORT    proc      far

          push bp
          mov  bp,sp
          push dx

          mov  dx,[bp+6]
          in   al,dx
          xor  ah,ah

          pop  dx
          pop  bp

          ret  2

INPORT    endp


port      ends

          end
