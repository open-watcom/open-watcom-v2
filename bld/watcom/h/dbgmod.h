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
* Description:  Header to support symbolic debugging of user loaded modules.
*
****************************************************************************/


#if defined(__WATCOMC__) && defined(__386__) && !defined(NDEBUG)

#define WATCOM_DEBUG_SYMBOLS

/* Variable used to determine if the debugger is present */

extern char volatile __WD_Present;

/* Macro to enter the debugger and pass a message */

void EnterDebuggerWithMessage( const char __far * );
#pragma aux EnterDebuggerWithMessage parm caller [] = \
        "int 3" \
        "jmp short L1" \
        'W' 'V' 'I' 'D' 'E' 'O' \
        "L1:"

/* Inline assembler to get DS selector */

unsigned short GetCS(void);
#pragma aux GetCS parm caller [] = \
        "mov ax,cs"

/* Messages to load debug symbols */

#define DEBUGGER_LOADMODULE_COMMAND "!LOADMODULE "
#define DEBUGGER_LOADMODULE_FORMAT DEBUGGER_LOADMODULE_COMMAND "0x%4.4x:0x%8.8x,%s"

/* Messages to unload debug symbols */

#define DEBUGGER_UNLOADMODULE_COMMAND "!UNLOADMODULE "
#define DEBUGGER_UNLOADMODULE_FORMAT DEBUGGER_UNLOADMODULE_COMMAND "%s"

/****************************************************************************
DESCRIPTION:
Notify the Open Watcom debugger of module load events. WD will attempt
to load symbolic debugging information for the module much like it would for
OS loaded DLLs.
****************************************************************************/
static void NotifyWDLoad(
    char *modname,
    unsigned long offset)
{
    char buf[_MAX_PATH + sizeof(DEBUGGER_LOADMODULE_COMMAND) + 2+4+1+8+1+1];
    sprintf(buf, DEBUGGER_LOADMODULE_FORMAT, GetCS(), offset, modname );
    if (__WD_Present)
        EnterDebuggerWithMessage(buf);
}

/****************************************************************************
DESCRIPTION:
Notify the Open Watcom debugger of module unload events.
****************************************************************************/
static void NotifyWDUnload(
    char *modname)
{
    char buf[_MAX_PATH + sizeof(DEBUGGER_UNLOADMODULE_COMMAND) + 1];
    sprintf(buf, DEBUGGER_UNLOADMODULE_FORMAT, modname);
    if (__WD_Present)
        EnterDebuggerWithMessage(buf);
}
#endif
