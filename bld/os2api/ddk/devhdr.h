/*
 * Device driver header
 */
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

/*
 * Device driver header
 */
typedef struct DEVHEADER DEVHEADER;
typedef struct DEVHEADER {
  DEVHEADER FAR *next;           // next driver in chain
  USHORT         DAWFlags;       // device attribute word
  NPVOID         StrategyEntry;  // offset to strategy routine
  NPVOID         IDCEntry;       // offset to IDC routine
  CHAR           Name[8];        // driver name
  CHAR           Reserved[8];    // Reserved
  ULONG          Capabilities;   // Capabilities bit strip
};

/*
 * Marks end of DEVHEADER chain
 */
#define       FENCE       ((DEVHEADER FAR*)0xFFFFFFFFul)

/*
 * Constants for device attribute word
 */
typedef enum{
    DAW_CHARACTER = 0x8000,     // Character device
    DAW_IDC       = 0x4000,     // IDC aware device
    DAW_NONIBM    = 0x2000,     // Non-IBM Block device
    DAW_SHARE     = 0x1000,     // Sharable device
    DAW_OPENCLOSE = 0x0800,     // Requires open and close requests
    DAW_LEVEL3    = 0x0180,     // Level 3 device
    DAW_LEVEL2    = 0x0100,     // Level 2 device
    DAW_LEVEL1    = 0x0080,     // Level 1 device
    DAW_CLOCK     = 0x0008,     // Clock device
    DAW_NUL       = 0x0004,     // Nul device
    DAW_STDOUT    = 0x0002,     // Standard output device
    DAW_STDIN     = 0x0001,     // Standard input device
} DAW_flags;

/*
 * Constants for capabilities bit strip (used by level 3 devices)
 */
typedef enum {
    CAP_COMPLETE  = 0x00000010, // Supports Init Complete request
    CAP_ADD       = 0x00000008, // Participates in ADD strategy
    CAP_PARALLEL  = 0x00000004, // Supports parallel ports
    CAP_32BIT     = 0x00000002, // Supports addressing above 16MB
    CAP_SHUTDOWN  = 0x00000001, // Supports IOCtl2 and shutdown
    CAP_NULL      = 0x00000000, //
} CAP_flags;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
