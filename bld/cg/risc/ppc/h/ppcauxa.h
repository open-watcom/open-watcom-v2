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


#define LINKAGE_OS                      ( 0x00000001L << _TARG_AUX_SHIFT )
#define LINKAGE_OSFUNC                  ( 0x00000002L << _TARG_AUX_SHIFT )
#define LINKAGE_OSENTRY                 ( 0x00000004L << _TARG_AUX_SHIFT )
#define LINKAGE_CLINK                   ( 0x00000008L << _TARG_AUX_SHIFT )
#define CALLER_POPS                     ( 0x00000010L << _TARG_AUX_SHIFT )
#define NO_MEMORY_READ                  ( 0x00000020L << _TARG_AUX_SHIFT )
#define NO_MEMORY_CHANGED               ( 0x00000040L << _TARG_AUX_SHIFT )
#define DLL_EXPORT                      ( 0x00000080L << _TARG_AUX_SHIFT )

#define LINKAGES (LINKAGE_OS+LINKAGE_OSFUNC+LINKAGE_OSENTRY+LINKAGE_CLINK)

#define LAST_TARG_AUX_ATTRIBUTE        ( 0x00800000L << _TARG_AUX_SHIFT )

#if LAST_TARG_AUX_ATTRIBUTE == 0
    #error Overflowed a long
#endif

typedef struct {
        hw_reg_set     gp;
        hw_reg_set     sp;
        hw_reg_set     ln;
        hw_reg_set     ra;
        hw_reg_set     pr;
        hw_reg_set     sa;
} linkage_regs;
