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


#ifndef _REWRITE_H
#define _REWRITE_H

typedef struct rewrite_tokens   REWRITE_TOKENS;
typedef struct srcfile_handle   SRCFILE_HANDLE;

#define TOKEN_BLOCK_SIZE        (256)

// 'free' must be two pointers from offset 0 (see CarveWalkAllFree)
struct rewrite_tokens {
    REWRITE_TOKENS              *next;
    unsigned                    count;
    unsigned                    free : 1;       // used for precompiled headers
    uint_8                      stream[TOKEN_BLOCK_SIZE];
};

struct srcfile_handle {
    SRCFILE_HANDLE              *next;
    SRCFILE                     srcfile;
};

struct rewrite_package {
    REWRITE_TOKENS              *list;
    REWRITE_TOKENS              *curr;
    SRCFILE_HANDLE              *srcfiles_refd;
    uint_8                      *token;
    uint_8                      last_token;
    unsigned                    busy : 1;       // being used by RewriteToken
    unsigned                    alternate : 1;  // nested use of a REWRITE
    unsigned                    free : 1;       // used for precompiled headers
};

extern void RewriteFree( REWRITE * );
extern REWRITE *RewritePackageFunction( PTREE );
extern REWRITE *RewritePackageDefArg( PTREE );
extern REWRITE *RewritePackageMemInit( PTREE );
extern REWRITE *RewritePackageClassTemplate( REWRITE *, TOKEN_LOCN * );
extern REWRITE *RewritePackageClassTemplateMember( REWRITE *, TOKEN_LOCN * );
extern REWRITE *RewritePackageToken( void );
extern REWRITE *RewriteRewind( REWRITE * );
extern REWRITE *RewriteRecordInit( TOKEN_LOCN * );
extern void RewriteRestoreToken( REWRITE * );
extern void RewriteRecordToken( REWRITE *, TOKEN_LOCN * );
extern void RewriteToken( void );
extern void RewriteClose( REWRITE * );

REWRITE *RewriteGetIndex( REWRITE * );
REWRITE *RewriteMapIndex( REWRITE * );

#endif
