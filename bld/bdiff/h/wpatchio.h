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


#define PATCH_FILE_PATCHED      100
#define PATCH_DIR_DELETED       101
#define PATCH_FILE_DELETED      102
#define PATCH_FILE_ADDED        103
#define PATCH_DIR_ADDED         104
#define PATCH_EOF               255

#define PATCH_MAX_PATH_SIZE     250

extern void PatchWrite( void *patch, int size );
extern void PatchWriteOpen( const char *name );
extern void PatchWriteClose( void );
extern void PatchWriteFile( short flag, const char *RelPath );
extern void PatchGetFile( const char *path );
extern void PatchReadOpen( const char *name );
extern void PatchReadClose( void );
extern void PatchReadFile( short *Pflag, char *RelPath );
