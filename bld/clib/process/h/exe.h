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


#define EXE_ID                  0x5a4d

#pragma pack(__push,1);
typedef struct an_exe_header {
    unsigned    id;             /* Contains EXE_ID.  This is the link programs
                                   signature to mark the file as a valid .EXE
                                   file. */
    unsigned    length_mod_512; /* Length of load module image mod 512. */
    unsigned    length_div_512; /* Size of file in pages, including header.
                                   (length+511)/512. */
    unsigned    reloc_count;    /* Number of relocation table items. */
    unsigned    header_para;    /* Size of header in paragraphs.  This is used
                                   to locate the beginning of the load module
                                   in the file. */
    unsigned    min_para;       /* Minimum number of paragraphs required above
                                   the end of the loaded program. */
    unsigned    max_para;       /* Maximum number of paragraphs required above
                                   the end of the loaded program. */
    unsigned    ss;             /* Displacement in paragraphs of stack segment
                                   within load module. */
    unsigned    sp;             /* Offset to be in the SP register when the
                                   module is given control. */
    unsigned    checksum;       /* Word checksum - negative sum of all the words
                                   in the file, ignoring overflow. */
    unsigned    ip;             /* Offset to be in the IP register when the
                                   module is given control. */
    unsigned    cs;             /* Displacement in paragraphs of code segment
                                   within load module. */
    unsigned    reloc_index;    /* Displacement in bytes of the first relocation
                                   item within the file. */
    unsigned    overlay;        /* Overlay number (0 for resident part of the
                                   program. */
} an_exe_header;

/*
    Each relocation table item segment value is added to the start segment
    value.  This calculated segment, in conjunction with the relocation offset
    value, points to a word in the load module to which is added the start
    segment value.  The result is placed back into the word in the load module.
*/

typedef struct a_reloc {
    unsigned    offset;
    unsigned    segment;
} a_reloc;
#pragma pack(__pop);
