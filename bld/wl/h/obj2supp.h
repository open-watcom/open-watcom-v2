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
    FIX_CHANGE_SEG      = 0x00000001,   // has to be 1.  used in pointers!
    FIX_ADDEND_ZERO     = 0x00000002,
    FIX_UNSAFE          = 0x00000004,
    FIX_ABS             = 0x00000008,

    FIX_BASE            = 0x00000010,
    FIX_HIGH            = 0x00000020,
    FIX_REL             = 0x00000040,
    FIX_SHIFT           = 0x00000080,

    FIX_TARGET_SHIFT    = 8,            // contains frame_type
    FIX_TARGET_MASK     = 0x00000700,

    FIX_NO_BASE         = 0x00001000,
    FIX_SIGNED          = 0x00002000,
    FIX_LOADER_RES      = 0x00004000,
    FIX_SEC_REL         = 0x00008000,

    FIX_NO_OFFSET       = 0,
    FIX_OFFSET_8        = 0x00010000,
    FIX_OFFSET_16       = 0x00020000,
    FIX_OFFSET_21       = 0x00030000,
    FIX_OFFSET_32       = 0x00040000,
    FIX_OFFSET_24       = 0x00050000,
    FIX_OFFSET_SHIFT    = 16,
    FIX_OFFSET_MASK     = 0x00070000,

    FIX_TOC             = 0x00100000,   // PPC PE
    FIX_TOCV            = 0x00200000,   // PPC PE
    FIX_IFGLUE          = 0x00300000,   // PPC PE
    FIX_SPECIAL_MASK    = 0x00300000,

    FIX_FRAME_SHIFT     = 24,           // contains frame_type
    FIX_FRAME_MASK      = 0x07000000,


// now for some handy constants which use these

    FIX_BASE_OFFSET_16  = (FIX_BASE | FIX_OFFSET_16),
    FIX_BASE_OFFSET_32  = (FIX_BASE | FIX_OFFSET_32),
    FIX_HIGH_OFFSET_8   = (FIX_HIGH | FIX_OFFSET_8),
    FIX_HIGH_OFFSET_16  = (FIX_HIGH | FIX_OFFSET_16),
} fix_type;

#define FIX_GET_OFFSET(x) (((x) & FIX_OFFSET_MASK) >> FIX_OFFSET_SHIFT)
#define FIX_GET_FRAME(x)  (((x) & FIX_FRAME_MASK) >> FIX_FRAME_SHIFT)
#define FIX_GET_TARGET(x) (((x) & FIX_TARGET_MASK) >> FIX_TARGET_SHIFT)

/* This ordering is roughly the same as intel's - don't mess it up without
   a good reason (and make sure the code that depends on it is fixed!) */

typedef enum {
    FIX_FRAME_SEG,      /* segdata */
    FIX_FRAME_GRP,      /* group_entry */
    FIX_FRAME_EXT,      /* symbol * */
    FIX_FRAME_ABS,      /* absolute value */
    FIX_FRAME_LOC,      /* frame containing location */
    FIX_FRAME_TARG,     /* frame same as target */
    FIX_FRAME_FLAT      /* frame is flat group */
} frame_type;

#define FRAME_HAS_DATA( fix ) ((fix) < FIX_FRAME_LOC)

typedef struct {
    union {
        segdata *       sdata;
        group_entry *   group;
        symbol *        sym;
        segment         abs;
        void *          ptr;
        unsigned        val;
    } u;
    frame_type  type;
} frame_spec;

// functions external to obj2supp

extern unsigned IncExecRelocs( void * );
extern unsigned IncSaveRelocs( void * );
extern unsigned RelocMarkSyms( void *fix );
extern void     RelocStartMod( void );
extern void     StoreFixup(offset, fix_type, frame_spec *, frame_spec *,offset);
