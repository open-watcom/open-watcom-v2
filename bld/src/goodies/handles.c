/*
 *  HANDLES.C
 *  This C program grows the number of file handles so
 *  more than 16 files can be opened. This program
 *  illustrates the interaction between _grow_handles and
 *  the DOS 5.0 file system. If you are running a network
 *  such as Novell's NetWare, this will also affect the
 *  number of available file handles. In the actual trial,
 *  FILES=40 was specified in CONFIG.SYS.
 */

#include <stdio.h>

void main()
{
    int    i, j, maxh, maxo;
    FILE  *temp_files[50];

    for( i = 25; i < 40; i++ ) {
        /*
            count 5 for stdin, stdout, stderr,
            stdaux, and stdprn
        */
        printf( "Trying for %2.2d handles...", 5 + i );
        maxh = _grow_handles( 5 + i );
        maxo = 0;
        for( j = 0; j < maxh; j++ ) {
            temp_files[j] = tmpfile();
            if( temp_files[j] == NULL )break;
            maxo++;
        }
        printf( " %d/%d temp files opened\n", maxo, maxh );
        for( j = 0; j < maxo; j++ ) {
            fclose( temp_files[j] );
        }
    }
}
