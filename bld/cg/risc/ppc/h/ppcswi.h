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
* Description:  Codegen switches specific to PowerPC.
*
****************************************************************************/


/*
 * Keep next 3 for now, since the C front end references these consts
 * in 'target independant' files
 */
#define BIG_DATA                0x00000001L
#define BIG_CODE                0x00000002L
#define FLOATING_SS             0x00000004L
#define ASM_OUTPUT              0x00000008L
#define ALIGNED_SHORT           0x00000010L
#define I_MATH_INLINE           0x00000020L
#define NO_CALL_RET_TRANSFORM   0x00000040L
#define CHEAP_POINTER           0x00000080L
#define STACK_INIT              0x00000100L
#define EXCEPT_FILTER_USED      0x00000200L
#define STACK_CHECK             0x00000400L
#define CG_OS2_CC               0x00000800L
#define CG_NT_CC                0x00001000L
#define OWL_LOGGING             0x00002000L
#define LAST_TARG_CGSWITCH      0x00002000L

typedef enum {
    I_PPC
} proc_revision;
