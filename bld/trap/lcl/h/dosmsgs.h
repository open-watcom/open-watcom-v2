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


pick( ERR_0,                    "" )
pick( ERR_INVALID_FUNCTION,     TRP_ERR_invalid_function_number )
pick( ERR_FILE_NOT_FOUND,       TRP_ERR_file_not_found )
pick( ERR_PATH_NOT_FOUND,       TRP_ERR_path_not_found )
pick( ERR_TOO_MANY_OPEN_FILES,  TRP_ERR_too_many_open_files )
pick( ERR_ACCESS_DENIED,        TRP_ERR_access_denied )
pick( ERR_INVALID_HANDLE,       TRP_ERR_invalid_handle )
pick( ERR_MEMORY_CONTROL_BLOCKS,TRP_ERR_memory_control_blocks_destroyed )
pick( ERR_INSUFFICIENT_MEMORY,  TRP_ERR_insufficient_memory )
pick( ERR_INVALID_MEMORY_BLOCK, TRP_ERR_invalid_memory_block_address )
pick( ERR_INVALID_ENVIRONMENT,  TRP_ERR_invalid_environment )
pick( ERR_INVALID_FORMAT,       TRP_ERR_invalid_format )
pick( ERR_INVALID_ACCESS_CODE,  TRP_ERR_invalid_access_code )
pick( ERR_INVALID_DATA,         TRP_ERR_invalid_data )
pick( ERR_RESERVED,             TRP_ERR_reserved_error_code )
pick( ERR_INVALID_DRIVE,        TRP_ERR_invalid_drive_was_specified )
pick( ERR_ATTEMPT_TO_REMOVE_CD, TRP_ERR_attempt_to_remove_current_directory )
pick( ERR_NOT_SAME_DEVICE,      TRP_ERR_not_same_device )
pick( ERR_NO_MORE_FILES,        TRP_ERR_no_more_files )
#ifdef REAL_MODE
pick( ERR_RATIONAL_EXE,         TRP_ERR_must_use_rsi )
pick( ERR_PHARLAP_EXE,          TRP_ERR_must_use_pls )
#endif
