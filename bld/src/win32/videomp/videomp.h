/*
 * Internal video miniport interfaces.
 */

/* Video Port versions that affect miniport behavior. */

#define VP_VER_NT31     0x030100
#define VP_VER_NT4      0x040000        /* Also NT 3.51 */
#define VP_VER_W2K      0x050000
#define VP_VER_XP       0x050100

/* Video mode description structure */
typedef struct {
    USHORT              HorzRes;                /* Horizontal resolution */
    USHORT              VertRes;                /* Vertical resolution */
    UCHAR               Bpp;                    /* Bits per pixel */
    BOOLEAN             bValid;                 /* Valid mode flag */
} VIDEOMP_MODE, *PVIDEOMP_MODE;

/* The device extension - private miniport data */
typedef struct {
    PVOID               IoPorts;                /* I/O ports mapping */
    PVOID               FrameAddress;           /* Framebuffer mapping */
    PHYSICAL_ADDRESS    PhysicalFrameAddress;   /* Physical FB address */
    ULONG               FramebufLen;            /* Framebuffer length */
    ULONG               CurrentModeNumber;      /* Current mode index */
    ULONG               NumValidModes;          /* Number of valid modes */
    ULONG               NumMonitors;            /* Number of attached monitors */
    PUCHAR              IOAddrVGA;              /* VGA I/O ports mapping */
    ULONG               ulSlot;                 /* PCI slot the adapter is in. */
    ULONG               PortVersion;            /* Video Port version */
} HW_DEV_EXT, *PHW_DEV_EXT;

/* Variables defined in vidmpdat.c */
extern VIDEOMP_MODE         VideoModes[];
extern ULONG                ulAllModes;
extern VIDEO_ACCESS_RANGE   LegacyRanges[];
extern ULONG                ulNumLegacyRanges;
extern ULONG                PortVersion;
