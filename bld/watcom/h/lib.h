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


#ifndef LIB_H

#define LIB_H

#pragma pack(push,1);

#define LIBMAG          "!<mlib>\n"
#define LIBMAG_LEN      8

#define LIB_CLASS_LEN   4
#define LIB_DATA_LEN    4

#define LIB_CLASS_DATA_SHOULDBE "1   1   "

#define LIBFMAG         "`\n"
#define LIBFMAG_LEN     2

#define LIB_NAME_LEN    16
#define LIB_DATE_LEN    12
#define LIB_UID_LEN     6
#define LIB_GID_LEN     6
#define LIB_MODE_LEN    8
#define LIB_SIZE_LEN    10

#define LIB_NAME_END_CHAR       '/'
#define LIB_VALUE_END_CHAR      ' '

#define LIB_SYMTAB_NAME "/               "      // Symbol Table.
#define LIB_LFTAB_NAME  "//              "      // Long filename table.
#define LIB_FFTAB_NAME  "///             "      // Full filename table.

typedef struct
{
    unsigned char       lib_magic[LIBMAG_LEN];
    unsigned char       lib_class[LIB_CLASS_LEN];
    unsigned char       lib_data[LIB_DATA_LEN];
} Lib32_File;

typedef struct
{
    unsigned char       lib_name[LIB_NAME_LEN];
    unsigned char       lib_date[LIB_DATE_LEN];
    unsigned char       lib_uid[LIB_UID_LEN];
    unsigned char       lib_gid[LIB_GID_LEN];
    unsigned char       lib_mode[LIB_MODE_LEN];
    unsigned char       lib_size[LIB_SIZE_LEN];
    unsigned char       lib_fmag[LIBFMAG_LEN];
} Lib32_Hdr;

#define LIB_HEADER_SIZE sizeof( Lib32_Hdr )

#define LIB_FILE_PADDING_STRING "\n"
#define LIB_FILE_PADDING_STRING_LEN     1

#define LIB_LONG_NAME_END_STRING        "/\n"
#define LIB_LONG_NAME_END_STRING_LEN    2

#pragma pack(pop);

#endif
