#ifndef PCI_H_INCLUDED
#define PCI_H_INCLUDED

/*
 * Following definitions correspond directly to the PCI Specification
 */

/* Byte offsets into PCI configuration register */
#define PCI_VENDORID        0x00
#define PCI_DEVICEID        0x02
#define PCI_COMMAND         0x04
#define PCI_STATUS          0x06
#define PCI_REVISION_ID     0x08
#define PCI_CLASS           0x09
#define PCI_CACHE_LINSZ     0x0c
#define PCI_LATENCY         0x0d
#define PCI_HEADER_TYPE     0x0e
#define PCI_BIST            0x0f
#define PCI_BASE0           0x10
#define PCI_BASE1           0x14
#define PCI_BASE2           0x18
#define PCI_BASE3           0x1c
#define PCI_BASE4           0x20
#define PCI_BASE5           0x24
#define PCI_CB_CIS          0x28
#define PCI_SUBSYS_VENDORID 0x2c
#define PCI_SUBSYS          0x2e
#define PCI_ROM_BASE        0x30
#define PCI_INT_LINE        0x3c
#define PCI_INT_PIN         0x3d

/* PCI Command Register bit definitions */
typedef enum {
    PCI_CMD_IO_ACCESS         = 0x0001,
    PCI_CMD_MEM_ACCESS        = 0x0002,
    PCI_CMD_MASTER            = 0x0004,
    PCI_CMD_SPEC_CICLE_REC    = 0x0008,
    PCI_CMD_MEM_WR_INVALIDATE = 0x0010,
    PCI_CMD_VGA_PALETE_SNOOP  = 0x0020,
    PCI_CMD_PARITY_ERROR      = 0x0040,
    PCI_CMD_WAIT_CYCLE        = 0x0080,
    PCI_CMD_SYSTEM_ERROR      = 0x0100,
    PCI_CMD_FAST_BACK2BACK    = 0x0200
} PCI_Cmd_bits;

/* PCI Status Register bit defintions */
typedef enum {
    PCI_STATUS_66MHz                = 0x0020,
    PCI_STATUS_UDF_SUPPORT          = 0x0040,
    PCI_STATUS_FAST_BACK2BACK       = 0x0080,
    PCI_STATUS_DATA_PARITY          = 0x0100,
    PCI_STATUS_DEV_TIMING_MASK      = 0x0600,
    PCI_STATUS_SIGNAL_TARGET_ABORT  = 0x0800,
    PCI_STATUS_RECEIVE_TARGET_ABORT = 0x1000,
    PCI_STATUS_RECEIVE_MASTER_ABORT = 0x2000,
    PCI_STATUS_SIGNAL_SYSTEM_ERROR  = 0x4000,
    PCI_STATUS_PARITY_ERROR         = 0x8000
} PCI_Status_bits;

/* PCI Class/Subclass/Interface masks */
#define PCI_CLASS_CODE_MASK     0xff0000
#define PCI_SUB_CLASS_CODE_MASK 0x00ff00
#define PCI_PROG_IF_MASK        0x0000ff

/* PCI Device Class definitions */
typedef enum {
    PCI_CLASS_STORAGE_CONTROLLER    = 0x010000,
    PCI_CLASS_NETWORK_CONTROLLER    = 0x020000,
    PCI_CLASS_DISPLAY_CONTROLLER    = 0x030000,
    PCI_CLASS_MULTIMEDIA_DEVICE     = 0x040000,
    PCI_CLASS_MEMORY_CONTROLLER     = 0x050000,
    PCI_CLASS_BRIDGE_DEVICE         = 0x060000,
    PCI_CLASS_COMM_CONTROLLER       = 0x070000,
    PCI_CLASS_BASE_PERIPHERIAL      = 0x080000,
    PCI_CLASS_INPUT_DEVICE          = 0x090000,
    PCI_CLASS_DOCKING_STATION       = 0x0a0000,
    PCI_CLASS_PROCESSOR             = 0x0b0000,
    PCI_CLASS_SERIAL_BUS_CONTROLLER = 0x0c0000
} PCI_Class;

/* PCI Video Subclass definitions */
typedef enum {
    PCI_SCLASS_VID_NONVGA = 0x000000,
    PCI_SCLASS_VID_VGA    = 0x000100
} PCI_Video_scls;

/* PCI Storage Subclass definitions */
typedef enum {
    PCI_SCLASS_STORAGE_SCSI   = 0x000000,
    PCI_SCLASS_STORAGE_IDE    = 0x000100,
    PCI_SCLASS_STORAGE_FLOPPY = 0x000200,
    PCI_SCLASS_STORAGE_IPI    = 0x000300,
    PCI_SCLASS_STORAGE_RAID   = 0x000400
} PCI_Storage_scls;

/* PCI Network Subclass definitions */
typedef enum {
    PCI_SCLASS_NETWORK_ETHERNET = 0x000000,
    PCI_SCLASS_NETWORK_TR       = 0x000100,
    PCI_SCLASS_NETWORK_FDDI     = 0x000200,
    PCI_SCLASS_NETWORK_ATM      = 0x000300
} PCI_Network_scls;

/* PCI Display Controller Subclass/Interface definitions */
typedef enum {
    PCI_SCLASS_DISPLAY_VGA  = 0x000000,
    PCI_SCLASS_DISPLAY_8514 = 0x000000,
        PCI_IF_DISPLAY_VGA  = 0x000000,
        PCI_IF_DISPLAY_1514 = 0x000001,
    PCI_SCLASS_DISPLAY_XGA  = 0x000100
} PCI_Display_scls;

/* PCI Multimedia Subclass definitions */
typedef enum {
    PCI_SCLASS_MMEDIA_VIDEO = 0x000000,
    PCI_SCLASS_MMEDIA_AUDIO = 0x000100
} PCI_MMedia_scls;

/* PCI Memory Controller Subclass definitions */
typedef enum {
    PCI_SCLASS_MEMORY_RAM   = 0x000000,
    PCI_SCLASS_MEMORY_FLASH = 0x000100
} PCI_Memory_scls;

/* PCI Bridge Subclass definitions */
typedef enum {
    PCI_SCLASS_BRIDGE_HOST    = 0x000000,
    PCI_SCLASS_BRIDGE_ISA     = 0x000100,
    PCI_SCLASS_BRIDGE_EISA    = 0x000200,
    PCI_SCLASS_BRIDGE_MC      = 0x000300,
    PCI_SCLASS_BRIDGE_PCI     = 0x000400,
    PCI_SCLASS_BRIDGE_PCMCIA  = 0x000500,
    PCI_SCLASS_BRIDGE_NUBUS   = 0x000600,
    PCI_SCLASS_BRIDGE_CARDBUS = 0x000700
} PCI_Bridge_scls;

/* PCI Communications Device Subclass/Interface definitions */
typedef enum {
    PCI_SCLASS_COMM_SERIAL   = 0x000000,
        PCI_IF_COMM_XT       = 0x000000,
        PCI_IF_COMM_16450    = 0x000001,
        PCI_IF_COMM_16550    = 0x000002,
    PCI_SCLASS_COMM_PARALLEL = 0x000100,
        PCI_IF_COMM_PARALLEL = 0x000000,
        PCI_IF_COMM_BIDIRECT = 0x000001,
        PCI_IF_COMM_ECP      = 0x000002
} PCI_Comms_scls;

/* PCI Base System Peripherials Subclass/Interface definitions */
typedef enum {
    PCI_SCLASS_PERIPHERIAL_PIC     = 0x000000,
    PCI_SCLASS_PERIPHERIAL_DMA     = 0x000100,
    PCI_SCLASS_PERIPHERIAL_TIMER   = 0x000200,
    PCI_SCLASS_PERIPHERIAL_RTC     = 0x000300,
        PCI_IF_PERIPHERIAL_GENERIC = 0x000000,
        PCI_IF_PERIPHERIAL_ISA     = 0x000001,
        PCI_IF_PERIPHERIAL_EISA    = 0x000002
} PCI_Periph_scls;

/* PCI Input Device Subclass definitions */
typedef enum {
    PCI_SCLASS_INPUT_KEYBOARD = 0x000000,
    PCI_SCLASS_INPUT_PEN      = 0x000100,
    PCI_SCLASS_INPUT_MOUSE    = 0x000200
} PCI_Input_scls;

/* PCI Processor Subclass definitions */
typedef enum {
    PCI_SCLASS_PROCESSOR_386     = 0x000000,
    PCI_SCLASS_PROCESSOR_486     = 0x000100,
    PCI_SCLASS_PROCESSOR_PENTIUM = 0x000200,
    PCI_SCLASS_PROCESSOR_ALPHA   = 0x001000,
    PCI_SCLASS_PROCESSOR_POWERPC = 0x002000,
    PCI_SCLASS_COPROCESSOR       = 0x004000
} PCI_Proc_scls;

/* PCI Serial Bus Controller Subclass definitions */
typedef enum {
    PCI_SCLASS_SERIALBUS_IEEE1394 = 0x000000,
    PCI_SCLASS_SERIALBUS_ACCESS   = 0x000100,
    PCI_SCLASS_SERIALBUS_SSA      = 0x000200,
    PCI_SCLASS_SERIALBUS_USB      = 0x000300,
    PCI_SCLASS_SERIALBUS_FIBRE    = 0x000400
} PCI_Serial_scls;

/* PCI Memory vs. I/O address range bits */
#define    PCI_BASE_IO  1
#define    PCI_BASE_MEM 0


/*
 * Following definitions are OS/2 specific
 */

/* OEMHLP PCI IOCtl Subfunctions */
#define OEMHLP_QUERY_PCI_BIOS       0x00
#define OEMHLP_FIND_PCI_DEVICE      0x01
#define OEMHLP_FIND_PCI_CLASS       0x02
#define OEMHLP_READ_PCI_CONFIG      0x03
#define OEMHLP_WRITE_PCI_CONFIG     0x04

#pragma pack( push, 1 )

typedef struct _PCI_PARM {
    UCHAR   SubFunc;
    union {
        struct {
            USHORT  DeviceID;
            USHORT  VendorID;
            UCHAR   Index;
        } FindDevice;
        struct {
            ULONG   Class;
            UCHAR   Index;
        } FindClass;
        struct {
            UCHAR   BusNum;
            UCHAR   DevFunc;
            UCHAR   CfgReg;
            UCHAR   Size;
        } ReadCfg;
        struct {
            UCHAR   BusNum;
            UCHAR   DevFunc;
            UCHAR   CfgReg;
            UCHAR   Size;
            ULONG   Data;
        } WriteCfg;
    };
} PCI_PARM;

typedef struct _PCI_DATA {
    UCHAR   Return;
    union {
        struct {
            UCHAR   HWMech;
            UCHAR   MajorVer;
            UCHAR   MinorVer;
            UCHAR   LastBus;
        } BIOSInfo;
        struct {
            UCHAR   BusNum;
            UCHAR   DevFunc;
        } FindDevice;
        struct {
            ULONG   Data;
        } ReadCfg;
    };
} PCI_DATA;

#pragma pack( pop )

#endif
