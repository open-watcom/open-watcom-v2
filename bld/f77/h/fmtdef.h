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

#ifndef __FMTDEF_H_INCLUDED
#define __FMTDEF_H_INCLUDED

//
// Format code structure definitions -- used at runtime
//

#ifdef __RT__           // Data is packed in the executable
  #include "pushpck1.h"
#endif

typedef struct fmt {    //  used for: S, SP, SS, BN, BZ, /, :,
    byte        code;   //            (, and ) format codes
} fmt;

typedef struct fmt1 {   //  used for: L, Z and P format codes
    byte        code;
    byte        fld1;
} fmt1;

typedef struct fmt2 {   //  used for: Iw, Iw.m, Fw.d, and Dw.d
    byte        code;   //            format codes
    byte        fld1;
    byte        fld2;
} fmt2;


typedef struct fmt3 {   //  used for: Ew.d, Ew.dE.e, Ew.dDe,
    byte        code;   //            Gw.d, and Gw.dEe format codes
    byte        fld1;
    byte        fld2;
    byte        fld3;
} fmt3;

typedef struct fmt4 {   //  used for: A, Aw, T, TL, TR, X,
    byte        code;   //            repeat code and end of
    int         fld1;   //            format code
} fmt4;

typedef struct fmtstring {
    byte        code;
    int         fld1;
    char        str[1];
} fmtstring;

#ifdef __RT__           // Return packing to normal
  #include "poppck.h"
#endif

typedef union fmt_desc {
    struct fmt          fmt;
    struct fmt1         fmt1;
    struct fmt2         fmt2;
    struct fmt3         fmt3;
    struct fmt4         fmt4;
    struct fmtstring    fmtstring;
} fmt_desc;

typedef union fmt_ptr {
    obj_ptr             cp;     // compile-time pointer
    char PGM            *rt;    // run-time pointer
} fmt_ptr;
#endif /* __FMTDEF_H_INCLUDED */
