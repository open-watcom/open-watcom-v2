/*
 * A simple WDM driver for a generic PCI device. This sample does not
 * contain any device specific functionality.
 */

#include <wdm.h>
#include "../boxioctl.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT,  DriverEntry )
#pragma alloc_text( PAGED, HwAddDevice )
#pragma alloc_text( PAGED, HwDefaultDispatch )
#pragma alloc_text( PAGED, HwDispatchIoctl )
#pragma alloc_text( PAGED, HwDispatchPnP )
#pragma alloc_text( PAGED, HwDriverUnload )
#pragma alloc_text( PAGED, DoStartDevice )
#pragma alloc_text( PAGED, DoRemoveDevice )
#endif

#define DRV_NAME        "boxdev"
#define DRV_TAG         'BOXS'

#define DEVICE_NAME     L"\\Device\\boxsys"
#define PUBLIC_NAME     L"\\DosDevices\\boxsys"

/* The device extension. This sturcture contains the driver's 
 * private data.
 */
typedef struct {
    PDEVICE_OBJECT      LowerDevObj;        /* Lower driver in the stack. */
    IO_REMOVE_LOCK      RemoveLock;         /* Device removal lock. */
    BOOLEAN             Started;            /* Device is started. */
    BOOLEAN             Removed;            /* Device is removed (default). */
    UNICODE_STRING      LinkName;           /* Symbolic link for userland. */
    USHORT              VendorID;           /* PCI Vendor ID. */
    USHORT              DeviceID;           /* PCI Device ID. */
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


/* Driver unload callback. Cleans up after DriverEntry. */
VOID NTAPI HwDriverUnload( IN PDRIVER_OBJECT DriverObject )
{
    DbgPrint( DRV_NAME ": HwDriverUnload\n" );
}


/* Add a new device object plus symlink. */
NTSTATUS NTAPI HwAddDevice( IN PDRIVER_OBJECT DriverObject,
                            IN PDEVICE_OBJECT PhysDevObject )
{
    PDEVICE_OBJECT      DeviceObject;
    PDEVICE_EXTENSION   DevExt;
    UNICODE_STRING      DeviceName;
    UNICODE_STRING      PublicName;
    NTSTATUS            status;

    DbgPrint( DRV_NAME ": HwAddDevice\n" );
    PAGED_CODE();   /* Verify we're running with the right IRQL. */

    RtlInitUnicodeString( &DeviceName, DEVICE_NAME );

    /* Call the I/O Manager to create the device object. */
    status = IoCreateDevice( DriverObject, sizeof( DEVICE_EXTENSION ),
                             &DeviceName, FILE_DEVICE_UNKNOWN,
                             0, FALSE, &DeviceObject );
    if( !NT_SUCCESS( status ) ) {
        DbgPrint( DRV_NAME ": IoCreateDevice failed!\n" );
        return( status );
    }

    DbgPrint( DRV_NAME ": Device object created\n" );

    /* Create a symlink for user applications. */
    RtlInitUnicodeString( &PublicName, PUBLIC_NAME );

    status = IoCreateSymbolicLink( &PublicName, &DeviceName );
    if( !NT_SUCCESS( status ) ) {
        DbgPrint( DRV_NAME ": Failed to create symbolic link!\n" );
        IoDeleteDevice( DeviceObject );
        return( status );
    }

    /* Set up the device extension. */
    DevExt = DeviceObject->DeviceExtension;

    RtlZeroMemory( DevExt, sizeof( DEVICE_EXTENSION ) );

    DevExt->Removed = FALSE;
    DevExt->Started = FALSE;

    /* Store the symlink so we can remove it later. */
    DevExt->LinkName = PublicName;

    /* A remove lock prevents removal during IRP processing. */
    IoInitializeRemoveLock( &DevExt->RemoveLock, DRV_TAG, 0, 0 );
                            
    /* Attach to lower driver so we can pass IRPs down. */
    DevExt->LowerDevObj = IoAttachDeviceToDeviceStack( DeviceObject, PhysDevObject );

    /* Indicate pageable power routines and buffered I/O. */
    DeviceObject->Flags |= DO_POWER_PAGABLE | DO_BUFFERED_IO;

    /* Done initializing. */
    DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	
    return( STATUS_SUCCESS );
}


/* Dispatch miscellanoeus IRPs. */
NTSTATUS NTAPI HwDefaultDispatch( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    PDEVICE_EXTENSION   DevExt;
    PIO_STACK_LOCATION  Stack;
    NTSTATUS            status;

    DbgPrint( DRV_NAME ": HwDefaultDispatch\n" );
    PAGED_CODE();

    DevExt = DeviceObject->DeviceExtension;
    Irp->IoStatus.Information = 0;

    status = IoAcquireRemoveLock( &DevExt->RemoveLock, Irp );
    if( !NT_SUCCESS( status ) ) {
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status      = status;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return( status );
    }

    if( !DevExt->Started ) {
        /* Fail IRPs received before the device is ready */
        status = STATUS_DEVICE_NOT_READY;
        Irp->IoStatus.Status = status;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        IoReleaseRemoveLock( &DevExt->RemoveLock, Irp );
        return( status );
    }
    
    Stack = IoGetCurrentIrpStackLocation( Irp );

	switch( Stack->MajorFunction ) {
        case IRP_MJ_CREATE:
        case IRP_MJ_CLOSE:
            status = STATUS_SUCCESS;
            break;
        default: 
            status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status      = status;

    /* Complete the IRP; don't bump up priority. */
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    IoReleaseRemoveLock( &DevExt->RemoveLock, Irp );
    return( status );
}


/* Process our device specific IOCTLs. */
NTSTATUS NTAPI HwDispatchIoctl( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    PDEVICE_EXTENSION   DevExt;
    PIO_STACK_LOCATION  Stack;
    PULONG              IdBuff;
    NTSTATUS            status;
    ULONG               info;
	
    DbgPrint( DRV_NAME ": HwDispatchIoctl\n" );
    PAGED_CODE();

    DevExt = DeviceObject->DeviceExtension;
    Irp->IoStatus.Information = 0;

    status = IoAcquireRemoveLock( &DevExt->RemoveLock, Irp );
    if( !NT_SUCCESS( status ) ) {
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status      = status;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return( status );
    }

    if( !DevExt->Started ) {
        /* If the device isn't started, fail right away. */
        Irp->IoStatus.Status = status = STATUS_DEVICE_NOT_READY;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        IoReleaseRemoveLock( &DevExt->RemoveLock, Irp );
        return( status );
    }
    
    Stack = IoGetCurrentIrpStackLocation( Irp );
    
    /* Process an IOCTL. */
    switch( Stack->Parameters.DeviceIoControl.IoControlCode ) {
    case IOCTL_BOXDEV_GET_IDS:
        IdBuff  = Irp->AssociatedIrp.SystemBuffer;
        *IdBuff = (DevExt->VendorID << 16) | DevExt->DeviceID;
        status  = STATUS_SUCCESS;
        info    = sizeof( *IdBuff );
        break;
    default:
        status  = STATUS_INVALID_PARAMETER;
        info    = 0;
        break;
    }
 
    Irp->IoStatus.Information = info;
    Irp->IoStatus.Status      = status;

    /* Complete the IRP; don't bump up priority. */
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    IoReleaseRemoveLock( &DevExt->RemoveLock, Irp );
    return( status );
}


/*
 * Simple completion routine. The context is a pointer to a kernel
 * event semaphore.
*/
NTSTATUS NTAPI CompletionRoutine( PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context )
{
    PKEVENT     Event = Context;

    KeSetEvent( Event, 0, FALSE );
    return( StopCompletion );   /* We'll complete the IRP ourselves. */
}


/* 
 * Read a given number of bytes at specified offset from the PCI configuration
 * space of our device. Calls the underlying PCI bus driver to do the work.
 */
NTSTATUS ReadPCICfgSpace( PDEVICE_OBJECT DeviceObject, VOID *buffer,
                          USHORT offset, ULONG length )
{
    PDEVICE_EXTENSION   DevExt;
    PIO_STACK_LOCATION  Stack;
    PIRP                Irp;
    KEVENT              Event;
    NTSTATUS            status;

    DevExt = DeviceObject->DeviceExtension;
    Irp = IoAllocateIrp( DeviceObject->StackSize, FALSE );
    if( Irp == NULL ) {
        DbgPrint( DRV_NAME ": ReadPCICfgSpace: Failed to allocate IRP!\n" );
        return( STATUS_INSUFFICIENT_RESOURCES );
    }

    /* Build an IRP to send to the lower (PCI) driver. */
    Stack = IoGetNextIrpStackLocation( Irp );
    Irp->IoStatus.Status                         = STATUS_NOT_SUPPORTED;
    Stack->MajorFunction                         = IRP_MJ_PNP;
    Stack->MinorFunction                         = IRP_MN_READ_CONFIG;
    Stack->Parameters.ReadWriteConfig.WhichSpace = PCI_WHICHSPACE_CONFIG;
    Stack->Parameters.ReadWriteConfig.Buffer     = buffer;
    Stack->Parameters.ReadWriteConfig.Offset     = offset;
    Stack->Parameters.ReadWriteConfig.Length     = length;

    KeInitializeEvent( &Event, NotificationEvent, FALSE );
    IoSetCompletionRoutine( Irp, CompletionRoutine, &Event, TRUE, TRUE, TRUE );

    /* Send off the IRP and wait for completion. */
    status = IoCallDriver( DevExt->LowerDevObj, Irp );
    if( status == STATUS_PENDING ) {
        KeWaitForSingleObject( &Event, Executive, KernelMode, FALSE, NULL );
        status = Irp->IoStatus.Status;
    }

    IoFreeIrp( Irp );
    return( status );
}


/* Start the physical device. */
NTSTATUS DoStartDevice( IN PDEVICE_OBJECT DeviceObject )
{
    PDEVICE_EXTENSION                DevExt;
    PCI_COMMON_CONFIG                PciCfgRegs;
    NTSTATUS                         status;

    DbgPrint( DRV_NAME ": DoStartDevice\n" );

    DevExt = DeviceObject->DeviceExtension;

    if( DevExt->Removed ) {
        /* Surprise removal? No device to start... */
        return( STATUS_DELETE_PENDING );
    }

    /* Read the the entire common PCI config space of our device. */
    status = ReadPCICfgSpace( DeviceObject, &PciCfgRegs, 0,
                              sizeof( PCI_COMMON_CONFIG ) );
    if( NT_SUCCESS( status ) ) {
        DevExt->VendorID = PciCfgRegs.VendorID;
        DevExt->DeviceID = PciCfgRegs.DeviceID;
        DbgPrint( DRV_NAME ": PCI ID %04x:%04x\n", DevExt->VendorID,
                  DevExt->DeviceID );
    } else {
        DbgPrint( DRV_NAME ": ReadPCICfgSpace failed, status=%x\n", status );
    }

    return( status );
}


/* Remove the symlink and clean up. */
NTSTATUS DoRemoveDevice( IN PDEVICE_EXTENSION DevExt )
{
    NTSTATUS            status = STATUS_SUCCESS;

    DbgPrint( DRV_NAME ": DoRemoveDevice\n" );

    DevExt->Removed = TRUE;
    DevExt->Started = FALSE;

    /* Remove the symlink which AddDevice created. */
    if( DevExt->LinkName.Length ) {
        DbgPrint( DRV_NAME ": Deleting symbolic link\n" );
        status = IoDeleteSymbolicLink( &DevExt->LinkName );
        if( !NT_SUCCESS( status ) )
            DbgPrint( DRV_NAME ": Failed to delete symbolic link!\n" );
    }
    return( status );
}


/* Process PnP IRPs. One of the more interesting parts of WDM. */
NTSTATUS NTAPI HwDispatchPnP( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    PDEVICE_EXTENSION   DevExt;
    KEVENT              Event;
    PIO_STACK_LOCATION  Stack;
    NTSTATUS            status;

    DbgPrint( DRV_NAME ": HwDispatchPnP\n" );
    PAGED_CODE(); 

    DevExt = DeviceObject->DeviceExtension;
    Stack  = IoGetCurrentIrpStackLocation( Irp );

    status = IoAcquireRemoveLock( &DevExt->RemoveLock, Irp );
    if( !NT_SUCCESS( status ) ) {
        Irp->IoStatus.Status = status;	
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return( status );
    }

    switch( Stack->MinorFunction ) {
    case IRP_MN_START_DEVICE:
        /* Handle start device event. */
        IoCopyCurrentIrpStackLocationToNext( Irp );
        KeInitializeEvent( &Event, NotificationEvent, FALSE );

        IoSetCompletionRoutine( Irp, CompletionRoutine,
                                &Event, TRUE, TRUE, TRUE );
		                
        status = IoCallDriver( DevExt->LowerDevObj, Irp );
        if( status == STATUS_PENDING ) {
            KeWaitForSingleObject( &Event, Executive, 
                                   KernelMode, FALSE, NULL );
        }
        if( NT_SUCCESS( status ) && NT_SUCCESS( Irp->IoStatus.Status ) ) {
            /* Perform any device initialization required. */
            status = DoStartDevice( DeviceObject );
            if( NT_SUCCESS( status ) ) {
                DevExt->Started = TRUE;
                DevExt->Removed = FALSE;
            }
        }
        Irp->IoStatus.Status      = status;
        Irp->IoStatus.Information = 0; 
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        break;  

    case IRP_MN_QUERY_STOP_DEVICE:
        /* Don't allow device stop. */
        status = STATUS_UNSUCCESSFUL;
        Irp->IoStatus.Status      = status;
        Irp->IoStatus.Information = 0; 
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        break;
        
    case IRP_MN_QUERY_REMOVE_DEVICE:
        /* Allow device removal. */
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation( Irp );
        status = IoCallDriver( DevExt->LowerDevObj, Irp );
        break;

    case IRP_MN_SURPRISE_REMOVAL:
        /* Process a surprise removal. */
        DoRemoveDevice( DevExt );
        IoSkipCurrentIrpStackLocation( Irp );
        Irp->IoStatus.Status = STATUS_SUCCESS;
        status = IoCallDriver( DevExt->LowerDevObj, Irp );
        break;

    case IRP_MN_REMOVE_DEVICE:
        /* Handle orderly device removal. */
        if( !DevExt->Removed ) {
            DoRemoveDevice( DevExt );
        }

        IoReleaseRemoveLockAndWait( &DevExt->RemoveLock, Irp );
			
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation( Irp );
        status = IoCallDriver( DevExt->LowerDevObj, Irp );

        /* Detach the device from the driver stack. */
        if( DevExt->LowerDevObj )
            IoDetachDevice( DevExt->LowerDevObj );

        /* Delete the Functional Device Object. */
        DbgPrint( DRV_NAME ": Deleting device object\n" );
        IoDeleteDevice( DeviceObject );
        return( status );

    case IRP_MN_STOP_DEVICE:
        /* Shouldn't happen - we failed query stop. */
    case IRP_MN_CANCEL_REMOVE_DEVICE: 
    case IRP_MN_CANCEL_STOP_DEVICE: 
        /* Cancel remove/stop need no special handling. */
        Irp->IoStatus.Status = STATUS_SUCCESS;
        /* Fall through! */
    default:
        /* Let the lower driver handle the IRP. */
        IoSkipCurrentIrpStackLocation( Irp );
        status = IoCallDriver( DevExt->LowerDevObj, Irp );
        break;
    }
    IoReleaseRemoveLock( &DevExt->RemoveLock, Irp );
    return( status );
}


/* Dispatch Power IRPs. Simply passes IRP down the stack. */
NTSTATUS NTAPI HwDispatchPower( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp ) 
{
    PDEVICE_EXTENSION       DevExt;

    DbgPrint( DRV_NAME ": HwDispatchPower\n" );

    DevExt = DeviceObject->DeviceExtension;
    PoStartNextPowerIrp( Irp );

    /* If our device was removed, there's no lower driver. */
    if( DevExt->Removed ) {
        Irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return( STATUS_DELETE_PENDING );
    } else {
        IoSkipCurrentIrpStackLocation( Irp );
        return( PoCallDriver( DevExt->LowerDevObj, Irp ) );
    }
}


/* Standard NT driver entry point. */
NTSTATUS NTAPI DriverEntry( IN PDRIVER_OBJECT DriverObject,
                            IN PUNICODE_STRING RegistryPath )
{
    DbgPrint( DRV_NAME ": DriverEntry\n" );

    /* Fill in the callbacks in the driver object. */
    DriverObject->DriverUnload                         = HwDriverUnload; 
    DriverObject->DriverExtension->AddDevice           = HwAddDevice;	
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = HwDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = HwDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HwDispatchIoctl;
    DriverObject->MajorFunction[IRP_MJ_PNP]            = HwDispatchPnP;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = HwDispatchPower; 
	    
    return( STATUS_SUCCESS );
}
