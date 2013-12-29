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
* Description:  dtor commands enum definition and reference to carvers
*
****************************************************************************/


/*    kind          carver                                           */
                                  /* commands:                       */
pick( SET_SV,       SET_SV      ) /* - set state variable            */
pick( TRY,          TRY         ) /* - TRY start                     */
pick( FN_EXC,       FN_EXC      ) /* - function exception spec       */
pick( TEST_FLAG,    TEST_FLAG   ) /* - select state from flag        */
pick( ARRAY_INIT,   ARRAY_INIT  ) /* - array initialization          */
pick( DLT_1,        DLT_1       ) /* - delete: op del(void*)         */
pick( DLT_2,        DLT_2       ) /* - delete: op del(void*,size_t)  */
pick( DLT_1_ARRAY,  DLT_1_ARRAY ) /* - dlt-array:op del(void*)       */
pick( DLT_2_ARRAY,  DLT_2_ARRAY ) /* - dlt-array:op del(void*,size_t)*/
pick( CTOR_TEST,    CTOR_TEST   ) /* - test if ctor'ed               */
pick( COMP_VBASE,   COMPONENT   ) /* - component : virtual base      */
pick( COMP_DBASE,   COMPONENT   ) /* - component : direct base       */
pick( COMP_MEMB,    COMPONENT   ) /* - component : member            */
pick( ACTUAL_VBASE, COMPONENT   ) /* - vbase always dtor'ed          */
pick( ACTUAL_DBASE, COMPONENT   ) /* - dbase always dtor'ed          */
#ifndef ONLY_DTC_CMDS
pick( ARRAY,                    ) /* - array object                  */
pick( CATCH,        CATCH       ) /* - CATCH block end               */
                                  /* following are used internally   */
pick( SYM_STATIC,   SYM_STATIC  ) /* - dtor static symbol            */
pick( SYM_AUTO,     SYM_AUTO    ) /* - dtor auto symbol              */
pick( SUBOBJ,       SUBOBJ      ) /* - sub-object                    */
#endif
