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


// WATLIB.H -- definitions for WATCOM Library
//
// 88/12/12 -- J.W.Welch        -- defined
// 89/09/27 -- J.W.Welch        -- added _FARD for optional TSR support
// 90/05/11 -- J.W.Welch        -- added optional definition for
//                                 __SMALL_DATA__ to indicate a small data
//                                 model
// 90/06/29 -- J.W.Welch        -- added ptr_to_FARD macro
// 91/04/18 -- J.W.Welch        -- add BORLAND support

#if defined( __BORLANDC__ )
    #include "bc_defn.h"
#endif

#include <stdlib.h>
#include <stddef.h>

#if defined( __386__ )
  #undef far
  #undef near
  #define far
  #define near
#endif

#define TRUE  1
#define FALSE 0

#if defined(__SMALL__) || defined(__MEDIUM__)
 #define __SMALL_DATA__
#endif

#if defined( TSR )
        #define _FAR        far
        #define _FARD       far
        #define _NR( text ) (void near *)text
        #if defined( __SMALL_DATA__ )
            #define ptr_to_FARD( ptr ) near_to_far( ptr )
        #else
            #define ptr_to_FARD( ptr ) ( ptr )
        #endif
#else
        #define _FAR
        #define _FARD
        #define _NR( text ) text
        #define ptr_to_FARD( ptr ) ( ptr )
#endif

void        BreakDisable(                               // disable breaks
              void );
void        BreakEnable(                                // enable breaks
              void );
void        cr_err_enable(                              // enable crit. errs
              void );
void        cr_err_disable(                             // disable crit. errs
              void );
void        display_messages(                           // display msg.s
              char _FARD * _FARD * msg_vector );        // - vector of msg.s
void        farmove(                                    // MOVE A FAR AREA
              void far *tgt,                            // - target
              const void far *src,                      // - source
              unsigned size );                          // - size
const void far *_fscan_over_black_space(    // SCAN OVER FAR BLACK SPACE
              const void far *src );        // - source ptr
const void far *_fscan_over_white_space(    // SCAN OVER FAR WHITE SPACE
              const void far *src );        // - source ptr
int         file_pattern_match(                         // IS FILE IN PATTERN?
              char _FARD *file,                         // - filename
              char _FARD *file_pattern );               // - file pattern
void       *_get_environ_begin                          // INIT. _get_environ
              ( void );
void        _get_environ_end(                           // FINI. _get_environ
              void  *env_var );                         // - environ pointer
char       *_get_environ_next(                          // GET NEXT ENV. VAR.
              char  *  *curr );                         // - current
int         inside_unsigned(                            // TEST FOR INCLUSION
              unsigned value,                           // - test value
              unsigned b_1,                             // - bound[1]
              unsigned b_2 );                           // - bound[2]
int         isblank(                                    // test if blank vect.
              const char _FARD *vector,                 // - vector
              unsigned vector_size );                   // - vector size
void        _llist_walk(                                // TRAVERSE LINK-LIST
              void _FARD * _FARD *list_hdr,             // - list hdr.
              void (_FAR *walker)(                      // - walking routine
                    void _FARD *element ) );            // - - passed element
int         maximum(                                    // max. function
              int v1,                                   // - arg[1]
              int v2 );                                 // - arg[2]
#if defined( __SMALL_DATA__ )
    void far *memcpy_fn(                                // COPY NEAR TO FAR
        void far *tgt,                                  // - target
        const void *src,                                // - source
        unsigned size );                                // - size of source
    void *memcpy_nf(                                    // COPY FAR TO NEAR
        void *tgt,                                      // - target
        const void far *src,                            // - source
        unsigned size );                                // - size of source
#else
    #define memcpy_fn memcpy
    #define memcpy_nf memcpy
#endif
void        memory_check(                               // CHECK MEMORY
                void );
int         memsamechar(                                // test if filled
              const void _FARD *block,                  // - memory block
              int test_char,                            // - fill character
              unsigned size );                          // - size of block
int         minimum(                                    // min. function
              int v1,                                   // - arg[1]
              int v2 );                                 // - arg[2]
int         msgerr(                                     // DISPLAY ERR MSG
              const char _FARD *error_message );        // - error message
int         msgerr_count(                               // GET # ERRORS
              void );
int         msgerr_list(                                // DISPLAY ERR MSG
              const char _FARD *frag, ... );            // - message fragments
void        msgerr_return(                              // SET RETURN FOR ERR
              int return_code );                        // - return code
int         msgerr_vlist(                               // DISPLAY ERROR MSG.
              const char _FARD * _FARD *frags );        // - fragments list
void        msgbare(                                    // DISPLAY BARE MSG
              const char _FARD *bare_message );         // - error message
void        msgbare_list(                               // DISPLAY BARE MSG
              const char _FARD *frag, ... );            // - message fragments
void        msgbare_vlist(                              // DISPLAY BARE MSG.
              const char _FARD * _FARD *frags );        // - fragments list
void        msgnote(                                    // DISPLAY NOTE MSG
              const char _FARD *error_message );        // - error message
void        msgnote_list(                               // DISPLAY NOTE MSG
              const char _FARD *frag, ... );            // - message fragments
void        msgnote_vlist(                              // DISPLAY NOTE MSG.
              const char _FARD * _FARD *frags );        // - fragments list
void        msg_progname(                               // SET MSG. PGM. NAME
              const char _FARD *name );                 // - program name
void        msgwarn(                                    // DISPLAY WARN MSG
              const char _FARD *error_message );        // - error message
void        msgwarn_list(                               // DISPLAY WARN MSG
              const char _FARD *frag, ... );            // - message fragments
void        msgwarn_vlist(                              // DISPLAY WARN MSG.
              const char _FARD * _FARD *frags );        // - fragments list
void far *  near_to_far(                                // NEAR TO FAR POINTER
              void near *ptr );                         // - the near pointer
char _FARD *pgm_directory(                              // GET LOAD DIRECTORY
              char _FARD *dir );                        // - directory
void        _ring_add(                                  // add element to ring
              void _FARD * _FARD * _FARD *header,       // - addr( ring hdr. )
              void _FARD * _FARD * new_element );       // - element
void _FARD *_FARD * _ring_alloc(                        // allocate ring elem.
              void _FARD * _FARD * _FARD *header,       // - addr( ring hdr. )
              unsigned size );                          // - size of element
unsigned    _ring_count(                                // count elements
              void _FARD * _FARD * header );            // - ring hdr.
void        _ring_delete(                               // dalloc.ring element
              void _FARD * _FARD * _FARD *header,       // - addr( ring hdr. )
              void _FARD * _FARD * old_element );       // - element
void        _ring_empty(                                // empty a ring
              void _FARD * _FARD * _FARD *header );     // - addr( ring hdr. )
void _FARD *_FARD * _ring_ilookup(                      // insensitive lookup
              void _FARD * _FARD * header,              // - ring hdr.
              unsigned str_offset,                      // - offset of string
              const char _FARD * str );                 // - lookup text
void _FARD *_FARD * _ring_ilookup_ref(                  // insensitive lookup
              void _FARD * _FARD * header,              // - ring hdr.
              unsigned str_offset,                      // - offset of string
              const char _FARD * str );                 // - lookup text
void _FARD *_FARD * _ring_index(                        // index into a ring
              void _FARD * _FARD * header,              // - ring hdr.
              unsigned index );                         // - index (base 0)
void        _ring_insert(                               // insert into ring
              void _FARD * _FARD * _FARD *header,       // - addr( ring hdr. )
              void _FARD * _FARD * new_element,         // - new element
              void _FARD * _FARD * insert );            // - insertion point
void        _ring_join(                                 // join two rings
              void _FARD * _FARD * _FARD *base_ring,    // - base ring
              void _FARD * _FARD * to_be_added );       // - ring to be added
void _FARD * _FARD *_ring_lookup(   // LOOKUP IN A RING (CASE-SENSITIVE)
    void _FARD *_FARD *ring,        // - the ring
    unsigned st_off,                // - offset of string in ring
    const char _FARD *search );     // - search string
unsigned    _ring_position(                             // POSITION FOR ELEM.
              void _FARD *_FARD *header,                // - ring header
              void _FARD *_FARD *element );             // - element
void _FARD *_FARD * _ring_predecessor(                  // preceding ring elem
              void _FARD * _FARD * old );               // - current element
void        _ring_remove(                               // remove ring elem.
              void _FARD * _FARD * _FARD *header,       // - addr( ring hdr. )
              void _FARD * _FARD * old_element );       // - element

void        _ring_sort(                                 // SORT A RING
              void _FARD *_FARD *_FARD *hdr,            // - ring hdr
              int (*compare)                            // - compare rtn.
                (void _FARD *elem1,                     // - - op[1]
                void _FARD *elem2 )                     // - - op[2]
                      );
void _FARD *_FARD * _ring_successor(                    // succeeding ring el.
              void _FARD * _FARD * old );               // - current element
void        _ring_walk(                                 // walk a ring
              void _FARD * _FARD * header,              // - ring hdr.
              void (_FAR *walker)(                      // - walk routine
                void _FARD *element ) );                // - - passed element
unsigned char _ring_walk_char(                          // UNTIL-WALK A RING
              void _FARD * _FARD *ring_hdr,             // - ring hdr.
              unsigned char (_FAR *walker)(             // - walking routine
                void _FARD *element ),                  // - - passed element
              unsigned char continue_value );           // - continue value
int         _ring_walk_int(                             // until-walk a ring
              void _FARD * _FARD * header,              // - ring hdr.
              int (_FAR *walker)(                       // - walk routine
                void _FARD *element ),                  // - - passed element
              int continue_value );                     // - continue value
void _FARD *_ring_walk_ptr(                             // until-walk a ring
              void _FARD * _FARD * header,              // - ring hdr.
              void _FARD *(_FAR *walker)(               // - walk routine
                void _FARD *element ),                  // - - passed element
              char _FARD *continue_value );             // - continue value
unsigned    _ring_walk_unsigned(                        // until-walk a ring
              void _FARD * _FARD *ring_hdr,             // - ring hdr.
              unsigned (_FAR *walker)(                  // - walking routine
                void _FARD *element ),                  // - - passed element
                unsigned continue_value );              // - continue value
const char _FARD *scan_over_black_space(                // scan past non-white
              const char _FARD * location );            // - start position
const char _FARD *scan_over_digits(                     // scan past digits
              const char _FARD * location );            // - start position
unsigned scan_over_vector_spaces(                       // SCAN PAST Vct. ' '
              const char _FARD *vector,                 // - vector
              unsigned vsize );                         // - vector size
const char _FARD *scan_over_white_space(                // scan past white
              const char _FARD * location );            // - start position
char _FARD *stxicpy(                                    // concat. integer
              void _FARD * target,                      // - target position
              int value );                              // - integer
char _FARD *stxpcpy(                                    // concat. string
              void _FARD * target,                      // - target position
              const void _FARD * source );              // - string
#if defined( __SMALL_DATA__ )
  char far   *stxpcpyfn(                                // concat. string
              void far * target,                        // - target position
              const void _FARD * source );              // - string
  char _FARD *stxpcpynf(                                // concat. string
              void _FARD * target,                      // - target position
              const void far * source );                // - string
#else
  #define stxpcpyfn stxpcpy                             // - use stxpcpy
  #define stxpcpynf stxpcpy                             // - use stxpcpy
#endif
char _FARD *stxpcpylist(                                 // concat. list str.s
              char _FARD * target,                      // - target position
              const char _FARD * string1,               // - strings, NULL
              ... );
char _FARD *stxpcpyvlist(                                // CONCAT FROM LIST
              char _FARD *target,                       // - target address
              const char _FARD * _FARD * list );        // - points to list
char _FARD *stpfill(                                    // concat. fill char.
              void _FARD * target,                      // - target position
              char fill_char,                           // - fill char.
              unsigned fill_size );                     // - fill size
char _FARD *strchrs(                                    // test occurence
              const void _FARD *source,                 // - test string
              const void _FARD *search );               // - search char.s
#if defined( __SMALL_DATA__ )
  char far   *strcpyfn(                                 // copy string
              void far * target,                        // - target position
              const void _FARD * source );              // - string
  char _FARD *strcpynf(                                 // copy string
              void _FARD * target,                      // - target position
              const void far * source );                // - string
#else
  #define strcpyfn stxpcpy                              // - use stxpcpy
  #define strcpynf stxpcpy                              // - use stxpcpy
#endif
char _FARD *strend(                                     // point at '\0'
              const void _FARD * string );              // - string
#if defined( __SMALL_DATA__ )
    unsigned strlen_f(                                  // FAR-STRING LENGTH
        const void far *string );                       // - the string
#else
    #define strlen_f strlen
#endif
char _FARD *StrToVct(                                   // string -> vector
              char _FARD * string,                      // - location
              unsigned size );                          // - size of vector
char _FARD *StrToVctCopy(                               // copy str. to vect.
              const char _FARD * string,                // - string
              char _FARD * vector,                      // - vector
              unsigned size );                          // - vector size
int         strvect_ilookup(                            // CASE-INSENS. LOOKUP
              const void _FARD * _FARD *strvect,        // - string vector
              const void _FARD *string );               // - search string
char _FARD *stucpy(                                     // concat. unsigned
              void _FARD * target,                      // - target position
              unsigned value );                         // - unsigned value
char _FARD *stxvcpy(                                    // concat. vector
              void _FARD * target,                      // - target position
              const void _FARD * vector,                // - vector
              unsigned max_size );                      // - size
char _FARD *stxcpy(                                     // concat. hex numb
              void _FARD * target,                      // - target position
              unsigned value );                         // - unsigned value
#if defined( __SMALL_DATA__ )
  char *to_near(                                        // MOVE TO NEAR BUFFER
              const char far *string );                 // - far string
#else
  #define to_near
#endif
void _FARD *trim_right_white_space(                     // trim white from rt.
              char _FARD * string );                    // - string
unsigned    uminimum(                                   // RETURN UNSIGNED MINIMUM
              unsigned value1,                          // - value(1)
              unsigned value2 );                        // - value(2)
unsigned    VctCondense(                                // compact a vector
              char _FARD *vector,                       // - vector
              unsigned max_size );                      // - vector size
unsigned    VctSize(                                    // signif. size
              const void _FARD * vector,                // - vector
              unsigned max_size );                      // - vector size
unsigned    VctToStr(                                   // copy vect. to str.
              const void _FARD * vector,                // - vector
              unsigned max_size,                        // - vector size
              char _FARD * string );                    // - string
unsigned    VctToStrCondensed(                          // compact & copy
              const void _FARD * vector,                // - vector
              unsigned max_size,                        // - vector size
              char _FARD * string );                    // - string
unsigned    VctToTrmStr(                                // VECT. TO RT-TRIMMED
              const void _FARD * vector,                // - vector
              unsigned max_size,                        // - vector size
              char _FARD * string );                    // - string
