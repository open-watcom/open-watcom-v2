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


#ifndef _MYSTDARG_H_INCLUDED
#define _MYSTDARG_H_INCLUDED
#undef va_list
#define va_list my_va_list
#undef va_start
#define va_start my_va_start
#undef va_arg
#define va_arg my_va_arg
#undef va_end
#define va_end my_va_end
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#ifndef _MY_VA_LIST_DEFINED
#define _MY_VA_LIST_DEFINED
typedef char *__my_va_list;
#endif
typedef __my_va_list my_va_list;

  #define my_va_start(ap,pn) ((ap)=(char *)&pn+\
      ((sizeof(pn)+sizeof(int)-1)&~(sizeof(int)-1)),(void)0)
  #define my_va_arg(ap,type)     ((ap)+=\
      ((sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)),\
      (*(type *)((ap)-((sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)))))
  #define my_va_end(ap)     ((ap)=0,(void)0)

#ifdef __cplusplus
};
#endif
#endif
