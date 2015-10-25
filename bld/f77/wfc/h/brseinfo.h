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


#ifndef _F77_BROWSE_INFO_GENERATOR_
#define _F77_BROWSE_INFO_GENERATOR_

extern void     BIInit( void );
extern void     BIEnd( void );
extern void     BIStartSubProg( void );
extern void     BIEndSubProg( void );
extern void     BIResolveUndefTypes( void );
extern void     BIOutSymbol( sym_id ste_ptr );
extern void     BIOutComSymbol( sym_id ste_ptr );
extern void     BIOutSrcLine( void );
extern void     BISetSrcFile( void );
extern void     BIStartSubroutine( void );
extern void     BIFiniStartOfSubroutine( void );
extern void     BIEndSF( sym_id ste_ptr );
extern void     BIStartRBorEP( sym_id ste_ptr );
extern void     BIEndRBorEP( void );
extern void     BIStartComBlock( sym_id ste_ptr );
extern void     BIEndComBlock( void );
extern void     BIStartBlockData( sym_id ste_ptr );
extern void     BIEndBlockData( void );
extern void     BIOutNameList( sym_id ste_ptr );

#endif
