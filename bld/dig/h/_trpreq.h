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


/*    num                            dumb                       std                                    */
pick( CONNECT,                       Connect,                   Connect )
pick( DISCONNECT,                    SimpleStub,                Disconnect )
pick( SUSPEND,                       SimpleStub,                Suspend )
pick( RESUME,                        SimpleStub,                Resume )
pick( GET_SUPPLEMENTARY_SERVICE,     Get_supplementary_service, Get_supplementary_service )
pick( PERFORM_SUPPLEMENTARY_SERVICE, SimpleStub,                Perform_supplementary_service )
pick( GET_SYS_CONFIG,                SimpleStub,                Get_sys_config )
pick( MAP_ADDR,                      Map_addr,                  Map_addr )
//pick( ADDR_INFO,                     SimpleStub,                Addr_info )   // obsolete
pick( CHECKSUM_MEM,                  Checksum_mem,              Checksum_mem )
pick( READ_MEM,                      SimpleStub,                Read_mem )
pick( WRITE_MEM,                     SimpleStub,                Write_mem )
pick( READ_IO,                       SimpleStub,                Read_io )
pick( WRITE_IO,                      SimpleStub,                Write_io )
//pick( READ_CPU,                      SimpleStub,                Read_cpu )    // obsolete
//pick( READ_FPU,                      SimpleStub,                Read_fpu )    // obsolete
//pick( WRITE_CPU,                     SimpleStub,                Write_cpu )   // obsolete
//pick( WRITE_FPU,                     SimpleStub,                Write_fpu )   // obsolete
pick( PROG_GO,                       SimpleStub,                Prog_go )
pick( PROG_STEP,                     SimpleStub,                Prog_step )
pick( PROG_LOAD,                     Prog_load,                 Prog_load )
pick( PROG_KILL,                     Prog_kill,                 Prog_kill )
pick( SET_WATCH,                     Set_watch,                 Set_watch )
pick( CLEAR_WATCH,                   SimpleStub,                Clear_watch )
pick( SET_BREAK,                     Set_break,                 Set_break )
pick( CLEAR_BREAK,                   SimpleStub,                Clear_break )
pick( GET_NEXT_ALIAS,                Get_next_alias,            Get_next_alias )
pick( SET_USER_SCREEN,               SimpleStub,                Set_user_screen )
pick( SET_DEBUG_SCREEN,              SimpleStub,                Set_debug_screen )
pick( READ_USER_KEYBOARD,            Read_user_keyboard,        Read_user_keyboard )
pick( GET_LIB_NAME,                  Get_lib_name,              Get_lib_name )
pick( GET_ERR_TEXT,                  SimpleStub,                Get_err_text )
pick( GET_MESSAGE_TEXT,              SimpleStub,                Get_message_text )
pick( REDIRECT_STDIN,                Redirect_stdin,            Redirect_stdin )
pick( REDIRECT_STDOUT,               Redirect_stdout,           Redirect_stdout )
pick( SPLIT_CMD,                     Split_cmd,                 Split_cmd )
pick( READ_REGS,                     SimpleStub,                Read_regs )
pick( WRITE_REGS,                    SimpleStub,                Write_regs )
pick( MACHINE_DATA,                  SimpleStub,                Machine_data )
