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


typedef unsigned_32 cue_idx;

typedef struct cue_state{
    cue_idx     cue;
    cg_linenum  line;
    short       fno;
    short       col;
}cue_state;

#define CUES_PER_BLK  200
#define MAX_LINE_DELTA  20
#define PRIMARY_RANGE   0x8000

typedef struct cue_blk {
    struct cue_blk *next;
    cue_state       info[CUES_PER_BLK];
}cue_blk;

typedef struct cue_ctl {
    cue_blk        *head;
    cue_blk        *curr;
    cue_blk       **lnk;
    cue_state      *next;
    cue_state      *end;
    cue_state       state;
    cue_state       start[1];
    long            count;
}cue_ctl;

/* filename to number mapping */
typedef struct fname_lst {
    struct fname_lst   *next;
    unsigned_16         len;
    char                fname[1];
}fname_lst;

typedef struct {
    fname_lst   *lst;
    short       count;
}fname_ctl;

extern bool CueFind( cue_idx cue, cue_state *ret );
extern  char *SrcFNoFind( uint fno );
