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


#include <watcom.h>
#include <tinyio.h>

typedef struct {
    unsigned_16 off;
    unsigned_16 seg;
} dos_addr;

#include "ovltab.h"
#include "ovlvec.h"

// definitions used in the overlay loader.

#pragma aux default "*" modify [];

#define CALL_INSTRUCTION 0xe8

/* this definition used in the old overlay loader only */

#define SUB_ADDR(l,r) ((long)(int)(FP_SEG(l)-FP_SEG(r))*16+(FP_OFF(l)-FP_OFF(r)))

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

/* flags in __OVLFLAGS__ */
extern unsigned_16 far __OVLFLAGS__;
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

/*
    Common overlay routines
*/
#pragma aux __OvlExit__ aborts;
extern  void near __OvlExit__( unsigned int );
extern  tiny_ret_t near __OvlSeek__( tiny_handle_t, unsigned long );
extern  tiny_ret_t near __OvlRead__( tiny_handle_t, void far *, unsigned );
extern  void near __OvlCodeLoad__( ovltab_entry far *, tiny_handle_t );
extern  int near __OvlRelocLoad__( ovltab_entry far *, tiny_handle_t );
extern  void near __OvlNum__( unsigned int );
extern  void near __OvlMsg__( unsigned int );
extern  tiny_ret_t near __OpenOvl__( unsigned int );
extern tiny_ret_t near __OvlOpen__( char far *fname );
extern void near __OvlClose__( tiny_handle_t hdl );

/*
    Common overlay data
*/
extern  unsigned int    far __OVLFILEPREV__;
extern  signed long     far __OVLHDLPREV__;
extern  ovl_table       far __OVLTAB__;
extern  unsigned_16     far __OVLTABEND__;
extern  unsigned int    far __OVLPSP__;
extern  char far *      far __OVLMSGS__[];
extern  void            far * far __OVLCAUSE__;
extern  char            far __OVLISRET__;
extern  char            far __OVLDOPAR__;
extern  vector          far __OVLSTARTVEC__;
extern  vector          far __OVLENDVEC__;
extern  unsigned        far __OVLSHARE__;
extern  char            far __OVLNULLSTR__[];
