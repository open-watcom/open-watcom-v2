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
* Description:  Librarian helper functions prototypes.
*
****************************************************************************/


extern void GetFileContents( char *name, libfile io, arch_header *arch, char **contents );
extern void Copy( libfile source, libfile dest, file_offset size );
extern char *MakeObjOutputName( char *src, char *new );
extern char *MakeListName( void );
extern char *MakeFName( const char *a );
extern char *MakeBakName( void );
extern bool SameName( char *a, char *b );
extern int  SymbolNameCmp( const char *s1, const char *s2);
extern bool IsExt( char *a, char *b );
extern void NewArchHeader( arch_header *arch, char *name );
extern void DefaultExtension( char *name, char *def_ext );
extern char *TrimPath( char * );
extern bool SameFile( char *a, char *b );
extern char *FormSym(char*);
extern char *WlibGetEnv( char *name);
extern void Banner( void );
extern char *LibFormat( void );
