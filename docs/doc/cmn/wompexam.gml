.if '&ccmd' eq 'wcc' .do begin
.code begin
&prompt.wcc calendar
&prompt.womp &sw.f- &sw.dx calendar
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting hello.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Small
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : C
.code end
.code begin
&prompt.wcc &sw.d1 calendar
&prompt.womp &sw.f- &sw.dx calendar
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting hello.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Small
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : C
.code end
.code begin
&prompt.wcc &sw.d2 calendar
&prompt.womp &sw.f- &sw.dx calendar
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting hello.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Small
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : C
.code break
                                Type Information
                                ----------------
0x0000: RESERVED
.code break
0x0001: RESERVED
.code break
0x0002: INTEGER size 0x00000008 bits SIGNED
.code break
0x0003: INTEGER size 0x00000008 bits UNSIGNED
.code break
0x0004: INTEGER size 0x00000010 bits SIGNED
.code break
0x0005: INTEGER size 0x00000010 bits UNSIGNED
.code break
0x0006: INTEGER size 0x00000020 bits SIGNED
.code break
0x0007: INTEGER size 0x00000020 bits UNSIGNED
.code break
0x0008: REAL size 0x00000020 bits
.code break
0x0009: REAL size 0x00000040 bits
.code break
0x000a: VOID
.code break
0x000b: POINTER size 0x00000010 bits base_type 0x0003 NEAR
.code break
0x000c: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"
.code break
0x000d: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"
.code break
0x000e: POINTER size 0x00000010 bits base_type 0x0002 NEAR
.code break
0x000f: PROCEDURE
    NEAR ret_type 0x000e num_parms 00
.code break
0x0010: POINTER size 0x00000010 bits base_type 0x000c NEAR
.code break
0x0011: ARRAY_ZERO size 0x00000010 bits
    base_type 0x0010 high_bound 0x00000000
.code break
0x0012: STRUCT size 0x00000070 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x0010 name "_ptr"
    bit_offset 0x00000010 type 0x0004 name "_cnt"
    bit_offset 0x00000020 type 0x0010 name "_base"
    bit_offset 0x00000030 type 0x0005 name "_flag"
    bit_offset 0x00000040 type 0x0004 name "_handle"
    bit_offset 0x00000050 type 0x0005 name "_bufsize"
    bit_offset 0x00000060 type 0x0003 name "_ungotten"
    bit_offset 0x00000068 type 0x0003 name "_tmpfchar"
.code break
0x0013: TYPEDEF size 0x00000070 bits
    scope struct type 0x0012 name "__iobuf"
.code break
0x0014: ARRAY_ZERO size 0x000008c0 bits
    base_type 0x0013 high_bound 0x00000013
.code break
0x0015: ARRAY_ZERO size 0x00000140 bits
    base_type 0x0005 high_bound 0x00000013
.code break
0x0016: POINTER size 0x00000010 bits base_type 0x0013 NEAR
.code break
0x0017: POINTER size 0x00000010 bits base_type 0x000d NEAR
.code break
0x0018: POINTER size 0x00000010 bits base_type 0x0006 NEAR
.code break
0x0019: POINTER size 0x00000010 bits base_type 0x000a NEAR
.code break
0x001a: POINTER size 0x00000010 bits base_type 0x0006 NEAR
.code break
0x001b: POINTER size 0x00000010 bits base_type 0x000a NEAR
.code break
0x001c: POINTER size 0x00000010 bits base_type 0x0010 NEAR
.code break
0x001d: ARRAY_ZERO size 0x00000070 bits
    base_type 0x0003 high_bound 0x0000000d
.code break
0x001e: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "fpos_t"
.code break
0x001f: TYPEDEF size 0x00000010 bits
    scope none type 0x0011 name "__va_list"
.code break
0x0020: TYPEDEF size 0x00000010 bits
    scope none type 0x0005 name "size_t"
.code break
0x0021: TYPEDEF size 0x00000070 bits
    scope none type 0x0013 name "FILE"
.code break
0x0022: PROCEDURE
    NEAR ret_type 0x000a num_parms 00
.code break
                               Symbol Information
                               ------------------
CHANGE_SEG
    data 00 00 00 00
    fixup SEG_REL  POINTER  Frame: TARGET  Target: SI(0x0001)
BLOCK name "main" type 0x0022 start_offset 0x00000000 size 0x00000017
    PROLOG size 09 ret_addr_offset 0x00000002
    EPILOG size 04
BLOCK_END
.code end
.do end
.if '&ccmd' eq 'wcc386' .do begin
.code begin
&prompt.wcc386 calendar
&prompt.womp &sw.f- &sw.dx calendar
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting calendar.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : C
.code end
.code begin
&prompt.wcc386 &sw.d1 calendar
&prompt.womp &sw.f- &sw.dx calendar
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting calendar.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : C
.code end
.code begin
&prompt.wcc &sw.d2 calendar
&prompt.womp &sw.f- &sw.dx calendar
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting calendar.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : C

                                Type Information
                                ----------------
0x0000: RESERVED

0x0001: RESERVED

0x0002: INTEGER size 0x00000008 bits SIGNED

0x0003: INTEGER size 0x00000008 bits UNSIGNED

0x0004: INTEGER size 0x00000010 bits SIGNED

0x0005: INTEGER size 0x00000010 bits UNSIGNED

0x0006: INTEGER size 0x00000020 bits SIGNED

0x0007: INTEGER size 0x00000020 bits UNSIGNED

0x0008: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"

0x0009: TYPEDEF size 0x00000008 bits
    scope none type 0x0003 name "char"

0x000a: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "size_t"

0x000b: POINTER size 0x00000020 bits base_type 0x0008 NEAR 386

0x000c: ARRAY_ZERO size 0x00000020 bits
    base_type 0x000b high_bound 0x00000000

0x000d: TYPEDEF size 0x00000020 bits
    scope none type 0x000c name "__va_list"

0x000e: STRUCT size 0x000000d0 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x000b name "_ptr"
    bit_offset 0x00000020 type 0x0006 name "_cnt"
    bit_offset 0x00000040 type 0x000b name "_base"
    bit_offset 0x00000060 type 0x0007 name "_flag"
    bit_offset 0x00000080 type 0x0006 name "_handle"
    bit_offset 0x000000a0 type 0x0007 name "_bufsize"
    bit_offset 0x000000c0 type 0x0003 name "_ungotten"
    bit_offset 0x000000c8 type 0x0003 name "_tmpfchar"

0x000f: TYPEDEF size 0x000000d0 bits
    scope struct type 0x000e name "__iobuf"

0x0010: TYPEDEF size 0x000000d0 bits
    scope none type 0x000f name "FILE"

0x0011: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "fpos_t"

0x0012: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "fpos_t"

0x0013: TYPEDEF size 0x00000010 bits
    scope none type 0x0005 name "wchar_t"

0x0014: STRUCT size 0x00000040 bits num_entries 0x0002
    bit_offset 0x00000000 type 0x0006 name "quot"
    bit_offset 0x00000020 type 0x0006 name "rem"

0x0015: TYPEDEF size 0x00000040 bits
    scope none type 0x0014 name "div_t"

0x0016: STRUCT size 0x00000040 bits num_entries 0x0002
    bit_offset 0x00000000 type 0x0006 name "quot"
    bit_offset 0x00000020 type 0x0006 name "rem"

0x0017: TYPEDEF size 0x00000040 bits
    scope none type 0x0016 name "ldiv_t"

0x0018: TYPEDEF size 0x00000010 bits
    scope none type 0x0005 name "wchar_t"

0x0019: VOID

0x001a: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 00

0x001b: POINTER size 0x00000020 bits base_type 0x001a NEAR 386

0x001c: TYPEDEF size 0x00000020 bits
    scope none type 0x001b name "onexit_t"

0x001d: TYPEDEF size 0x00000020 bits
    scope none type 0x0006 name "ptrdiff_t"

0x001e: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "time_t"

0x001f: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "clock_t"

0x0020: STRUCT size 0x00000120 bits num_entries 0x0009
    bit_offset 0x00000000 type 0x0006 name "tm_sec"
    bit_offset 0x00000020 type 0x0006 name "tm_min"
    bit_offset 0x00000040 type 0x0006 name "tm_hour"
    bit_offset 0x00000060 type 0x0006 name "tm_mday"
    bit_offset 0x00000080 type 0x0006 name "tm_mon"
    bit_offset 0x000000a0 type 0x0006 name "tm_year"
    bit_offset 0x000000c0 type 0x0006 name "tm_wday"
    bit_offset 0x000000e0 type 0x0006 name "tm_yday"
    bit_offset 0x00000100 type 0x0006 name "tm_isdst"

0x0021: TYPEDEF size 0x00000120 bits
    scope struct type 0x0020 name "tm"

0x0022: STRUCT size 0x00000120 bits num_entries 0x0009
    bit_offset 0x00000000 type 0x0006 name "tm_sec"
    bit_offset 0x00000020 type 0x0006 name "tm_min"
    bit_offset 0x00000040 type 0x0006 name "tm_hour"
    bit_offset 0x00000060 type 0x0006 name "tm_mday"
    bit_offset 0x00000080 type 0x0006 name "tm_mon"
    bit_offset 0x000000a0 type 0x0006 name "tm_year"
    bit_offset 0x000000c0 type 0x0006 name "tm_wday"
    bit_offset 0x000000e0 type 0x0006 name "tm_yday"
    bit_offset 0x00000100 type 0x0006 name "tm_isdst"

0x0023: TYPEDEF size 0x00000120 bits
    scope struct type 0x0022 name "tm"

0x0024: TYPEDEF size 0x00000020 bits
    scope none type 0x0007 name "time_t"

0x0025: STRUCT size 0x000000e0 bits num_entries 0x0007
    bit_offset 0x00000000 type 0x0007 name "eax"
    bit_offset 0x00000020 type 0x0007 name "ebx"
    bit_offset 0x00000040 type 0x0007 name "ecx"
    bit_offset 0x00000060 type 0x0007 name "edx"
    bit_offset 0x00000080 type 0x0007 name "esi"
    bit_offset 0x000000a0 type 0x0007 name "edi"
    bit_offset 0x000000c0 type 0x0007 name "cflag"

0x0026: TYPEDEF size 0x000000e0 bits
    scope struct type 0x0025 name "DWORDREGS"

0x0027: STRUCT size 0x000000e0 bits num_entries 0x0007
    bit_offset 0x00000000 type 0x0005 name "ax"
    bit_offset 0x00000020 type 0x0005 name "bx"
    bit_offset 0x00000040 type 0x0005 name "cx"
    bit_offset 0x00000060 type 0x0005 name "dx"
    bit_offset 0x00000080 type 0x0005 name "si"
    bit_offset 0x000000a0 type 0x0005 name "di"
    bit_offset 0x000000c0 type 0x0007 name "cflag"

0x0028: TYPEDEF size 0x000000e0 bits
    scope struct type 0x0027 name "WORDREGS"

0x0029: STRUCT size 0x00000070 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x0003 name "al"
    bit_offset 0x00000008 type 0x0003 name "ah"
    bit_offset 0x00000020 type 0x0003 name "bl"
    bit_offset 0x00000028 type 0x0003 name "bh"
    bit_offset 0x00000040 type 0x0003 name "cl"
    bit_offset 0x00000048 type 0x0003 name "ch"
    bit_offset 0x00000060 type 0x0003 name "dl"
    bit_offset 0x00000068 type 0x0003 name "dh"

0x002a: TYPEDEF size 0x00000070 bits
    scope struct type 0x0029 name "BYTEREGS"

0x002b: STRUCT size 0x000000e0 bits num_entries 0x0003
    bit_offset 0x00000000 type 0x002a name "h"
    bit_offset 0x00000000 type 0x0026 name "x"
    bit_offset 0x00000000 type 0x0028 name "w"

0x002c: TYPEDEF size 0x000000e0 bits
    scope union type 0x002b name "REGS"

0x002d: STRUCT size 0x00000060 bits num_entries 0x0006
    bit_offset 0x00000000 type 0x0005 name "es"
    bit_offset 0x00000010 type 0x0005 name "cs"
    bit_offset 0x00000020 type 0x0005 name "ss"
    bit_offset 0x00000030 type 0x0005 name "ds"
    bit_offset 0x00000040 type 0x0005 name "fs"
    bit_offset 0x00000050 type 0x0005 name "gs"

0x002e: TYPEDEF size 0x00000060 bits
    scope struct type 0x002d name "SREGS"

0x002f: STRUCT size 0x00000070 bits num_entries 0x0008
    bit_offset 0x00000000 type 0x0003 name "al"
    bit_offset 0x00000008 type 0x0003 name "ah"
    bit_offset 0x00000020 type 0x0003 name "bl"
    bit_offset 0x00000028 type 0x0003 name "bh"
    bit_offset 0x00000040 type 0x0003 name "cl"
    bit_offset 0x00000048 type 0x0003 name "ch"
    bit_offset 0x00000060 type 0x0003 name "dl"
    bit_offset 0x00000068 type 0x0003 name "dh"

0x0030: TYPEDEF size 0x00000070 bits
    scope struct type 0x002f name "REGPACKB"

0x0031: STRUCT size 0x00000140 bits num_entries 0x000c
    bit_offset 0x00000000 type 0x0005 name "ax"
    bit_offset 0x00000020 type 0x0005 name "bx"
    bit_offset 0x00000040 type 0x0005 name "cx"
    bit_offset 0x00000060 type 0x0005 name "dx"
    bit_offset 0x00000080 type 0x0005 name "bp"
    bit_offset 0x000000a0 type 0x0005 name "si"
    bit_offset 0x000000c0 type 0x0005 name "di"
    bit_offset 0x000000e0 type 0x0005 name "ds"
    bit_offset 0x000000f0 type 0x0005 name "es"
    bit_offset 0x00000100 type 0x0005 name "fs"
    bit_offset 0x00000110 type 0x0005 name "gs"
    bit_offset 0x00000120 type 0x0007 name "flags"

0x0032: TYPEDEF size 0x00000140 bits
    scope struct type 0x0031 name "REGPACKW"

0x0033: STRUCT size 0x00000140 bits num_entries 0x000c
    bit_offset 0x00000000 type 0x0007 name "eax"
    bit_offset 0x00000020 type 0x0007 name "ebx"
    bit_offset 0x00000040 type 0x0007 name "ecx"
    bit_offset 0x00000060 type 0x0007 name "edx"
    bit_offset 0x00000080 type 0x0007 name "ebp"
    bit_offset 0x000000a0 type 0x0007 name "esi"
    bit_offset 0x000000c0 type 0x0007 name "edi"
    bit_offset 0x000000e0 type 0x0005 name "ds"
    bit_offset 0x000000f0 type 0x0005 name "es"
    bit_offset 0x00000100 type 0x0005 name "fs"
    bit_offset 0x00000110 type 0x0005 name "gs"
    bit_offset 0x00000120 type 0x0007 name "flags"

0x0034: TYPEDEF size 0x00000140 bits
    scope struct type 0x0033 name "REGPACKX"

0x0035: STRUCT size 0x00000140 bits num_entries 0x0003
    bit_offset 0x00000000 type 0x0030 name "h"
    bit_offset 0x00000000 type 0x0034 name "x"
    bit_offset 0x00000000 type 0x0032 name "w"

0x0036: TYPEDEF size 0x00000140 bits
    scope union type 0x0035 name "REGPACK"

0x0037: STRUCT size 0x000001e0 bits num_entries 0x000f
    bit_offset 0x00000000 type 0x0007 name "gs"
    bit_offset 0x00000020 type 0x0007 name "fs"
    bit_offset 0x00000040 type 0x0007 name "es"
    bit_offset 0x00000060 type 0x0007 name "ds"
    bit_offset 0x00000080 type 0x0007 name "edi"
    bit_offset 0x000000a0 type 0x0007 name "esi"
    bit_offset 0x000000c0 type 0x0007 name "ebp"
    bit_offset 0x000000e0 type 0x0007 name "esp"
    bit_offset 0x00000100 type 0x0007 name "ebx"
    bit_offset 0x00000120 type 0x0007 name "edx"
    bit_offset 0x00000140 type 0x0007 name "ecx"
    bit_offset 0x00000160 type 0x0007 name "eax"
    bit_offset 0x00000180 type 0x0007 name "eip"
    bit_offset 0x000001a0 type 0x0007 name "cs"
    bit_offset 0x000001c0 type 0x0007 name "flags"

0x0038: TYPEDEF size 0x000001e0 bits
    scope struct type 0x0037 name "INTPACKX"

0x0039: STRUCT size 0x000001e0 bits num_entries 0x000f
    bit_offset 0x00000000 type 0x0005 name "gs"
    bit_offset 0x00000020 type 0x0005 name "fs"
    bit_offset 0x00000040 type 0x0005 name "es"
    bit_offset 0x00000060 type 0x0005 name "ds"
    bit_offset 0x00000080 type 0x0005 name "di"
    bit_offset 0x000000a0 type 0x0005 name "si"
    bit_offset 0x000000c0 type 0x0005 name "bp"
    bit_offset 0x000000e0 type 0x0005 name "sp"
    bit_offset 0x00000100 type 0x0005 name "bx"
    bit_offset 0x00000120 type 0x0005 name "dx"
    bit_offset 0x00000140 type 0x0005 name "cx"
    bit_offset 0x00000160 type 0x0005 name "ax"
    bit_offset 0x00000180 type 0x0005 name "ip"
    bit_offset 0x000001a0 type 0x0005 name "cs"
    bit_offset 0x000001c0 type 0x0007 name "flags"

0x003a: TYPEDEF size 0x000001e0 bits
    scope struct type 0x0039 name "INTPACKW"

0x003b: STRUCT size 0x00000170 bits num_entries 0x0008
    bit_offset 0x00000100 type 0x0003 name "bl"
    bit_offset 0x00000108 type 0x0003 name "bh"
    bit_offset 0x00000120 type 0x0003 name "dl"
    bit_offset 0x00000128 type 0x0003 name "dh"
    bit_offset 0x00000140 type 0x0003 name "cl"
    bit_offset 0x00000148 type 0x0003 name "ch"
    bit_offset 0x00000160 type 0x0003 name "al"
    bit_offset 0x00000168 type 0x0003 name "ah"

0x003c: TYPEDEF size 0x00000170 bits
    scope struct type 0x003b name "INTPACKB"

0x003d: STRUCT size 0x000001e0 bits num_entries 0x0003
    bit_offset 0x00000000 type 0x003c name "h"
    bit_offset 0x00000000 type 0x0038 name "x"
    bit_offset 0x00000000 type 0x003a name "w"

0x003e: TYPEDEF size 0x000001e0 bits
    scope union type 0x003d name "INTPACK"

0x003f: STRUCT size 0x00000038 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0006 name "exterror"
    bit_offset 0x00000020 type 0x0008 name "class"
    bit_offset 0x00000028 type 0x0008 name "action"
    bit_offset 0x00000030 type 0x0008 name "locus"

0x0040: TYPEDEF size 0x00000038 bits
    scope struct type 0x003f name "DOSERROR"

0x0041: STRUCT size 0x00000028 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0003 name "day"
    bit_offset 0x00000008 type 0x0003 name "month"
    bit_offset 0x00000010 type 0x0005 name "year"
    bit_offset 0x00000020 type 0x0003 name "dayofweek"

0x0042: TYPEDEF size 0x00000028 bits
    scope struct type 0x0041 name "dosdate_t"

0x0043: STRUCT size 0x00000020 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0003 name "hour"
    bit_offset 0x00000008 type 0x0003 name "minute"
    bit_offset 0x00000010 type 0x0003 name "second"
    bit_offset 0x00000018 type 0x0003 name "hsecond"

0x0044: TYPEDEF size 0x00000020 bits
    scope struct type 0x0043 name "dostime_t"

0x0045: ARRAY_ZERO size 0x000000a8 bits
    base_type 0x0008 high_bound 0x00000014

0x0046: ARRAY_ZERO size 0x00000068 bits
    base_type 0x0008 high_bound 0x0000000c

0x0047: STRUCT size 0x00000158 bits num_entries 0x0006
    bit_offset 0x00000000 type 0x0045 name "reserved"
    bit_offset 0x000000a8 type 0x0008 name "attrib"
    bit_offset 0x000000b0 type 0x0005 name "wr_time"
    bit_offset 0x000000c0 type 0x0005 name "wr_date"
    bit_offset 0x000000d0 type 0x0007 name "size"
    bit_offset 0x000000f0 type 0x0046 name "name"

0x0048: TYPEDEF size 0x00000158 bits
    scope struct type 0x0047 name "find_t"

0x0049: STRUCT size 0x00000040 bits num_entries 0x0004
    bit_offset 0x00000000 type 0x0005 name "total_clusters"
    bit_offset 0x00000010 type 0x0005 name "avail_clusters"
    bit_offset 0x00000020 type 0x0005 name "sectors_per_cluster"
    bit_offset 0x00000030 type 0x0005 name "bytes_per_sector"

0x004a: TYPEDEF size 0x00000040 bits
    scope struct type 0x0049 name "diskfree_t"

0x004b: ARRAY_ZERO size 0x00000180 bits
    base_type 0x0006 high_bound 0x0000000b

0x004c: ARRAY_ZERO size 0x00000180 bits
    base_type 0x0006 high_bound 0x0000000b

0x004d: ARRAY_ZERO size 0x00000180 bits
    base_type 0x000b high_bound 0x0000000b

0x004e: PROCEDURE
    NEAR 386 ret_type 0x0006 num_parms 00

0x004f: POINTER size 0x00000020 bits base_type 0x0021 NEAR 386

0x0050: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 06
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
    parm_num 02 parm_type 0x0006
    parm_num 03 parm_type 0x0006
    parm_num 04 parm_type 0x0006
    parm_num 05 parm_type 0x000b

0x0051: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 04
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
    parm_num 02 parm_type 0x0006
    parm_num 03 parm_type 0x0006

0x0052: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 06
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006
    parm_num 02 parm_type 0x0006
    parm_num 03 parm_type 0x0008
    parm_num 04 parm_type 0x0008
    parm_num 05 parm_type 0x0008

0x0053: ARRAY_ZERO size 0x00000280 bits
    base_type 0x0008 high_bound 0x0000004f

0x0054: PROCEDURE
    NEAR 386 ret_type 0x0019 num_parms 02
    parm_num 00 parm_type 0x0006
    parm_num 01 parm_type 0x0006

                               Symbol Information
                               ------------------
MEM_LOC (static) name "NarrowTitle" type 0x000b
    data 94 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
MEM_LOC (static) name "WideTitle" type 0x000b
    data 90 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
MEM_LOC (static) name "Jump" type 0x004b
    data 00 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
MEM_LOC (static) name "MonthDays" type 0x004c
    data 30 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
MEM_LOC (static) name "MonthName" type 0x004d
    data 60 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
CHANGE_SEG
    data 00 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0001)
BLOCK name "main" type 0x004e start_offset 0x00000000 size 0x000000e7
    PROLOG size 12 ret_addr_offset 0x00000010
    Return Value:
        REGISTER name "" type 0x0000: EAX
    BP_OFFSET 0xfffffffc name "curr_time" type 0x001e
    BP_OFFSET 0xfffffff8 name "tyme" type 0x004f
    EPILOG size 05
BLOCK_END
BLOCK name "Calendar" type 0x0050 start_offset 0x000000e7 size 0x00000199
    Parms:
        REGISTER name "month" type 0x0006: EAX
        REGISTER name "year" type 0x0006: EDX
        REGISTER name "row" type 0x0006: EBX
        REGISTER name "col" type 0x0006: ECX
        BP_OFFSET 0x00000008 name "width" type 0x0006
        BP_OFFSET 0x0000000c name "title" type 0x000b
    PROLOG size 16 ret_addr_offset 0x00000004
    BP_OFFSET 0xfffffffc name "start" type 0x0006
    BP_OFFSET 0xfffffff8 name "days" type 0x0006
    BP_OFFSET 0xfffffff4 name "box_width" type 0x0006
    BP_OFFSET 0xfffffff0 name "str" type 0x000b
    BP_OFFSET 0xffffffec name "i" type 0x0006
    BP_OFFSET 0x0000000c name "title" type 0x000b
    BP_OFFSET 0x00000008 name "width" type 0x0006
    BP_OFFSET 0xffffffdc name "col" type 0x0006
    BP_OFFSET 0xffffffe0 name "row" type 0x0006
    BP_OFFSET 0xffffffe4 name "year" type 0x0006
    BP_OFFSET 0xffffffe8 name "month" type 0x0006
    EPILOG size 04
BLOCK_END
BLOCK name "Box" type 0x0051 start_offset 0x00000280 size 0x000000c4
    Parms:
        REGISTER name "row" type 0x0006: EAX
        REGISTER name "col" type 0x0006: EDX
        REGISTER name "width" type 0x0006: EBX
        REGISTER name "height" type 0x0006: ECX
    PROLOG size 14 ret_addr_offset 0x00000004
    BP_OFFSET 0xfffffffc name "i" type 0x0006
    BP_OFFSET 0xffffffec name "height" type 0x0006
    BP_OFFSET 0xfffffff0 name "width" type 0x0006
    BP_OFFSET 0xfffffff4 name "col" type 0x0006
    BP_OFFSET 0xfffffff8 name "row" type 0x0006
    EPILOG size 02
BLOCK_END
BLOCK name "Line" type 0x0052 start_offset 0x00000344 size 0x00000064
    Parms:
        REGISTER name "row" type 0x0006: EAX
        REGISTER name "col" type 0x0006: EDX
        REGISTER name "width" type 0x0006: EBX
        REGISTER name "left" type 0x0008: ECX
        BP_OFFSET 0x00000008 name "centre" type 0x0008
        BP_OFFSET 0x0000000c name "right" type 0x0008
    PROLOG size 15 ret_addr_offset 0x00000004
    BP_OFFSET 0xffffffac name "buffer" type 0x0053
    BP_OFFSET 0x0000000c name "right" type 0x0008
    BP_OFFSET 0x00000008 name "centre" type 0x0008
    BP_OFFSET 0xfffffffc name "left" type 0x0008
    BP_OFFSET 0xffffffa0 name "width" type 0x0006
    BP_OFFSET 0xffffffa4 name "col" type 0x0006
    BP_OFFSET 0xffffffa8 name "row" type 0x0006
    EPILOG size 04
BLOCK_END
BLOCK name "PosCursor" type 0x0054 start_offset 0x000003a8 size 0x00000045
    Parms:
        REGISTER name "row" type 0x0006: EAX
        REGISTER name "col" type 0x0006: EDX
    PROLOG size 15 ret_addr_offset 0x00000008
    BP_OFFSET 0xffffffe4 name "regs" type 0x002c
    BP_OFFSET 0xffffffdc name "col" type 0x0006
    BP_OFFSET 0xffffffe0 name "row" type 0x0006
    EPILOG size 03
BLOCK_END
BLOCK name "ClearScreen" type 0x001a start_offset 0x000003ed size 0x0000003c
    PROLOG size 12 ret_addr_offset 0x0000000c
    BP_OFFSET 0xffffffe4 name "regs" type 0x002c
    EPILOG size 04
BLOCK_END
.code end
.do end
.if '&ccmd' eq 'wfc' .do begin
.code begin
&prompt.wfc sieve
&prompt.womp &sw.f- &sw.dx sieve
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Large
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.code end
.code begin
&prompt.wfc &sw.d1 sieve
&prompt.womp &sw.f- &sw.dx sieve
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Large
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.code end
.code begin
&prompt.wfc &sw.d2 sieve
&prompt.womp &sw.f- &sw.dx sieve
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 8086
Memory Model    : Large
Floating-point  : Inline Emulation
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : Fortran

                                Type Information
                                ----------------
0x0000: RESERVED

0x0001: RESERVED

0x0002: INTEGER size 0x00000008 bits UNSIGNED

0x0003: INTEGER size 0x00000020 bits UNSIGNED

0x0004: INTEGER size 0x00000008 bits SIGNED

0x0005: INTEGER size 0x00000010 bits SIGNED

0x0006: INTEGER size 0x00000020 bits SIGNED

0x0007: REAL size 0x00000020 bits

0x0008: REAL size 0x00000040 bits

0x0009: COMPLEX size 0x00000040 bits

0x000a: COMPLEX size 0x00000080 bits

0x000b: PROCEDURE
    FAR ret_type 0x000c num_parms 00

0x000c: VOID

0x000d: SUBRANGE size 0x00000020 bits
    base_type 0x0006(SIGNED) low 0x00000002 high 0x000003e8

0x000e: ARRAY size 0x00001f38 bits
    base_type 0x0002 index_type 0x000d

                               Symbol Information
                               ------------------
CHANGE_SEG
    data 00 00 00 00
    fixup SEG_REL  POINTER  Frame: TARGET  Target: SI(0x0007)
BLOCK name "FMAIN" type 0x000b start_offset 0x00000000 size 0x00000146
    PROLOG size 0b ret_addr_offset 0x0000000c
    MEM_LOC (static) name "NUMBERS" type 0x000e
        data 00 00 00 00
        fixup SEG_REL  POINTER  Frame: TARGET  Target: SI(0x0004)
    BP_OFFSET 0xfffffff6 name "PRIMES" type 0x0006
    BP_OFFSET 0xfffffff2 name "K" type 0x0006
    BP_OFFSET 0xffffffee name "I" type 0x0006
    EPILOG size 09
BLOCK_END
.code end
.do end
.if '&ccmd' eq 'wfc386' .do begin
.code begin
&prompt.wfc386 sieve
&prompt.womp &sw.f- &sw.dx sieve
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : No
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.code end
.code begin
&prompt.wfc386 &sw.d1 sieve
&prompt.womp &sw.f- &sw.dx sieve
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : No
Symbs Present   : No
Source Language : Fortran
.code end
.code begin
&prompt.wfc386 &sw.d1 sieve
&prompt.womp &sw.f- &sw.dx sieve
WATCOM Object Module Processor Version 1.0
.cpyrit 1990
.trdmrk
Converting sieve.OBJ
                            Miscellaneous Information
                            -------------------------
Processor       : 80386
Memory Model    : Flat
Floating-point  : 80x87 Inline Code
Optimized       : Yes
Lines Present   : Yes
Types Present   : Yes
Symbs Present   : Yes
Source Language : Fortran

                                Type Information
                                ----------------
0x0000: RESERVED

0x0001: RESERVED

0x0002: INTEGER size 0x00000008 bits UNSIGNED

0x0003: INTEGER size 0x00000020 bits UNSIGNED

0x0004: INTEGER size 0x00000008 bits SIGNED

0x0005: INTEGER size 0x00000010 bits SIGNED

0x0006: INTEGER size 0x00000020 bits SIGNED

0x0007: REAL size 0x00000020 bits

0x0008: REAL size 0x00000040 bits

0x0009: COMPLEX size 0x00000040 bits

0x000a: COMPLEX size 0x00000080 bits

0x000b: PROCEDURE
    NEAR 386 ret_type 0x000c num_parms 00

0x000c: VOID

0x000d: SUBRANGE size 0x00000020 bits
    base_type 0x0006(SIGNED) low 0x00000002 high 0x000003e8

0x000e: ARRAY size 0x00001f38 bits
    base_type 0x0002 index_type 0x000d

                               Symbol Information
                               ------------------
CHANGE_SEG
    data 00 00 00 00 00 00
    fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0007)
BLOCK name "FMAIN" type 0x000b start_offset 0x00000000 size 0x000000c3
    PROLOG size 09 ret_addr_offset 0x00000010
    MEM_LOC (static) name "NUMBERS" type 0x000e
        data 00 00 00 00 00 00
        fixup SEG_REL  POINTER386  Frame: GI(0x0002)  Target: SI(0x0004)
    BP_OFFSET 0xfffffff8 name "PRIMES" type 0x0006
    BP_OFFSET 0xfffffff4 name "K" type 0x0006
    BP_OFFSET 0xfffffff0 name "I" type 0x0006
    EPILOG size 05
BLOCK_END
.code end
.do end
