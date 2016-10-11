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


#ifndef HELPIO_H_INCLUDED
#define HELPIO_H_INCLUDED

#define HELPIO

typedef int     HelpFp;

typedef enum {
    HELP_SEEK_SET = 0,
    HELP_SEEK_CUR,
    HELP_SEEK_END
} HelpSeekType;

#define HELP_OPEN_RDONLY        0x000000001
#define HELP_OPEN_WRONLY        0x000000002
#define HELP_OPEN_RDWR          0x000000004
#define HELP_OPEN_APPEND        0x000000008
#define HELP_OPEN_CREAT         0x000000010
#define HELP_OPEN_TRUNC         0x000000020
#define HELP_OPEN_BINARY        0x000000040
#define HELP_OPEN_TEXT          0x000000080

#define HELP_ACCESS_READ        0x0001
#define HELP_ACCESS_WRITE       0x0002
#define HELP_ACCESS_EXEC        0x0004
#define HELP_ACCESS_EXIST       0x0008

extern HELPIO size_t    HelpRead( HelpFp fp, void *buf, size_t len );
extern HELPIO size_t    HelpWrite( HelpFp fp, const char *buf, size_t len );
extern HELPIO long int  HelpSeek( HelpFp fp, long int offset, HelpSeekType where );
extern HELPIO long int  HelpTell( HelpFp fp );
extern HELPIO HelpFp    HelpOpen( const char *path, unsigned long mode );
extern HELPIO int       HelpClose( HelpFp fp );
extern HELPIO long int  HelpFileLen( HelpFp fp );
#ifndef __NLM__
extern HELPIO void      HelpSearchEnv( const char *name, const char *env_var, char *buf );
#endif
extern HELPIO char      *HelpGetCWD( char *buf, int size );
extern HELPIO int       HelpAccess( const char *path, int mode );

#endif
