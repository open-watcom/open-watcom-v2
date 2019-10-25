/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


pick( "__DATE__",           MACRO_DATE,         MFLAG_NONE )
pick( "__FILE__",           MACRO_FILE,         MFLAG_NONE )
pick( "__LINE__",           MACRO_LINE,         MFLAG_NONE )
pick( "__STDC__",           MACRO_STDC,         MFLAG_NONE )
pick( "__STDC_HOSTED__",    MACRO_STDC_HOSTED,  MFLAG_NONE )
pick( "__STDC_VERSION__",   MACRO_STDC_VERSION, MFLAG_NONE )
pick( "__TIME__",           MACRO_TIME,         MFLAG_NONE )
/* compile time macros */
pick( "__FUNCTION__",       MACRO_FUNCTION,     MFLAG_NONE )
pick( "__func__",           MACRO_FUNC,         MFLAG_HIDDEN )
