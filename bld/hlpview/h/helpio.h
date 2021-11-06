/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef HELPIO_H_INCLUDED
#define HELPIO_H_INCLUDED

#define HELPIO

typedef enum {
    HELP_SEEK_SET = 0,
    HELP_SEEK_CUR,
    HELP_SEEK_END
} HelpSeekType;

extern HELPIO size_t    HelpRead( FILE *fp, void *buf, size_t len );
extern HELPIO long      HelpSeek( FILE *fp, long offset, HelpSeekType where );
extern HELPIO long      HelpTell( FILE *fp );
extern HELPIO FILE      *HelpOpen( const char *path );
extern HELPIO int       HelpClose( FILE *fp );
extern HELPIO long      HelpFileLen( FILE *fp );
#ifndef __NLM__
extern HELPIO void      HelpSearchEnv( const char *name, const char *env_var, char *buf );
#endif
extern HELPIO char      *HelpGetCWD( char *buf, int size );
extern HELPIO int       HelpFileAccess( const char *path );

#endif
