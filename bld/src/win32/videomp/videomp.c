/* 
 * Windows NT Video Miniport for the VirtualBox/bochs/qemu SVGA adapter.
 * This miniport programs the hardware directly and does not use or require
 * the video BIOS or VBE.
 *
 * NB: This is not a "VGA compatible" miniport and standard VGA modes are
 * handled by the default VGA miniport.
 */

#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>
#include <dderror.h>
#include <stddef.h>

#include "videomp.h"
#include "boxv.h"


/* Number of PCI access ranges. */
#define NUM_PCI_RANGES      3

/* RGB color channel constants. */
typedef enum {
    CHANNEL_RED,
    CHANNEL_GREEN,
    CHANNEL_BLUE
} COLOR_CHANNEL;


#if defined( ALLOC_PRAGMA )
#pragma alloc_text( PAGE, DriverEntry )
#pragma alloc_text( PAGE, HwVidFindAdapter )
#pragma alloc_text( PAGE, HwVidInitialize )
#pragma alloc_text( PAGE, HwVidStartIO )
#endif


/* Calculate pitch based on scanline length and bit depth. */
static ULONG vmpPitchByBpp( ULONG Length, UCHAR Bpp )
{
    ULONG   ulPitch = 0;

    switch( Bpp ) {
    case 4:
        ulPitch = (Length + 1) / 2;
        break;
    case 8:
        ulPitch = Length;
        break;
    case 15:
    case 16:
        ulPitch = Length * 2;
        break;
    case 24:
        ulPitch = Length * 3;
        break;
    case 32:
        ulPitch = Length * 4;
        break;
    }
    return( ulPitch );
}


/* Validate a given mode and set the appropriate flag. Note that the
 * validation criteria are somewhat arbitrary.
 */
static void vmpValidateMode( PVIDEOMP_MODE Mode, ULONG FramebufLen )
{
    ULONG   ulModeMem;

    Mode->bValid = FALSE;

    do {
        /* Horizontal resolution should be divisible by 8. */
        if( Mode->HorzRes % 8)
            break;
        
        /* Validate memory requirements. */
        ulModeMem = vmpPitchByBpp( Mode->HorzRes, Mode->Bpp ) * Mode->VertRes;
        if( ulModeMem > FramebufLen )
            break;

        /* Passed all checks, mode is valid. */
        Mode->bValid = TRUE;
    } while( 0 );
}

/* Determine whether the supported adapter is present. Note that this 
 * function is not allowed to change the state of the adapter!
 */
VP_STATUS HwVidFindAdapter( PVOID HwDevExt, PVOID HwContext, PWSTR ArgumentString,
                            PVIDEO_PORT_CONFIG_INFO ConfigInfo, PUCHAR Again )
{
    PHW_DEV_EXT             pExt = HwDevExt;
    PVOID                   *pVirtAddr;
    ULONG                   i;
    INT                     chip_id;
    VP_STATUS               status;
    ULONG                   cbVramSize;
    PWSTR                   pwszDesc;
    ULONG                   cbDesc;
    VIDEO_ACCESS_RANGE      pciAccessRanges[NUM_PCI_RANGES];
    USHORT                  usVendorId = BOXV_PCI_VEN;
    USHORT                  usDeviceId = BOXV_PCI_DEV;
    ULONG                   ulSlot = 0;

    //@todo: The framebuffer address should not be hardcoded for non-PCI access
#define NUM_ACCESS_RANGES   2
    VIDEO_ACCESS_RANGE accessRanges[NUM_ACCESS_RANGES] = {
        /* StartLo     StartHi     Len       IO Vis Shr */
        { 0x000001CE, 0x00000000, 0x00000002, 1, 1, 0 },    /* I/O ports */
        { 0xE0000000, 0x00000000, 0x00400000, 0, 1, 0 }     /* Framebuffer */
    };

    VideoDebugPrint( (1, "videomp: HwVidFindAdapter\n") );

    /* Fail if the passed structure is smaller than the NT 3.1 version. */
    if( ConfigInfo->Length < offsetof( VIDEO_PORT_CONFIG_INFO, DmaChannel ) ) {
        return( ERROR_INVALID_PARAMETER );
    }

    /* If PCI is supported, query the bus for resource mappings. */
    if( ConfigInfo->AdapterInterfaceType == PCIBus ) {
        /* Ask for bus specific access ranges. */
        VideoPortZeroMemory( pciAccessRanges, sizeof( pciAccessRanges ) );
        status = VideoPortGetAccessRanges( HwDevExt, 0, NULL,
                                           NUM_PCI_RANGES, pciAccessRanges,
                                           &usVendorId, &usDeviceId, &ulSlot );
        if( status == NO_ERROR ) {
            VideoDebugPrint( (1, "videomp: Found adapter in PCI slot %d\n", ulSlot) );
            pExt->ulSlot = ulSlot;
            /* The framebuffer is in the first slot of the PCI ranges. Copy
             * the data into the access ranges we're going to request.
             */
            accessRanges[1].RangeStart  = pciAccessRanges[0].RangeStart;
            accessRanges[1].RangeLength = pciAccessRanges[0].RangeLength;
        } else {
            /* On NT versions without PCI support, we won't even attempt this.
             * So if we tried to query the PCI device and failed to find it, 
             * it really isn't there and we have to give up. 
             */
            VideoDebugPrint( (1, "videomp: PCI adapter not found\n") );
            return( ERROR_DEV_NOT_EXIST );
        }
    }

    /* Some versions of vga.sys trap accesses to ports 0x1CE-0x1CF used on
     * old ATI cards. On Windows 2000 and later we can report legacy
     * resources to resolve this conflict. On NT 4 and older, we use a hack
     * and claim other, non-conflicting ports.
     */
    if( PortVersion < VP_VER_W2K )
        accessRanges[0].RangeStart = RtlConvertUlongToLargeInteger( 0x1CC );
 
    /* Check for a conflict in case someone else claimed our resources. */
    status = VideoPortVerifyAccessRanges( HwDevExt, NUM_ACCESS_RANGES, accessRanges );
    if( status != NO_ERROR ) {
        return( status );
    }

    /* Indicate no emulator support. */
    ConfigInfo->NumEmulatorAccessEntries     = 0;
    ConfigInfo->EmulatorAccessEntries        = NULL;
    ConfigInfo->EmulatorAccessEntriesContext = 0;

    ConfigInfo->HardwareStateSize = 0;

    ConfigInfo->VdmPhysicalVideoMemoryAddress.LowPart  = 0;
    ConfigInfo->VdmPhysicalVideoMemoryAddress.HighPart = 0;
    ConfigInfo->VdmPhysicalVideoMemoryLength           = 0;

    /* Describe the framebuffer. We claimed the range already. */
    pExt->PhysicalFrameAddress = accessRanges[1].RangeStart;


    /*
     * Map all memory and I/O ranges into system virtual address space.
     * NB: The virtual addresses in the HwDevExt must match the number
     * and type of AccessRange entries.
     */
    pVirtAddr = &pExt->IoPorts;

    /* Attempt to claim and map the memory and I/O address ranges. */
    for( i = 0; i < NUM_ACCESS_RANGES; ++i, ++pVirtAddr ) {
        *pVirtAddr = VideoPortGetDeviceBase( pExt, 
                                             accessRanges[i].RangeStart,
                                             accessRanges[i].RangeLength,
                                             accessRanges[i].RangeInIoSpace );
        if( *pVirtAddr == NULL ) {
            return( ERROR_INVALID_PARAMETER );
        }
    }

    /* Verify that supported hardware is present. */
    chip_id = BOXV_detect( pExt, &pExt->FramebufLen );
    if( !chip_id ) {
        /* If supported hardware was not found, free allocated resources. */
        pVirtAddr = &pExt->IoPorts;
        for( i = 0; i < NUM_ACCESS_RANGES; ++i, ++pVirtAddr )
            VideoPortFreeDeviceBase( pExt, *pVirtAddr );

        return( ERROR_DEV_NOT_EXIST );
    }

    /* We need to access VGA and other I/O ports. Fortunately the HAL doesn't 
     * care at all how the I/O ports are or aren't mapped on x86 platforms.
     */
    pExt->IOAddrVGA = NULL;

    /* Only support one attached monitor. */
    pExt->NumMonitors = 1;

    /* Set up mode information. */
    pExt->CurrentModeNumber = 0;
    pExt->NumValidModes     = 0;

    for( i = 0; i < ulAllModes; ++i ) {
        vmpValidateMode( &VideoModes[i], pExt->FramebufLen );
        if( VideoModes[i].bValid )
            ++pExt->NumValidModes;
    }

    /* Only one adapter supported, no need to call us again. */
    *Again = 0;

    /* Report the hardware names via registry. */

#define TEMP_CHIP_NAME  L"bochs Mk II"
    pwszDesc = TEMP_CHIP_NAME;
    cbDesc   = sizeof( TEMP_CHIP_NAME );

    VideoPortSetRegistryParameters( pExt, L"HardwareInformation.ChipType",
                                    pwszDesc, cbDesc );

#define TEMP_DAC_NAME  L"Integrated DAC"
    pwszDesc = TEMP_DAC_NAME;
    cbDesc   = sizeof( TEMP_DAC_NAME );
    VideoPortSetRegistryParameters( pExt, L"HardwareInformation.DacType",
                                    pwszDesc, cbDesc );

#define TEMP_ADAPTER_NAME  L"VirtualBox/bochs"
    pwszDesc = TEMP_ADAPTER_NAME;
    cbDesc   = sizeof( TEMP_ADAPTER_NAME );
    VideoPortSetRegistryParameters( pExt, L"HardwareInformation.AdapterString",
                                    pwszDesc, cbDesc );

    cbVramSize = pExt->FramebufLen;
    VideoPortSetRegistryParameters( pExt, L"HardwareInformation.MemorySize",
                                    &cbVramSize, sizeof( ULONG ) );
    /* All is well. */
    return( NO_ERROR );
}


/* Perform one-time device initialization. Once this function has been
 * entered, the miniport may change adapter state.
 */
BOOLEAN HwVidInitialize( PVOID HwDevExt )
{
    VideoDebugPrint( (1, "videomp: HwVidInitialize\n") );
    return( TRUE );
}


/* Determine pixel mask given color depth and color channel. */
static ULONG vmpMaskByBpp( UCHAR Bpp, COLOR_CHANNEL Channel )
{
    ULONG   ulMask;

    switch( Bpp ) {
    case 24:
    case 32:
        ulMask = 0x00FF0000 >> (Channel * 8);
        break;
    case 15:
        ulMask = 0x00007C00 >> (Channel * 5);
        break;
    case 16:
        switch( Channel ) {
        case CHANNEL_RED:
            ulMask = 0x0000F800;
            break;
        case CHANNEL_GREEN:
            ulMask = 0x000007E0;
            break;
        case CHANNEL_BLUE:
            ulMask = 0x0000001F;
            break;
        }
        break;
    case 4:
    case 8:
    default:
        ulMask = 0;     /* Palettized modes don't have a mask. */
    }

    return( ulMask );
}

/* Fill out NT specific video mode information struct based on resolution
 * and color depth.
 */
static void vmpFillModeInfo( PVIDEO_MODE_INFORMATION ModeInfo, USHORT HRes, USHORT VRes, UCHAR Bpp)
{
    /* First the basic mode information. */
    ModeInfo->Length          = sizeof( VIDEO_MODE_INFORMATION );
    ModeInfo->ModeIndex       = 0;      /* Filled in later. */
    ModeInfo->VisScreenWidth  = HRes;   /* Horizontal resolution in pixels. */
    ModeInfo->VisScreenHeight = VRes;   /* Vertical resolution in pixels. */

    /* Assume no rounding is necessary. */
    ModeInfo->ScreenStride    = vmpPitchByBpp( HRes, Bpp );

    ModeInfo->NumberOfPlanes  = 1;      /* Always one plane - packed pixel only. */
    ModeInfo->BitsPerPlane    = Bpp;    /* Number of bits per pixel. */
    ModeInfo->Frequency       = 60;     /* Irrelevant; just make something up. */

    /* Screen size is made up, but should correspond to aspect ratio. */
    ModeInfo->XMillimeter     = 320;
    ModeInfo->YMillimeter     = ModeInfo->XMillimeter * VRes / HRes;

    /* The DAC always works with 8 bits per channel. */
    ModeInfo->NumberRedBits   = 8;      /* Red pixels in DAC. */
    ModeInfo->NumberGreenBits = 8;      /* Green pixels in DAC. */
    ModeInfo->NumberBlueBits  = 8;      /* Blue pixels in DAC. */

    /* Pixel mask depends on color depth. */
    ModeInfo->RedMask         = vmpMaskByBpp( Bpp, CHANNEL_RED );
    ModeInfo->GreenMask       = vmpMaskByBpp( Bpp, CHANNEL_GREEN );
    ModeInfo->BlueMask        = vmpMaskByBpp( Bpp, CHANNEL_BLUE );

    /* Mode attributes are only different for 8bpp modes. */
    ModeInfo->AttributeFlags      = VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR;
    if( Bpp <= 8 )
        ModeInfo->AttributeFlags |= VIDEO_MODE_PALETTE_DRIVEN | VIDEO_MODE_MANAGED_PALETTE;

    /* Strictly speaking, the following don't need to be filled out. */
    ModeInfo->VideoMemoryBitmapWidth       = 0;
    ModeInfo->VideoMemoryBitmapHeight      = 0;
    ModeInfo->DriverSpecificAttributeFlags = 0;
}

/* Main I/O request handler routine. */
BOOLEAN HwVidStartIO( PVOID HwDevExt, PVIDEO_REQUEST_PACKET ReqPkt )
{
    PHW_DEV_EXT                         pExt = HwDevExt;
    VP_STATUS                           status = NO_ERROR;
    PVIDEO_MODE_INFORMATION             modeInfo;
    PVIDEO_MEMORY                       vidMem;
    PVIDEO_SHARE_MEMORY                 pShrMem;
    PVOID                               virtualAddress;
    ULONG                               inIoSpace;
    ULONG                               modeNumber;
    ULONG                               ulLen;
    ULONG                               i;

    VideoDebugPrint( (2, "videomp: HwVidStartIO: ") );

    /* Process the VRP. Required requests are handled first. */
    switch( ReqPkt->IoControlCode ) {
    case IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES:
    {
        PVIDEO_NUM_MODES        numModes;

        VideoDebugPrint( (2, "QUERY_NUM_AVAIL_MODES\n") );        
        if( ReqPkt->OutputBufferLength < sizeof( VIDEO_NUM_MODES ) ) {
            status = ERROR_INSUFFICIENT_BUFFER;
        } else {
            ReqPkt->StatusBlock->Information = sizeof( VIDEO_NUM_MODES );
            numModes = (PVIDEO_NUM_MODES)ReqPkt->OutputBuffer;
            numModes->ModeInformationLength = sizeof( VIDEO_MODE_INFORMATION );
            numModes->NumModes = pExt->NumValidModes;
        }
        break;
    }

    case IOCTL_VIDEO_QUERY_AVAIL_MODES:
        VideoDebugPrint( (2, "QUERY_AVAIL_MODES\n") );
        ulLen = pExt->NumValidModes * sizeof( VIDEO_MODE_INFORMATION );
        if( ReqPkt->OutputBufferLength < ulLen ) {
            status = ERROR_INSUFFICIENT_BUFFER;
        } else {
            ReqPkt->StatusBlock->Information = ulLen;
            modeInfo = ReqPkt->OutputBuffer;
            for( i = 0; i < ulAllModes; ++i ) {
                if( VideoModes[i].bValid ) {
                    vmpFillModeInfo( modeInfo, VideoModes[i].HorzRes,
                                     VideoModes[i].VertRes, VideoModes[i].Bpp );
                    modeInfo->ModeIndex = i; //VideoModes[i].modeInformation.ModeIndex;

                    modeInfo++;
                }
            }
        }
        break;

    case IOCTL_VIDEO_QUERY_CURRENT_MODE:
        VideoDebugPrint( (2, "QUERY_CURRENT_MODE\n") );
        if( ReqPkt->OutputBufferLength < sizeof( VIDEO_MODE_INFORMATION ) ) {
            status = ERROR_INSUFFICIENT_BUFFER;
        } else {
            ReqPkt->StatusBlock->Information = sizeof( VIDEO_MODE_INFORMATION );
            modeInfo  = ReqPkt->OutputBuffer;
            vmpFillModeInfo( modeInfo, 
                             VideoModes[pExt->CurrentModeNumber].HorzRes,
                             VideoModes[pExt->CurrentModeNumber].VertRes,
                             VideoModes[pExt->CurrentModeNumber].Bpp );
            modeInfo->ModeIndex = pExt->CurrentModeNumber; //VideoModes[pExt->CurrentModeNumber].modeInformation.ModeIndex;
        }

        break;

    case IOCTL_VIDEO_SET_CURRENT_MODE:
        VideoDebugPrint( (2, "SET_CURRENT_MODE\n") );

        /* Ensure the mode is valid. */
        modeNumber = ((PVIDEO_MODE)(ReqPkt->InputBuffer))->RequestedMode;

        if( (modeNumber > ulAllModes) || (!VideoModes[modeNumber].bValid) ) {
            status = ERROR_INVALID_PARAMETER;
            break;
        }

        BOXV_ext_mode_set( pExt, VideoModes[modeNumber].HorzRes, 
                           VideoModes[modeNumber].VertRes, VideoModes[modeNumber].Bpp,
                           VideoModes[modeNumber].HorzRes, VideoModes[modeNumber].VertRes );

        pExt->CurrentModeNumber = modeNumber;
        break;

    case IOCTL_VIDEO_RESET_DEVICE:
        VideoDebugPrint( (2, "RESET_DEVICE\n") );
	/* Not calling the following routine avoids some visual glitches. */
        /* BOXV_ext_disable( pExt ); */
        break;

    case IOCTL_VIDEO_MAP_VIDEO_MEMORY:
    {
        PVIDEO_MEMORY_INFORMATION           memInfo;

        VideoDebugPrint( (2, "MAP_VIDEO_MEMORY\n") );
        if( (ReqPkt->OutputBufferLength < sizeof( VIDEO_MEMORY_INFORMATION )) ||
            (ReqPkt->InputBufferLength < sizeof( VIDEO_MEMORY )) ) {

            status = ERROR_INSUFFICIENT_BUFFER;
            break;
        }

        ReqPkt->StatusBlock->Information =  sizeof( VIDEO_MEMORY_INFORMATION );

        vidMem    = (PVIDEO_MEMORY)ReqPkt->InputBuffer;
        memInfo   = ReqPkt->OutputBuffer;
        inIoSpace = FALSE;

        memInfo->VideoRamBase   = vidMem->RequestedVirtualAddress;
        memInfo->VideoRamLength = pExt->FramebufLen;

        status = VideoPortMapMemory( pExt, pExt->PhysicalFrameAddress,
                                     &memInfo->VideoRamLength, &inIoSpace,
                                     &memInfo->VideoRamBase );

        /* The framebuffer covers the entire video memory. */
        memInfo->FrameBufferBase   = memInfo->VideoRamBase;
        memInfo->FrameBufferLength = memInfo->VideoRamLength;
        break;
    }

    case IOCTL_VIDEO_UNMAP_VIDEO_MEMORY:
        VideoDebugPrint( (2, "UNMAP_VIDEO_MEMORY\n") );
        if( ReqPkt->InputBufferLength < sizeof( VIDEO_MEMORY ) ) {
            status = ERROR_INSUFFICIENT_BUFFER;
        } else {
            vidMem  = (PVIDEO_MEMORY)ReqPkt->InputBuffer;
            status = VideoPortUnmapMemory( pExt,
                                           vidMem->RequestedVirtualAddress,
                                           0 );
        }
        break;

    /* The following request is required for palettized modes. */
    case IOCTL_VIDEO_SET_COLOR_REGISTERS:
    {
        PVIDEO_CLUT     clutBuffer;

        VideoDebugPrint( (2, "SET_COLOR_REGISTERS\n") );
        clutBuffer = ReqPkt->InputBuffer;

        if( (ReqPkt->InputBufferLength < sizeof( VIDEO_CLUT ) - sizeof( ULONG )) ||
            (ReqPkt->InputBufferLength < sizeof( VIDEO_CLUT ) +
                    (sizeof( ULONG ) * (clutBuffer->NumEntries - 1)) ) ) {

            status = ERROR_INSUFFICIENT_BUFFER;
            break;
        }

        if( VideoModes[pExt->CurrentModeNumber].Bpp == 8 ) {
            BOXV_dac_set( pExt, clutBuffer->FirstEntry, clutBuffer->NumEntries,
                          clutBuffer->LookupTable );
        }
        break;
    }

    /* The following requests are optional. */
    case IOCTL_VIDEO_QUERY_POINTER_CAPABILITIES:
    {
        PVIDEO_POINTER_CAPABILITIES     ptrCaps = ReqPkt->OutputBuffer;

        VideoDebugPrint( (2, "QUERY_POINTER_CAPABILITIES\n") );
        if( ReqPkt->OutputBufferLength < sizeof( VIDEO_POINTER_CAPABILITIES ) ) {
            ReqPkt->StatusBlock->Information = 0;
            status = ERROR_INSUFFICIENT_BUFFER;
        }

        ptrCaps->Flags = 0;     /* Indicate no pointer support. */
        ptrCaps->MaxWidth = ptrCaps->MaxHeight = 0;
        /* Documentation and sample code disagree on whether no display
         * memory for cursor is indicated by 0 or -1.
         */
        ptrCaps->HWPtrBitmapStart = ptrCaps->HWPtrBitmapEnd = ~0;

        ReqPkt->StatusBlock->Information = sizeof( VIDEO_POINTER_CAPABILITIES );
        break;
    }

    /* The share/unshare IOCTLs are new for NT 3.51. */
    case IOCTL_VIDEO_SHARE_VIDEO_MEMORY:
    {
        PVIDEO_SHARE_MEMORY_INFORMATION     pShrMemInfo;
        PHYSICAL_ADDRESS                    shareAddress;
        ULONG                               sharedViewSize;

        VideoDebugPrint( (2, "SHARE_VIDEO_MEMORY\n") );
        if( (ReqPkt->OutputBufferLength < sizeof( VIDEO_SHARE_MEMORY_INFORMATION )) ||
            (ReqPkt->InputBufferLength < sizeof( VIDEO_MEMORY )) ) {

            status = ERROR_INSUFFICIENT_BUFFER;
            break;
        }

        pShrMem = ReqPkt->InputBuffer;

        if( (pShrMem->ViewOffset > pExt->FramebufLen) ||
            ((pShrMem->ViewOffset + pShrMem->ViewSize) > pExt->FramebufLen) ) {

            status = ERROR_INVALID_PARAMETER;
            break;
        }

        ReqPkt->StatusBlock->Information = sizeof( VIDEO_SHARE_MEMORY_INFORMATION );

        /* The input buffer is also the output buffer; remember the input. */
        virtualAddress = pShrMem->ProcessHandle;
        sharedViewSize = pShrMem->ViewSize;

        inIoSpace = FALSE;

        /* NB: ViewOffset is not being taken into account. */
        shareAddress.QuadPart = pExt->PhysicalFrameAddress.QuadPart;

        status = VideoPortMapMemory( pExt, shareAddress,
                                     &sharedViewSize, &inIoSpace,
                                     &virtualAddress );

        pShrMemInfo = ReqPkt->OutputBuffer;
        pShrMemInfo->SharedViewOffset = pShrMem->ViewOffset;
        pShrMemInfo->VirtualAddress   = virtualAddress;
        pShrMemInfo->SharedViewSize   = sharedViewSize;
        break;
    }

    case IOCTL_VIDEO_UNSHARE_VIDEO_MEMORY:
        VideoDebugPrint( (2, "UNSHARE_VIDEO_MEMORY\n") );
        if( ReqPkt->InputBufferLength < sizeof( VIDEO_SHARE_MEMORY ) ) {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
        }

        pShrMem = ReqPkt->InputBuffer;
        status = VideoPortUnmapMemory( pExt, pShrMem->RequestedVirtualAddress,
                                       pShrMem->ProcessHandle );
        break;


    /* The child state IOCTLs are new for NT 5.0 (Windows 2000). */
    case IOCTL_VIDEO_GET_CHILD_STATE:
    {
        PULONG      pChildIndex;
        PULONG      pChildState;

        VideoDebugPrint( (2, "GET_CHILD_STATE\n") );
        if( ReqPkt->InputBufferLength < sizeof( ULONG ) ||
            ReqPkt->OutputBufferLength < sizeof( ULONG ) ) {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
        }

        pChildIndex = ReqPkt->InputBuffer;
        pChildState = ReqPkt->OutputBuffer;

        /* Always say the child is active. */
        *pChildState = VIDEO_CHILD_ACTIVE;
        break;
    }

    /* Any other request is invalid and fails. */
    default:
        VideoDebugPrint( (1, "Unhandled IoControlCode %08x!\n", ReqPkt->IoControlCode) );
        status = ERROR_INVALID_FUNCTION;
        break;
    }

    ReqPkt->StatusBlock->Status = status;
    return( TRUE );
}

/* Validate support for a requested power state. */
VP_STATUS HwGetPowerState( PVOID HwDevExt, ULONG HwId,
                           PVIDEO_POWER_MANAGEMENT VideoPowerControl )
{
    VideoDebugPrint( (1, "videomp: HwGetPowerState\n") );
    return( NO_ERROR );
}

/* Set the device power state. */
VP_STATUS HwSetPowerState( PVOID HwDevExt, ULONG HwId,
                           PVIDEO_POWER_MANAGEMENT VideoPowerControl )
{
    VideoDebugPrint( (1, "videomp: HwSetPowerState\n") );
    return( NO_ERROR );
}

/* Return child device descriptors. In this case just a single monitor. */
VP_STATUS HwGetChildDesc( PVOID HwDevExt, PVIDEO_CHILD_ENUM_INFO ChildEnumInfo,
                          PVIDEO_CHILD_TYPE VideoChildType, PUCHAR pChildDescriptor,
                          PULONG UId, PULONG pUnused )
{
    PHW_DEV_EXT     pExt = HwDevExt;

    VideoDebugPrint( (1, "videomp: HwGetChildDesc\n") );

    if( ChildEnumInfo->ChildIndex > 0 ) {
        if( (int)ChildEnumInfo->ChildIndex <= pExt->NumMonitors ) {
            *VideoChildType = Monitor;
            *UId = ChildEnumInfo->ChildIndex;
            return( VIDEO_ENUM_MORE_DEVICES );
        }
    }

    return( ERROR_NO_MORE_DEVICES );
}

/* Reset the adapter into a VGA-compatible state. */
BOOLEAN HwVidResetHw( PVOID HwDevExt, ULONG Columns, ULONG Rows )
{
    PHW_DEV_EXT     pExt = HwDevExt;

    VideoDebugPrint( (1, "videomp: HwVidResetHw\n") );

    BOXV_ext_disable( pExt );
    /* Indicate that we didn't actually set the requested text mode. */
    return( FALSE );
}

/* Standard NT driver initialization entry point. */
ULONG DriverEntry( PVOID Context1, PVOID Context2 )
{
    VIDEO_HW_INITIALIZATION_DATA    hwInitData;
    ULONG                           status;

    VideoDebugPrint( (1, "videomp: DriverEntry\n") );

    /* Prepare the initialization structure. */
    VideoPortZeroMemory( &hwInitData, sizeof( VIDEO_HW_INITIALIZATION_DATA ) );
    hwInitData.HwInitDataSize = sizeof( VIDEO_HW_INITIALIZATION_DATA );

    /* Set up driver callbacks. */
    hwInitData.HwFindAdapter = HwVidFindAdapter;
    hwInitData.HwInitialize  = HwVidInitialize;
    hwInitData.HwStartIO     = HwVidStartIO;
    /* There's no interrupt or timer callback. */
    hwInitData.HwInterrupt   = NULL;
    hwInitData.HwTimer       = NULL;
    hwInitData.HwResetHw     = HwVidResetHw;

    /* Power and child device management callbacks were added in NT 5.0. */
    hwInitData.HwGetPowerState           = HwGetPowerState;
    hwInitData.HwSetPowerState           = HwSetPowerState;
    hwInitData.HwGetVideoChildDescriptor = HwGetChildDesc;

    /* Report legacy resources. */
    hwInitData.HwLegacyResourceList  = LegacyRanges;
    hwInitData.HwLegacyResourceCount = ulNumLegacyRanges;

    /* Report the device extension size. */
    hwInitData.HwDeviceExtensionSize = sizeof( HW_DEV_EXT );

    /* Refer to the CurrentControlSet\Services\xxx\Device0 registry key. */
    hwInitData.StartingDeviceNumber = 0;

    /* Later NT versions support PCI; recent versions ignore this entirely */
    hwInitData.AdapterInterfaceType = PCIBus;

    /* The PsGetVersion function was not available in NT 3.x. We therefore 
     * implement a poor man's version detection by successively reducing the
     * HwInitDataSize until the video miniport (we hope) accepts it.
     */
    do {
        /* First try with NT 5.1 (Windows XP) structure size. */
        VideoDebugPrint( (1, "videomp: Trying DDI 5.1 HwInitDataSize\n") );
        hwInitData.HwInitDataSize = SIZE_OF_WXP_VIDEO_HW_INITIALIZATION_DATA;
        PortVersion = VP_VER_XP;
        status = VideoPortInitialize( Context1, Context2, &hwInitData, NULL );
        if( status != STATUS_REVISION_MISMATCH ) {
            /* If status is anything other than a version mismatch, don't
             * try calling VideoPortInitialize again. The call may have
             * succeeded, or it failed for some reason whe can't easily fix.
             */
            break;
        }

        /* Try the NT 5.0 (Windows 2000) structure size. */
        VideoDebugPrint( (1, "videomp: Trying DDI 5.0 HwInitDataSize\n") );
        hwInitData.HwInitDataSize = SIZE_OF_W2K_VIDEO_HW_INITIALIZATION_DATA;
        PortVersion = VP_VER_W2K;
        status = VideoPortInitialize( Context1, Context2, &hwInitData, NULL );
        if( status != STATUS_REVISION_MISMATCH ) {
            break;
        }

        /* Try the NT 4.0 (and also NT 3.51) structure size. */
        VideoDebugPrint( (1, "videomp: Trying DDI 4.0 HwInitDataSize\n") );
        hwInitData.HwInitDataSize = SIZE_OF_NT4_VIDEO_HW_INITIALIZATION_DATA;
        PortVersion = VP_VER_NT4;
        status = VideoPortInitialize( Context1, Context2, &hwInitData, NULL );
        if( status != STATUS_REVISION_MISMATCH ) {
            break;
        }

        /* Try the original NT 3.1/3.5 HwInitDataSize. No PCI support. */
        VideoDebugPrint( (1, "videomp: Trying DDI 3.1 HwInitDataSize\n") );
        hwInitData.HwInitDataSize = offsetof( VIDEO_HW_INITIALIZATION_DATA, HwResetHw );
        hwInitData.AdapterInterfaceType = Isa;
        PortVersion = VP_VER_NT31;
        status = VideoPortInitialize( Context1, Context2, &hwInitData, NULL );
    } while( 0 );
    VideoDebugPrint( (1, "videomp: VideoPortInitialize rc=%08x\n", status ) );
    return( status );
}
