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
* Description:  Fatal error messages, they are shared with WOMP
*
****************************************************************************/

pick( MSG_OUT_OF_MEMORY, 0, OUT_OF_MEMORY, AsmShutDown, 0 ),
pick( MSG_CANNOT_OPEN_FILE, 1, CANNOT_OPEN_FILE, NULL, 1 ),
pick( MSG_CANNOT_CLOSE_FILE, 1, CANNOT_CLOSE_FILE, NULL, 1 ),
pick( MSG_CANNOT_GET_START_OF_SOURCE_FILE, 0, CANNOT_GET_FILE, AsmShutDown, 1 ),
pick( MSG_CANNOT_SET_TO_START_OF_SOURCE_FILE, 0, CANNOT_SET_FILE, AsmShutDown, 1 ),
pick( MSG_TOO_MANY_FILES, 0, TOO_MANY_FILES, NULL, 1 ),
