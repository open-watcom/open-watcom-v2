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

#ifndef WF77_PTYPES_INCLUDED
#define WF77_PTYPES_INCLUDED

#define PT_NOTYPE       0            // no type
#define PT_LOG_1        1            // logical*1
#define PT_LOG_4        2            // logical*4
#define PT_INT_1        3            // integer*1
#define PT_INT_2        4            // integer*2
#define PT_INT_4        5            // integer*4
#define PT_REAL_4       6            // real*4
#define PT_REAL_8       7            // real*8
#define PT_REAL_16      8            // real*16
#define PT_CPLX_8       9            // complex*8
#define PT_CPLX_16      10           // complex*16
#define PT_CPLX_32      11           // complex*32
#define PT_CHAR         12           // character
#define PT_STRUCT       13           // structure
#define PT_ARRAY        14           // array

#endif
