#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

main( int argc, char *argv[] )
{
    struct      find_t  fileinfo;
    unsigned            rc;
    unsigned            sectors;
    unsigned            total_sectors;

    if( argc < 2 ) {
        printf( "Usage: filesize path\n" );
        exit( 1 );
    }
    total_sectors = 0;
    rc = _dos_findfirst( argv[1], _A_NORMAL, &fileinfo );
    while( rc == 0 ) {
        sectors = (fileinfo.size + 511) / 512u;
        printf( "%-14s %10lu %4u\n", fileinfo.name, fileinfo.size, sectors );
        total_sectors += sectors;
        rc = _dos_findnext( &fileinfo );
    }
    printf( "Total number of sectors: %u\n", total_sectors );
    printf( "Total amount of space  : %lu\n", total_sectors * 512ul );
    if( total_sectors < 2847 ) {
        printf( "%u sectors available on 1.44MB diskette\n",
                        2847 - total_sectors );
    } else if( total_sectors > 2847 ) {
        printf( "%u too many sectors to fit onto 1.44MB diskette\n",
                        total_sectors - 2847 );
    }
}
