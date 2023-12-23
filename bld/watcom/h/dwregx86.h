/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF x86 register definitions and related data.
*
****************************************************************************/

/*    id      name    ci   start len */
pick( EAX,    eax,    CI_EAX, 0, 32 )
pick( ECX,    ecx,    CI_ECX, 0, 32 )
pick( EDX,    edx,    CI_EDX, 0, 32 )
pick( EBX,    ebx,    CI_EBX, 0, 32 )
pick( ESP,    esp,    CI_ESP, 0, 32 )
pick( EBP,    ebp,    CI_EBP, 0, 32 )
pick( ESI,    esi,    CI_ESI, 0, 32 )
pick( EDI,    edi,    CI_EDI, 0, 32 )
pick( EIP,    eip,    CI_EIP, 0, 32 )
pick( EFLAGS, eflags, CI_EFL, 0, 32 )
pick( TRAPNO, trapno, 0,      0, 0  )
pick( ST0,    st0,    CI_ST0, 0, 80 )
pick( ST1,    st1,    CI_ST1, 0, 80 )
pick( ST2,    st2,    CI_ST2, 0, 80 )
pick( ST3,    st3,    CI_ST3, 0, 80 )
pick( ST4,    st4,    CI_ST4, 0, 80 )
pick( ST5,    st5,    CI_ST5, 0, 80 )
pick( ST6,    st6,    CI_ST6, 0, 80 )
pick( ST7,    st7,    CI_ST7, 0, 80 )
pick( AL,     al,     CI_EAX, 0, 8  )
pick( AH,     ah,     CI_EAX, 8, 8  )
pick( BL,     bl,     CI_EBX, 0, 8  )
pick( BH,     bh,     CI_EBX, 8, 8  )
pick( CL,     cl,     CI_ECX, 0, 8  )
pick( CH,     ch,     CI_ECX, 8, 8  )
pick( DL,     dl,     CI_EDX, 0, 8  )
pick( DH,     dh,     CI_EDX, 8, 8  )
pick( AX,     ax,     CI_EAX, 0, 16 )
pick( BX,     bx,     CI_EBX, 0, 16 )
pick( CX,     cx,     CI_ECX, 0, 16 )
pick( DX,     dx,     CI_EDX, 0, 16 )
pick( SI,     si,     CI_ESI, 0, 16 )
pick( DI,     di,     CI_EDI, 0, 16 )
pick( BP,     bp,     CI_EBP, 0, 16 )
pick( SP,     sp,     CI_ESP, 0, 16 )
pick( CS,     cs,     CI_CS,  0, 16 )
pick( SS,     ss,     CI_SS,  0, 16 )
pick( DS,     ds,     CI_DS,  0, 16 )
pick( ES,     es,     CI_ES,  0, 16 )
pick( FS,     fs,     CI_FS,  0, 16 )
pick( GS,     gs,     CI_GS,  0, 16 )
