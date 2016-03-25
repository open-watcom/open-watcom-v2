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
* Description:  Processing of the NEW command, program and symbol loading.
*
****************************************************************************/


extern bool InitCmd( void );
extern void FindLocalDebugInfo( const char *name );
extern bool DownLoadCode( void );
extern void FiniCmd( void );
extern void InitLocalInfo( void );
extern void FiniLocalInfo( void );
extern image_entry *ImagePrimary( void );
extern image_entry *ImageEntry( mod_handle mh );
extern address DefAddrSpaceForMod( mod_handle mh );
extern address DefAddrSpaceForAddr( address addr );
extern void MapAddrForImage( image_entry *image, addr_ptr *addr );
extern bool UnMapAddress( mappable_addr *loc, image_entry *image );
extern void UnMapPoints( image_entry *image );
extern void FreeImage( image_entry *image );
extern char *GetLastImageName( void );
extern void UnLoadImgSymInfo( image_entry *image, bool nofree );
extern bool ReLoadImgSymInfo( image_entry *image );
extern remap_return ReMapImageAddress( mappable_addr *loc, image_entry *image );
extern bool ReMapAddress( mappable_addr *loc );
extern void ReMapPoints( image_entry *image );
extern bool LoadDeferredSymbols( void );
extern bool AddLibInfo( bool already_stopping, bool *force_stop );
extern bool SetProgStartHook( bool new );
extern void LoadProg( void );
extern void ReleaseProgOvlay( bool free_sym );
extern void InitMappableAddr( mappable_addr *loc );
extern void FiniMappableAddr( mappable_addr *loc );
extern size_t GetProgName( char *where, size_t len );
extern size_t GetProgArgs( char *where, size_t len );
extern void SetSymFileName( const char *file );
extern void LoadNewProg( const char *cmd, const char *parms );
extern void ReStart( void );
extern bool SymUserModLoad( const char *fname, address *loadaddr );
extern bool SymUserModUnload( char *fname );
extern void ProcNew( void );
extern void RecordNewProg( void );
