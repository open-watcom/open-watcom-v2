/*
    MEMWIN.C - This example shows how to get information
    about free memory with DPMI call 0x0500 using Windows
    as a DPMI host.  Note that only the first field of the
    structure is guaranteed to contain a valid value; any
    field that is not returned by the DPMI implementation
    is set to -1 (0FFFFFFFFh).

    Compile & Link: wcl386 /l=win386 /zw memwin
    Bind: wbind -n memwin
 */
#include <windows.h>
#include <i86.h>
#include <dos.h>
#include <stdio.h>

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
    DWORD mi_16;

    regs.w.ax = 0x0500;
    mi_16 = AllocAlias16( &MemInfo );
    sregs.ds = 0;
    sregs.es = HIWORD( mi_16 );
    regs.x.di = LOWORD( mi_16 );

    int86x( DPMI_INT, &regs, &regs, &sregs );
    printf( "Largest available block (in bytes): %ld\n",
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
    FreeAlias16( mi_16 );
}
