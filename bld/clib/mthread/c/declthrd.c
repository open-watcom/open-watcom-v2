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
* Description:  TLS segment definitions.
*
****************************************************************************/


#include "variety.h"
#include "langenvd.h"
#if defined(__PPC__)
  #define   __TGT_SYS   __TGT_SYS_PPC_NT
#elif defined(__AXP__)
  #define   __TGT_SYS   __TGT_SYS_AXP_NT
#else
  #define   __TGT_SYS   __TGT_SYS_X86
#endif
#include "langenv.h"

unsigned long __based( __segname( TS_SEG_TLSB ) ) _tls_start = (unsigned long)&_tls_start;
unsigned long __based( __segname( TS_SEG_TLSE ) ) _tls_end = (unsigned long)&_tls_end;
unsigned long _tls_index;

unsigned long *_tls_used[] = {
    &_tls_start,
    &_tls_end,
    &_tls_index
};

#if defined(_M_IX86)
 extern void **get_tls_array();
 extern void *_tls_array;
 #pragma aux get_tls_array = "mov eax, dword ptr fs:_tls_array";

 void *_tls_region( void ) {
    void **array;
    array = get_tls_array();
    return( (void *)(((unsigned long)array[_tls_index]) - ((unsigned long)_tls_used[0])) );
 }
#endif

#if defined(_M_IX86)
 #pragma aux _tls_used "_*";
 #pragma aux _tls_start "_*";
 #pragma aux _tls_end "_*";
 #pragma aux _tls_index "_*";
 #pragma aux _tls_region "_*";
 #pragma aux _tls_array "_*";
#endif

