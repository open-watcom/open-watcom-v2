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


#include "dfdip.h"
#include "dfline.h"

typedef struct {
    addr_off        offset;
    dword           line;
    unsigned_16     fno;
} cue_info;

enum {
    CUES_PER_BLK = 256,
};

typedef struct cue_blk {
    struct cue_blk  *next;
    cue_info        info[CUES_PER_BLK]; /* variableb */
} cue_blk;

typedef struct seg_cue {
    struct seg_cue  *next;
    addr_seg        seg;
    addr_off        low;
    addr_off        high;
    unsigned_16     count;    /* #entries collected by head */
    cue_blk         *head;    /* list of cues               */
} seg_cue;



void  InitCueList( cue_list *ctl ) {
/*********************************/

    ctl->head = NULL;
}

seg_cue *InitSegCue( cue_list *ctl, addr_seg seg, addr_off offset )
/*****************************************************************/
/* Keep in asending order  seg:offset */
{
    seg_cue     *curr;
    seg_cue     **lnk;

    for( lnk = &ctl->head; (curr = *lnk) != NULL; lnk = &curr->next ) {
        if( seg < curr->seg || ( seg == curr->seg && offset < curr->low ) ) {
            break;
        }
    }
    curr = DCAlloc( sizeof( *curr ) );
    curr->seg = seg;
    curr->low = offset;
    curr->high = offset;
    curr->count = 0;
    curr->head = NULL;
    curr->next = *lnk;
    *lnk = curr;
    return( curr );
}

void AddCue( seg_cue *ctl, dr_line_data *new )
/********************************************/
// Add a new offset to the last block if full alloc a new one
// bump the item count
{
    cue_blk     *blk, **lnk;
    cue_info    *next;
    unsigned_16 rem;    /*#entries in last blk */

    rem = ctl->count % CUES_PER_BLK;
    blk = ctl->head;
    if( blk != NULL ) {
        while( blk->next  != NULL ) {
            blk = blk->next;
        }
        lnk = &blk->next;
    } else {
        lnk = &ctl->head;
    }
    if( rem == 0 ) {
        blk = DCAlloc( sizeof( *blk ) );
        blk->next = NULL;
        *lnk = blk;
    }
    next = blk->info + rem;
    next->offset = new->offset;
    next->line = new->line;
    next->fno = new->file;
    if( new->offset > ctl->high ) {
        ctl->high = new->offset;
    }
    ++ctl->count;
}


typedef struct{
    unsigned_16 hi;
    cue_info   *base;
    addr_off    key;
    unsigned_16 last;
}off_cmp;


static  long BlkOffSearch( off_cmp *cmp  ) {
/****************************************/
// Do a B-search on the blk
    cue_info    *curr;
    cue_info    *base;
    addr_off    key;
    unsigned_16 lo;
    unsigned_16 mid;
    unsigned_16 hi;

    key = cmp->key;
    base = cmp->base;
    hi = cmp->hi;
    lo = 0;
    for(;;) {
        mid = MIDIDX16( lo, hi );
        curr = base + mid;
        if( mid == lo ) { /* fix up last cmp */
            break;
        }
        if( key < curr->offset ) {  // key < mid
            hi = mid;
        } else {                    // key > mid
            lo = mid;
        }
    }
    cmp->last = mid;
    cmp->base = curr;
    return( key - curr->offset );
}

static  seg_cue  *FindSegCue( cue_list *list, addr_ptr *mach )
/************************************************************/
{
    seg_cue   *ctl;
    seg_cue   *last;

    last = NULL;
    for( ctl = list->head; ctl != NULL; ctl = ctl->next ) {
        if( mach->segment == ctl->seg ) {
            if( mach->offset < ctl->low )
                break;
            last = ctl;
        }
    }
    return( last );
}

bool FindCueOffset( cue_list *list, addr_ptr *mach, cue_item *ret )
/*****************************************************************/
{
    cue_blk     *blk;
    cue_info    *info;
    off_cmp     cmp;
    unsigned_16 rem;
    seg_cue     *ctl;
    long        diff;
    addr_off    next_off;

    info = NULL;
    ctl = FindSegCue( list, mach );
    if( ctl != NULL ) {
        rem = CUES_PER_BLK;
        cmp.last = 0;
        cmp.hi = 0;
        for( blk = ctl->head; blk != NULL; blk = blk->next ) {
            if( blk->next == NULL ) { /* last blk */
                rem = ctl->count % CUES_PER_BLK;
                if( rem == 0 ) {
                    rem = CUES_PER_BLK; /* never have empty blocks */
                }
            }
            info = blk->info + rem - 1;
            cmp.last = rem - 1;
            cmp.hi = rem;
            if( info->offset >= mach->offset ) {
                cmp.key  = mach->offset;
                cmp.base = blk->info;
                diff = BlkOffSearch( &cmp );
                if( diff >= 0 ) {
                    info = cmp.base;
                    break;
                } else {
                     //shouldn't happen an error
                    return( false );
                }
            }
        }
        ret->fno = info->fno;
        ret->line = info->line;
        ret->mach.offset = info->offset;
        ret->mach.segment = ctl->seg;
        ret->col = 0;
        if( cmp.last + 1 < cmp.hi ) {
            ++info;
            next_off = info->offset;
        } else {
            if( blk != NULL ) {
                blk = blk->next;
            }
            if( blk == NULL ) {
                next_off = 0xffffffff; /* high value */
                if( (ctl = ctl->next) != NULL ) {
                    if( ctl->seg == ret->mach.segment ) {
                        next_off = ctl->low;
                    }
                }
            } else {
                next_off= blk->info[0].offset;
            }
        }
        ret->next_offset = next_off;
        return( true );
    }
    return( false );
}

typedef enum {
    ST_START_LOW,         // look for next lower than cue
    ST_START_HIGH,        // look for next higher than cue
    ST_START_CLOSEST,     // look for lower or equal to cue
    ST_START_FILE,        // look for any cue with file
    ST_WRAP_LOW,          // found wrapped past low
    ST_WRAP_HIGH,         // found wrapped past high
    ST_SQUEEZE_LOW,       // found next lower
    ST_FOUND_CLOSEST,     // found next lower
    ST_SQUEEZE_HIGH,      // found next higher
} line_state;

dfline_find FindCue( cue_list *list, cue_item *item, dfline_search what )
/***********************************************************************/
{
    cue_blk         *blk;
    cue_info        *info;
    unsigned_16     rem;
    addr_seg        seg_last;
    cue_info        *info_last;
    seg_cue         *ctl;
    line_state      state;
    dfline_find     ret;

    state = ST_START_LOW;
    switch( what ) { /* init state */
    case LOOK_LOW:
        state = ST_START_LOW;
        break;
    case LOOK_HIGH:
        state = ST_START_HIGH;
        break;
    case LOOK_CLOSEST:
        state = ST_START_CLOSEST;
        break;
    case LOOK_FILE:
        state = ST_START_FILE;
        break;
    }
    seg_last = 0;
    info_last = NULL;
    for( ctl = list->head; ctl != NULL; ctl = ctl->next ) {
        for( blk = ctl->head; blk != NULL; blk = blk->next ) {
            info = blk->info;
            if( blk->next == NULL ) {   /* last blk */
                rem = ctl->count % CUES_PER_BLK;
                if( rem == 0 ) {
                    rem = CUES_PER_BLK; /* never have empty blocks */
                }
            } else {
                rem = CUES_PER_BLK;
            }
            while( rem > 0 ) {
                if( info->fno == item->fno ) {
                    switch( state ) {
                    case ST_START_FILE:
                        info_last = info;
                        seg_last = ctl->seg;
                        ret = LINE_FOUND;
                        goto found;
                    case ST_START_LOW:
                        info_last = info;
                        seg_last = ctl->seg;
                        if( item->line > info->line ) {
                            state = ST_SQUEEZE_LOW;
                        } else {
                            state = ST_WRAP_LOW;
                        }
                        break;
                    case ST_START_HIGH:
                        info_last = info;
                        seg_last = ctl->seg;
                        if( item->line < info->line ) {
                            state = ST_SQUEEZE_HIGH;
                        } else {
                            state = ST_WRAP_HIGH;
                        }
                        break;
                    case ST_WRAP_LOW:
                        if( item->line  > info->line ) {
                            info_last = info;
                            seg_last = ctl->seg;
                            state = ST_SQUEEZE_LOW;
                        } else if( info_last->line < info->line ) {
                            info_last = info;
                            seg_last = ctl->seg;
                        }
                        break;
                    case ST_WRAP_HIGH:
                        if( item->line < info->line ) {
                            info_last = info;
                            seg_last = ctl->seg;
                            state = ST_SQUEEZE_HIGH;
                        } else if( info_last->line > info->line ) {
                            info_last = info;
                            seg_last = ctl->seg;
                        }
                        break;
                    case ST_SQUEEZE_HIGH:
                        if( item->line < info->line ) {
                            if( info_last->line > info->line ) {
                                info_last = info;
                                seg_last = ctl->seg;
                            }
                        }
                        break;
                    case ST_START_CLOSEST:
                        if( item->line < info->line ) {
                            break; //continue on
                        }
                        info_last = info;
                        seg_last = ctl->seg;
                        state = ST_FOUND_CLOSEST;
                        //drop through
                    case ST_FOUND_CLOSEST:
                        if( item->line == info->line ) {
                            info_last = info;
                            seg_last = ctl->seg;
                            ret = LINE_FOUND;
                            goto found;
                        }// else drop through
                    case ST_SQUEEZE_LOW:
                        if( item->line > info->line ) {
                            if( info_last->line < info->line ) {
                                info_last = info;
                                seg_last = ctl->seg;
                            }
                        }
                        break;
                    }
                }
                ++info;
                --rem;
            }
        }
    }
    ret = LINE_NOT;
    switch( state  ) {
    case ST_START_LOW:
    case ST_START_HIGH:
    case ST_START_CLOSEST:
    case ST_START_FILE:
        ret = LINE_NOT;
        goto not_found;
        break;
    case ST_WRAP_LOW:
    case ST_WRAP_HIGH:
        ret = LINE_WRAPPED;
        break;
    case ST_SQUEEZE_LOW:
    case ST_SQUEEZE_HIGH:
        ret = LINE_FOUND;
        break;
    case ST_FOUND_CLOSEST:
        ret = LINE_CLOSEST;
        break;
    }
found:
    item->mach.offset = info_last->offset;
    item->mach.segment = seg_last;
    item->next_offset = 0;
    item->line = info_last->line;
//  item->col = info_last->col;
    item->fno = info_last->fno;
not_found:
    return( ret );
}

static void FreeSegCue( seg_cue *curr )
/***********************************************/
// Free all offset blocks for a segment
{
    cue_blk    *blk, *next;

    for( blk = curr->head; blk != NULL; blk = next ) {
        next = blk->next;
        DCFree( blk );
    }
}

void  FiniCueInfo( cue_list *list )
/*********************************/
//Free all offset blocks for a line segment
//Free all line segments
{
    seg_cue   *ctl;

    while( (ctl = list->head) != NULL ) {
        list->head = ctl->next;
        FreeSegCue( ctl );
        DCFree( ctl );
    }
}
#ifdef DEBUG
#include <stdarg.h>
void myprintf( char *ctl, ... )
/** my printf ****************/
{
    va_list argument;
    char buff[100];
    long val;
    char *curr;
    char form;

    curr = buff;
    va_start( argument, ctl );
    while( *ctl != '\0' ) {
        if( *ctl == '%' ) {
            ++ctl;
            if( *ctl == 'l' ) {
                ++ctl;
                form = 'l';
            } else {
                form = ' ';
            }
            switch( *ctl ) {
            case 'x':
                if( form == 'l' ) {
                    val = va_arg( argument, long );
                } else {
                    val = va_arg( argument, int );
                }
                ultoa( val, curr, 16 );
                break;
            case 'd':
                if( form == 'l' ) {
                    val = va_arg( argument, long );
                } else {
                    val = va_arg( argument, int );
                }
                ltoa( val, curr, 10 );
                break;
            default:
                curr[0] = '%';
                curr[1] = *ctl;
                curr[2] ='\0';
                break;
            }
            ++ctl;
            while( *curr != '\0' ) {
                ++curr;
            }
        } else {
            *curr = *ctl;
            ++curr;
            ++ctl;
        }
    }
    va_end( argument );
    DCWrite( 1, buff, curr - buff );

}

void DmpCueOffset( cue_list *list )
/*********************************/
{
    cue_blk     *blk;
    cue_info    *info;
    unsigned_16 rem;
    seg_cue     *ctl;

    for( ctl = list->head; ctl != NULL; ctl = ctl->next ) {
        myprintf( "seg %x:%x\n", ctl->seg, ctl->low );
        for( blk = ctl->head; blk != NULL; blk = blk->next ) {
            info = blk->info;
            if( blk->next == NULL ) { /* last blk */
                rem = ctl->count % CUES_PER_BLK;
                if( rem == 0 ) {
                    rem = CUES_PER_BLK; /* never have empty blocks */
                }
            } else {
                rem = CUES_PER_BLK;
            }
            while( rem > 0 ) {
                myprintf( "    %lx f%d.%d\n", info->offset, info->fno, info->line );
                ++info;
                --rem;
            }
        }
    }
}
#endif
