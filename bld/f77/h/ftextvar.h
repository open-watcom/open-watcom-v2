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
* Description:  Declaration of external vars for f77 compiler & lib
*
****************************************************************************/

#ifndef _F77_EXTERNAL_VARS_H
#define _F77_EXTERNAL_VARS_H 1

#include "csetinfo.h"
#include "fio.h"
#include "fmttab.h"

extern  character_set    CharSetInfo;
extern  b_file           *FStdErr;
extern  b_file           *FStdIn;
extern  b_file           *FStdOut;
//conflicting from rstdio.c; file_handle is effectively void
// above versions pass f77/regress tests
//extern  file_handle     FStdIn;
//extern  file_handle     FStdOut;
extern  char            *_LpPgmName;
extern  char            DefFName[];
extern  char            NormalCtrlSeq[];
extern  char            SDTermOut[];
extern  char            SDTermIn[];
extern  char            *SpecId[];
extern  const char      __FAR ErrWord[];
extern  const unsigned char __FAR GrpCodes[];
extern  const unsigned char __FAR * const __FAR GroupTable[];
extern  const byte      __FAR SizeVars[];

extern  void            (*FmtRoutine)( void );
extern  void            (*TraceRoutine)( char * );
extern  void            (*_ExceptionInit)( void );
extern  void            (*_ExceptionFini)( void );
extern  void            (*_AccessFIO)( void );
extern  void            (*_ReleaseFIO)( void );
extern  void            (*_PartialReleaseFIO)( void );
// eliminate const as freeout.c says so
extern  void            (* /*const*/ __FAR OutRtn[])( void );

extern const            FmtElements RFmtStruct;



#endif
