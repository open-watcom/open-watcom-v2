#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "../boxioctl.h"

#define DEV_NAME "\\\\.\\boxsys"

int main( void )
{
    HANDLE      hDriver;
    BOOL        bRc;
    ULONG       ulIDs;
    ULONG       ulRet;

    /* Attempt to open the driver. */
    hDriver = CreateFile( DEV_NAME, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING, 0, NULL );
    if( hDriver == INVALID_HANDLE_VALUE ) {
        printf( "Failed to open device!\n" );
        return( EXIT_FAILURE );
    }
    /* Submit the IOCTL. */
    bRc = DeviceIoControl( hDriver, IOCTL_BOXDEV_GET_IDS, NULL, 0,
                           &ulIDs, sizeof( ulIDs ), &ulRet, NULL );
    if( bRc && (ulRet == sizeof( ulIDs )) ) {
        printf( "PCI Device ID is %04x:%04x\n", ulIDs >> 16, ulIDs & 0xFFFF );
    } else {
        printf( "Device IOCTL failed!\n" );
    }

    CloseHandle( hDriver );
    return( EXIT_SUCCESS );
}
