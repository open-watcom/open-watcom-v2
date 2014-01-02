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


/* aligned */
typedef int             import_handle;
typedef pointer         abspatch_handle;
#define BACKSEGS        (segment_id)32767
#define UNDEFSEG        (segment_id)-1

#define ADDR_UNKNOWN    ((offset)(-1))
#define NOT_IMPORTED    0

typedef struct bck_info {
        import_handle   imp;
        import_handle   imp_alt;
        label_handle    lbl;
        segment_id      seg;
} bck_info;

typedef struct segdef {
        struct segdef   *next;
        char            *str;
        segment_id      id;
        byte            align;
        seg_attr        attr;
} segdef;

typedef enum {
        CG_FE,
        CG_LBL,
        CG_BACK,
        CG_TBL,
        CG_VTB,
        CG_CLB
} cg_class;

#define PROC_ALIGN      ((unsigned)-1)
#define DEEP_LOOP_ALIGN ((unsigned)-2)
