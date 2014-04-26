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
* Description:  RISC oriented string functions designed to access memory
*               as aligned 32- or 64-bit words whenever possible.
*
****************************************************************************/


#ifndef _RISCSTR_H_INCLUDED
#define _RISCSTR_H_INCLUDED


/*
 * Determine if we're building RISC versions of string/memory routines.
 */

#if defined(__AXP__) || defined(__PPC__)
    #define __RISCSTR__
#endif

#ifdef __RISCSTR__              /* do nothing if not RISC target */

#include <wchar.h>
#include "watcom.h"

/*
 * Choose between 32- and 64-bit words.
 */

#define USE_INT64               0       /* no 64-bit stuff for now */

#ifndef USE_INT64
    #ifdef __AXP__
        #define USE_INT64       1
    #else
        #define USE_INT64       0
    #endif
#endif

#if USE_INT64
    #define INT                 __int64
    #define UINT                unsigned __int64
#else
    #define INT                 int
    #define UINT                uint_32
#endif

#define BYTES_PER_WORD          ( sizeof( UINT ) )
#define INT_SIZE                ( BYTES_PER_WORD * 8 )
#define CHARS_PER_WORD          ( BYTES_PER_WORD / CHARSIZE )

#define MOD_BYTES_PER_WORD(__n) ( (__n) & (BYTES_PER_WORD-1) )



/*
 * Macros to mask off a single character.
 */

#if USE_INT64
    #define BYTE1               ( _riscdata->byteMasks[0].val )
    #define BYTE2               ( _riscdata->byteMasks[1].val )
    #define BYTE3               ( _riscdata->byteMasks[2].val )
    #define BYTE4               ( _riscdata->byteMasks[3].val )
    #define BYTE5               ( _riscdata->byteMasks[4].val )
    #define BYTE6               ( _riscdata->byteMasks[5].val )
    #define BYTE7               ( _riscdata->byteMasks[6].val )
    #define BYTE8               ( _riscdata->byteMasks[7].val )
#else
    #define BYTE1               ( 0x000000FF )
    #define BYTE2               ( 0x0000FF00 )
    #define BYTE3               ( 0x00FF0000 )
    #define BYTE4               ( 0xFF000000 )
#endif

#ifdef __WIDECHAR__
    #define CHR1MASK            ( BYTE1 | BYTE2 )
    #define CHR2MASK            ( BYTE3 | BYTE4 )
#else
    #define CHR1MASK            ( BYTE1 )
    #define CHR2MASK            ( BYTE2 )
    #define CHR3MASK            ( BYTE3 )
    #define CHR4MASK            ( BYTE4 )
#endif


#ifdef __WIDECHAR__
    #define CHR1(__w)           ( (__w) & CHR1MASK )
    #define CHR2(__w)           ( (__w) & CHR2MASK )
#else
    #define CHR1(__w)           ( (__w) & BYTE1 )
    #define CHR2(__w)           ( (__w) & BYTE2 )
    #define CHR3(__w)           ( (__w) & BYTE3 )
    #define CHR4(__w)           ( (__w) & BYTE4 )
    #if USE_INT64
        #define CHR5(__w)       ( (__w) & BYTE5 )
        #define CHR6(__w)       ( (__w) & BYTE6 )
        #define CHR7(__w)       ( (__w) & BYTE7 )
        #define CHR8(__w)       ( (__w) & BYTE8 )
    #endif
#endif



/*
 * Macros for extracting the first characters in a word.
 */

#if USE_INT64
    #define FRONT_BYTES(__n)    ( _riscdata->frontCharsMasks[(__n)].val )
    #define FRONT_CHRS(__w,__o) ( (__w) & FRONT_BYTES_riscdata[(__o)].val )
#else
//    extern UINT __FRONT_BYTES[];
//    #define FRONT_BYTES               __FRONT_BYTES
    #define FRONT_BYTES         ( _riscdata->frontCharsMasks )
    #define FRONT_CHRS(__w,__o) ( (__w) & FRONT_BYTES[(__o)] )
#endif



/*
 * Macros for ignoring the first characters in a word.
 */

#if USE_INT64
    #define SKIP_CHRS_MASKS(__n)    ( _riscdata->skipCharsMasks[(__n)].val )
    #define SKIP_CHRS(__w,__o)      ( (__w) & SKIP_CHRS_MASKS(__o) )
#else
//    extern UINT __SKIP_CHRS_MASKS[];
//    #define SKIP_CHRS_MASKS           __SKIP_CHRS_MASKS
    #define SKIP_CHRS_MASKS(__n)    ( _riscdata->skipCharsMasks[(__n)] )
    #define SKIP_CHRS(__w,__o)      ( (__w) & SKIP_CHRS_MASKS(__o) )
#endif



/*
 * Macros for checking if a word contains a null byte.
 */

#if USE_INT64
    #define SUB_M               ( _riscdata->_01Mask.val )
    #define NIL_M               ( _riscdata->_80Mask.val )
    #define SUB_MASK(__n)       ( _riscdata->subMasks[(__n)].val )
#else
    #ifdef __WIDECHAR__
        #define SUB_M           ( 0x00010001 )
        #define NIL_M           ( 0x80008000 )
    #else
        #define SUB_M           ( 0x01010101 )
        #define NIL_M           ( 0x80808080 )
    #endif
//    extern UINT __SubMask[];
//    #define SUB_MASK          __SubMask
    #define SUB_MASK(__n)       ( _riscdata->subMasks[(__n)] )
#endif

#define GOT_NIL(__w)            ( ( (~(__w)) & ((__w) - SUB_M) ) & NIL_M )

#ifdef __WIDECHAR__
    #define OFFSET_GOT_NIL(__w,__o) ( ( (~(__w)) & ((__w) - SUB_MASK((__o)/CHARSIZE)) ) & NIL_M )
#else
    #define OFFSET_GOT_NIL(__w,__o) ( ( (~(__w)) & ((__w) - SUB_MASK(__o)) ) & NIL_M )
#endif



/*
 * Some handy pointer manipulation macros.
 */

#define ROUND(__p)              ( (UINT*) ( (UINT)(__p) & (-sizeof(UINT)) ) )

#define OFFSET(__p)             ( ((unsigned int)(__p)) & (sizeof(UINT)-1) )



/*
 * Macros for uppercase and lowercase stuff.
 */

#ifdef __WIDECHAR__
    #define CHR1_A              ( 0x00000041 )
    #define CHR2_A              ( 0x00410000 )

    #define CHR1_Z              ( 0x0000005A )
    #define CHR2_Z              ( 0x005A0000 )

    #define CHR1_A2a(s)         ( s | 0x00000020 )
    #define CHR2_A2a(s)         ( s | 0x00200000 )
#else
    #define CHR1_A              ( 0x00000041 )
    #define CHR2_A              ( 0x00004100 )
    #define CHR3_A              ( 0x00410000 )
    #define CHR4_A              ( 0x41000000 )

    #define CHR1_Z              ( 0x0000005A )
    #define CHR2_Z              ( 0x00005A00 )
    #define CHR3_Z              ( 0x005A0000 )
    #define CHR4_Z              ( 0x5A000000 )

    #define CHR1_A2a(s)         ( s | 0x00000020 )
    #define CHR2_A2a(s)         ( s | 0x00002000 )
    #define CHR3_A2a(s)         ( s | 0x00200000 )
    #define CHR4_A2a(s)         ( s | 0x20000000 )
#endif

#ifdef __WIDECHAR__
    #define TO_LOW_CHR1(s) ( ( (s>=CHR1_A) && (s<=CHR1_Z) ) ? CHR1_A2a(s) : s )
    #define TO_LOW_CHR2(s) ( ( (s>=CHR2_A) && (s<=CHR2_Z) ) ? CHR2_A2a(s) : s )
#else
    #define TO_LOW_CHR1(s) ( ( (s>=CHR1_A) && (s<=CHR1_Z) ) ? CHR1_A2a(s) : s )
    #define TO_LOW_CHR2(s) ( ( (s>=CHR2_A) && (s<=CHR2_Z) ) ? CHR2_A2a(s) : s )
    #define TO_LOW_CHR3(s) ( ( (s>=CHR3_A) && (s<=CHR3_Z) ) ? CHR3_A2a(s) : s )
    #define TO_LOW_CHR4(s) ( ( (s>=CHR4_A) && (s<=CHR4_Z) ) ? CHR4_A2a(s) : s )
#endif


#ifdef __WIDECHAR__
    #define CHR1_a              ( 0x00000061 )
    #define CHR2_a              ( 0x00610000 )

    #define CHR1_z              ( 0x0000007A )
    #define CHR2_z              ( 0x007A0000 )

    #define CHR1_a2A(s)         ( s & 0x000000DF )
    #define CHR2_a2A(s)         ( s & 0x00DF0000 )
#else
    #define CHR1_a              ( 0x00000061 )
    #define CHR2_a              ( 0x00006100 )
    #define CHR3_a              ( 0x00610000 )
    #define CHR4_a              ( 0x61000000 )

    #define CHR1_z              ( 0x0000007A )
    #define CHR2_z              ( 0x00007A00 )
    #define CHR3_z              ( 0x007A0000 )
    #define CHR4_z              ( 0x7A000000 )

    #define CHR1_a2A(s)         ( s & 0x000000DF )
    #define CHR2_a2A(s)         ( s & 0x0000DF00 )
    #define CHR3_a2A(s)         ( s & 0x00DF0000 )
    #define CHR4_a2A(s)         ( s & 0xDF000000 )
#endif

#ifdef __WIDECHAR__
    #define TO_UPR_CHR1(s) ( ( (s>=CHR1_a) && (s<=CHR1_z) ) ? CHR1_a2A(s) : s )
    #define TO_UPR_CHR2(s) ( ( (s>=CHR2_a) && (s<=CHR2_z) ) ? CHR2_a2A(s) : s )
#else
    #define TO_UPR_CHR1(s) ( ( (s>=CHR1_a) && (s<=CHR1_z) ) ? CHR1_a2A(s) : s )
    #define TO_UPR_CHR2(s) ( ( (s>=CHR2_a) && (s<=CHR2_z) ) ? CHR2_a2A(s) : s )
    #define TO_UPR_CHR3(s) ( ( (s>=CHR3_a) && (s<=CHR3_z) ) ? CHR3_a2A(s) : s )
    #define TO_UPR_CHR4(s) ( ( (s>=CHR4_a) && (s<=CHR4_z) ) ? CHR4_a2A(s) : s )
#endif



/*
 * Tweak characters within a word.
 */

#ifdef __WIDECHAR__
    #define REVERSE_CHARS(__w)  ( (CHR1(__w)<<16) | (CHR2(__w)>>16) )
#else
    #define REVERSE_CHARS(__w)  ( (CHR1(__w)<<24) | (CHR2(__w)<<8) |    \
                                  (CHR3(__w)>>8)  | (CHR4(__w)>>24) )
#endif

#define SWAP_BYTES(__w)         ( (((__w)&BYTE1)<<8) | (((__w)&BYTE2)>>8) | \
                                  (((__w)&BYTE3)<<8) | (((__w)&BYTE4)>>8) )



/*
 * Data used by the RISC string functions.
 */

struct __F_NAME(__RISC_StrData,__wRISC_StrData) {
#if USE_INT64
    unsigned_64         byteMasks[8];
    unsigned_64         frontCharsMasks[8];
    unsigned_64         skipCharsMasks[8];
    unsigned_64         _01Mask;
    unsigned_64         _80Mask;
    unsigned_64         subMasks[8];
#else
    #ifdef __WIDECHAR__
        uint_32         frontCharsMasks[2];
        uint_32         skipCharsMasks[2];
        uint_32         subMasks[2];
    #else
        uint_32         frontCharsMasks[4];
        uint_32         skipCharsMasks[4];
        uint_32         subMasks[4];
    #endif
#endif
};


#ifdef __WIDECHAR__
    extern struct __wRISC_StrData   __wRISC_StringData;
    #define RISC_DATA_LOCALREF  struct __wRISC_StrData *_riscdata = &__wRISC_StringData
#else
    extern struct __RISC_StrData    __RISC_StringData;
    #define RISC_DATA_LOCALREF  struct __RISC_StrData *_riscdata = &__RISC_StringData
#endif



/*
 * Prototype functions called by the RISC-oriented string functions.
 */

#ifdef __WIDECHAR__
_WCRTLINK extern wchar_t *  __simple_wcschr( const wchar_t *str, wint_t ch );
_WCRTLINK extern int        __simple_wcscmp( const wchar_t *s1, const wchar_t *s2 );
_WCRTLINK extern wchar_t *  __simple_wcscpy( wchar_t *dest, const wchar_t *src );
_WCRTLINK extern int        __simple__wcsicmp( const wchar_t *s1, const wchar_t *s2 );
_WCRTLINK extern size_t     __simple_wcslen( const wchar_t *str );
_WCRTLINK extern wchar_t *  __simple__wcslwr( wchar_t *str );
_WCRTLINK extern int        __simple_wcsncmp( const wchar_t *s1, const wchar_t *s2, size_t n );
_WCRTLINK extern wchar_t *  __simple_wcsncpy( wchar_t *dest, const wchar_t *src, size_t n );
_WCRTLINK extern int        __simple__wcsnicmp( const wchar_t *s1, const wchar_t *s2, size_t n );
_WCRTLINK extern wchar_t *  __simple__wcsnset( wchar_t *str, int ch, size_t n );
_WCRTLINK extern wchar_t *  __simple_wcsrchr( const wchar_t *str, wint_t ch );
_WCRTLINK extern wchar_t *  __simple__wcsset( wchar_t *str, wchar_t ch );
_WCRTLINK extern wchar_t *  __simple__wcsupr( wchar_t *str );
#endif


#endif          /* #ifdef __RISCSTR__ */

#endif
