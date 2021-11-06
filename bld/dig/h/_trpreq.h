/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2021-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  TRAP core requests related data definitions.
*
****************************************************************************/


/*    sym                            dumb                       std                         */
pick( CONNECT,                       Connect,                   Connect )
pick( DISCONNECT,                    EmptyStub,                 Disconnect )
pick( SUSPEND,                       EmptyStub,                 Suspend )
pick( RESUME,                        EmptyStub,                 Resume )
pick( GET_SUPPLEMENTARY_SERVICE,     Get_supplementary_service, Get_supplementary_service )
pick( PERFORM_SUPPLEMENTARY_SERVICE, EmptyStub,                 Perform_supplementary_service )
pick( GET_SYS_CONFIG,                EmptyStub,                 Get_sys_config )
pick( MAP_ADDR,                      Map_addr,                  Map_addr )
//pick( ADDR_INFO,                     EmptyStub,                 Addr_info )   // obsolete
pick( CHECKSUM_MEM,                  Checksum_mem,              Checksum_mem )
pick( READ_MEM,                      EmptyStub,                 Read_mem )
pick( WRITE_MEM,                     EmptyStub,                 Write_mem )
pick( READ_IO,                       EmptyStub,                 Read_io )
pick( WRITE_IO,                      EmptyStub,                 Write_io )
//pick( READ_CPU,                      EmptyStub,                 Read_cpu )    // obsolete
//pick( READ_FPU,                      EmptyStub,                 Read_fpu )    // obsolete
//pick( WRITE_CPU,                     EmptyStub,                 Write_cpu )   // obsolete
//pick( WRITE_FPU,                     EmptyStub,                 Write_fpu )   // obsolete
pick( PROG_GO,                       EmptyStub,                 Prog_go )
pick( PROG_STEP,                     EmptyStub,                 Prog_step )
pick( PROG_LOAD,                     Prog_load,                 Prog_load )
pick( PROG_KILL,                     Prog_kill,                 Prog_kill )
pick( SET_WATCH,                     Set_watch,                 Set_watch )
pick( CLEAR_WATCH,                   EmptyStub,                 Clear_watch )
pick( SET_BREAK,                     Set_break,                 Set_break )
pick( CLEAR_BREAK,                   EmptyStub,                 Clear_break )
pick( GET_NEXT_ALIAS,                Get_next_alias,            Get_next_alias )
pick( SET_USER_SCREEN,               EmptyStub,                 Set_user_screen )
pick( SET_DEBUG_SCREEN,              EmptyStub,                 Set_debug_screen )
pick( READ_USER_KEYBOARD,            Read_user_keyboard,        Read_user_keyboard )
pick( GET_LIB_NAME,                  Get_lib_name,              Get_lib_name )
pick( GET_ERR_TEXT,                  EmptyStub,                 Get_err_text )
pick( GET_MESSAGE_TEXT,              EmptyStub,                 Get_message_text )
pick( REDIRECT_STDIN,                Redirect_stdin,            Redirect_stdin )
pick( REDIRECT_STDOUT,               Redirect_stdout,           Redirect_stdout )
pick( SPLIT_CMD,                     Split_cmd,                 Split_cmd )
pick( READ_REGS,                     EmptyStub,                 Read_regs )
pick( WRITE_REGS,                    EmptyStub,                 Write_regs )
pick( MACHINE_DATA,                  EmptyStub,                 Machine_data )
