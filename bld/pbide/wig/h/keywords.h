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


#ifndef KEYWORD_DATA_STRUCTURE_DEFINE
#define KEYWORD_DATA_STRUCTURE_DEFINE

#include "types.h"

typedef struct keyword {
        struct keyword  *next;
        char            *key;
        id_type         id;
        id_type         thunk;
} keyword;


extern keyword  Statements[];
extern keyword  DataTypes[];

extern int      IsPtrType( id_type );
extern char     *FindType( id_type );
extern char     *FindStmt( id_type );
extern unsigned GetStmntCnt( void );
extern char     *ConvertRetType( id_type id );
extern void     ConvertVarType( char *typebuf, char *arraybuf, id_type id, void *array );
extern void     ConvertParmType( char *buf, char *name, id_type id, void *array );

#endif
