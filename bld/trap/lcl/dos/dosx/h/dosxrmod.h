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
* Description:  Real mode related routines used by protected mode servers.
*
****************************************************************************/



#if defined(DOS4G)
  #define GetDosByte(x)   (*(byte __far *)RMLinToPM(x,1))
  #define GetDosLong(x)   (*(unsigned long __far *)RMLinToPM(x,1))
  #define PutDosByte(x,d) (*(byte __far *)RMLinToPM(x,1)=d)
  #define PutDosLong(x,d) (*(unsigned long __far *)RMLinToPM(x,1)=d)
#else
  #define GetDosByte(x)   (*(byte __far *)MK_FP(Meg1,x))
  #define GetDosLong(x)   (*(unsigned long __far *)MK_FP(Meg1,x))
  #define PutDosByte(x,d) (*(byte __far *)MK_FP(Meg1,x)=d)
  #define PutDosLong(x,d) (*(unsigned long __far *)MK_FP(Meg1,x)=d)
#endif

#if defined(DOS4G)
  extern void             __far *RMLinToPM( unsigned long linear_addr, int pool );
#else
  #define RMLinToPM(x,y)  MK_FP(Meg1,x)
#endif
extern void               CallRealMode( unsigned long dos_addr );
