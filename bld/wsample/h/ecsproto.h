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


#pragma aux fortran "*" parm caller []\
   value struct float struct routine [ax]  modify [ax bx cx dx es si di];

#pragma aux (fortran) OS386_Get_Version;
#pragma aux (fortran) OS386_Get_Protected_Machine;
#pragma aux (fortran) OS386_Init_Machine;
#pragma aux (fortran) OS386_Create_Task;
#pragma aux (fortran) OS386_Task_Control;
#pragma aux (fortran) OS386_Step_Task;
#pragma aux (fortran) OS386_Get_Exit_Code;
#pragma aux (fortran) OS386_Read_Mem;
#pragma aux (fortran) OS386_Write_Mem;
#pragma aux (fortran) OS386_Read_Protected_Mem;
#pragma aux (fortran) OS386_Write_Protected_Mem;
#pragma aux (fortran) OS386_Get_Segment_Info;
#pragma aux (fortran) OS386_Get_CR;
#pragma aux (fortran) OS386_Get_DR;
#pragma aux (fortran) OS386_Get_GDT;
#pragma aux (fortran) OS386_Get_LDT;
#pragma aux (fortran) OS386_Get_TSS;
#pragma aux (fortran) OS386_Get_Task_ID;
#pragma aux (fortran) OS386_Set_CR;
#pragma aux (fortran) OS386_Set_DR;
#pragma aux (fortran) OS386_Hook_Int10;
#pragma aux (fortran) OS386_Hook_Int16;
#pragma aux (fortran) OS386_Queue_Status;
#pragma aux (fortran) OS386_Hook_Packets;
#pragma aux (fortran) OS386_Connect_To_Driver;
#pragma aux (fortran) OS386_Get_Linear_Addr;
#pragma aux (fortran) OS386_Get_Physical_Addr;
#pragma aux (fortran) OS386_Declare_RPC;
#pragma aux (fortran) OS386_Delete_RPC;
#pragma aux (fortran) OS386_Generate_Signal;
#pragma aux (fortran) OS386_Get_Parameter_Block;
#pragma aux (fortran) OS386_Get_Base_PSP;
#pragma aux (fortran) OS386_Get_NPX_State;
#pragma aux (fortran) OS386_Set_NPX_State;
#pragma aux (fortran) OS386_Get_Ptr_to_Intr;
