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


// NOTE!!
//
// Wpack relies on the behaviour of qsort when it sorts entries of
// equal value. So, this function has been stolen from Watcom CLIB so
// that it won't change on us unexpectedly and so that Wpack can be
// ported to other platforms (using other compilers).
//

#ifdef __WATCOMC__
#undef __INLINE_FUNCTIONS__
#endif

#include <stdio.h>
#include <stdlib.h>

/* Support OS/2 16-bit protected mode - will never get stack overflow */
#define MAXDEPTH        (sizeof(long) * 8)

#define SHELL           3       /* Shell constant used in shell sort */

typedef int WORD;
#define W sizeof( WORD )

/*
    swap() is a macro that chooses between an in_line function call and
    an exchange macro.
*/
#define exch( a, b, t)          ( t = a, a = b, b = t )
#define swap( a, b )    \
    swaptype != 0 ? byteswap( a, b, size ) : \
    ( void ) exch( *( WORD* )( a ), *( WORD* )( b ), t )

/*
    Note:   The following assembly was timed against several other methods
    of doing the same thing.  The pragmas here were either fastest on all
    machines tested, or fastest on most machines tested. (including an 8088,
    386 16mhz, 386 33mhz, and 486 25mhz).
*/

#if defined( __WATCOMC__ ) && defined( __386__ )
    /* this is intended for 386 only... */
    void inline_swap( char *p, char *q, size_t size );
    #pragma aux inline_swap = \
        0x06                            /*      push es             */ \
        0x1e                            /*      push ds             */ \
        0x07                            /*      pop  es             */ \
        0x0f 0xb6 0xd1                  /*      movzx   edx,cl      */ \
        0xc1 0xe9 0x02                  /*      shr     ecx,02H     */ \
        0x74 0x0b                       /*      je      L1          */ \
        0x8b 0x07                       /*L2    mov     eax,[edi]   */ \
        0x87 0x06                       /*      xchg    eax,[esi]   */ \
        0xab                            /*      stosd               */ \
        0x83 0xc6 0x04                  /*      add     esi,0004H   */ \
        0x49                            /*      dec     ecx         */ \
        0x75 0xf5                       /*      jne     L2          */ \
        0x80 0xe2 0x03                  /*L1    and     dl,03H      */ \
        0x74 0x09                       /*      je      L3          */ \
        0x8a 0x07                       /*L4    mov     al,[edi]    */ \
        0x86 0x06                       /*      xchg    al,[esi]    */ \
        0xaa                            /*      stosb               */ \
        0x46                            /*      inc     esi         */ \
        0x4a                            /*      dec     edx         */ \
        0x75 0xf7                       /*      jne     L4          */ \
                                        /*L3                        */ \
        0x07                            /*      pop  es             */ \
        parm caller [esi] [edi] [ecx] \
        value \
        modify exact [esi edi ecx eax edx];
    #pragma aux byteswap parm [esi] [edi] [ecx] \
        modify exact [esi edi ecx eax edx];
    static void byteswap( char *p, char *q, size_t size ) {
        inline_swap( p, q, size );
    }

#elif defined( __WATCOMC__ ) && defined( _M_I86 ) && defined( __BIG_DATA__ )
    void inline_swap( char *p, char *q, size_t size );
    #pragma aux inline_swap = \
        0x1e                            /*      push ds             */ \
        0x8e 0xda                       /*      mov ds,dx           */ \
        0xd1 0xe9                       /*      shr cx,1            */ \
        0x74 0x0b                       /*      je L1               */ \
        0x26 0x8b 0x05                  /*L2    mov ax,es:[di]      */ \
        0x87 0x04                       /*      xchg ax,[si]        */ \
        0xab                            /*      stosw               */ \
        0x46                            /*      inc si              */ \
        0x46                            /*      inc si              */ \
        0x49                            /*      dec cx              */ \
        0x75 0xf5                       /*      jne L2              */ \
        0x73 0x07                       /*L1    jnc L3              */ \
        0x8a 0x04                       /*      mov al,[si]         */ \
        0x26 0x86 0x05                  /*      xchg al,es:[di]     */ \
        0x88 0x04                       /*      mov [si],al         */ \
        0x1f                            /*L3    pop ds              */ \
        parm caller [dx si] [es di] [cx] \
        value \
        modify exact [si di cx ax];
    #pragma aux byteswap parm [dx si] [es di] [cx] modify exact [si di cx ax];
    static void byteswap( char *p, char *q, size_t size ) {
        inline_swap( p, q, size );
    }

#elif defined( __WATCOMC__ ) && defined( _M_I86 ) && defined( __SMALL_DATA__ )
    /* we'll ask for char __far *q to save us writing code to load es */
    void inline_swap( char *p, char *q, size_t size );
    #pragma aux inline_swap = \
        0xd1 0xe9                       /*      shr cx,1            */ \
        0x74 0x0b                       /*      je L1               */ \
        0x26 0x8b 0x05                  /*L2    mov ax,es:[di]      */ \
        0x87 0x04                       /*      xchg ax,[si]        */ \
        0xab                            /*      stosw               */ \
        0x46                            /*      inc si              */ \
        0x46                            /*      inc si              */ \
        0x49                            /*      dec cx              */ \
        0x75 0xf5                       /*      jne L2              */ \
        0x73 0x07                       /*L1    jnc L3              */ \
        0x8a 0x04                       /*      mov al,[si]         */ \
        0x26 0x86 0x05                  /*      xchg al,es:[di]     */ \
        0x88 0x04                       /*      mov [si],al         */ \
                                        /*L3                        */ \
        parm caller [si] [es di] [cx] \
        value \
        modify exact [si di cx ax];
    #pragma aux byteswap parm [si] [es di] [cx] modify exact [si di cx ax];
    static void byteswap( char *p, char *q, size_t size ) {
        inline_swap( p, q, size );
    }

#else
    /* this is an optimized version of a simple byteswap */
    #define inline_swap byteswap
    static void byteswap( char *p, char *q, size_t size ) {
        long dword;
        short word;
        char byte;

        #if 1       /* this is for 32 bit machines */
            while( size > 3 ) {
                dword = *(long *)p;
                *(long *)p = *(long *)q;
                *(long *)q = dword;
                p += 4;
                q += 4;
                size -= 4;
            }
            if( size > 1 ) {
                word = *(short *)p;
                *(short *)p = *(short *)q;
                *(short *)q = word;
                p += 2;
                q += 2;
                size -= 2;
            }
        #else       /* this is for 16 bit machines */
            while( size > 1 ) {
                word = *(short *)p;
                *(short *)p = *(short *)q;
                *(short *)q = word;
                p += 2;
                q += 2;
                size -= 2;
            }
        #endif
        if( size ) {
            byte = *p;
            *p = *q;
            *q = byte;
        }
    }
#endif

typedef int qcomp( char *, char * );
#if defined( __WATCOMC__ ) && defined(_M_IX86)
    // this is only needed when being built for the C library
    //#pragma aux (__outside_CLIB) qcomp;
#endif


/* Function to find the median value */
static char *med3( char *a, char *b, char *c, qcomp cmp )
{
    if( cmp( a, b ) > 0 ) {
        if( cmp( a, c ) > 0 ) {
            if( cmp( b, c ) > 0 ) {
                return( b );
            } else {
                return( c );
            }
        } else {
            return( a );
        }
    } else {
        if( cmp( a, c ) >= 0 ) {
            return( a );
        } else {
            if( cmp( b, c ) > 0 ) {
                return( c );
            } else {
                return( b );
            }
        }
    }
}

void wpack_qsort( char *base, size_t n, size_t size,
                  int (*compar)(char *, char *) )
/***********************************************/
{
    char *p1, *p2, *pa, *pb, *pc, *pd, *pn, *pv;
    char *mid;
    WORD v, t; /* v is used in pivot initialization, t in exch() macro */
    int  comparison, swaptype, shell;
    size_t r, s;
    unsigned int sp;
    auto char *base_stack[MAXDEPTH];
    auto unsigned int n_stack[MAXDEPTH];
    qcomp *cmp = compar;

    /*
        Initialization of the swaptype variable, which determines which
        type of swapping should be performed when swap() is called.
        0 for single-word swaps, 1 for general swapping by words, and
        2 for swapping by bytes.  W (it's a macro) = sizeof(WORD).
    */
    swaptype = ( ( base - (char *)0 ) | size ) % W ? 2 : size > W ? 1 : 0;
    sp = 0;
    for(;;) {
        while( n > 1 ) {
            if( n < 16 ) {      /* 2-shell sort on smallest arrays */
                for( shell = (size * SHELL) ;
                     shell > 0 ;
                     shell -= ((SHELL-1) * size) ) {
                    p1 = base + shell;
                    for( ; p1 < base + n * size; p1 += shell ) {
                        for( p2 = p1;
                             p2 > base && cmp( p2 - shell, p2 ) > 0;
                             p2 = p2 - shell ) {
                            swap( p2, p2 - shell );
                        }
                    }
                }
                break;
            } else {    /* n >= 16 */
                /* Small array (15 < n < 30), mid element */
                mid = base + (n >> 1) * size;
                if( n > 29 ) {
                    p1 = base;
                    p2 = base + ( n - 1 ) * size;
                    if( n > 42 ) {      /* Big array, pseudomedian of 9 */
                        s = (n >> 3) * size;
                        p1  = med3( p1, p1 + s, p1 + (s << 1), cmp );
                        mid = med3( mid - s, mid, mid + s, cmp );
                        p2  = med3( p2 - (s << 1), p2 - s, p2, cmp );
                    }
                    /* Mid-size (29 < n < 43), med of 3 */
                    mid = med3( p1, mid, p2, cmp );
                }
                /*
                    The following sets up the pivot (pv) for partitioning.
                    It's better to store the pivot value out of line
                    instead of swapping it to base. However, it's
                    inconvenient in C unless the element size is fixed.
                    So, only the important special case of word-size
                    objects has utilized it.
                */
                if( swaptype != 0 ) { /* Not word-size objects */
                    pv = base;
                    swap( pv, mid );
                } else {        /* Storing the pivot out of line (at v) */
                    pv = ( char* )&v;
                    v = *( WORD* )mid;
                }

                pa = pb = base;
                pc = pd = base + ( n - 1 ) * size;
                for(;;) {
                    while(pb <= pc && (comparison = cmp(pb, pv)) <= 0) {
                        if( comparison == 0 ) {
                            swap( pa, pb );
                            pa += size;
                        }
                        pb += size;
                    }
                    while(pc >= pb && (comparison = cmp(pc, pv)) >= 0) {
                        if( comparison == 0 ) {
                            swap( pc, pd );
                            pd = pd - size;
                        }
                        pc = pc - size;
                    }
                    if( pb > pc ) break;
                    swap( pb, pc );
                    pb += size;
                    pc = pc - size;
                }
                pn = base + n * size;
                s = pa - base;
                if( s > pb - pa )
                    s = pb - pa;
                if( s > 0 ) {
                    inline_swap( base, pb - s, s );
                }
                s = pd - pc;
                if( s > pn - pd - size )
                    s = pn - pd - size;
                if( s > 0 ) {
                    inline_swap( pb, pn - s, s );
                }
                /* Now, base to (pb-pa) needs to be sorted             */
                /* Also, pn-(pd-pc) needs to be sorted                 */
                /* The middle 'chunk' contains all elements=pivot value*/
                r = pb - pa;
                s = pd - pc;
                if( s >= r ) {  /* Stack up the larger chunk */
                    base_stack[sp] = pn - s;    /* Stack up base           */
                    n_stack[sp] = s / size;     /* Stack up n              */
                    n = r / size;               /* Set up n for next 'call'*/
                                                /* next base is still base */
                } else {
                    if( r <= size ) break;
                    base_stack[sp] = base;      /* Stack up base           */
                    n_stack[sp] = r / size;     /* Stack up n              */
                    base = pn - s;              /* Set up base and n for   */
                    n = s / size;               /* next 'call'             */
                }
                ++sp;
            }
        }
        if( sp == 0 ) break;
        --sp;
        base = base_stack[sp];
        n    = n_stack[sp];
    }
}
