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
* Description:  Definition used by ar format
*
****************************************************************************/


#ifndef AR_H

#define AR_H

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

#define AR_ELEMENT_BASE 10
#define AR_MODE_BASE    8

#define AR_NAME_CONTINUED_AFTER     "#1/"
#define AR_NAME_CONTINUED_AFTER_LEN 3

#define AR_NAME_END_CHAR    '/'
#define AR_VALUE_END_CHAR   ' '

struct ar_header {
    char        name[AR_NAME_LEN];
    char        date[AR_DATE_LEN];
    char        uid[AR_UID_LEN];
    char        gid[AR_GID_LEN];
    char        mode[AR_MODE_LEN];
    char        size[AR_SIZE_LEN];
    char        header_ident[AR_HEADER_IDENT_LEN];
};
typedef struct ar_header ar_header;

#define AR_HEADER_SIZE sizeof( ar_header )

#define AR_FILE_PADDING_STRING      "\n"
#define AR_FILE_PADDING_STRING_LEN  1

#define AR_LONG_NAME_END_STRING     "/\n"
#define AR_LONG_NAME_END_STRING_LEN 2

#endif
