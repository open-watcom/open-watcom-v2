/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Watcom Debug FMT x86 register definitions and related data.
*
****************************************************************************/


/*    name ci   start len */
pick( AL,  CI_EAX, 0, 8 )
pick( AH,  CI_EAX, 8, 8 )
pick( BL,  CI_EBX, 0, 8 )
pick( BH,  CI_EBX, 8, 8 )
pick( CL,  CI_ECX, 0, 8 )
pick( CH,  CI_ECX, 8, 8 )
pick( DL,  CI_EDX, 0, 8 )
pick( DH,  CI_EDX, 8, 8 )
pick( AX,  CI_EAX, 0, 16 )
pick( BX,  CI_EBX, 0, 16 )
pick( CX,  CI_ECX, 0, 16 )
pick( DX,  CI_EDX, 0, 16 )
pick( SI,  CI_ESI, 0, 16 )
pick( DI,  CI_EDI, 0, 16 )
pick( BP,  CI_EBP, 0, 16 )
pick( SP,  CI_ESP, 0, 16 )
pick( CS,  CI_CS,  0, 16 )
pick( SS,  CI_SS,  0, 16 )
pick( DS,  CI_DS,  0, 16 )
pick( ES,  CI_ES,  0, 16 )
pick( ST0, CI_ST0, 0, 80 )
pick( ST1, CI_ST1, 0, 80 )
pick( ST2, CI_ST2, 0, 80 )
pick( ST3, CI_ST3, 0, 80 )
pick( ST4, CI_ST4, 0, 80 )
pick( ST5, CI_ST5, 0, 80 )
pick( ST6, CI_ST6, 0, 80 )
pick( ST7, CI_ST7, 0, 80 )
pick( EAX, CI_EAX, 0, 32 )
pick( EBX, CI_EBX, 0, 32 )
pick( ECX, CI_ECX, 0, 32 )
pick( EDX, CI_EDX, 0, 32 )
pick( ESI, CI_ESI, 0, 32 )
pick( EDI, CI_EDI, 0, 32 )
pick( EBP, CI_EBP, 0, 32 )
pick( ESP, CI_ESP, 0, 32 )
pick( FS,  CI_FS,  0, 16 )
pick( GS,  CI_GS,  0, 16 )

