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


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------- Global variables ----------------------------*/

DWORD   StartType = SERVICE_AUTO_START;
DWORD   ErrorControl = SERVICE_ERROR_NORMAL;
char    DependencyList[2048];
BOOL    Quiet;

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
REMARKS:
Installs the driver
****************************************************************************/
static BOOL InstallDriver(
        IN SC_HANDLE  SchSCManager,
        IN LPCTSTR    DriverName,
        IN LPCTSTR    ServiceExe)
{
    SC_HANDLE  schService;
    DWORD      err;


        // NOTE: This creates an entry for a standalone driver. If this
        //       is modified for use with a driver that requires a Tag,
        //       Group, and/or Dependencies, it may be necessary to
        //       query the registry for existing driver information
        //       (in order to determine a unique Tag, etc.).
        schService = CreateService (SchSCManager,               // SCManager database
                                                                DriverName,             // name of service
                                                                DriverName,             // name to display
                                                                SERVICE_ALL_ACCESS,     // desired access
                                                                SERVICE_KERNEL_DRIVER,  // service type
                                                                StartType,              // start type
                                                                ErrorControl,           // error control type
                                                                ServiceExe,             // service's binary
                                                                NULL,                   // no load ordering group
                                                                NULL,                   // no tag identifier
                                                                DependencyList[0] == '\0' ?
                                                                                NULL : DependencyList,
                                                                NULL,                   // LocalSystem account
                                                                NULL);                  // no password
        if (schService == NULL) {
                err = GetLastError();
                if (err == ERROR_SERVICE_EXISTS)
                        // A common cause of failure (easier to read than an error code)
                        fprintf (stderr, "failure: CreateService, ERROR_SERVICE_EXISTS\n");
                else
                        fprintf ( stderr, "failure: CreateService (0x%02x)\n", err);
                return FALSE;
                }
        else if(!Quiet)
                printf ("CreateService SUCCESS\n");
        CloseServiceHandle (schService);
        return TRUE;
}

/****************************************************************************
REMARKS:
Removes the driver
****************************************************************************/
static BOOL RemoveDriver(
        IN SC_HANDLE  SchSCManager,
        IN LPCTSTR    DriverName)
{
    SC_HANDLE  schService;
    BOOL       ret;

    schService = OpenService (SchSCManager,
                              DriverName,
                                                          SERVICE_ALL_ACCESS);
        if (schService == NULL) {
                fprintf ( stderr, "failure: OpenService (0x%02x)\n", GetLastError());
                return FALSE;
                }
        ret = DeleteService (schService);
        if (ret == 0)
                printf ("failure: DeleteService (0x%02x)\n", GetLastError());
        else if(!Quiet)
                printf ("DeleteService SUCCESS\n");
        CloseServiceHandle (schService);
        return ret;
}

/****************************************************************************
REMARKS:
Starts the driver
****************************************************************************/
static BOOL StartDriver(
        IN SC_HANDLE  SchSCManager,
        IN LPCTSTR    DriverName)
{
    SC_HANDLE  schService;
    BOOL       ret;
    DWORD      err;

    schService = OpenService (SchSCManager,
                              DriverName,
                                                          SERVICE_ALL_ACCESS);
        if (schService == NULL) {
                fprintf ( stderr, "failure: OpenService (0x%02x)\n", GetLastError());
                return FALSE;
                }
        ret = StartService (schService,    // service identifier
                                                0,             // number of arguments
                                                NULL           // pointer to arguments
                                                );
        if (ret == 0) {
                err = GetLastError();
                if (err == ERROR_SERVICE_ALREADY_RUNNING)
                        // A common cause of failure (easier to read than an error code)
                        fprintf ( stderr, "failure: StartService, ERROR_SERVICE_ALREADY_RUNNING\n");
                else
                        fprintf ( stderr, "failure: StartService (0x%02x)\n", err);
                }
        else if (!Quiet)
        printf ("StartService SUCCESS\n");
        CloseServiceHandle (schService);
        return ret;
}

/****************************************************************************
REMARKS:
Stops the driver
****************************************************************************/
static BOOL StopDriver(
        IN SC_HANDLE  SchSCManager,
        IN LPCTSTR    DriverName)
{
    SC_HANDLE       schService;
    BOOL            ret;
    SERVICE_STATUS  serviceStatus;

    schService = OpenService (SchSCManager,
                              DriverName,
                                                          SERVICE_ALL_ACCESS);
        if (schService == NULL) {
        fprintf ( stderr, "failure: OpenService (0x%02x)\n", GetLastError());
        return FALSE;
                }
        ret = ControlService (schService,
                                                  SERVICE_CONTROL_STOP,
                                                  &serviceStatus);
        if (ret == 0)
                fprintf(stderr, "failure: ControlService (0x%02x)\n", GetLastError());
        else if (!Quiet)
                printf ("ControlService SUCCESS\n");
        CloseServiceHandle (schService);
        return ret;
}

/****************************************************************************
REMARKS:
Opens the device driver
****************************************************************************/
static BOOL OpenDevice(
        IN LPCTSTR    test_file)
{
    char     completeDeviceName[64];
    HANDLE   hDevice;

        sprintf( completeDeviceName, "\\\\.\\%s", test_file );
        hDevice = CreateFile( completeDeviceName,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL
                          );
        if (hDevice == INVALID_HANDLE_VALUE)
                return FALSE;
        CloseHandle (hDevice);
        return TRUE;
}

/****************************************************************************
REMARKS:
Print usage information for the driver
****************************************************************************/
static void Usage(void)
{
    printf( "drvinst [options] <driver_name> <driver_image>\n" );
    printf( "    options:\n" );
    printf( "\t-r\t\t\tRemove driver\n" );
    printf( "\t-d<dependency_name>\tAdd a driver dependency\n" );
    printf( "\t-s<start_type>\t\tSet the starting type of driver\n" );
    printf( "\t-e<error_control>\tSet the startup error response\n" );
    printf( "\t-t<file_name>\t\tSet the file name to test open with\n" );
    printf( "\t-q\t\t\tQuiet installation\n" );
    exit( 1 );
}

int main(int argc, char *argv[])
{
    SC_HANDLE   schSCManager;
        BOOL        remove = FALSE;
    char        *test_file = NULL;
        char        *curr_dep;
        char        ServiceName[256] = "";
        char            ServiceExe[256] = "";

        curr_dep = &DependencyList[0];
        for (;;) {
                ++argv;
                --argc;
                if (argv[0] == NULL)
                        break;
                if (argv[0][0] != '-')
                        break;
                switch( argv[0][1] ) {
                        case 'r':
                                remove = TRUE;
                                break;
                        case 'd':
                                strcpy( curr_dep, &argv[0][2] );
                                curr_dep += strlen( curr_dep ) + 1;
                                break;
                        case 's':
                                StartType = atoi( &argv[0][2] );
                                break;
                        case 'e':
                                ErrorControl = atoi( &argv[0][2] );
                                break;
                        case 't':
                                test_file = &argv[0][2];
                                break;
                        case 'q':
                                Quiet = TRUE;
                                break;
                        case 'h':
                                Usage();
                                break;
                        default:
                                fprintf( stderr, "Invalid option '%c'\n", argv[0][1] );
                                Usage();
                                break;
                        }
                }

        // Handle defaults if driver names are not specified
        if (curr_dep == &DependencyList[0]) {
                strcpy(curr_dep, "ParPort");
                curr_dep += strlen(curr_dep) + 1;
                }
        if (argc < 1)
                strcpy(ServiceName,"DbgPort");
        else
                strcpy(ServiceName,argv[0]);
        if (argc < 2) {
                GetSystemDirectory(ServiceExe,sizeof(ServiceExe));
                strcat(ServiceExe,"\\drivers\\dbgport.sys");
                }
        else
                strcpy(ServiceExe,argv[1]);

        if (GetVersion() & 0x80000000) {
                if (!Quiet) printf( "Not on Windows NT, can not install driver.\n" );
                return 0;
                }
        if (test_file != NULL && OpenDevice(test_file)) {
                if (!Quiet) printf( "Driver already running\n" );
                return 0;
                }
        schSCManager = OpenSCManager (NULL,                 // machine (NULL == local)
                                  NULL,                 // database (NULL == default)
                                  SC_MANAGER_ALL_ACCESS // access required
                                  );
        if (schSCManager == NULL) {
        fprintf( stderr, "Can not open service manager (%ld)\n", GetLastError() );
        return 1;
                }
        if (remove) {
                StopDriver( schSCManager, ServiceName );
                RemoveDriver( schSCManager, ServiceName );
                }
        else if (ServiceExe == NULL) {
        fprintf( stderr, "Missing service executable\n" );
        Usage();
                }
        else {
        *curr_dep = '\0';
                if (InstallDriver( schSCManager, ServiceName, ServiceExe ) ) {
                        if (StartDriver( schSCManager, ServiceName ) ) {
                                if (test_file != NULL ) {
                                        if (OpenDevice( test_file ) )
                                                if( !Quiet ) printf( "Driver Installation SUCCESS\n" );
                                        else
                                                fprintf ( stderr, "Driver not started\n" );
                                        }
                                }

                        }
                }
        CloseServiceHandle (schSCManager);
        return 0;
}
