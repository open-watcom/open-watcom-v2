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
* Description:  front end calling info structure and initialization
*
****************************************************************************/

#ifndef __CALLINFO_H__
#define __CALLINFO_H__

#if !defined( BY_C_FRONT_END ) && !defined( BY_CPP_FRONT_END ) && !defined( BY_FORTRAN_FRONT_END )

#error "!!!! Call info - Unknown front end !!!!"

#else

typedef struct aux_info {
        call_class      cclass;
        byte_seq        *code;          // also used by C pre-compiled header
        hw_reg_set      *parms;         // also used by C pre-compiled header
        hw_reg_set      returns;
        hw_reg_set      streturn;
        hw_reg_set      save;
        char            *objname;       // also used by C pre-compiled header
        unsigned        use;            // use count
        aux_flags       flags;
        unsigned        index;          // for C/C++ pre-compiled header
  #ifdef BY_C_FRONT_END
    #if _CPU == 370
        linkage_regs    *linkage;
    #endif
    #if _CPU == _AXP
        char            *except_rtn;
    #endif
  #endif
} aux_info;

#define DefaultInfo      BuiltinAuxInfo[0]
#define WatcallInfo      BuiltinAuxInfo[1]
#define CdeclInfo        BuiltinAuxInfo[2]
#define PascalInfo       BuiltinAuxInfo[3]
#define FortranInfo      BuiltinAuxInfo[4]
#define SyscallInfo      BuiltinAuxInfo[5]
#define OptlinkInfo      BuiltinAuxInfo[6]
#define StdcallInfo      BuiltinAuxInfo[7]
#define FastcallInfo     BuiltinAuxInfo[8]
#if _CPU == 386
#define Far16CdeclInfo   BuiltinAuxInfo[9]
#define Far16PascalInfo  BuiltinAuxInfo[10]
#endif

#if _CPU == 386
#define MAX_BUILTIN_AUXINFO (9 + 2)
#else
#define MAX_BUILTIN_AUXINFO 9
#endif

global aux_info          BuiltinAuxInfo[MAX_BUILTIN_AUXINFO];

extern  void    PragmaAuxInfoInit( int );
extern  void    SetAuxStackConventions( void );
extern  void    SetAuxWatcallInfo( void );
extern  void    SetAuxDefaultInfo( void );
extern  int     IsAuxParmsBuiltIn( hw_reg_set *parms );
extern  int     IsAuxInfoBuiltIn( aux_info *info );
extern  char    *VarNamePattern( aux_info *inf );

#endif

#endif


