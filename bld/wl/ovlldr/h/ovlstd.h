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


#include "watcom.h"
#include "tinyio.h"
#include "ovltab.h"
#include "ovldbg.h"

// definitions used in the overlay loader.

#define CALL_INSTRUCTION 0xe8

#define OVLNUM(x) ((x-__OVLTAB__.entries)+1)

#define OVLTAB_OK(x) (FP_OFF(x)<FP_OFF(__OVLTABEND__))
#define OVLVEC_OK(x) (FP_OFF(x)<FP_OFF(__OVLENDVEC__))

#define WALK_ALL_OVL(x) for(x=__OVLTAB__.entries;OVLTAB_OK(x);++x)
#define WALK_ALL_VECT(x) for(x=__OVLSTARTVEC__;OVLVEC_OK(x);++x)

/* these are used in the whoosh overlay loader */
enum {
    FLAG_CHANGED        = 0x8000,
    FLAG_INMEM          = 0x4000,
    FLAG_SELF_REF       = 0x2000,
    FLAG_RET_TRAP       = 0x1000,
    FLAG_DBG_SECT_LOAD  = 0x0800
};

/* messages used by the overlay manager. */

enum {
  OVL_OPEN_ERR,
  OVL_IO_ERR,
  OVL_BAD_VERSION,
  OVL_OUT_OF_MEMORY
#ifdef OVL_DEBUG
  ,OVL_SECTION,
  OVL_LOADED,
  OVL_RESIDENT,
  OVL_UNLOADED,
  OVL_MOVED,
  OVL_RET_TRAPPED
#endif
};

enum {
    OVL_386FLAG         = 0x0001, /* whoosh only: 386 present */
    OVL_DOS3            = 0x0002, /* DOS major version 3 or greater */
    DBGAREA_LOADED      = 0x0004, /* ovldbg whoosh only */
    DBGAREA_VALID       = 0x0008  /*   "      "      "  */
};

#ifdef OVL_SMALL
#define NAME( n )       __S##n##__
#else
#define NAME( n )       __L##n##__
#endif

#ifdef OVL_SMALL
#define vector          svector
#define vector_ptr      svector_ptr
#else
#define vector          lvector
#define vector_ptr      lvector_ptr
#endif

#if defined( OVL_WHOOSH )
#define GNAME( n )       __N##n##__
#else
#define GNAME( n )       NAME( n )
#endif

/*
    Common overlay routines
*/
#pragma aux __OvlExit__ aborts;
extern  void        __near __OvlExit__( unsigned );
extern  tiny_ret_t  __near __OvlSeek__( tiny_handle_t, unsigned long );
extern  tiny_ret_t  __near __OvlRead__( tiny_handle_t, void __far *, unsigned );
extern  void        __near __OvlCodeLoad__( ovltab_entry_ptr, tiny_handle_t );
extern  int         __near __OvlRelocLoad__( ovltab_entry_ptr, tiny_handle_t );
extern  void        __near __OvlNum__( unsigned );
extern  void        __near __OvlMsg__( unsigned );
extern  tiny_ret_t  __near __OpenOvl__( unsigned );
extern  tiny_ret_t  __near __OvlOpen__( const char __far *fname );
extern  void        __far  __CloseOvl__( void );
extern  void        __near __OvlClose__( tiny_handle_t hdl );

/*
    Common overlay data
*/
extern  unsigned        _CODE_BASED __OVLFILEPREV__;
extern  tiny_ret_t      _CODE_BASED __OVLHDLPREV__;
extern  ovl_table       _CODE_BASED __OVLTAB__;
extern  ovltab_entry    _CODE_BASED __OVLTABEND__[];
extern  unsigned        _CODE_BASED __OVLPSP__;
extern  char __far      *_CODE_BASED __OVLMSGS__[];
extern  void __far      *_CODE_BASED __OVLCAUSE__;
extern  char            _CODE_BASED __OVLISRET__;
extern  char            _CODE_BASED __OVLDOPAR__;
extern  vector          _CODE_BASED __OVLSTARTVEC__[];
extern  vector          _CODE_BASED __OVLENDVEC__[];
extern  unsigned        _CODE_BASED __OVLSHARE__;
extern  char            _CODE_BASED __OVLNULLSTR__[];
extern  unsigned_16     _CODE_BASED __OVLFLAGS__;


extern ovl_dbg_hook_func *_CODE_BASED GNAME( DBG_HOOK );
extern ovl_dbg_req_func GNAME( DBG_HANDLER );
extern int  __near GNAME( CheckRetAddr )( void __far * );

/*
    Global symbols used by linker
    They mustn't be mangled
*/
#pragma aux __OVLTAB__ "*";
#pragma aux __OVLTABEND__ "*";
#pragma aux __OVLSTARTVEC__ "*";
#pragma aux __OVLENDVEC__ "*";
#pragma aux __LOVLLDR__ "*";
#pragma aux __LOVLINIT__ "*";
#pragma aux __NOVLLDR__ "*";
#pragma aux __NOVLINIT__ "*";
#pragma aux __SOVLLDR__ "*";
#pragma aux __SOVLINIT__ "*";
