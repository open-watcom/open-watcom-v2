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


typedef enum {
    WL_PROC_NONE,
    WL_PROC_AXP,
    WL_PROC_PPC,
    WL_PROC_X86
} processor_type;

typedef enum {
    WL_FTYPE_NONE,
    WL_FTYPE_ELF,
    WL_FTYPE_COFF,
    WL_FTYPE_OMF
} file_type;

typedef enum {
    WL_LTYPE_NONE,
    WL_LTYPE_AR,
    WL_LTYPE_MLIB,
    WL_LTYPE_OMF
} lib_type;

typedef enum {
    AR_FMT_NONE,
    AR_FMT_COFF,
    AR_FMT_GNU,
    AR_FMT_BSD
} ar_format;

typedef struct {
    // user options
    char            *input_name;
    char            *output_directory;
    char            *list_file;
    char            *output_name;
    char            *export_list_file;
    char            *explode_ext;
    unsigned short  page_size;
    unsigned        no_backup        :1;
    unsigned        respect_case     :1;
    unsigned        list_contents    :1;
    unsigned        mangled          :1;
    unsigned        quiet            :1;
    unsigned        explode          :1;
    unsigned        trim_path        :1;
    unsigned        new_library      :1;
    unsigned        nr_ordinal       :1; //do non residant table using ordinals
    unsigned        r_ordinal        :1; //do residant table using ordinals
    unsigned        strip_line       :1;
    unsigned        strip_expdef     :1;
    unsigned        strip_dependency :1;
    unsigned        strip_library    :1;
    unsigned        verbose          :1;
    unsigned        update           :1; //update older files only
    unsigned        no_c_warn        :1; //no create lib warning
    unsigned        ar               :1;
    // flags
    unsigned        modified         :1;
    unsigned        omf_found        :1; // if omf objects found
    unsigned        coff_found       :1; // if coff objects found
    unsigned        elf_found        :1; // if elf objects found
    // internal options
    unsigned        terse_listing    :1; // -tl switch

    processor_type  processor;
    file_type       filetype;
    lib_type        libtype;
    ar_format       ar_libformat;
} options_def;


#define MAX_PAGE_SIZE 32768
#define MIN_PAGE_SIZE 16
#define DEFAULT_PAGE_SIZE 512

#define WL_OS_WIN16     1
#define WL_OS_WIN32     2
#define WL_OS_OS2       3
