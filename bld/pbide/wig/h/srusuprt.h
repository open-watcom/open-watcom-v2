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


#ifndef SRUPARSE_SUPPORT_INCLUDED
#define SRUPARSE_SUPPORT_INCLUDED

#include "types.h"
#include "list.h"
#include "sruinter.h"

void yyerror(char *);

extern void ProcessStatement(void);
extern void EndSection(void);
extern void SetSubroutine(char *);
extern void SetHeader(char *, char *);
extern void UserCode(void);
extern void SetReturn(void);
extern void SetComment(void);
extern void SetFunction( TypeInfo *ret, char *fname );
extern void StartSubProgram(void);
extern void FiniParmList(void);
extern void EndSubProgram(void);
extern void AddParm( TypeInfo *typ, char *tname, ArrayInfo *array );
extern void StartSection(id_type, id_type);
extern void AddDataMethod( id_type access_type, TypeInfo *typ, List *varlist );
extern void RegisterEvent(char *);
extern void InitSru(void);
extern void FiniSru(void);
extern void GenCPPInterface(void);
extern void DoPostProcessing( void );
extern void AddDataMethod2( TypeInfo *typ, List *varlist );
extern void SetDefaultAccess( id_type type );
extern void FreeSru( void );
extern void GetDestructor( char *uoname, char *buf );
extern void GetConstructor( char *uoname, char *buf );

#endif
