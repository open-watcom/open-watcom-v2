.*
.*
.option HSHIFT
.*
.np
The "HSHIFT" defines the relationship between segment and linear address
in a segmented executable. The format of the "HSHIFT" option is as follows.
.mbigbox
    OPTION HSHIFT=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the number of digits to right shift a 32-bit value containing a
segment address in its upper 16 bits in order to convert it to part of a
linear address. In more conventional terms, (16 - 
.sy n
) is the amount to shift a segment value left in order to convert it to
part of a linear address.
.np
The "HSHIFT" Option is useful for non-standard segmented architectures that
have different alignment between segments and linear addresses, such as the
IP cores by ARC, Inc. These cores support a 24-bit addressing mode where
segment addresses are shifted 8 bits to form part of the linear address.  
The 
.sy n
value and its semantics match the analogous variable used by the 
compiler for computing addresses in the huge memory model.
.np	
The default value of
.sy n
is 12, representing the 4-bit shift used in conventional x86 CPUs.
