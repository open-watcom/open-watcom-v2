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


#ifndef _F77_BROWSE_2_ELF_GENERATOR_
#define _F77_BROWSE_2_ELF_GENERATOR_

typedef struct section_data {
    char                sec_type;
    unsigned_32         cur_offset;
    unsigned_32         max_offset;
    union {
        file_handle     *fp;
        long            size;
    };
    union {
        char            *data;
        char            *filename;
    };
    unsigned_32         sec_number;
} section_data;

#define MEM_INCREMENT   10240

int CreateBrowseFile(FILE         *browseFile,   /*target file */
                     section_data *abbrevFile,   /*.debug_abbrev section*/
                     section_data *debugFile,    /*.debug_info section*/
                     section_data *referenceFile,/*.WATCOM_reference section */
                     section_data *lineFile,     /*.debug_line section */
                     section_data *macroFile,    /*.debug_macinfo section*/
                     int          (*sect_read)( char *buf, int size, int sec),
                     char         *filename      /* name of browse file */
                    );
#endif
