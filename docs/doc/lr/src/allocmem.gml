.func _dos_allocmem
.synop begin
#include <&doshdr>
#if defined(__NT__) || \
  ( defined(__OS2__) && \
        (defined(__386__) || defined(__PPC__)) )
unsigned _dos_allocmem( unsigned size,
                        void * *segment);
#else
unsigned _dos_allocmem( unsigned size,
                        unsigned *segment);
#endif
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The &func function uses system call 0x48 to allocate
.arg size
paragraphs directly from DOS.
The size of a paragraph is 16 bytes.
The allocated memory is always paragraph aligned.
The segment descriptor for the allocated memory is returned in the
word pointed to by
.arg segment
.ct .li .
If the allocation request fails, the maximum number of paragraphs that
can be allocated is returned in this word instead.
.np
For 32-bit DOS applications, it is recommended that the corresponding
DPMI services be used.
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_allocmem alloca calloc _dos_freemem _dos_setblock halloc malloc
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
.exmp break
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
        printf( "Only %u paragraphs available\n",
                 segment );
    } else {
        printf( "_dos_allocmem succeeded\n" );
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
