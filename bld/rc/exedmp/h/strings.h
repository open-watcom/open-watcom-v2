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
* Description:  PE Dump Utility user visible strings.
*
****************************************************************************/


#ifndef STRINGS_H
#define STRINGS_H

#define ERR_PARAM_INDENTSPACES          "Error! Missing number of indent spaces after -n\n"
#define ERR_PARAM_HEXINDENTSPACES       "Error! Missing number of hex indent spaces after -x\n"
#define ERR_PARAM_SPECIFICTYPE          "Error! Missing name of specific resource type after -s\n"
#define ERR_PARAM_SPECIFICTYPE_UNKNOWN  "Error! Unknown specific resource type\n"
#define ERR_PARAM_UNKNOWN               "Error! Unknown parameter %s\n"
#define ERR_READ_CANNOT_OPEN_FILE       "Error! Cannot open exe file %s\n"
#define ERR_READ_CANNOT_FIND_START      "Error! Cannot find start position of exe file\n"
#define ERR_READ_DOS_EXE_HEADER         "Error! Cannot read dos exe header\n"
#define ERR_READ_CANNOT_FIND_PE_HEADER  "Error! Cannot find pe exe header\n"
#define ERR_READ_PE_EXE_HEADER          "Error! Cannot read pe exe header\n"
#define ERR_READ_CANNOT_FIND_OBJECTS    "Error! Cannot find table of objects\n"
#define ERR_READ_CANNOT_FIND_RES_OBJECT "Error! Cannot find resource object\n"
#define ERR_READ_OBJECT                 "Error! Cannot read next object in table\n"
#define ERR_READ_CANNOT_FIND_TABLE_HEADER       "Error! Cannot find a table header\n"
#define ERR_READ_TABLE_HEADER           "Error! Cannot read a table header\n"
#define ERR_READ_OUT_OF_MEMORY          "Error! Out of memory"
#define ERR_READ_DIR_ENTRY              "Error! Cannot read a dir entry\n"
#define ERR_READ_CANNOT_FIND_DATA_ENTRY "Error! Cannot find a data entry\n"
#define ERR_READ_DATA_ENTRY             "Error! Cannot read a data entry\n"
#define ERR_READ_DIR_ENTRY_NAME         "Error! Cannot read a dir entry name\n"
#define ERR_READ_NOT_PE_EXE             "Error! File is not a PE Exe.\n"
#define ERR_FORMAT_CANNOT_DUMP_HEX      "(cannot dump hex: out of memory or cannot read file)"

#define LBL_DEFAULTPARAMETERS           " Default Options:\n"
#define LBL_DUMPOFFSETS                 "       Dump offsets                           "
#define LBL_DUMPHEXHEADERS              "       Dump hex headers                       "
#define LBL_DUMPHEXDATA                 "       Dump hex contents of data entry        "
#define LBL_DUMPINTERPRETATION          "       Print interpretation                   "
#define LBL_DUMPEXEHEADERINFO           "       Dump Exe headers                       "
#define LBL_DUMPRESOBJECTINFO           "       Dump resource object info              "
#define LBL_PRINTRULER                  "       Print ruler                            "
#define LBL_SPECIFICTYPE                "       Print only specific resource type      "
#define LBL_INDENTSPACES                "       Number of indent spaces for dir levels "
#define LBL_HEXINDENTSPACES             "       Number of indent spaces for hex dump   "
#define LBL_YES                         "yes"
#define LBL_NO                          "no"

#define MSG_BANNER \
        banner1w( "PE ExeDmp Utility", "1.0" ) "\n" \
        banner2          "\n" \
        banner2a("1997") "\n" \
        banner3          "\n" \
        banner3a         "\n\n"

#define MSG_HELP \
        " Usage:\n" \
        "       exedmp [options] <filename> [options]\n" \
        "\n" \
        " Toggle Options:\n" \
        " -o    dump offset of each dir header, dir entry, and data entry\n" \
        " -h    dump hex header of each dir header, dir entry, and data entry\n" \
        " -d    dump hex contents of each data entry\n" \
        " -i    print interpretation of each dir header, dir entry, and data entry\n" \
        " -x    dump Exe headers\n" \
        " -r    dump information about resource object\n" \
        " -l    print ruler\n" \
        "\n" \
        " Other Options:\n" \
        " -tX   dump only resource of type X (e.g. -tgroupicon)\n" \
        " -sX   indent each level of dir by X spaces\n" \
        " -nX   indent hex contents of data entries by X spaces\n" \
        "       (use -1 to align hex contents with their data entries)\n" \
        "\n" \
        " Available Resource Types:\n"

#define MSG_DOS_HEADER                  "Dos Exe Header:\n"
#define MSG_DOS_SIGNATURE               "Signature:                                    0x%8.4X\n"
#define MSG_DOS_MODSIZE                 "Length of load module mod 200H:               0x%8.4X\n"
#define MSG_DOS_FILESIZE                "Number of 200H pages in load module:          0x%8.4X\n"
#define MSG_DOS_NUMRELOCS               "Number of relocation items:                   0x%8.4X\n"
#define MSG_DOS_HDRSIZE                 "Size of header in paragraphs:                 0x%8.4X\n"
#define MSG_DOS_MIN16                   "Min no. of paragraphs req'd above load mod:   0x%8.4X\n"
#define MSG_DOS_MAX16                   "Max no. of paragraphs req'd above load mod:   0x%8.4X\n"
#define MSG_DOS_SSOFFSET                "Offset of stack segment in load module:       0x%8.4X\n"
#define MSG_DOS_SP                      "Initial value of SP:                          0x%8.4X\n"
#define MSG_DOS_CHKSUM                  "Checksum:                                     0x%8.4X\n"
#define MSG_DOS_IP                      "Initial value of IP:                          0x%8.4X\n"
#define MSG_DOS_CSOFFSET                "Offset of code segment in load module:        0x%8.4X\n"
#define MSG_DOS_RELOCOFFSET             "File offset of first relocation item:         0x%8.4X\n"
#define MSG_DOS_OVERLAYNUM              "Overlay number:                               0x%8.4X\n"

#define MSG_PE_HEADER                   "PE Exe Header:\n"
#define MSG_PE_SIGNATURE                "Signature:                                    0x%8.8X\n"
#define MSG_PE_CPUTYPE                  "CPU type:                                     0x%8.4X"
#define MSG_PE_CPUTYPETEXT              " (%s)\n"
#define MSG_PE_CPUTYPENOTRECOGNIZED     " (not recognized)\n"
#define MSG_PE_NUMOBJECTS               "Number of object entries:                     0x%8.4X\n"
#define MSG_PE_TIMESTAMP                "Time/date stamp:                              0x%8.8X\n"
#define MSG_PE_SYMTABLE                 "Symbol table:                                 0x%8.8X\n"
#define MSG_PE_NUMSYMS                  "Number of symbols:                            0x%8.8X\n"
#define MSG_PE_NTHDRSIZE                "NT header size:                               0x%8.4X\n"
#define MSG_PE_FLAGS                    "Flags:                                        0x%8.4X\n"
#define MSG_PE_FLAGSINDENT              "                                              "
#define MSG_PE_MAGIC                    "Magic (reserved):                             0x%8.4X\n"
#define MSG_PE_LNKMAJOR                 "Link major version number:                    0x%8.2X\n"
#define MSG_PE_LNKMINOR                 "Link minor version number:                    0x%8.2X\n"
#define MSG_PE_CODESIZE                 "Code size:                                    0x%8.8X (%li bytes)\n"
#define MSG_PE_INITDATASIZE             "Initialized data size:                        0x%8.8X (%li bytes)\n"
#define MSG_PE_UNINITDATASIZE           "Uninitialized data size:                      0x%8.8X (%li bytes)\n"
#define MSG_PE_ENTRYRVA                 "Entrypoint rva:                               0x%8.8X\n"
#define MSG_PE_CODEBASE                 "Code base:                                    0x%8.8X\n"
#define MSG_PE_DATABASE                 "Data base:                                    0x%8.8X\n"
#define MSG_PE_IMAGEBASE                "Image base:                                   0x%8.8X\n"
#define MSG_PE_OBJECTALIGN              "Object alignment, power of 2, 512 to 256M:    0x%8.8X\n"
#define MSG_PE_FILEALIGN                "File alignment factor to align image pages:   0x%8.8X\n"
#define MSG_PE_OSMAJOR                  "OS major version number:                      0x%8.4X\n"
#define MSG_PE_OSMINOR                  "OS minor version number:                      0x%8.4X\n"
#define MSG_PE_USERMAJOR                "User major version number:                    0x%8.4X\n"
#define MSG_PE_USERMINOR                "User minor version number:                    0x%8.4X\n"
#define MSG_PE_SUBSYSMAJOR              "Subsystem major version number:               0x%8.4X\n"
#define MSG_PE_SUBSYSMINOR              "Subsystem minor version number:               0x%8.4X\n"
#define MSG_PE_RSVD1                    "Reserved1:                                    0x%8.8X\n"
#define MSG_PE_IMAGESIZE                "Virtual size of image:                        0x%8.8X\n"
#define MSG_PE_HEADERSIZE               "Total header size:                            0x%8.8X\n"
#define MSG_PE_FILECHECKSUM             "File checksum:                                0x%8.8X\n"
#define MSG_PE_SUBSYSTEM                "NT Subsystem:                                 0x%8.4X"
#define MSG_PE_SUBSYSTEMTEXT            " (%s)\n"
#define MSG_PE_SUBSYSTEMNOTRECOGNIZED   " (not recognized)\n"
#define MSG_PE_DLLFLAGS                 "DLL flags:                                    0x%8.4X\n"
#define MSG_PE_DLLFLAGSINDENT           "                                              "
#define MSG_PE_STACKRESERVESIZE         "Stack reserve size:                           0x%8.8X\n"
#define MSG_PE_STACKCOMMITSIZE          "Stack commit size:                            0x%8.8X\n"
#define MSG_PE_HEAPRESERVESIZE          "Heap reserve size:                            0x%8.8X\n"
#define MSG_PE_HEAPCOMMITSIZE           "Heap commit size:                             0x%8.8X\n"
#define MSG_PE_TLSIDXADDR               "Address of tlx index:                         0x%8.8X\n"
#define MSG_PE_NUMTABLES                "Number of tables:                             0x%8.8X\n"

#define MSG_RESOBJ_HEADING              "Resource Object:\n"
#define MSG_RESOBJ_VIRTUALSIZE          "Virtual memory size:                0x%8.8X\n"
#define MSG_RESOBJ_RVA                  "Relative virtual address:           0x%8.8X\n"
#define MSG_RESOBJ_PHYSICALSIZE         "Physical size of initialized data:  0x%8.8X\n"
#define MSG_RESOBJ_PHYSICALOFFSET       "Physical offset:                    0x%8.8X\n"
#define MSG_RESOBJ_RELOCSRVA            "Relocs rva:                         0x%8.8X\n"
#define MSG_RESOBJ_LINNUMRVA            "Linnum rva:                         0x%8.8X\n"
#define MSG_RESOBJ_NUMRELOCS            "Number of relocs:                   0x%8.4X\n"
#define MSG_RESOBJ_NUMLINNUMS           "Number of linnums:                  0x%8.4X\n"
#define MSG_RESOBJ_FLAGS                "Flags:                              0x%8.8X\n"
#define MSG_RESOBJ_FLAGSINDENT          "                                    "

#define LBL_DIRHEADER                   "DIR HEADER"
#define LBL_DIRENTRY                    "DIR ENTRY"
#define LBL_DATAENTRY                   "DATA ENTRY"

#define MSG_TABLE_TIMESTAMP             "Time stamp:               0x%8.8X\n"
#define MSG_TABLE_MAJOR                 "Major version number:     0x%8.4X\n"
#define MSG_TABLE_MINOR                 "Minor version number:     0x%8.4X\n"
#define MSG_TABLE_NUMNAMEENTRIES        "Number of named entries:  0x%8.4X (%i)\n"
#define MSG_TABLE_NUMIDENTRIES          "Number of id entries:     0x%8.4X (%i)\n"
#define MSG_TABLE_FLAGS                 "Flags:                    0x%8.8X\n"
#define MSG_TABLE_FLAGSINDENT           "                          "

#define MSG_DIR_ID                      "ID:            0x%8.4X\n"
#define MSG_DIR_IDTYPE                  "Type:          %s\n"
#define MSG_DIR_NAME                    "Name:          "
#define MSG_DIR_NAMEADDRESS             "Name Address:  0x%8.8X\n"
#define MSG_DIR_NAMESIZE                "Name Size:       %8i (16-bit)\n"
#define MSG_DIR_ENTRYRVA                "Entry rva:     0x%8.8X\n"

#define MSG_DATA_RVA                    "Data rva:      0x%8.8X\n"
#define MSG_DATA_SIZE                   "Size:          0x%8.8X (%i bytes)\n"
#define MSG_DATA_CODEPAGE               "Code page:     0x%8.8X\n"
#define MSG_DATA_RSVD                   "RSVD:          0x%8.8X (must be 0)\n"

#endif
