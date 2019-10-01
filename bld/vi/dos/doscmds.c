/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "getspcmd.h"


const char _NEAR * _NEAR InternalCommands[] = {
    CONST_NEAR_STRING( "BREAK" ),
    CONST_NEAR_STRING( "CALL" ),
    CONST_NEAR_STRING( "CD" ),
    CONST_NEAR_STRING( "CHDIR" ),
    CONST_NEAR_STRING( "CLS" ),
    CONST_NEAR_STRING( "COMMAND" ),
    CONST_NEAR_STRING( "COPY" ),
    CONST_NEAR_STRING( "CTTY" ),
    CONST_NEAR_STRING( "DATE" ),
    CONST_NEAR_STRING( "DEL" ),
    CONST_NEAR_STRING( "DIR" ),
    CONST_NEAR_STRING( "ECHO" ),
    CONST_NEAR_STRING( "ERASE" ),
    CONST_NEAR_STRING( "FOR" ),
    CONST_NEAR_STRING( "IF" ),
    CONST_NEAR_STRING( "MD" ),
    CONST_NEAR_STRING( "MKDIR" ),
    CONST_NEAR_STRING( "PATH" ),
    CONST_NEAR_STRING( "PAUSE" ),
    CONST_NEAR_STRING( "PROMPT" ),
    CONST_NEAR_STRING( "RD" ),
    CONST_NEAR_STRING( "REM" ),
    CONST_NEAR_STRING( "REN" ),
    CONST_NEAR_STRING( "RENAME" ),
    CONST_NEAR_STRING( "RMDIR" ),
    CONST_NEAR_STRING( "SET" ),
    CONST_NEAR_STRING( "TIME" ),
    CONST_NEAR_STRING( "TYPE" ),
    CONST_NEAR_STRING( "VER" ),
    CONST_NEAR_STRING( "VERIFY" ),
    CONST_NEAR_STRING( "VOL" )
};

int InternalCommandCount = sizeof( InternalCommands ) / sizeof( InternalCommands[0] );

const char _NEAR * _NEAR ExeExtensions[] = {
    CONST_NEAR_STRING( ".bat" ),
    CONST_NEAR_STRING( ".com" ),
    CONST_NEAR_STRING( ".exe" )
};

int ExeExtensionCount = sizeof( ExeExtensions ) / sizeof( ExeExtensions[0] );
