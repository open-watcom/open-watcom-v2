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


/*
*   COMMENTS: Function prototypes contolling the abstract type "a_variable"
*/

#include "vhandle.h"

extern vhandle      NextGlobalVar( vhandle var_handle );
extern void         InitGlobalVarList( void );
extern int          GetVariableByName( const char *vbl_name );
extern vhandle      GetVariableById( gui_ctl_id id );
extern gui_ctl_id   VarGetId( vhandle var_handle );
extern const char   *VarGetStrVal( vhandle var_handle );
extern int          VarGetIntVal( vhandle var_handle );
extern int          GetVariableIntVal( const char *vbl_name );
extern const char   *GetVariableStrVal( const char *vbl_name );
extern vhandle      SetVariableByHandle( vhandle var_handle, const char *strval );
extern vhandle      SetVariableByName( const char *vbl_name, const char *strval );
extern void         SetDefaultGlobalVarList( void );
extern void         FreeGlobalVarList( bool );
extern const char   *VarGetName( vhandle var_handle );
extern void         VarSetHook( vhandle var_handle, void (*hook)(vhandle) );
extern bool         VarHasValue( vhandle var_handle );
extern vhandle      AddVariable( const char *name );
extern void         VarSetAutoSetCond( vhandle var_handle, const char *cond );
extern const char   *VarGetAutoSetCond( vhandle var_handle );
extern void         VarSetAutoSetRestriction( vhandle var_handle, const char *val );
extern int          VarIsRestrictedFalse( vhandle var_handle );
extern int          VarIsRestrictedTrue( vhandle var_handle );
