 .386p
 _TEXT segment USE32 public 'CODE'
 OneBankvgaBitBlt:
;
;       The "int 0dch" instruction assembles to "0x66, 0xcd, 0xdc"
;       (there's an extra operand size prefix). This needs to be
;       fixed the the 9.5 inline assembler as well as WASM.
;
        int     0dch
 _TEXT ends
 end
