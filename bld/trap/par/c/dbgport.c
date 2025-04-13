/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include <ntddk.h>
#include "bool.h"
#include "parallel.h"
#include "dbgioctl.h"
#include "pardata.h"


#if defined( __WATCOMC__ )
typedef __int64 _int64;
#endif

#define my_inp(p)       READ_PORT_UCHAR((PUCHAR)(hwd->controller.ptr + (p)))
#define my_outp(p,v)    WRITE_PORT_UCHAR((PUCHAR)(hwd->controller.ptr + (p)), (UCHAR)(v))
#define dbgrtn(x)

#define PARALLEL_REGISTER_SPAN 3

typedef struct _DEVICE_EXTENSION {
    /*
     * Points to the device object that contains this device extension.
     */
    PDEVICE_OBJECT                  DeviceObject;
    /*
     * Points to the port device object that this class device is connected to.
     */
    PDEVICE_OBJECT                  PortDeviceObject;
    /*
     * This holds the result of the get parallel port info
     * request to the port driver.
     */
    PHYSICAL_ADDRESS                OriginalController;
    ULONG                           SpanOfController;
    PPARALLEL_FREE_ROUTINE          FreePort;
    PPARALLEL_TRY_ALLOCATE_ROUTINE  TryAllocatePort;
    PVOID                           AllocFreePortContext;
    /*
     * Records whether we actually created the symbolic link name
     * at driver load time and the symbolic link itself.  If we didn't
     * create it, we won't try to destroy it when we unload.
     */
    BOOLEAN                         CreatedSymbolicLink;
    UNICODE_STRING                  SymbolicLinkName;
    /*
     * Internal variables used by the driver
     */
    hw_data                         hwdata;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

/*
 * This is the "structure" of the IOPM.  It is just a simple
 * character array of length 0x2000.
 *
 * This holds 8K * 8 bits -> 64K bits of the IOPM, which maps the
 * entire 64K I/O space of the x86 processor.  Any 0 bits will give
 * access to the corresponding port for user mode processes.  Any 1
 * bits will disallow I/O access to the corresponding port.
 */
#define IOPM_SIZE       0x2000
typedef UCHAR IOPM[IOPM_SIZE];

/*-------------------------- Implementation -------------------------------*/

/*
 * The following are undocumented calls that are used to grant
 * I/O access to the calling process.
 *
 * Ke386IoSetAccessMap() copies the passed map to the TSS.
 *
 * Ke386IoSetAccessProcess() adjusts the IOPM offset pointer so that
 * the newly copied map is actually used.  Otherwise, the IOPM offset
 * points beyond the end of the TSS segment limit, causing any I/O
 * access by the user mode process to generate an exception.
 */
NTKERNELAPI extern void     NTAPI Ke386SetIoAccessMap( int, IOPM * );
NTKERNELAPI extern void     NTAPI Ke386QueryIoAccessMap( int, IOPM * );
NTKERNELAPI extern void     NTAPI Ke386IoSetAccessProcess( PEPROCESS, int );

NTKERNELAPI extern void     NTAPI ZwYieldExecution( void );

/*
 * This will hold simply an array of 0's which will be copied
 * into our actual IOPM in the TSS by Ke386SetIoAccessMap().
 * The memory is allocated at driver load time.
 */
static IOPM *IOPM_local = 0;
static IOPM *IOPM_saved = 0;

static void NothingToDo( void )
{
    ZwYieldExecution();
}

static unsigned long Ticks( void )
{
    _int64 ticks;

    KeQueryTickCount( (PLARGE_INTEGER)&ticks );
    return( (unsigned long)( ticks / 10 ) );
}

#include "parproc.c"


static unsigned RemoteGet( PDEVICE_EXTENSION ext, char *data, unsigned len )
{
    return( DataGet( &ext->hwdata, data, len ) );
}

static unsigned RemotePut( PDEVICE_EXTENSION ext, char *data, unsigned len )
{
    return( DataPut( &ext->hwdata, data, len ) );
}

static bool RemoteConnectServer( PDEVICE_EXTENSION ext )
{
    return( DataConnect( &ext->hwdata, true ) );
}

static bool RemoteConnectClient( PDEVICE_EXTENSION ext )
{
    return( DataConnect( &ext->hwdata, false ) );
}

static void RemoteDisco( PDEVICE_EXTENSION ext )
{
    DataDisconnect( &ext->hwdata );
}

static void RemoteLink( PDEVICE_EXTENSION ext )
{
    DataReset( &ext->hwdata, true );
}

/****************************************************************************
PARAMETERS:
ParallelPortNumber  - Supplies the port number.
PortName            - Returns the port name.
ClassName           - Returns the class name.
LinkName            - Returns the symbolic link name.

RETURNS:
FALSE on Failure, TRUE on Success.

REMARKS:
This routine generates the names \Device\ParallelPortN and
\Device\ParallelDebugN, \DosDevices\DBGPORTn.
****************************************************************************/
static BOOLEAN ParMakeNames(
    IN  ULONG           ParallelPortNumber,
    OUT PUNICODE_STRING PortName,
    OUT PUNICODE_STRING ClassName,
    OUT PUNICODE_STRING LinkName)
{
    UNICODE_STRING  prefix, digits, linkPrefix, linkDigits;
    WCHAR           digitsBuffer[10], linkDigitsBuffer[10];
    UNICODE_STRING  portSuffix, classSuffix, linkSuffix;
    NTSTATUS        status;

    /*
     * Put together local variables for constructing names.
     */
    RtlInitUnicodeString( &prefix, L"\\Device\\" );
    RtlInitUnicodeString( &linkPrefix, L"\\DosDevices\\" );
    RtlInitUnicodeString( &portSuffix, DD_PARALLEL_PORT_BASE_NAME_U );
    RtlInitUnicodeString( &classSuffix, L"ParallelDebug" );
    RtlInitUnicodeString( &linkSuffix, L"DBGPORT" );
    digits.Length = 0;
    digits.MaximumLength = 20;
    digits.Buffer = digitsBuffer;
    linkDigits.Length = 0;
    linkDigits.MaximumLength = 20;
    linkDigits.Buffer = linkDigitsBuffer;
    status = RtlIntegerToUnicodeString( ParallelPortNumber, 10, &digits );
    if( !NT_SUCCESS( status ) )
        return( FALSE );
    status = RtlIntegerToUnicodeString( ParallelPortNumber + 1, 10, &linkDigits );
    if( !NT_SUCCESS( status ) )
        return( FALSE );
    /*
     * Make the port name.
     */
    PortName->Length = 0;
    PortName->MaximumLength = prefix.Length + portSuffix.Length + digits.Length + sizeof( WCHAR );
    PortName->Buffer = ExAllocatePool( PagedPool, PortName->MaximumLength );
    if( !PortName->Buffer )
        return( FALSE );
    RtlZeroMemory( PortName->Buffer, PortName->MaximumLength );
    RtlAppendUnicodeStringToString( PortName, &prefix );
    RtlAppendUnicodeStringToString( PortName, &portSuffix );
    RtlAppendUnicodeStringToString( PortName, &digits );
    /*
     * Make the class name.
     */
    ClassName->Length = 0;
    ClassName->MaximumLength = prefix.Length + classSuffix.Length + digits.Length + sizeof( WCHAR );
    ClassName->Buffer = ExAllocatePool( PagedPool, ClassName->MaximumLength );
    if( !ClassName->Buffer ) {
        ExFreePool( PortName->Buffer );
        return( FALSE );
    }
    RtlZeroMemory( ClassName->Buffer, ClassName->MaximumLength );
    RtlAppendUnicodeStringToString( ClassName, &prefix );
    RtlAppendUnicodeStringToString( ClassName, &classSuffix );
    RtlAppendUnicodeStringToString( ClassName, &digits );
    /*
     * Make the link name.
     */
    LinkName->Length = 0;
    LinkName->MaximumLength = linkPrefix.Length + linkSuffix.Length + linkDigits.Length + sizeof( WCHAR );
    LinkName->Buffer = ExAllocatePool( PagedPool, LinkName->MaximumLength );
    if( !LinkName->Buffer ) {
        ExFreePool( PortName->Buffer );
        ExFreePool( ClassName->Buffer );
        return( FALSE );
    }
    RtlZeroMemory( LinkName->Buffer, LinkName->MaximumLength );
    RtlAppendUnicodeStringToString( LinkName, &linkPrefix );
    RtlAppendUnicodeStringToString( LinkName, &linkSuffix );
    RtlAppendUnicodeStringToString( LinkName, &linkDigits );
    return( TRUE );
}

/****************************************************************************
PARAMETERS:
Extension   - Supplies the device extension.

RETURNS:
STATUS_SUCCESS on Success, !STATUS_SUCCESS on Failure.

REMARKS:
This routine will request the port information from the port driver
and fill it in the device extension.
****************************************************************************/
static NTSTATUS ParGetPortInfoFromPortDevice(
    IN OUT  PDEVICE_EXTENSION   ext)
{
    KEVENT                      event;
    PIRP                        irp;
    PARALLEL_PORT_INFORMATION   portInfo;
    IO_STATUS_BLOCK             ioStatus;
    NTSTATUS                    status;

    KeInitializeEvent( &event, NotificationEvent, FALSE );
    irp = IoBuildDeviceIoControlRequest( IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO,
        ext->PortDeviceObject,
        NULL, 0, &portInfo,
        sizeof( PARALLEL_PORT_INFORMATION ),
        TRUE, &event, &ioStatus );
    if( !irp )
        return( STATUS_INSUFFICIENT_RESOURCES );
    status = IoCallDriver( ext->PortDeviceObject, irp );
    if( !NT_SUCCESS( status ) )
        return( status );
    status = KeWaitForSingleObject( &event, Executive, KernelMode, FALSE, NULL );
    if( !NT_SUCCESS( status ) )
        return( status );
    ext->OriginalController = portInfo.OriginalController;
    ext->hwdata.controller.ptr = portInfo.Controller;
    ext->SpanOfController = portInfo.SpanOfController;
    ext->TryAllocatePort = portInfo.TryAllocatePort;
    ext->FreePort = portInfo.FreePort;
    ext->AllocFreePortContext = portInfo.Context;
    if( ext->SpanOfController < PARALLEL_REGISTER_SPAN )
        return( STATUS_INSUFFICIENT_RESOURCES );
    return( status );
}

/****************************************************************************
PARAMETERS:
DriverObject        - Supplies the driver object.
ParallelPortNumber  - Supplies the number for this port.

REMARKS:
This routine is called for every parallel port in the system.  It
will create a class device upon connecting to the port device
corresponding to it.
****************************************************************************/
static VOID ParInitializeDeviceObject(
    IN  PDRIVER_OBJECT  DriverObject,
    IN  ULONG           ParallelPortNumber)
{
    UNICODE_STRING      portName, className, linkName;
    NTSTATUS            status;
    PDEVICE_OBJECT      deviceObject;
    PDEVICE_EXTENSION   ext;
    PFILE_OBJECT        fileObject;

    /*
     * Cobble together the port and class device names.
     */
    if( !ParMakeNames( ParallelPortNumber, &portName, &className, &linkName ) )
        return;
    /*
     * Create the device object.
     */
    status = IoCreateDevice( DriverObject, sizeof( DEVICE_EXTENSION ),
        &className, FILE_DEVICE_PARALLEL_PORT, 0, TRUE,
        &deviceObject );
    if( !NT_SUCCESS( status ) ) {
        ExFreePool( linkName.Buffer );
        goto Cleanup;
    }
    /*
     * Now that the device has been created,
     * set up the device extension.
     */
    ext = deviceObject->DeviceExtension;
    RtlZeroMemory( ext, sizeof( DEVICE_EXTENSION ) );
    ext->DeviceObject = deviceObject;
    deviceObject->Flags |= DO_BUFFERED_IO;
    status = IoGetDeviceObjectPointer( &portName, FILE_READ_ATTRIBUTES,
        &fileObject,
        &ext->PortDeviceObject );
    if( !NT_SUCCESS( status ) ) {
        IoDeleteDevice( deviceObject );
        ExFreePool( linkName.Buffer );
        goto Cleanup;
    }
    ObDereferenceObject( fileObject );
    ext->DeviceObject->StackSize = ext->PortDeviceObject->StackSize + 1;
    /*
     * Get the port information from the port device object.
     */
    status = ParGetPortInfoFromPortDevice( ext );
    if( !NT_SUCCESS( status ) ) {
        IoDeleteDevice( deviceObject );
        ExFreePool( linkName.Buffer );
        goto Cleanup;
    }
    /*
     * Set up the symbolic link for windows apps.
     */
    status = IoCreateSymbolicLink( &linkName, &className );
    if( !NT_SUCCESS( status ) ) {
        ext->CreatedSymbolicLink = FALSE;
        ExFreePool( linkName.Buffer );
        goto Cleanup;
    }
    /*
     * We were able to create the symbolic link, so record this
     * value in the extension for cleanup at unload time.
     */
    ext->CreatedSymbolicLink = TRUE;
    ext->SymbolicLinkName = linkName;

Cleanup:
    ExFreePool( portName.Buffer );
    ExFreePool( className.Buffer );
}

/****************************************************************************
REMARKS:
Set the IOPM (I/O permission map) of the calling process so that it
is given full I/O access.  Our IOPM_local[] array is all zeros, so
the IOPM will be all zeros.  If OnFlag is 1, the process is given I/O
access.  If it is 0, access is removed.
****************************************************************************/
static VOID SetIOPermissionMap( int OnFlag )
{
    if( OnFlag ) {
        /*
         * Enable I/O for the process
         */
        Ke386QueryIoAccessMap( 1, IOPM_saved );
        Ke386IoSetAccessProcess( PsGetCurrentProcess(), 1 );
        Ke386SetIoAccessMap( 1, IOPM_local );
    } else {
        /*
         * Disable I/O for the process, restoring old IOPM table
         */
        Ke386IoSetAccessProcess( PsGetCurrentProcess(), 0 );
        Ke386SetIoAccessMap( 1, IOPM_saved );
    }
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

RETURNS:
STATUS_SUCCESS          - Success.
STATUS_NOT_A_DIRECTORY  - This device is not a directory.

REMARKS:
This routine is the dispatch for create requests.
****************************************************************************/
DRIVER_DISPATCH ParCreate;
NTSTATUS ParCreate(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    PIO_STACK_LOCATION  irpSp;
    NTSTATUS            status;
    PDEVICE_EXTENSION   ext;

    /*
     * Give the debugger process full I/O port access. Ideally we should
     * restrict this to the actual I/O ports in use, and this can be done
     * in the future if desired.
     */
    SetIOPermissionMap( 1 );
    /*
     * Now create the parallel port extension device
     */
    ext = DeviceObject->DeviceExtension;
    irpSp = IoGetCurrentIrpStackLocation( Irp );
    if( irpSp->Parameters.Create.Options & FILE_DIRECTORY_FILE ) {
        status = STATUS_NOT_A_DIRECTORY;
    } else if( !ext->TryAllocatePort( ext->AllocFreePortContext ) ) {
        status = STATUS_DEVICE_BUSY;
    } else {
        status = STATUS_SUCCESS;
    }
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return( status );
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the cancel routine for this driver.
****************************************************************************/
DRIVER_CANCEL ParCancel;
VOID ParCancel(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    /* unused parameters */ (void)DeviceObject; (void)Irp;
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the IOCtl routine for this driver.
****************************************************************************/
DRIVER_DISPATCH ParIOCTL;
NTSTATUS ParIOCTL(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    PIO_STACK_LOCATION  irpSp;
    NTSTATUS            status;
    PDEVICE_EXTENSION   ext;
    DBGPORT_IO          *IOBuffer;

    status = STATUS_SUCCESS;
    Irp->IoStatus.Information = sizeof( *IOBuffer );
    ext = DeviceObject->DeviceExtension;
    irpSp = IoGetCurrentIrpStackLocation( Irp );
    IOBuffer = (DBGPORT_IO *)Irp->AssociatedIrp.SystemBuffer;
    /*
     * NT copies inbuf here before entry and copies this to outbuf after
     * return, for METHOD_BUFFERED IOCTL's.
     */
    switch( irpSp->Parameters.DeviceIoControl.IoControlCode ) {
    case IOCTL_DBG_READ_PORT_U8:
        IOBuffer->data.u8 = READ_PORT_UCHAR( (PUCHAR)( ext->hwdata.controller.ptr + IOBuffer->port ) );
        break;
    case IOCTL_DBG_READ_PORT_U16:
        IOBuffer->data.u16 = READ_PORT_USHORT( (PUSHORT)( ext->hwdata.controller.ptr + IOBuffer->port ) );
        break;
    case IOCTL_DBG_READ_PORT_U32:
        IOBuffer->data.u32 = READ_PORT_ULONG( (PULONG)( ext->hwdata.controller.ptr + IOBuffer->port ) );
        break;
    case IOCTL_DBG_WRITE_PORT_U8:
        WRITE_PORT_UCHAR( (PUCHAR)( ext->hwdata.controller.ptr + IOBuffer->port ), IOBuffer->data.u8 );
        break;
    case IOCTL_DBG_WRITE_PORT_U16:
        WRITE_PORT_USHORT( (PUSHORT)( ext->hwdata.controller.ptr + IOBuffer->port ), IOBuffer->data.u16 );
        break;
    case IOCTL_DBG_WRITE_PORT_U32:
        WRITE_PORT_ULONG( (PULONG)( ext->hwdata.controller.ptr + IOBuffer->port ), IOBuffer->data.u32 );
        break;
    case IOCTL_DBG_REMOTE_GET:
        IOBuffer->status = RemoteGet( ext, Irp->AssociatedIrp.SystemBuffer, irpSp->Parameters.DeviceIoControl.OutputBufferLength );
        break;
    case IOCTL_DBG_REMOTE_PUT:
        IOBuffer->status = RemotePut( ext, Irp->AssociatedIrp.SystemBuffer, irpSp->Parameters.DeviceIoControl.InputBufferLength );
        break;
    case IOCTL_DBG_REMOTE_CONNECT_SERV:
        IOBuffer->status = RemoteConnectServer( ext );
        break;
    case IOCTL_DBG_REMOTE_CONNECT_CLIENT:
        IOBuffer->status = RemoteConnectClient( ext );
        break;
    case IOCTL_DBG_REMOTE_DISCO:
        RemoteDisco( ext );
        break;
    case IOCTL_DBG_REMOTE_LINK:
        RemoteLink( ext );
        break;
    default:
        Irp->IoStatus.Information = 0;
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }
    Irp->IoStatus.Status = status;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return( status );
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the cleanup routine for this driver.
****************************************************************************/
DRIVER_DISPATCH ParCleanup;
NTSTATUS ParCleanup(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    /* unused parameters */ (void)DeviceObject;

    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return( STATUS_CANCELLED );
}

/****************************************************************************
PARAMETERS:
DeviceObject    - Supplies the device object.
Irp             - Supplies the I/O request packet.

REMARKS:
This is the close routine for this driver.
****************************************************************************/
DRIVER_DISPATCH ParClose;
NTSTATUS ParClose(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp)
{
    PDEVICE_EXTENSION   ext;

    /*
     * Restore the original I/O port mappings
     */
    SetIOPermissionMap( 0 );

    ext = DeviceObject->DeviceExtension;
    ext->FreePort( ext->AllocFreePortContext );
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return( STATUS_SUCCESS );
}

/****************************************************************************
PARAMETERS:
DriverObject    - Supplies the driver object.

REMARKS:
This routine loops through the device list and cleans up after
each of the devices.
****************************************************************************/
DRIVER_UNLOAD ParUnload;
VOID ParUnload(
    IN  PDRIVER_OBJECT  DriverObject)
{
    PDEVICE_OBJECT          currentDevice;
    PDEVICE_EXTENSION   ext;

    while( (currentDevice = DriverObject->DeviceObject) != 0 ) {
        ext = currentDevice->DeviceExtension;
        if( ext->CreatedSymbolicLink ) {
            IoDeleteSymbolicLink( &ext->SymbolicLinkName );
            ExFreePool( ext->SymbolicLinkName.Buffer );
        }
        IoDeleteDevice( currentDevice );
    }
    /*
     * Free the local IOPM table if allocated
     */
    if( IOPM_local )
        MmFreeNonCachedMemory( IOPM_local, sizeof( IOPM ) );
    if( IOPM_saved ) {
        MmFreeNonCachedMemory( IOPM_saved, sizeof( IOPM ) );
    }
}

/****************************************************************************
PARAMETERS:
DriverObject    - Supplies the driver object.
RegistryPath    - Supplies the registry path for this driver.

REMARKS:
This routine is called at system initialization time to initialize
this driver.
****************************************************************************/
DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry(
    IN  PDRIVER_OBJECT  DriverObject,
    IN  PUNICODE_STRING RegistryPath)
{
    ULONG       i;

    /* unused parameters */ (void)RegistryPath;

    /*
     * TODO: We should be able to re-code this driver to use a call-gate
     *               to give the calling process full IOPL access, without needing
     *               the gross IOPM hack we currently use. This would make it
     *               slightly faster also.
     *
     * Allocate a buffer for the local IOPM and zero it.
     */
    IOPM_local = MmAllocateNonCachedMemory( sizeof( IOPM ) );
    IOPM_saved = MmAllocateNonCachedMemory( sizeof( IOPM ) );
    if( !IOPM_local || !IOPM_saved )
        return( STATUS_INSUFFICIENT_RESOURCES );
    RtlZeroMemory( IOPM_local, sizeof( IOPM ) );
    Ke386QueryIoAccessMap( 1, IOPM_saved );
    /*
     * Initialise all the device objects
     */
    for( i = 0; i < IoGetConfigurationInformation()->ParallelCount; i++ )
        ParInitializeDeviceObject( DriverObject, i );
    if( !DriverObject->DeviceObject )
        return( STATUS_NO_SUCH_DEVICE );
    /*
     * Initialize the Driver Object with driver's entry points
     */
    DriverObject->MajorFunction[IRP_MJ_CREATE] = ParCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = ParClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = ParCleanup;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ParIOCTL;
    DriverObject->DriverUnload = ParUnload;
    return( STATUS_SUCCESS );
}
