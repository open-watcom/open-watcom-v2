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


typedef struct {
    char        *typename;
} TypeInfo;

typedef struct {
    BOOL        m_near;
    BOOL        m_const;
    BOOL        m_far;
    BOOL        m_huge;
    BOOL        m_cdecl;
    BOOL        m_pascal;
    BOOL        m_fortran;
    BOOL        m_syscall;
    BOOL        m_export;
} ModifierInfo;

typedef struct paraminfo {
    struct paraminfo    *next;
    struct paraminfo    *parameters;
    BOOL                err;
    ModifierInfo        *modifiers;
    TypeInfo            *type;
    char                *name;
    unsigned            ptrcnt;
    unsigned            arraycnt;
} ParamInfo;

typedef struct {
    char                *name;
    ModifierInfo        *modifiers;
    TypeInfo            *type;
    unsigned            ptrcnt;
} FunctionInfo;

void SemLine( char *fname, char *lineno );
ParamInfo *SemCreateParam( TypeInfo *tinfo, char *name, int ptrcnt,
                           int arraycnt );
ModifierInfo *SemAddSpecifier( ModifierInfo *table, int tok );
void SemFunction( ParamInfo *finfo );
void DoOutput( void );
