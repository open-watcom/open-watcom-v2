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
* Description:  dtor kind carvers definition
*
****************************************************************************/


/*    carver       se cnt cmd cnt  cmd ring */
pick( SET_SV,      8,     8,       SetSv     )
pick( TRY,         4,     4,       Try       )
pick( FN_EXC,      4,     4,       FnExc     )
pick( TEST_FLAG,   16,    16,      TestFlag  )
pick( ARRAY_INIT,  4,     8,       ArrayInit )
pick( DLT_1,       4,     4,       Dlt1      )
pick( DLT_2,       4,     4,       Dlt2      )
pick( DLT_1_ARRAY, 4,     4,       Dlt1Array )
pick( DLT_2_ARRAY, 4,     4,       Dlt2Array )
pick( CTOR_TEST,   4,     1,       CtorTest  )
pick( COMPONENT,   16,    32,      Component )
#if !defined( ONLY_DTC_CMDS ) 
pick( CATCH,       8,     0,                 )
pick( SYM_STATIC,  32,    0,                 )
pick( SYM_AUTO,    32,    0,                 )
pick( SUBOBJ,      16,    0,                 )
#endif 
