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
* Description:  Special macros that can't be undefined or redefined.
*
****************************************************************************/


pick( "__LINE__",       MACRO_LINE,        MFLAG_NONE )
pick( "__FILE__",       MACRO_FILE,        MFLAG_NONE )
pick( "__DATE__",       MACRO_DATE,        MFLAG_NONE )
pick( "__TIME__",       MACRO_TIME,        MFLAG_NONE )
pick( "__cplusplus",    MACRO_CPLUSPLUS,   MFLAG_NONE )
pick( "__FUNCTION__",   MACRO_FUNCTION,    MFLAG_CAN_BE_REDEFINED )

// __func__ shouldn't be a macro, define it special for now
pick( "__func__",       MACRO_FUNC,        MFLAG_CAN_BE_REDEFINED | MFLAG_SPECIAL )

// alternative tokens for C++
pick( NULL,             MACRO_ALT_MARKER,  MFLAG_NONE )
pick( "and",            MACRO_ALT_AND,     MFLAG_SPECIAL )
pick( "bitand",         MACRO_ALT_BITAND,  MFLAG_SPECIAL )
pick( "and_eq",         MACRO_ALT_AND_EQ,  MFLAG_SPECIAL )
pick( "or",             MACRO_ALT_OR,      MFLAG_SPECIAL )
pick( "bitor",          MACRO_ALT_BITOR,   MFLAG_SPECIAL )
pick( "or_eq",          MACRO_ALT_OR_EQ,   MFLAG_SPECIAL )
pick( "xor",            MACRO_ALT_XOR,     MFLAG_SPECIAL )
pick( "xor_eq",         MACRO_ALT_XOR_EQ,  MFLAG_SPECIAL )
pick( "not",            MACRO_ALT_NOT,     MFLAG_SPECIAL )
pick( "not_eq",         MACRO_ALT_NOT_EQ,  MFLAG_SPECIAL )
pick( "compl",          MACRO_ALT_COMPL,   MFLAG_SPECIAL )
