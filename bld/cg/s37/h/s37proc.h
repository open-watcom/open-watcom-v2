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


typedef enum {
        ROUTINE_MODIFY_EXACT            = 0x0001,
        ROUTINE_MODIFIES_NO_MEMORY      = 0x0002,
        ROUTINE_READS_NO_MEMORY         = 0x0004,
        ROUTINE_REMOVES_PARMS           = 0x0008,
        ROUTINE_NEVER_RETURNS           = 0x0010,
        ROUTINE_WANTS_DEBUGGING         = 0x0020,
        ROUTINE_ALTERNATE_AR            = 0x0040,
        ROUTINE_OS                      = 0x0080,
        ROUTINE_ENTRY                   = 0x0100,
        ROUTINE_FUNC                    = 0x0200
} call_attributes;

typedef byte    reg_num;

#define NO_REGISTER     ((reg_num)-1)

typedef struct {
        reg_num                 SP;             /* stack pointer */
        reg_num                 AR;             /* activiation record */
        reg_num                 RA;             /* return address */
        reg_num                 LN;             /* linkage register (retval)*/

        reg_num                 BR;             /* base register */
        reg_num                 GP;             /* pointer to global data */
        reg_num                 PA;             /* parm pointer */
        reg_num                 SA;             /* save area pointer */

        reg_num                 PR;             /* initial parm register */
        reg_num                 sAR;            /* saved AR */
        reg_num                 sBR;            /* saved BR */
        reg_num                 sGP;            /* saved GP */

        reg_num                 sPA;            /* saved PR */
        reg_num                 sSA;            /* saved SA */
} call_registers;

typedef struct target_proc_def {
        type_length             save_area;
        type_length             flt_save_area;
        type_length             far_local_size;
        call_registers          regs;
        struct hw_sym           *using_label;
        reg_num                 first_save_reg; /* first reg in STM */
        reg_num                 last_save_reg;  /* last reg in STM */
} target_proc_def;
