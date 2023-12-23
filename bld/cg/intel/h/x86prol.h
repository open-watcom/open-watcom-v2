/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


pick( PST_PROLOG_FAT,       0x0001 )
pick( PST_PROLOG_THUNK,     0x0002 )
pick( PST_PROLOG_HOOKS,     0x0004 )
pick( PST_PROLOG_RDOSDEV,   0x0008 )
pick( PST_PROLOG_GENERATED, 0x0010 )
pick( PST_EPILOG_HOOKS,     0x0020 )
pick( PST_EPILOG_GENERATED, 0x0040 )
pick( PST_EXPORT,           0x0080 )
pick( PST_FUNCTION_NAME,    0x0100 )
pick( PST_GROW_STACK,       0x0200 )
pick( PST_RESET_SP,         0x0400 )
pick( PST_TOUCH_STACK,      0x0800 )
