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

/*          enum    unix    others */
#define OFT_DEFS() \
    pick( OFT_OBJ,  ".o",   ".obj" )    /* - object file                        */ \
    pick( OFT_PPO,  ".i",   ".i" )      /* - preprocessor output                */ \
    pick( OFT_ERR,  ".err", ".err" )    /* - error file name                    */ \
    pick( OFT_MBR,  ".mbr", ".mbr" )    /* - browse file name                   */ \
    pick( OFT_DEF,  ".def", ".def" )    /* - prototype definitions              */ \
    pick( OFT_DEP,  ".d",   ".d" )      /* - make-style auto-depend filename    */ \
    pick( OFT_TRG,  ".o",   ".obj" )    /* - target filename                    */
#define OFT_BRI_DEFS() \
    pick( OFT_BRI,  ".brm", ".brm" )    /* - new browser: module information    */

typedef enum {                  // TYPES OF SOURCE FILES
    FT_SRC,                     // - primary source file
    FT_HEADER,                  // - header file (i.e., #include "file.h")
    FT_HEADER_FORCED,           // - header file forced by compiler -fi option
    FT_HEADER_PRE,              // - header file (pre-include)
    FT_LIBRARY,                 // - library file (i.e., #include <stdfile.h>)
    FT_CMD                      // - command line option file (i.e., @compflags)
} src_file_type;

typedef enum {                  // TYPES OF OUTPUT FILES
    #define pick(e,u,o)     e,
        OFT_DEFS()
#ifdef OPT_BR
        OFT_BRI_DEFS()
#endif
    #undef pick
    OFT_SRCDEP,                 // - name of first depend(source) in autodep file
    OFT_MAX
} out_file_type;

#endif
