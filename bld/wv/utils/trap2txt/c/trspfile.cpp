#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trpcore.h>
#include <madregs.h>
#include <trpfile.h>

#include <windows.h>
#include <winbase.h>    /* For GetSystemTime */

int SSD_Files( int rq, unsigned char * pkt , unsigned short )
{
    if( rq > 0 ) {
        
        unsigned char file_rq = pkt[0];
        
        printf( "    " );
        switch( file_rq ) {
            case REQ_FILE_GET_CONFIG:
                printf( "REQ_FILE_GET_CONFIG\n" );
                break;
            case REQ_FILE_OPEN:
                printf( "REQ_FILE_OPEN\n" );
                break;
            case REQ_FILE_SEEK:
                printf( "REQ_FILE_SEEK\n" );
                break;
            case REQ_FILE_READ:
                printf( "REQ_FILE_READ\n" );
                break;
            case REQ_FILE_WRITE:
                printf( "REQ_FILE_WRITE\n" );
                break;
            case REQ_FILE_WRITE_CONSOLE:
                printf( "REQ_FILE_WRITE_CONSOLE\n" );
                break;
            case REQ_FILE_CLOSE:
                printf( "REQ_FILE_CLOSE\n" );
                break;
            case REQ_FILE_ERASE:
                printf( "REQ_FILE_ERASE\n" );
                break;
            case REQ_FILE_STRING_TO_FULLPATH:
                printf( "REQ_FILE_STRING_TO_FULLPATH\n" );
                break;
            case REQ_FILE_RUN_CMD:
                printf( "REQ_FILE_RUN_CMD\n" );
                break;
            default:
                printf( "unknown REQ_FILE(%u)\n", file_rq );
                break;
        }
       
    } else {
        printf( "reply " );
    }
    
    return -1;
}
