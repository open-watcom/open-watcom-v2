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


// CONTEXT.H -- compiler context processing
//
// Compiler context is used to generate headers for error messages, as well as
// for debugging.
//
// 92/07/08 -- J.W.Welch        -- defined

CT( CTX_INIT        , "Compiler Initialization"         ),
CT( CTX_FINI        , "Compiler Completion"             ),
CT( CTX_CMDLN_ENV   , "Environment command line switch" ),
CT( CTX_CMDLN_PGM   , "Program command line switch"     ),
CT( CTX_CMDLN_VALID , "Command line validation"         ),
CT( CTX_FORCED_INCS , "Compiling forced includes"       ),
CT( CTX_SOURCE      , "Compiling source"                ),
CT( CTX_FUNC_GEN    , "Compiling generated function"    ),
CT( CTX_CG_FUNC     , "Code generation for"             ),
CT( CTX_CG_OPT      , "Code optimization"               ),
CT( CTX_ENDFILE     , "End of file"                     ),
CT( CTX_START       , "Start-up"                        )
