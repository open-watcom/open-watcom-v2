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


#ifndef _MISC_H
#define _MISC_H  1
typedef struct EnvTracker {
    struct EnvTracker   *next;
    char                value[1];
} ENV_TRACKER;

extern char *FixName( char *name );
extern int FNameCmp( const char *a, const char *b );
#ifdef USE_FAR
extern int _fFNameCmp( const char FAR *a, const char FAR *b );
#endif
extern const char *DoWildCard( const char *base );
extern int KWCompare( const char **p1, const char **p2 );
extern char *SkipWS( const char *p );
extern int PutEnvSafe( ENV_TRACKER *env );
#ifndef NDEBUG
extern void PutEnvFini( void );
#endif

#endif
