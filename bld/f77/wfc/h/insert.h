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
* Description:  prototypes for insert information into error messages functions
*
****************************************************************************/


void    StmtExtension( int errcode );
void    StmtErr( int errcode );
void    StmtIntErr( int errcode, int num );
void    StmtPtrErr( int errcode, void *ptr );
void    NameWarn( int errcod, sym_id sym );
void    NameErr( int errcod, sym_id sym );
void    NameStmtErr( int errcod, sym_id sym, STMT stmt );
void    PrmCodeErr( int errcode, int code );
void    NameExt( int errcod, sym_id sym );
void    NamNamErr( int errcod, sym_id var1, sym_id var2 );
void    ClassNameErr( int errcod, sym_id sym );
void    PrevDef( sym_id sym );
void    NameTypeErr( int errcod, sym_id sym );
void    TypeTypeErr( int errcod, TYPE typ1, TYPE typ2 );
void    TypeNameTypeErr( int errcod, TYPE typ1, sym_id sym, TYPE typ2 );
void    TypeErr( int errcod, TYPE typ );
void    KnownClassErr( int errcod, uint idx );
void    ClassErr( int errcod, sym_id sym );
void    OpndErr( int errcod );
void    IllName( sym_id sym );
void    IllType( sym_id sym );
void    StructErr( int errcode, sym_id sym );
void    FieldErr( int errcode, sym_id sym );
