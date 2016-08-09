/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Interfaces provided by macros.c
*
****************************************************************************/


#ifndef _MACROS_H
#define _MACROS_H   1

#include "mtarget.h"
#include "mlex.h"


extern bool         ImplicitDeMacro;
extern bool         IsPartDeMacro;
extern bool         DoingBuiltIn;

extern void         MacroInit( void );
extern void         MacroFini( void );
extern char         *GetMacroValue( const char *name );
extern char         *WrnGetMacroValue( const char *name );
extern void         DefMacro( const char *name );
extern char         *DeMacro( TOKEN_T end );
extern bool         ForceDeMacro( void );
extern char         *PartDeMacro( bool forceDeMacro );
extern void         UnDefMacro( const char *name );
extern bool         IsMacroName( const char *name );
extern void         PrintMacros( void );
extern char         *ignoreWSDeMacro( bool partDeMacro, bool forceDeMacro );
extern const char   *procPath( const char *fullpath );
extern char         *DeMacroSpecial( const char *InString );

#endif  /* !_MACROS_H */
