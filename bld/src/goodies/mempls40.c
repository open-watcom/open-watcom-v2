/*
    MEMPLS40.C - This is an example of how to get the
    amount of physical memory present under Phar Lap
    386|DOS-Extender v4.0.

    Compile & Link: wcl386 /l=pharlap MEMPLS40
 */
#include <dos.h>
#include <stdio.h>

typedef struct {
    unsigned data[25];
} pharlap_mem_status;

/* Names suggested in Phar Lap documentation */
#define APHYSPG     5
#define SYSPHYSPG   7
#define NFREEPG     21

unsigned long memavail( void )
{
    pharlap_mem_status status;
    union REGS regs;
    unsigned long amount;

    regs.h.ah = 0x25;
    regs.h.al = 0x20;
    regs.h.bl = 0;
    regs.x.edx = (unsigned int) &status;
    intdos( &regs, &regs );
    /* equation is given in description for nfreepg */
    amount = status.data[ APHYSPG ];
    amount += status.data[ SYSPHYSPG ];
    amount += status.data[ NFREEPG ];
    return( amount * 4096 );
}

void main()
{
    printf( "%lu bytes of memory available\n",
            memavail() );
}
