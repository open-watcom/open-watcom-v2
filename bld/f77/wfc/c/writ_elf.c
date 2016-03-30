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
* Description:  code to generate ELF output
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include "wio.h"
#include "dw.h"
#include "exeelf.h"
#include "ftnstd.h"
#include "errcod.h"
#include "fio.h"
#include "cioconst.h"
#include "global.h"
#include "brow2elf.h"
#include "ferror.h"
#include "cspawn.h"
#include "sdcio.h"


// -- code to generate ELF output ------------------------------------------
//
// note: the pre-initialized fields in these structures assume the following
//       layout in the file
//              elf_header
//              string_table
//              .debug_abbrev
//              .debug_info
//              .debug_ref
//              .debug_line
//              .debug_macinfo
//              section_header_index0
//              section_header_string_table
//              section_header_template( .debug_abbrev )
//              section_header_template( .debug_info )
//              section_header_template( .debug_ref )
//              section_header_template( .debug_line )
//              section_header_template( .debug_macinfo )
//

Elf32_Ehdr elf_header = {
    { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
      ELFCLASS32, ELFDATA2LSB, EV_CURRENT },
    ET_DYN,
    EM_386,
    EV_CURRENT,
    0,
    0,
    0,//<offset of section table>=sizeof(Elf32_Ehdr)+sizeof(string_table)+sizes of 5 sections
    0,
    sizeof( Elf32_Ehdr ),
    sizeof( Elf32_Phdr ),
    0,
    sizeof( Elf32_Shdr ),
    7,
    1
};

#define STR_NAME0           "\0"
#define STR_SHSTRTAB        ".shstrtab\0"
#define STR_DBG_ABBREV      ".debug_abbrev\0"
#define STR_DBG_INFO        ".debug_info\0"
#define STR_DBG_REF         ".WATCOM_references\0"
#define STR_DBG_LINE        ".debug_line\0"
#define STR_DBG_MACINFO     ".debug_macinfo\0"
#define OFF_NAME0           (0)
#define OFF_SHSTRTAB        (OFF_NAME0 + sizeof( STR_NAME0 ) - 1)
#define OFF_DBG_ABBREV      (OFF_SHSTRTAB + sizeof( STR_SHSTRTAB ) - 1)
#define OFF_DBG_INFO        (OFF_DBG_ABBREV + sizeof( STR_DBG_ABBREV ) - 1)
#define OFF_DBG_REF         (OFF_DBG_INFO + sizeof( STR_DBG_INFO ) - 1)
#define OFF_DBG_LINE        (OFF_DBG_REF + sizeof( STR_DBG_REF ) - 1)
#define OFF_DBG_MACINFO     (OFF_DBG_LINE + sizeof( STR_DBG_LINE ) - 1)
#define OFF_MAX             (OFF_DBG_MACINFO + sizeof( STR_DBG_MACINFO ) - 1)
char string_table[OFF_MAX + 1] = {
    STR_NAME0
    STR_SHSTRTAB
    STR_DBG_ABBREV
    STR_DBG_INFO
    STR_DBG_REF
    STR_DBG_LINE
    STR_DBG_MACINFO
};
unsigned string_table_offsets[] = {
    OFF_DBG_ABBREV,
    OFF_DBG_INFO,
    OFF_DBG_REF,
    OFF_DBG_LINE,
    OFF_DBG_MACINFO
};

Elf32_Shdr section_header_index0 = {
    0, SHT_NULL, 0, 0, 0, 0, SHN_UNDEF, 0, 0, 0
};

Elf32_Shdr section_header_string_table = {
    OFF_SHSTRTAB,
    SHT_STRTAB,
    0,
    0,
    sizeof( Elf32_Ehdr ),
    sizeof( string_table ),
    SHN_UNDEF,
    0,
    0,
    0
};

Elf32_Shdr section_header_template = {
    0,//<index of name in string section>
    SHT_PROGBITS,
    0,
    0,
    0,//<offset of section in file>
    0,//<size of section>
    SHN_UNDEF,
    0,
    0,
    0
};

static  char    *fName;

static void mywrite( FILE *fp, void *data, size_t len )
/*****************************************************/
{
    char        err_msg[ERR_BUFF_SIZE+1];

    SDWrite( fp, data, len );
    if( SDError( fp, err_msg ) ) {
        Error( SM_IO_WRITE_ERR, fName, err_msg );
        CSuicide();
    }
}

int CreateBrowseFile(FILE*        browseFile,    /*target file */
                     section_data *abbrevFile,   /*.debug_abbrev section*/
                     section_data *debugFile,    /*.debug_info section*/
                     section_data *referenceFile,/*.WATCOM_reference section */
                     section_data *lineFile,     /*.debug_line section */
                     section_data *macroFile,    /*.debug_macinfo section*/
                     int          (*sect_read)( char *buf, int size, int sec),
                     char         *filename      /* name of browse file */
                    ) {
/*********************/

    char *buffer;
    size_t readSize;
    int fileNum;
    section_data *inFile[5];
    unsigned long sectionSize;
    unsigned long sectionOffset[5];

    fName = filename;

    // write elf header
    elf_header.e_shoff = sizeof( Elf32_Ehdr )
                        + sizeof( string_table )
                        + abbrevFile->max_offset
                        + debugFile->max_offset
                        + referenceFile->max_offset
                        + lineFile->max_offset
                        + macroFile->max_offset;
    mywrite( browseFile, (void *)&elf_header, sizeof( elf_header ) );

    // write string table
    mywrite( browseFile, (void *)string_table, sizeof( string_table ) );

    // write each of the 5 sections, tracking offset
    inFile[0] = abbrevFile;
    inFile[1] = debugFile;
    inFile[2] = referenceFile;
    inFile[3] = lineFile;
    inFile[4] = macroFile;
    sectionOffset[0] = sizeof( elf_header ) + sizeof( string_table );
    sectionOffset[1] = sectionOffset[0] + inFile[0]->max_offset;
    sectionOffset[2] = sectionOffset[1] + inFile[1]->max_offset;
    sectionOffset[3] = sectionOffset[2] + inFile[2]->max_offset;
    sectionOffset[4] = sectionOffset[3] + inFile[3]->max_offset;

    buffer = TokenBuff;
    for ( fileNum = 0; fileNum < 5; fileNum++ ) {
        readSize = TOKLEN;
        sectionSize = inFile[fileNum]->max_offset;
        while( sectionSize ) {
            readSize = sect_read(buffer, readSize,
                                inFile[fileNum]->sec_number);
            sectionSize -= readSize;
            mywrite( browseFile, (void *)buffer, readSize );
        }
    }

    // write section_header_index0
    mywrite( browseFile, (void *)&section_header_index0,
                         sizeof( section_header_index0 ) );

    // write section_header_string_table
    mywrite( browseFile, (void *)&section_header_string_table,
                         sizeof( section_header_string_table ) );

    // write rest of section headers
    for (fileNum=0;fileNum<5;fileNum++) {
        section_header_template.sh_name = string_table_offsets[fileNum];
        section_header_template.sh_offset = sectionOffset[fileNum];
        section_header_template.sh_size = inFile[fileNum]->max_offset;
        mywrite( browseFile, (void *)&section_header_template,
                             sizeof( section_header_template ) );
    }
    return 0;
}
