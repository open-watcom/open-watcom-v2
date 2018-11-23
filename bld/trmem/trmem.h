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
* Description:  Interface to the trmem memory allocation tracker and
*               validator.
*
****************************************************************************/


#ifndef _TRMEM_H_INCLUDED
#define _TRMEM_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined( _WIN64 )
typedef unsigned __int64    memsize;
#else
typedef unsigned long       memsize;
#endif

typedef struct _trmem_internal  *_trmem_hdl;

typedef void (*_trmem_who)( void );  /* generic pointer to code */
#define _TRMEM_NO_ROUTINE   ((_trmem_who)0)

/* generic pointer to code with realloc signature */
typedef void *(*_trmem_realloc_who)(void *, size_t);
#define _TRMEM_NO_REALLOC   ((_trmem_realloc_who)0)

/*
    These are some special conditions that trmem can detect.  OR together
    the ones you're interested in and pass them to _trmem_open in the __flags
    parameter.
*/
enum {
    _TRMEM_ALLOC_SIZE_0     =0x0001,/* attempted alloc of size 0 */
    _TRMEM_REALLOC_SIZE_0   =0x0002,/* attempted realloc/expand of size 0 */
    _TRMEM_REALLOC_NULL     =0x0004,/* attempted realloc/expand of a NULL ptr */
    _TRMEM_FREE_NULL        =0x0008,/* attempted free of a NULL pointer */
    _TRMEM_OUT_OF_MEMORY    =0x0010,/* warn if trmem can't allocate memory
                                        for its own purposes */
    _TRMEM_CLOSE_CHECK_FREE =0x0020 /* _trmem_close checks if all chunks
                                        were freed */
};

/*
    _trmem_open:

    __alloc must be supplied and behave like malloc() when passed a size of 0.

    __free must be supplied and behave like free() when passed a NULL ptr.

    __realloc may be omitted (use _TRMEM_NO_REALLOC).  __realloc must behave
        like realloc() when passed a NULL pointer or a size of 0.

    __expand may be omitted (use _TRMEM_NO_REALLOC).  __expand must behave
        like _expand() when passed a NULL pointer or a size of 0.

    __prt_parm is passed to __prt_line only.

    __prt_line must be supplied.  It is called to output any messages trmem
        needs to communicate.  __buf is a null-terminated string of length
        __len (including a trailing '\n').

    __flags see enum above for more information.

    trmem uses __alloc and __free for its own internal structures.  None of
    the internal structures will appear in the memory statistics given by
    _trmem_prt_usage or _trmem_prt_list.

    The handle returned uniquely identifies the tracker.  Multiple trackers
    may be used simultaneously.

    A NULL return indicates failure, for one of any reason.

    _trmem_open can/will use any of __alloc, __free, or __prt_line; so be
    sure they are initialized before calling _trmem_open.
*/
extern _trmem_hdl _trmem_open(
    void *(*__alloc)(size_t),
    void (*__free)(void*),
    void * (*__realloc)(void*,size_t),
    void * (*__expand)(void*,size_t),
    void *__prt_parm,
    void (*__prt_line)(void *__prt_parm, const char *__buf, size_t __len),
    unsigned __flags
);


/*
    If ( __flags & _TRMEM_CLOSE_CHECK_FREE ) then _trmem_close checks if all
    allocated chunks were freed before closing the handle.
    Returns number of unfreed chunks.
*/
extern unsigned _trmem_close( _trmem_hdl );


/*
    Replace calls such as
        ptr = malloc( size );
    with
        ptr = _trmem_alloc( size, _trmem_guess_who(), hdl );
*/
extern void *_trmem_alloc( size_t, _trmem_who, _trmem_hdl );
extern void _trmem_free( void *, _trmem_who, _trmem_hdl );
extern void *_trmem_realloc( void *, size_t, _trmem_who, _trmem_hdl );
extern void *_trmem_expand( void *, size_t, _trmem_who, _trmem_hdl );
extern char *_trmem_strdup( const char *str, _trmem_who who, _trmem_hdl hdl );
extern size_t _trmem_msize( void *, _trmem_hdl );


/*
    _trmem_prt_usage prints the current memory usage, and peak usage.
    _trmem_prt_list prints a list of all currently allocated chunks.
*/
extern void _trmem_prt_usage( _trmem_hdl );
extern unsigned _trmem_prt_list( _trmem_hdl );

/*
    _trmem_get_current_usage retrieves the current memory usage.
    _trmem_get_peak_usage retrieves the peak memory usage.
*/
extern memsize _trmem_get_current_usage( _trmem_hdl );
extern memsize _trmem_get_peak_usage( _trmem_hdl );

/*
    _trmem_set_min_alloc sets a minimum allocation size.  If an allocation is
    done which is smaller than this minimum, trmem will print a warning.
*/
extern void _trmem_set_min_alloc( size_t, _trmem_hdl );

/*
    _trmem_validate does some consitancy checks on an allocated chunk.
    _trmem_chk_range ensures that the __len memory locations beginning at
        __start are inside the boundaries of an allocated chunk.
*/
extern int _trmem_validate( void *__ptr, _trmem_who, _trmem_hdl );
extern int _trmem_validate_all( _trmem_hdl );
extern int _trmem_chk_range( void *__start, size_t __len, _trmem_who, _trmem_hdl );

/*
    _trmem_prt_use_seg_num changes whether the memory tracker (for windows
    programs) prints selector and offset, or segment number and offset for
    code pointers. Pass TRUE to set too segment number and offset; the old value
    is returned. The default is FALSE.
    NOTE: the memory tracker uses toolhelp to lookup the segment number.
*/
#ifdef __WINDOWS__
extern int _trmem_prt_use_seg_num( _trmem_hdl, int use_set_num );
#endif


/*
    !!!! WARNING !!!! WARNING !!!!

    _trmem_guess_who is a pragma that determines the caller of the current
    function.  The pragma makes the following assumptions:

        The current function was compiled with the /of option.
        The current function does not have a __near or __far keyword.

    You may not want your entire application compiled with /of; be careful.
    (i.e., /of generates fatter prologs than are necessary for most apps.)

    Actually, _trmem_guess_who doesn't really have to "guess" now that /of
    exists.

    _trmem_whoami returns the CS:eIP of an address within itself.  There are
    no restrictions on a module using _trmem_whoami (i.e., no need for /of).
*/
extern _trmem_who  _trmem_guess_who( void );
extern _trmem_who  _trmem_whoami( void );

#ifdef __WATCOMC__
#if defined( _M_I86SM ) || defined( _M_I86CM )
    #pragma aux _trmem_guess_who = \
        "mov ax,[bp+2]" \
        parm caller [] value [ax] \
        modify exact [ax];

    #pragma aux _trmem_whoami = \
        "call near ptr L1" \
    "L1: pop  ax" \
        parm caller [] value [ax] \
        modify exact [ax];

#elif defined( _M_I86LM ) || defined( _M_I86MM ) || defined( _M_I86HM )
    #pragma aux _trmem_guess_who = \
        "mov dx,[bp+4]" \
        "mov ax,[bp+2]" \
        parm caller [] value [dx ax] \
        modify exact [dx ax];

    #pragma aux _trmem_whoami = \
        "call near ptr L1" \
    "L1: pop  ax" \
        "mov  dx,cs" \
        parm caller [] value [dx ax] \
        modify exact [dx ax];

#elif defined( _M_IX86 )
    #pragma aux _trmem_guess_who = \
        "mov eax,[ebp+4]" \
        parm caller [] value [eax] \
        modify exact [eax];

    #pragma aux _trmem_whoami = \
        "call near ptr L1" \
    "L1: pop  eax" \
        parm caller [] value [eax] \
        modify exact [eax];

#endif
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
