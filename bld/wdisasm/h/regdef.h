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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define regpick( en, str ) en##_REG,
typedef enum {
    #include "regpick.h"
    LAST_REG
} processor_reg;
#undef regpick

#define FIRST_BYTE_REG          AL_REG
#define FIRST_WORD_REG          AX_REG
#define LAST_WORD_REG           DI_REG
#define FIRST_DWORD_REG         EAX_REG
#define FIRST_SEG_REG           ES_REG
#define FIRST_FP_REG            ST0_REG
#define FIRST_CONTROL_REG       CR0_REG
#define FIRST_DEBUG_REG         DR0_REG
#define FIRST_TEST_REG          TR0_REG
#define FIRST_MM_REG            MM0_REG
