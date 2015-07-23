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
* Description:  NetWare trap file startup.
*
****************************************************************************/


extern char *getcmd( char *buff );
extern char upper( char ch );
extern char lower( char ch );
extern int isalpha( char ch );
extern int strnicmp( const char *a, const char *b, size_t len );
extern int stricmp( const char *a, const char *b );
extern char *strupr( char *str );
extern char *strcpy( char *dst, const char *src );
extern char *strcat( char *dst, const char *src );
extern size_t strlen( const char *str );
extern void * memcpy( void *_dst, const void * _src, size_t len );
extern int memcmp( const void *a_in, const void *b_in, size_t len );
extern void *memset( void *dst, int c, size_t len );
extern char *strchr( const char *s, int c );
extern int toupper( int c );
extern void _Stop( void );
extern int _Check( void );
extern void    MainHelper( void );
extern LONG _Prelude(
         struct LoadDefinitionStructure  *NLMHandle,
         struct ScreenStruct            *initializationErrorScreenID,
         BYTE *                           cmdLineP,
         BYTE *                           loadDirectoryPath,
         LONG                             uninitializedDataLength,
         LONG                             NLMfileHandle,
         LONG                           (*readRoutineP)(),
         LONG                             customDataOffset,
         LONG                             customDataSize );
extern void __WATCOM_Prelude(void);
extern void __Null_Argv(void);
extern void __Init_Argv(void);
#ifdef DEBUG_ME
extern void ConsolePrintf( char *format, ... );
#endif
extern void WriteStdErr( char *str, int len );
