extern ULONG  DevHlp;

/*
 * Look at \WATCOM\H\OS2\DEVHLP.INC for the list of
 * definitions for device driver helper functions
 */

USHORT DevSetTimer            // Register a timer handler
  (
  NPVOID      Handler         // Handler function entry-point
  );                          // Returns: 0 = success, other = error code

#pragma aux DevSetTimer = \
    "mov dl,1Dh"            \
    "call dword ptr DevHlp" \
    "setc dl"               \
    "xor dh,dh"             \
    "neg dx"                \
    "and ax,dx"             \
    __modify [__dx]         \
    __parm [__ax]           \
    __value [__ax]

USHORT DevPhysToVirt          // Convert a physical address to a 16:16 address
  (
  ULONG     PhysAddr,         // Physical address to convert
  USHORT    Length,           // Length of virtual segment
  PVOID     VirtAddr          // Storage for virtual address
  );                          // Returns: 0 = success, other = error code

#pragma aux DevPhysToVirt = \
    "xchg ax,bx"            \
    "mov dx,0115h"          \
    "call dword ptr DevHlp" \
    "setc dl"               \
    "xor dh,dh"             \
    "neg dx"                \
    "mov fs:[si][0],di"     \
    "mov fs:[si][2],es"     \
    "and ax,dx"             \
    __modify [__dx __es __di] \
    __parm [__bx __ax] [__cx] [__fs __si] \
    __value [__ax]

