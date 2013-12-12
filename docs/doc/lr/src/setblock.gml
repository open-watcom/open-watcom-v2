.func _dos_setblock
#include <&doshdr>
unsigned _dos_setblock( unsigned size,
                        unsigned segment,
                        unsigned *maxsize );
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The &func function uses system call 0x4A to change the size of
.arg segment
.ct , which was previously allocated by
.kw _dos_allocmem
.ct, to
.arg size
paragraphs.
If the request fails, the maximum number of paragraphs that this
memory block can be changed to is returned in the word pointed to by
.arg maxsize
.ct .li .
.np
For 32-bit DOS applications, it is recommended that the corresponding
DPMI services be used.
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
to
.kw ENOMEM
indicating a bad segment value, insufficient memory or corrupted
memory.
.return end
.see begin
.seelist _dos_setblock _dos_allocmem _dos_freemem realloc
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>

void main( void )
{
#if defined(__NT__) || \
  ( defined(__OS2__) && \
        (defined(__386__) || defined(__PPC__)) )
    void *segment;
#else
    unsigned segment;
#endif
.exmp break
    /* Try to allocate 100 paragraphs, then free them */
    if( _dos_allocmem( 100, &segment ) != 0 ) {
        printf( "_dos_allocmem failed\n" );
        printf( "Only %u paragraphs available\n", segment);
    } else {
        printf( "_dos_allocmem succeeded\n" );
.exmp break
#if defined(__DOS__)
        { unsigned maxsize = 0;
        /* Try to increase it to 200 paragraphs */
        if( _dos_setblock( 200, segment, &maxsize ) != 0 ) {
            printf( "_dos_setblock failed: max=%u, err=%s\n",
                    maxsize, strerror( errno) );
        } else {
            printf( "_dos_setblock succeeded\n" );
        }
        }
#endif
.exmp break
        if( _dos_freemem( segment ) != 0 ) {
            printf( "_dos_freemem failed\n" );
        } else {
            printf( "_dos_freemem succeeded\n" );
        }
    }
}
.exmp end
.class DOS
.system
