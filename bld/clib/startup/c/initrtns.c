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
* Description:  Initialization and termination of clib.
*
****************************************************************************/


#include "variety.h"
#include "initfini.h"
#include "rtinit.h"
#include "getds.h"


#define PNEAR ((__type_rtp)0)
#define PFAR  ((__type_rtp)1)
#define PDONE ((__type_rtp)2)

#if ( COMP_CFG_COFF == 1 ) || defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    // following is an attempt to drop the need for an assembler
    // segment definitions file
    // unfortunately, the use of XIB,XIE,YIB,YIE doesn't get the
    // right sort of segments by default
    #pragma data_seg( ".rtl$xib", "DATA" );
    YIXI( TS_SEG_XIB, _Start_XI, 0, 0 )
    #pragma data_seg( ".rtl$xie", "DATA" );
    YIXI( TS_SEG_XIE, _End_XI, 0, 0 )
    #pragma data_seg( ".rtl$yib", "DATA" );
    YIXI( TS_SEG_YIB, _Start_YI, 0, 0 )
    #pragma data_seg( ".rtl$yie", "DATA" );
    YIXI( TS_SEG_YIE, _End_YI, 0, 0 )
    #pragma data_seg( ".data", "DATA" );
#elif defined(_M_IX86)
    extern struct rt_init _Start_XI;
    extern struct rt_init _End_XI;

    extern struct rt_init _Start_YI;
    extern struct rt_init _End_YI;
#else
    #error unsupported platform
#endif

typedef void (*pfn)(void);
typedef void (_WCI86FAR * _WCI86FAR fpfn)(void);
typedef void (_WCI86NEAR * _WCI86NEAR npfn)(void);

#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    #define __GETDS()
    #define save_ds()
    #define restore_ds()
    #define save_es()
    #define restore_es()
    #define setup_es()
#elif defined(__WINDOWS_386__)
    #define __GETDS()
    #define save_ds()
    #define restore_ds()
    #define save_es()
    #define restore_es()
    #define setup_es()
#elif defined(__386__)
    #define __GETDS()
    #define save_ds()
    #define restore_ds()
  #if defined(__FLAT__)
    #define save_es()
    #define restore_es()
    #define setup_es()
  #else
    extern void save_es( void );
    #pragma aux save_es = modify exact [es];
    extern void restore_es( void );
    #pragma aux restore_es = modify exact [es];
    extern void setup_es( void );
    #pragma aux setup_es = \
                "push ds" \
                "pop es" \
                modify exact [es];
  #endif
#elif defined( _M_I86 )
    extern void save_dx( void );
    #pragma aux save_dx = modify exact [dx];
    extern void save_ds( void );
    #pragma aux save_ds = "push ds" modify exact [sp];
    extern void restore_ds( void );
    #pragma aux restore_ds = "pop ds" modify exact [sp];
    #define save_es()
    #define restore_es()
#else
    #error unsupported platform
#endif

#if defined( _M_I86 )
static void callit_near( npfn *f ) {
    // don't call a null pointer
    if( *f ) {
        save_dx();
        save_ds();
        // call function
        (void)(**f)();
        restore_ds();
    }
}

static void callit_far( fpfn _WCI86NEAR *f ) {
    // don't call a null pointer
    if( *f ) {
        save_ds();
        // call function
        (void)(**f)();
        restore_ds();
    }
}
#else
static void callit( pfn *f ) {
    // don't call a null pointer
    if( *f ) {
        // QNX needs es==ds
        setup_es();
        // call function
        (void)(**f)();
    }
}
#endif

/*
; - takes priority limit parm in eax, code will run init routines whose
;       priority is < eax (really al [0-255])
;       eax==255 -> run all init routines
;       eax==15  -> run init routines whose priority is <= 15
;
*/
#if defined( _M_I86 )
void _WCI86FAR __FInitRtns( unsigned limit ) {
    __InitRtns( limit );
}
#endif
void __InitRtns( unsigned limit ) {
    __type_rtp local_limit;
    struct rt_init _WCI86NEAR *pnext;
    save_ds();
    save_es();
    __GETDS();

    local_limit = (__type_rtp)limit;
    for(;;) {
        {
            __type_rtp working_limit;
            struct rt_init _WCI86NEAR *pcur;

            pcur = (struct rt_init _WCI86NEAR*)&_Start_XI;
            #if defined(COMP_CFG_COFF)
                pcur++;
            #endif
            pnext = (struct rt_init _WCI86NEAR*)&_End_XI;
            working_limit = local_limit;

            // walk list of routines
            while( pcur < (struct rt_init _WCI86NEAR*)&_End_XI ) {
                // if this one hasn't been called
                if( pcur->rtn_type != PDONE ) {
                    // if the priority is better than best so far
                    if( pcur->priority <= working_limit ) {
                        // remember this one
                        pnext = pcur;
                        working_limit = pcur->priority;
                    }
                }
                // advance to next entry
                pcur++;
            }
            // check to see if all done, if we didn't find any
            // candidates then we can return
            if( pnext == (struct rt_init _WCI86NEAR*)&_End_XI ) {
                break;
            }
        }
#if defined( _M_I86 )
        if( pnext->rtn_type == PNEAR ) {
            callit_near( (npfn *)&pnext->rtn );
        } else {
            callit_far( (fpfn _WCI86NEAR *)&pnext->rtn );
        }
#else
        callit( &pnext->rtn );
#endif
        // mark entry as invoked
        pnext->rtn_type = PDONE;
    }
    restore_es();
    restore_ds();
}

/*
; - takes priority range parms in eax, edx, code will run fini routines whose
;       priority is >= eax (really al [0-255]) and <= edx (really dl [0-255])
;       eax==0,  edx=255 -> run all fini routines
;       eax==16, edx=255 -> run fini routines in range 16..255
;       eax==16, edx=40  -> run fini routines in range 16..40
*/
#if defined( _M_I86 )
void _WCI86FAR __FFiniRtns( unsigned min_limit, unsigned max_limit ) {
    __FiniRtns( min_limit, max_limit );
}
#endif
void __FiniRtns( unsigned min_limit, unsigned max_limit ) {
    __type_rtp local_min_limit;
    __type_rtp local_max_limit;
    struct rt_init _WCI86NEAR *pnext;
    save_ds();
    save_es();
    __GETDS();

    local_min_limit = (__type_rtp)min_limit;
    local_max_limit = (__type_rtp)max_limit;
    for(;;) {
        {
            __type_rtp working_limit;
            struct rt_init _WCI86NEAR *pcur;

            pcur = (struct rt_init _WCI86NEAR*)&_Start_YI;
            #if defined(COMP_CFG_COFF)
                pcur++;
            #endif
            pnext = (struct rt_init _WCI86NEAR*)&_End_YI;
            working_limit = local_min_limit;

            // walk list of routines
            while( pcur < (struct rt_init _WCI86NEAR*)&_End_YI ) {
                // if this one hasn't been called
                if( pcur->rtn_type != PDONE ) {
                    // if the priority is better than best so far
                    if( pcur->priority >= working_limit ) {
                        // remember this one
                        pnext = pcur;
                        working_limit = pcur->priority;
                    }
                }
                // advance to next entry
                pcur++;
            }
            // check to see if all done, if we didn't find any
            // candidates then we can return
            if( pnext == (struct rt_init _WCI86NEAR*)&_End_YI ) {
                break;
            }
        }
        if( pnext->priority <= local_max_limit ) {
#if defined( _M_I86 )
            if( pnext->rtn_type == PNEAR ) {
                callit_near( (npfn *)&pnext->rtn );
            } else {
                callit_far( (fpfn _WCI86NEAR *)&pnext->rtn );
            }
#else
            callit( &pnext->rtn );
#endif
        }
        // mark entry as invoked even if we don't call it
        // if we didn't call it, it is because we don't want to
        // call finirtns with priority > max_limit, in that case
        // marking the function as called, won't hurt anything
        pnext->rtn_type = PDONE;
    }
    restore_es();
    restore_ds();
}
