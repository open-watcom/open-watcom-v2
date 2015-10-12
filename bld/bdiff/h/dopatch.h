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
* Description:  Perform binary patching.
*
****************************************************************************/


#if !defined( BDIFF )
extern const char       *PatchName;
extern const char       *NewName;
extern int              DoPrompt;
extern int              DoBackup;
extern int              PrintLevel;
#endif

extern void             GetLevel( const char *name );
extern void             FileCheck( int fd, const char *name );
extern void             SeekCheck( long pos, const char *name );
#ifdef BDIFF
extern PATCH_RET_CODE   Execute( byte *dest );
#else
extern PATCH_RET_CODE   Execute( void );
extern PATCH_RET_CODE   DoPatch( const char *patchname, int doprompt, int dobackup, int printlevel, char *outfilename );
#endif
