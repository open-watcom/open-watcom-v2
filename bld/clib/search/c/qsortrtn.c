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


/*
 * This file is #included by qsort.c.
 */


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
    swaptype != 0 ? BYTESWAP( a, b, size ) : \
    ( void ) exch( *( WORD* )( a ), *( WORD* )( b ), t )

/*
    Note:   The following assembly was timed against several other methods
    of doing the same thing.  The pragmas here were either fastest on all
    machines tested, or fastest on most machines tested. (including an 8088,
    386 16mhz, 386 33mhz, and 486 25mhz).
*/

#if defined( __386__ )
    /* this is intended for 386 only... */
    #define AUX_INFO \
        parm caller     [esi] [edi] [ecx] \
        value           \
        modify exact    [eax ecx edx edi esi]

    void inline_swap( char *p, char *q, size_t size );
    #pragma aux inline_swap =   \
            "push  es"          \
            "push  ds"          \
            "pop   es"          \
            "movzx edx,cl"      \
            "shr   ecx,2"       \
            "je short L1"       \
        "L2: mov   eax,[edi]"   \
            "xchg  eax,[esi]"   \
            "stosd"             \
            "add   esi,4"       \
            "dec   ecx"         \
            "jne short L2"      \
        "L1: and   dl,3"        \
            "je short L3"       \
        "L4: mov   al,[edi]"    \
            "xchg  al,[esi]"    \
            "stosb"             \
            "inc   esi"         \
            "dec   edx"         \
            "jne short L4"      \
        "L3: pop   es"          \
        AUX_INFO

    #pragma aux byteswap AUX_INFO
    static void _WCNEAR byteswap( char *p, char *q, size_t size )
    {
        inline_swap( p, q, size );
    }

#elif defined( _M_I86 ) && defined( __BIG_DATA__ )
    #define AUX_INFO \
        parm caller     [dx si] [es di] [cx] \
        value           \
        modify exact    [ax cx di si]

    void inline_swap( char _WCFAR *p, char _WCFAR *q, size_t size );
    #pragma aux inline_swap = \
            "push ds" \
            "mov  ds,dx" \
            "shr  cx,1" \
            "je short L1" \
        "L2: mov  ax,es:[di]" \
            "xchg ax,[si]" \
            "stosw" \
            "inc  si" \
            "inc  si" \
            "dec  cx" \
            "jne short L2" \
        "L1: jnc short L3" \
            "mov  al,[si]" \
            "xchg al,es:[di]" \
            "mov  [si],al" \
        "L3: pop  ds" \
        AUX_INFO

    #pragma aux byteswap AUX_INFO
    static void _WCNEAR byteswap( char _WCFAR *p, char _WCFAR *q, size_t size )
    {
        inline_swap( p, q, size );
    }

#elif defined( _M_I86 ) && defined( __SMALL_DATA__ )
    /* we'll ask for char __far *q to save us writing code to load es */
    #define AUX_INFO \
        parm caller     [si] [es di] [cx] \
        value           \
        modify exact    [ax cx di si]

    void inline_swap( char *p, char _WCFAR *q, size_t size );
    #pragma aux inline_swap =   \
            "shr cx,1"          \
            "je short L1"       \
        "L2: mov ax,es:[di]"    \
            "xchg ax,[si]"      \
            "stosw"             \
            "inc si"            \
            "inc si"            \
            "dec cx"            \
            "jne short L2"      \
        "L1: jnc short L3"      \
            "mov al,[si]"       \
            "xchg al,es:[di]"   \
            "mov [si],al"       \
        "L3:"                   \
        AUX_INFO

    #pragma aux byteswap AUX_INFO
    static void _WCNEAR byteswap( char *p, char _WCFAR *q, size_t size )
    {
        inline_swap( p, q, size );
    }

#else
    /* this is an optimized version of a simple byteswap */
    #define inline_swap BYTESWAP
    static void _WCNEAR BYTESWAP( PTRATTR char *p, PTRATTR char *q, size_t size )
    {
        long dword;
        short word;
        char byte;

    #if 1       /* this is for 32 bit machines */
        while( size > 3 ) {
            dword = *(PTRATTR long *)p;
            *(PTRATTR long *)p = *(PTRATTR long *)q;
            *(PTRATTR long *)q = dword;
            p += 4;
            q += 4;
            size -= 4;
        }
        if( size > 1 ) {
            word = *(PTRATTR short *)p;
            *(PTRATTR short *)p = *(PTRATTR short *)q;
            *(PTRATTR short *)q = word;
            p += 2;
            q += 2;
            size -= 2;
        }
    #else       /* this is for 16 bit machines */
        while( size > 1 ) {
            word = *(PTRATTR short *)p;
            *(PTRATTR short *)p = *(PTRATTR short *)q;
            *(PTRATTR short *)q = word;
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


FUNCTION_LINKAGE void FUNCTION_NAME(
    PTRATTR void *in_base,
    size_t n,
    size_t size,
    int (*compar)(const void *, const void *)
)
/**********************************************************************/
{
    PTRATTR char *      base = (PTRATTR char*) in_base;
    PTRATTR char *      p1;
    PTRATTR char *      p2;
    PTRATTR char *      pa;
    PTRATTR char *      pb;
    PTRATTR char *      pc;
    PTRATTR char *      pd;
    PTRATTR char *      pn;
    PTRATTR char *      pv;
    PTRATTR char *      mid;
    WORD                v;              /* used in pivot initialization */
    WORD                t;              /* used in exch() macro */
    int                 comparison, swaptype, shell;
    size_t              count, r, s;
    unsigned int        sp;
    auto char *         base_stack[MAXDEPTH];
    auto unsigned int   n_stack[MAXDEPTH];
    qcomp *             cmp = (qcomp*) compar;

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
                             p2 -= shell ) {
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
                        p1  = MED3( p1, p1 + s, p1 + (s << 1), cmp );
                        mid = MED3( mid - s, mid, mid + s, cmp );
                        p2  = MED3( p2 - (s << 1), p2 - s, p2, cmp );
                    }
                    /* Mid-size (29 < n < 43), med of 3 */
                    mid = MED3( p1, mid, p2, cmp );
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
                count = n;
                /*
                    count keeps track of how many entries we have
                    examined.  Once we have looked at all the entries
                    then we know that the partitioning is complete.
                    We use count to terminate the looping, rather than
                    a pointer comparison, to handle 16bit pointer
                    limitations that may lead pb or pc to wrap.
                    i.e. pc  = 0x0000;
                         pc -= 0x0004;
                         pc == 0xfffc;
                         pc is no longer less that 0x0000;
                */
                for(;;) {
                    while(count && (comparison = cmp(pb, pv)) <= 0) {
                        if( comparison == 0 ) {
                            swap( pa, pb );
                            pa += size;
                        }
                        pb += size;
                        count--;
                    }
                    while(count && (comparison = cmp(pc, pv)) >= 0) {
                        if( comparison == 0 ) {
                            swap( pc, pd );
                            pd -= size;
                        }
                        pc -= size;
                        count--;
                    }
                    if( !count ) break;
                    swap( pb, pc );
                    pb += size;
                    count--;
                    if( !count ) break;
                    pc -= size;
                    count--;
                }
                pn = base + n * size;
                s = min( pa - base, pb - pa );
                if( s > 0 ) {
                    inline_swap( base, pb - s, s );
                }
                s = min( pd - pc, pn - pd - size);
                if( s > 0 ) {
                    inline_swap( pb, pn - s, s );
                }
                /* Now, base to (pb-pa) needs to be sorted             */
                /* Also, pn-(pd-pc) needs to be sorted                 */
                /* The middle 'chunk' contains all elements=pivot value*/
                r = pb - pa;
                s = pd - pc;
                if( s >= r ) {  /* Stack up the larger chunk */
                    base_stack[sp] = pn - s;/* Stack up base       */
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
