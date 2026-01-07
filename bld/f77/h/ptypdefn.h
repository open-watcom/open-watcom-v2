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
* Description:  Run and compile-time constants indicating parameter type
*               and attributes
*
****************************************************************************/

/**************** IMPORTANT NOTE *******************************

  data in files:  ptypes.inc, ptypes.h and ptypdefn.h

  must corespond each to other

****************************************************************/

//
// Run and compile-time constants indicating parameter type and attributes
// NOTE: These constants should stay in the following order.
//       ( routines rely on them being sequential )

//    id               type          dbgtype       cgtype            inpfun      outfun      desc
pick( FPT_NOTYPE,      FT_NO_TYPE,   DBG_NIL_TYPE, TY_USER_DEFINED,  NULL,       NULL,       ""  ) // type of statement #'s and subroutine return values
pick( FPT_LOG_1,       FT_LOGICAL_1, DBG_NIL_TYPE, TY_UINT_1,        &FCInpLOG1, &FCOutLOG1, "LOGICAL*1" ) // LOGICAL*1
pick( FPT_LOG_4,       FT_LOGICAL,   DBG_NIL_TYPE, TY_UINT_4,        &FCInpLOG4, &FCOutLOG4, "LOGICAL*4" ) // LOGICAL*4
pick( FPT_INT_1,       FT_INTEGER_1, DBG_NIL_TYPE, TY_INT_1,         &FCInpINT1, &FCOutINT1, "INTEGER*1" ) // INTEGER*1
pick( FPT_INT_2,       FT_INTEGER_2, DBG_NIL_TYPE, TY_INT_2,         &FCInpINT2, &FCOutINT2, "INTEGER*2" ) // INTEGER*2
pick( FPT_INT_4,       FT_INTEGER,   DBG_NIL_TYPE, TY_INT_4,         &FCInpINT4, &FCOutINT4, "INTEGER*4" ) // INTEGER*4
pick( FPT_REAL_4,      FT_REAL,      DBG_NIL_TYPE, TY_SINGLE,        &FCInpREAL, &FCOutREAL, "REAL*4"    ) // REAL*4
pick( FPT_REAL_8,      FT_DOUBLE,    DBG_NIL_TYPE, TY_DOUBLE,        &FCInpDBLE, &FCOutDBLE, "REAL*8"    ) // REAL*8
pick( FPT_REAL_16,     FT_EXTENDED,  DBG_NIL_TYPE, TY_LONGDOUBLE,    &FCInpXTND, &FCOutXTND, "REAL*16"   ) // REAL*16
pick( FPT_CPLX_8,      FT_COMPLEX,   DBG_NIL_TYPE, TY_COMPLEX,       &FCInpCPLX, &FCOutCPLX, "COMPLEX*8" ) // COMPLEX*8
pick( FPT_CPLX_16,     FT_DCOMPLEX,  DBG_NIL_TYPE, TY_DCOMPLEX,      &FCInpDBCX, &FCOutDBCX, "COMPLEX*16") // COMPLEX*16
pick( FPT_CPLX_32,     FT_XCOMPLEX,  DBG_NIL_TYPE, TY_XCOMPLEX,      &FCInpXTCX, &FCOutXTCX, "COMPLEX*32") // COMPLEX*32
pick( FPT_CHAR,        FT_CHAR,      DBG_NIL_TYPE, TY_LOCAL_POINTER, &InpString, &OutString, "CHARACTER" ) // CHARACTER
#ifndef ONLY_BASE_TYPES
pick( FPT_STRUCT,      FT_STRUCTURE, DBG_NIL_TYPE, TY_USER_DEFINED,  NULL,       NULL,       "STRUCTURE" ) // STRUCTURE
pick( FPT_ARRAY,       FT_NO_TYPE,   DBG_NIL_TYPE, TY_USER_DEFINED,  NULL,       NULL,       "ARRAY"     ) // ARRAY
pick( FPT_NOT_STARTED, FT_NO_TYPE,   DBG_NIL_TYPE, TY_USER_DEFINED,  NULL,       NULL,       ""          ) // IOCB->ptyp is initialized to this for i/o statements
#endif
