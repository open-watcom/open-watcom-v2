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
* Description:  Fatal error messages
*
****************************************************************************/

pick( MSG_OUT_OF_MEMORY, 0, OUT_OF_MEMORY, AsmShutDown, EXIT_SUCCESS )
pick( MSG_CANNOT_OPEN_FILE, 1, CANNOT_OPEN_FILE, NULL, EXIT_ERROR )
pick( MSG_CANNOT_CLOSE_FILE, 1, CANNOT_CLOSE_FILE, NULL, EXIT_ERROR )
pick( MSG_CANNOT_GET_START_OF_SOURCE_FILE, 0, CANNOT_GET_FILE, AsmShutDown, EXIT_ERROR )
pick( MSG_CANNOT_SET_TO_START_OF_SOURCE_FILE, 0, CANNOT_SET_FILE, AsmShutDown, EXIT_ERROR )
pick( MSG_TOO_MANY_FILES, 0, TOO_MANY_FILES, NULL, EXIT_ERROR )
pick( MSG_OBJECT_FILE_ERROR, 1, OBJECT_FILE_ERROR, AsmShutDown, EXIT_ERROR )
