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
* Description:  compile-time logical operators and constants definitions.
*
****************************************************************************/


/*    text      en          opr std */
pick( "EQ",     OPR_EQ,     1,  1 )
pick( "NE",     OPR_NE,     1,  1 )
pick( "LT",     OPR_LT,     1,  1 )
pick( "GT",     OPR_GT,     1,  1 )
pick( "LE",     OPR_LE,     1,  1 )
pick( "GE",     OPR_GE,     1,  1 )
pick( "OR",     OPR_OR,     1,  1 )
pick( "AND",    OPR_AND,    1,  1 )
pick( "NOT",    OPR_NOT,    1,  1 )
pick( "EQV",    OPR_EQV,    1,  1 )
pick( "NEQV",   OPR_NEQV,   1,  1 )
pick( "XOR",    OPR_NEQV,   1,  0 )
pick( "TRUE",   OPR_PHI,    0,  0 )
pick( "FALSE",  OPR_PHI,    0,  0 )
