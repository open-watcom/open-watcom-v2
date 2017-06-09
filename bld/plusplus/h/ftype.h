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
* Description:  Source and output file type definitions.
*
****************************************************************************/


#ifndef _FTYPE_H_INCLUDED
#define _FTYPE_H_INCLUDED

typedef enum {                  // TYPES OF SOURCE FILES
    FT_SRC,                     // - primary source file
    FT_HEADER,                  // - header file (i.e., #include "file.h")
    FT_HEADER_FORCED,           // - header file forced by compiler -fi option
    FT_HEADER_PRE,              // - header file (pre-include)
    FT_LIBRARY,                 // - library file (i.e., #include <stdfile.h>)
    FT_CMD                      // - command line option file (i.e., @compflags)
} src_file_type;

typedef enum {                  // TYPES OF OUTPUT FILES
    OFT_OBJ,                    // - object file
    OFT_PPO,                    // - preprocessor output
    OFT_ERR,                    // - error file name
    OFT_MBR,                    // - browse file name
    OFT_DEF,                    // - prototype definitions
#ifdef OPT_BR
    OFT_BRI,                    // - new browser: module information
#endif
    OFT_DEP,                    // - make-style auto-depend filename
    OFT_TRG,                    // - target filename
    OFT_SRCDEP,                 // - name of first depend(source) in autodep file
    OFT_MAX
} out_file_type;

#endif
