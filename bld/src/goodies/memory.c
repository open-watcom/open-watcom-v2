/*
    MEMORY.C - This example shows how to get information
    about free memory using DPMI call 0500h under DOS/4GW.
    Note that only the first field of the structure is
    guaranteed to contain a valid value; any field that
    is not returned by DOS/4GW is set to -1 (0FFFFFFFFh).

    Compile & Link: wcl386 /l=dos4g memory
 */
#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

struct meminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} MemInfo;

#define DPMI_INT        0x31

void main()
{
    union REGS regs;
    struct SREGS sregs;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof(sregs) );
    sregs.es = FP_SEG( &MemInfo );
    regs.x.edi = FP_OFF( &MemInfo );

    int386x( DPMI_INT, &regs, &regs, &sregs );
    printf( "Largest available block (in bytes): %lu\n",
            MemInfo.LargestBlockAvail );
    printf( "Maximum unlocked page allocation: %lu\n",
            MemInfo.MaxUnlockedPage );
    printf( "Pages that can be allocated and locked: %lu\n",
            MemInfo.LargestLockablePage );
    printf( "Total linear address space including allocated"
            " pages: %lu\n", MemInfo.LinAddrSpace );
    printf( "Number of free pages available: %lu\n",
             MemInfo.NumFreePagesAvail );
    printf( "Number of physical pages not in use: %lu\n",
             MemInfo.NumPhysicalPagesFree );
    printf( "Total physical pages managed by host: %lu\n",
             MemInfo.TotalPhysicalPages );
    printf( "Free linear address space (pages): %lu\n",
             MemInfo.FreeLinAddrSpace );
    printf( "Size of paging/file partition (pages): %lu\n",
             MemInfo.SizeOfPageFile );
}
