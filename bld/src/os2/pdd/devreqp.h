/*
 * Some type definitions for OS/2 2.x/3.x  driver request packets.
 */

#ifdef __cplusplus
extern "C" {
#endif
#if __WATCOMC__ >= 1000
#pragma pack(push,1);
#else
#pragma pack(1);
#endif

/*
 * Look at \WATCOM\H\OS2\DEVSYM.INC for a description
 * of the Device Driver Request Packet Definitions
 */

typedef struct REQP_HEADER REQP_HEADER;
struct REQP_HEADER {
    BYTE                length;         // Length of request packet
    BYTE                unit;           // Unit code (B)
    BYTE                command;        // Command code
    USHORT              status;         // Status code
    ULONG               res1;           // Flags
    REQP_HEADER FAR    *next;           // Link to next request packet in queue
};

typedef struct {
  REQP_HEADER header;
  union{
    struct{
        BYTE    res;            // Unused
        ULONG   devhlp;         // Address of Dev Help entry point
        PCHAR   parms;          // Command-line arguments
        BYTE    drive;          // Drive number of first unit
    } in;
    struct  {
        BYTE    units;          // Number of supported units
        USHORT  finalcs;        // Offset to end of code
        USHORT  finalds;        // Offset of end of data
        PVOID   bpb;            // BIOS parameter block
    } out;
  };
} REQP_INIT;

typedef struct {
    REQP_HEADER header;
    BYTE        media;          // Media descriptor
    ULONG       transaddr;      // Transfer physical address
    USHORT      count;          // bytes/sectors count
    ULONG       start;          // Starting sector for block device
    HFILE       fileid;         // System file number
} REQP_RWV;

typedef struct{
    REQP_HEADER header;
    HFILE       fileid;         // System file number
} REQP_OPENCLOSE;

typedef enum {
    RPERR               =   0x8000,     // Error
    RPDEV               =   0x4000,     // Device-specific error code
    RPBUSY              =   0x0200,     // Device is busy
    RPDONE              =   0x0100,     // Command complete
    RPERR_PROTECT       =   0x8000,     // Write-protect error
    RPERR_UNIT          =   0x8001,     // Unknown unit
    RPERR_READY         =   0x8002,     // Not ready
    RPERR_COMMAND       =   0x8003,     // Unknown command
    RPERR_CRC           =   0x8004,     // CRC error
    RPERR_LENGTH        =   0x8005,     // Bad request length
    RPERR_SEEK          =   0x8006,     // Seek error
    RPERR_FORMAT        =   0x8007,     // Unknown media
    RPERR_SECTOR        =   0x8008,     // Sector not found
    RPERR_PAPER         =   0x8009,     // Out of paper
    RPERR_WRITE         =   0x800A,     // Write fault
    RPERR_READ          =   0x800B,     // Read fault
    RPERR_GENERAL       =   0x800C,     // General failure
    RPERR_DISK          =   0x800D,     // Disk change
    RPERR_MEDIA         =   0x8010,     // Uncertain media
    RPERR_INTERRUPTED   =   0x8011,     // Call interrupted (character)
    RPERR_MONITOR       =   0x8012,     // Monitors unsupported
    RPERR_PARAMETER     =   0x8013,     // Invalid parameter
    RPERR_USE           =   0x8014,     // Device in use
    RPERR_INIT          =   0x8015,     // Non-critical init failure
} REQP_status;

typedef union {
    REQP_HEADER         header;
    REQP_INIT           init;
    REQP_RWV            rwv;
    REQP_OPENCLOSE      openclose;
} REQP_ANY;

/*
 * Look at \WATCOM\H\OS2\DEVCMD.INC for a list
 * of the Device Driver Request Packet Commands
 */

typedef enum {
     RP_INIT            =   0x00,
     RP_MEDIA_CHECK     =   0x01,
     RP_BUILD_BPB       =   0x02,
     RP_READ            =   0x04,
     RP_READ_NO_WAIT    =   0x05,
     RP_INPUT_STATUS    =   0x06,
     RP_INPUT_FLUSH     =   0x07,
     RP_WRITE           =   0x08,
     RP_WRITE_VERIFY    =   0x09,
     RP_OUTPUT_STATUS   =   0x0a,
     RP_OUTPUT_FLUSH    =   0x0b,
     RP_OPEN            =   0x0d,
     RP_CLOSE           =   0x0e,
     RP_REMOVABLE       =   0x0f,
     RP_IOCTL           =   0x10,
     RP_RESET           =   0x11,
     RP_GET_DRIVE_MAP   =   0x12,
     RP_SET_DRIVE_MAP   =   0x13,
     RP_DEINSTALL       =   0x14,
     RP_PARTITIONABLE   =   0x16,
     RP_GET_FIXED_MAP   =   0x17,
     RP_SHUTDOWN        =   0x1c,
     RP_GET_DRIVER_CAPS =   0x1d,
     RP_END
} REQP_command;

#if __WATCOMC__ >= 1000
#pragma pack(pop);
#else
#pragma pack();
#endif
#ifdef __cplusplus
};
#endif
