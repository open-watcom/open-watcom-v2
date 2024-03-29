/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Definition used by ar format
*
****************************************************************************/


#ifndef AR_H
#define AR_H

#include <sys/stat.h>

#define AR_IDENT        "!<arch>\n"
#define AR_IDENT_LEN    8

#define AR_HEADER_IDENT     "`\n"
#define AR_HEADER_IDENT_LEN 2

#define AR_NAME_LEN     16
#define AR_DATE_LEN     12
#define AR_UID_LEN      6
#define AR_GID_LEN      6
#define AR_MODE_LEN     8
#define AR_SIZE_LEN     10

#define AR_NAME_CONTINUED_AFTER     "#1/"
#define AR_NAME_CONTINUED_AFTER_LEN 3

#define AR_NAME_END_CHAR    '/'
#define AR_VALUE_END_CHAR   ' '

#define AR_HEADER_SIZE sizeof( ar_header )

#define AR_FILE_PADDING_STRING      "\n"
#define AR_FILE_PADDING_STRING_LEN  1

#define AR_LONG_NAME_END_STRING     "/\n"
#define AR_LONG_NAME_END_STRING_LEN 2

#ifdef S_IFREG
#define AR_S_IFREG S_IFREG
#else
#define AR_S_IFREG 0100000
#endif

#ifdef S_ISUID
#define AR_S_ISUID S_ISUID
#else
#define AR_S_ISUID 0004000
#endif
#ifdef S_ISGID
#define AR_S_ISGID S_ISGID
#else
#define AR_S_ISGID 0002000
#endif

#ifdef S_IRUSR
#define AR_S_IRUSR S_IRUSR
#else
#define AR_S_IRUSR 0000400
#endif
#ifdef S_IWUSR
#define AR_S_IWUSR S_IWUSR
#else
#define AR_S_IWUSR 0000200
#endif
#ifdef S_IXUSR
#define AR_S_IXUSR S_IXUSR
#else
#define AR_S_IXUSR 0000100
#endif
#ifdef S_IRGRP
#define AR_S_IRGRP S_IRGRP
#else
#define AR_S_IRGRP 0000040
#endif
#ifdef S_IWGRP
#define AR_S_IWGRP S_IWGRP
#else
#define AR_S_IWGRP 0000020
#endif
#ifdef S_IXGRP
#define AR_S_IXGRP S_IXGRP
#else
#define AR_S_IXGRP 0000010
#endif
#ifdef S_IROTH
#define AR_S_IROTH S_IROTH
#else
#define AR_S_IROTH 0000004
#endif
#ifdef S_IWOTH
#define AR_S_IWOTH S_IWOTH
#else
#define AR_S_IWOTH 0000002
#endif
#ifdef S_IXOTH
#define AR_S_IXOTH S_IXOTH
#else
#define AR_S_IXOTH 0000001
#endif

typedef struct ar_header {
    char        name[AR_NAME_LEN];
    char        date[AR_DATE_LEN];
    char        uid[AR_UID_LEN];
    char        gid[AR_GID_LEN];
    char        mode[AR_MODE_LEN];
    char        size[AR_SIZE_LEN];
    char        header_ident[AR_HEADER_IDENT_LEN];
} ar_header;

#endif
