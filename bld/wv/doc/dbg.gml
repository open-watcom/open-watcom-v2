:GDOC.
:INCLUDE file=extra.
:FRONTM
:TITLEP.
:TITLE.WATCOM Debugging Information Format
:TITLE.VERSION 4.0
:AUTHOR.Copyright 1994 by WATCOM International Corp.
:DATE
:eTITLEP
:TOC.
:BODY.
:H0.Introduction
This document describes the object and executable file structures
used by the WATCOM Debugger to provide symbolic information about a program.
This information is subject to change.
:P
Note that version 4.0 of the WATCOM debugger supports the DWARF and 
Codeview symbolic debugging information formats in addition to the format
described in this document. For the purposes of discussion, this format
will be known as the "WATCOM" format. It is anticipated that DWARF will
become the primary format used by WATCOM compilers and support for
generating the WATCOM format will be removed from later versions of 
the compilers.
:P.
Before reading this document you should understand the Intel 8086 Object
Module Format (OMF). This format is described in the Intel document
:CIT.
8086 Relocatable Object Module Formats
:eCIT.
and also the October 1985 issue of
:CIT.
PC Tech Journal:eCIT..
:P.
Responsibility for the Intel/Microsoft OMF specification has been taken over
by the Tools Interface Standards (TIS) Committee. The TIS standards
(including the OMF spec) may be obtained by phoning the Intel literature
center at 1-800-548-4725 and asking for order number 241597.
:P.
This document is for the WATCOM Debugger version 4.0 (or above.)
:H0.Object file structures
The compiler is responsible for placing extra information into
the object file in order to provide symbolic information for the WATCOM Debugger. There
are three classes of information, each of which may be present or absent
from the file individually. These classes are line number, type and
local symbol information.
:P.
For the WATCOM C compiler, line number information
is provided when the "/d1" switch is used and all three classes are provided
when the "/d2" switch is used.
:H1.Version number and source language identification
Since there may be different versions of the type and local symbol
information, and there may be multiple front-ends a special OMF COMENT record
is placed in the object file. It has the following form:
:XMP.
	comment_class = 0xfe
	'D'
	major_version_number (char)
	minor_version_number (char)
	source_language (string)
:eXMP.
:PC.
The :F.comment_class:eF. of 0xfe indicates a linker directive comment.
The character 'D' informs the linker that this record is providing debugging
information. The :F.major_version_number:eF. is changed whenever there
is a modification made to the types or local symbol classes that is not
upwardly compatible with previous versions. The :F.minor_version_number:eF.
increments by one whenever a change is made to those classes that is
upwardly compatible with previous versions. The :F.source_language:eF. field
is a string which determines what language that the file was compiled
from.
:P
If the debugging comment record is not present, the local and type segments
(described later) are not in WATCOM format and should be omitted from the
resulting executable file's debugging information.
The current major version is one, and the current minor version is three.
:H1.Line number information
Line number information is provided by standard Intel OMF LINNUM records.
A kludge has been added that allows for line numbers to refer to more
than one source file. See the section on the "Special Line Number Table"
in the executable structures portion of the document for more details.
:H1.Location information
A type or symbol definition may contain a location field. This field is of
variable length and identifies the memory (or register) location of the
symbol in question. A location field may consist of a single entry, or
a list of entries. Each entry describes an operation of a stack machine.
The value of the location field is the top entry of the stack after all
the operations have been performed. To tell whether a field is a single
entry or a list, the first byte is examined. If the value of the byte
is greater than 0x80, then the field consists of a list of entries, and
the length in bytes of the list is the value of the first byte minus
0x80. If the first byte is less than 0x80, the byte is the first byte
of a single entry field.
The top nibble of the first byte in each entry
is a general location class while the low nibble specifies
the sub-class.
:XMP.
BP_OFFSET   (value 0x1?)
    BYTE    (value 0x10) offset_byte
    WORD    (value 0x11) offset_word
    DWORD   (value 0x12) offset_dword

CONST	    (value 0x2?)
    ADDR286 (value 0x20) memory_location_32_pointer
    ADDR386 (value 0x21) memory_location_48_pointer
    INT_1   (value 0x22) const_byte
    INT_2   (value 0x23) const_word
    INT_4   (value 0x24) const_dword

MULTI_REG   (value 0x3?)
    Low nibble is number of register bytes that follow - 1.
    The registers are specified low order register first.

REG	    (value 0x4?)
    Low nibble is low nibble of the appropriate register value.
    This may only be used for the first 16 registers.

IND_REG     (value 0x5?)
    CALLOC_NEAR (value 0x50) register_byte
    CALLOC_FAR	(value 0x51) register_byte, register_byte
    RALLOC_NEAR (value 0x52) register_byte
    RALLOC_FAR	(value 0x53) register_byte, register_byte

OPERATOR     (value 0x6?)
    IND_2	(value 0x60)
    IND_4	(value 0x61)
    IND_ADDR286 (value 0x62)
    IND_ADDR386 (value 0x63)
    ZEB         (value 0x64)
    ZEW         (value 0x65)
    MK_FP	(value 0x66)
    POP         (value 0x67)
    XCHG	(value 0x68) stack_byte
    ADD         (value 0x69)
    DUP         (value 0x6a)
    NOP         (value 0x6b)


Here is the list of register numbers:
 0-AL,	1-AH,  2-BL,  3-BH,  4-CL,  5-CH,  6-DL,  7-DH
 8-AX,	9-BX, 10-CX, 11-DX, 12-SI, 13-DI, 14-BP, 15-SP
16-CS, 17-SS, 18-DS, 19-ES
20-ST0, 21-ST1, 22-ST2, 23-ST3, 24-ST4, 25-ST5, 26-ST6, 27-ST7
28-EAX, 29-EBX, 30-ECX, 31-EDX, 32-ESI, 33-EDI, 34-EBP, 35-ESP
36-FS,	37-GS
:eXMP.
:PC.
CONST pushes a single constant value onto the expression stack. INT_1 and
INT_2 constant values are sign-extended to four bytes before being pushed.
:P.
The OPERATOR class performs a variety of operations on the expression
stack.
:DL.
:DT.IND_2
:DD.Pick up two bytes at the location specified by the top entry of the
stack, sign-extend to four bytes and replace top of stack with the result.
:DT.IND_4
:DD.Replace the top of stack with the contents of the four bytes at the
location specified by the top of stack.
:DT.IND_ADDR286
:DD.Replace the top of stack with the contents of the four bytes, treated
as a far pointer, at the location specified by the top of stack.
:DT.IND_ADDR386
:DD.Replace the top of stack with the contents of the six bytes, treated
as a far pointer, at the location specified by the top of stack.
:DT.ZEB
:DD.Zero extend the top of stack from a byte to a dword (clear the high
three bytes).
:DT.ZEW
:DD.Zero extend the top of stack from a word to a dword.
:DT.MK_FP
:DD.Remove the top two entries from the stack, use the top of stack as
an offset and the next element as a segment to form a far pointer and
push that back onto the stack.
:DT.POP
:DD.Remove the top entry from the stack.
:DT.XCHG
:DD.Exchange the top of stack with the entry specified by :F.stack_byte:eF..
"XCHG 1" would exchange the top of stack with the next highest entry.
:DT.ADD
:DD.Remove the top two entries from the stack, add them together and push
the result.
:DT.DUP
:DD.Duplicate the value at the top of the stack.
:DT.NOP
:DD.Perform no operation.
:eDL.
:P.
REG and MULTI_REG push the 'lvalue' of the register. If they are the
only entry then the symbol exists in the specified register. To access
the value of the register, you must indirect it.
:P.
BP_OFFSET locations are for variables on the stack. The values given
are offsets from the BP register for 286 programs and from the EBP register
for 386 programs. A BP_OFFSET could also be expressed with the following
series of operations:
:XMP.
	MULTI_REG(1) SS
	IND_2 
	MULTI_REG(1) EBP
	IND_4
	MK_FP
	INT_1 offset_byte
	ADD
:eXMP.
:P.
The IND_REG location type is used for structured return values.
The register or register pair is used to point at the memory location
where the structure is returned.
CALLOC means that the calling procedure is responsible for allocating
the return area and passing a pointer to it as a parameter in the specified
registers.
RALLOC means that the called routine allocated the area and returns
a pointer to it in the given registers.
:H1.Typing information
The WATCOM Debugger typing information is contained in a special segment in the
object file. The segment name is "$$TYPES" and the segment class is
"DEBTYP".
To allow greater flexibility in demand loading the typing information and
also let it exceed 60K for a single module, each object file may have
multiple $$TYPES segments. Each segment is identified by an entry in
the demand link table (described in the executable file structures section).
No individual segment may exceed 60K and no individual type record may be
split across a segment boundry. Also, any type which is described by
multiple records (structures, enums, procedures) may not be split across
a segment boundry. Since each segment is loaded as a whole by the
debugger when demand loading, increasing the segment size requires larger
amounts of contiguous memory be present in the system. Decreasing the
size of the individual segments reduces memory requirements, but increases
debugger lookup time since it has to traverse more internal structures.
The current code generator starts a new type segment when the current one
exceeds 16K.
The segments are considered to be
a stream of variable length definitions, with each definition being
preceded by a length byte.
A number of the definitions contain indices
of some form. These indices are standard Intel format, with 0 meaning no index,
1 to 127 is represented in one byte, 128 to 32767 in high byte/low byte
form with the top bit on in the high byte. Definitions are given index numbers
by the order in which they appear in the module, with the first being
index one. Character strings representing names are always placed at the
end of a definition so that their length can be calculated by subtracting
the name's start point from the length of the record. They are not
preceded by a length byte or followed by a zero byte.
:P
The first byte identifies the kind of the type definition that follows.
The top nibble of the byte is used to indicate the general class of the
type definition (there are eight of these). The low order nibble is used
to qualify the general type class and uniquely identify the definition
type.
:H2.TYPE_NAME (value 0x1?)
This definition is used to give names to types. There are three
sub-classes.
:XMP.
SCALAR    (value 0x10) scalar_type_byte, name
SCOPE     (value 0x11) name
NAME      (value 0x12) scope_index, type_index, name
CUE_TABLE (value 0x13) table_offset_dword
EOF       (value 0x14)
:eXMP.
:P.
SCALAR is used to give a name to a basic scalar type. It can also be
used to give a type index to a scalar type without a name by specifying
the null name. The :F.scalar_type_byte:eF. informs the WATCOM Debugger what sort of
scalar item
is being given a name. It has the following form:
:XMP.
BIT: 7 6 5 4 3 2 1 0
     | |   | |	   |
     | |   | +-----+--- size in bytes - 1
     | +---+----------- class (000 - integer)
     |			      (001 - unsigned)
     |			      (010 - float)
     |			      (011 - void (size=0))
     |			      (100 - complex)
     +----------------- unused
:eXMP.
:PC.
To create an unnamed scalar type, for use in other definitions, just
use a zero length name. 
:NOTE.
BASIC would have been a better name for this, since complex is not a
scalar type, but the name was chosen before complex support was added.
:P.
SCOPE is used to restrict the scope of other type names. A restricted
scope type name must be preceded by its appropriate scope name in order
for the WATCOM Debugger to recognize it as a type name. This is useful for declaring
C structure, union, and enum tag names. You declare SCOPE names of "struct",
"union", and "enum" and then place the appropriate value in the
:F.scope_index:eF. field of the NAME record when declaring the tag.
:P.
NAME gives an arbitrary type a name. The field, :F.scope_index:eF., is
either zero, which indicates an unrestricted type name, or is the type
index of a SCOPE definition, which means that the type name must be preceded
by the given scope name in order to be recognized.
:P.
The next two records are kludges to allow OMF line numbers to refer to
more than one source file.
See the section of on the "Special Line Number Table"
in the executable structure for more details.
:P.
CUE_TABLE is followed by :F.table_offset_dword:eF. which gives the offset
in bytes from the begining of the typing information for a module to the
special line number table. If this record is present, it must be in
the first $$TYPES segment for the module and preferably as close to
the begining of the segment as possible.
:P.
EOF marks the end of the typing information for the module and the
begining of the special line number table.
:H2.ARRAY (value 0x2?)
This definition is used to define an array type. There are 6 sub-classes.
:XMP.
BYTE_INDEX     (value 0x20) high_bound_byte, base_type_index
WORD_INDEX     (value 0x21) high_bound_word, base_type_index
LONG_INDEX     (value 0x22) high_bound_dword, base_type_index
TYPE_INDEX     (value 0x23) index_type_index, base_type_index
DESC_INDEX     (value 0x24) scalar_type_byte, scalar_type_byte,
				bounds_32_pointer, base_type_index
DESC_INDEX_386 (value 0x25) scalar_type_byte, scalar_type_byte,
				bounds_48_pointer, base_type_index
:eXMP.
:P.
BYTE_INDEX, WORD_INDEX, LONG_INDEX are all used to describe a restricted
form of array. If one of these forms is used then the index type is an
integer with the low bound of the array being zero and the high
bound being whatever is specified.
:P.
The DESC_INDEX form is used when the array bounds are not known at
compile time. The :F.bounds_32_pointer:eF. is a far pointer to a structure
in memory. The type and size of the first field is given by the first
:F.scalar_type_byte:eF. and indicates the lower bound for the index. The
second field's type and size is given by the second :F.scalar_type_byte:eF. .
This field gives the number of elements in the array.
:P.
The DESC_INDEX_386 is the same as DESC_INDEX except that a 48-bit far pointer
is used to locate the structure in memory.
:H2.SUBRANGE (value 0x3?)
This definition is used to define a subrange type. There are 3
sub-classes.
:XMP.
BYTE_RANGE (value 0x30) lo_bnd_byte, hi_bnd_byte, base_type_index
WORD_RANGE (value 0x31) lo_bnd_word, hi_bnd_word, base_type_index
LONG_RANGE (value 0x32) lo_bnd_dword, hi_bnd_dword, base_type_index
:eXMP.
:P.
If the base type is unsigned then the low and high bounds should be
interpreted as containing unsigned quantities, otherwise they contain
integers. However, the decision to use the byte, word, or long form of
the definition is always made considering the high and low bounds as
signed numbers.
:H2.POINTER (value 0x4?)
This definition is used to define a pointer type. There are 10
sub-classes.
:XMP.
NEAR		(value 0x40) base_type_index [,base_locator]
FAR		(value 0x41) base_type_index
HUGE		(value 0x42) base_type_index
NEAR_DEREF	(value 0x43) base_type_index [,base_locator]
FAR_DEREF	(value 0x44) base_type_index
HUGE_DEREF	(value 0x45) base_type_index
NEAR386 	(value 0x46) base_type_index [,base_locator]
FAR386		(value 0x47) base_type_index
NEAR386_DEFREF	(value 0x48) base_type_index [,base_locator]
FAR386_DEREF	(value 0x49) base_type_index
:eXMP.
:P.
When a symbol is one of the *_DEREF types, the WATCOM Debugger will automatically
dereference the pointer. This "hidden" indirection may be used to define
reference parameter types, or other indirectly located symbols. The *_DEREF
types have now been superceeded by location expressions. They should no
longer be generated. The NEAR* pointer types all have an optional
:F.base_locator:eF. field. The debugger can tell if this field is present
by examining the length of the debug type entry at the begining of the
record and seeing if there are additional bytes after the
:F.base_type_index:eF. field. If there are more bytes, the
:F.base_locator:eF. is a location expression whose result is an address,
the value of which is the base selector and offset value
when indirecting through the pointer (based pointers). The contents of
the based pointer variable are added to result of the location expression
to form the true resulting address after an indirection.
The address of the pointer variable being indirected through is pushed
on the stack before the location expression is evaluated (needed for
self-based pointers).
If the :F.base_locator:eF. field is not present, the debugger will use
the default near segment and a zero offset.

:H2.ENUMERATED (value 0x5?)
This definition is used to define an enumerated type. There are 4
sub-classes.
:XMP.
LIST	   (value 0x50) #consts_word, scalar_type_byte
CONST_BYTE (value 0x51) value_byte, name
CONST_WORD (value 0x52) value_word, name
CONST_LONG (value 0x53) value_dword, name
:eXMP.
:P.
LIST is used to inform the WATCOM Debugger of the number of constants in the enumerated
type and the scalar type used to store them in memory. It will be
followed immediately by all the constant definitions for the enumerated
type. See TYPE_NAME for a description of the :F.scalar_type_byte:eF..
:P.
CONST_BYTE, CONST_WORD, and CONST_LONG define the individual constant
values for an enumerated type. The type of the constant is provided by
the preceeding LIST definition. The decision to use the byte, word, or
long form of the definition is made always by considering the value as
a signed number. The CONST_* definition records are not counted when
determining type index values.
:P.
The LIST record and its associated CONST_* records must all be contained
in the same $$TYPES segment.
:H2.STRUCTURE (value 0x6?)
This definition is used to define a structure type. There are 10
sub-classes.
:XMP.
LIST	   (value 0x60) #fields_word [,size_dword]
FIELD_BYTE (value 0x61) offset_byte, type_index, name
FIELD_WORD (value 0x62) offset_word, type_index, name
FIELD_LONG (value 0x63) offset_dword, type_index, name
BIT_BYTE   (value 0x64) offset_byte, start_bit_byte, bit_size_byte,
			type_index, name
BIT_WORD   (value 0x65) offset_word, start_bit_byte, bit_size_byte,
			type_index, name
BIT_LONG   (value 0x66) offset_dword, start_bit_byte, bit_size_byte,
			type_index, name
FIELD_CLASS (v    0x67) attrib_byte, field_locator, type_index, name
BIT_CLASS  (value 0x68) attrib_byte, field_locator, start_bit_byte,
			bit_size_byte, type_index, name
INHERIT_CLASS (v  0x69) adjust_locator, ancestor_type_index
:eXMP.
:P.
LIST is used to introduce a structure definition. It is followed
immediately by all the field definitions that make up the structure.
The optional :F.size_dword:eF. gives the size of the structure in bytes.
If it is not present, the debugger calculates the size of the structure
based on field offsets and sizes.
:P.
FIELD_BYTE, FIELD_WORD, FIELD_LONG, and FIELD_CLASS define a single field
entry in a structure defintion.
:P.
BIT_BYTE, BIT_WORD, BIT_LONG, and BIT_CLASS define a bit field in a structure.
:The FIELD_CLASS and BIT_CLASS records are used for defining fields in a
C++ class. The :F.attrib_byte:eF. contain a set of bits describing attributes
of the field:
:XMP.
BIT: 7 6 5 4 3 2 1 0
     |     | | | | |
     |     | | | | +--- internal
     |	   | | | +----- public
     |	   | | +------- protected
     |	   | +--------- private
     +-----+----------- unused
:eXMP.
:PC.
An internal field is one that is generated for compiler support. It is
not normally displayed to the user. The other bits have their usual C++
meanings.
:P.
The :F.field_locator:eF. is a location expression describing how to
calculate the field address. Before begining to evaluate the expression,
the debugger will implicitly push the base address of the class instance
onto the stack. The following is an example of the location expression used
to calculate an ordinary field at offset 10 from the start of the class:
:XMP.
	INT_1	10
	ADD
:eXMP.
:P.
The INHERIT_CLASS record indicates that a particular class should inherit
all the fields specified by :F.ancestor_type_index:eF.. This field must
point at either a STRUCTURE LIST record or a TYPE NAME that eventually
resolves to a STRUCTURE LIST. The :F.adjust_locator:eF. is a location
expression that tells the debugger how to adjust the field offset expressions
in the inherited class to their proper values for a class of this instance.
:P.
The FIELD_*, BIT_*, and INHERIT_CLASS records are not counted when
determining type index values.
:P.
A C union, or Pascal variant record is described by having a number of fields
all beginning at the same offset. The WATCOM Debugger will display the fields in the
reverse order that the records define them. This means that ordinarily,
the records should be sorted by descending offsets and bit positions.
:P.
The LIST record and it's associated field descriptions must all be contained
in the same $$TYPES segment.
:H2.PROCEDURE (value 0x7?)
This definition is used to define a procedure type. There are 4
sub-classes.
:XMP.
NEAR	  (value 0x70) ret_type_index, #parms_byte {,parm_type_index}
FAR	  (value 0x71) ret_type_index, #parms_byte {,parm_type_index}
NEAR386   (value 0x72) ret_type_index, #parms_byte {,parm_type_index}
FAR386	  (value 0x73) ret_type_index, #parms_byte {,parm_type_index}
EXT_PARMS (value 0x74) {,parm_type_index}
:eXMP.
:P.
The EXT_PARMS sub-class is used when there are too many parameter
types to fit into one PROCEDURE record. This condition can be recognized
when the #parms_byte indicates there are more parameter types than
fit into the record according to the length field at the beginning.
In this case the remaining parameter types are continued in the record
immediately following, which will always be of type EXT_PARMS. The EXT_PARMS
record must be contained in the same $$TYPES segment as the preceeding
procedure record.
:H2.CHARACTER_BLOCK (value 0x8?)
Items of type CHARACTER_BLOCK are length delimited strings. There are 4
sub-classes.
:XMP.
CHAR_BYTE    (value 0x80) length_byte
CHAR_WORD    (value 0x81) length_word
CHAR_LONG    (value 0x82) length_dword
CHAR_IND     (value 0x83) scalar_type_byte, length_32_pointer
CHAR_IND_386 (value 0x84) scalar_type_byte, length_48_pointer
CHAR_IND_LOC (value 0x85) scalar_type_byte, address_locator
:eXMP.
:PC.
The CHAR_BYTE, CHAR_WORD, and CHAR_LONG forms are used when the length
of the character string is known at compile time. Even though the length
given is an unsigned quantity, the decision on which form to use is made
by considering the value to be signed. The CHAR_IND form is used when the
length of the string is determined at run time. The :F.length_32_pointer:eF.
gives the far address of a location containing the length of the string. The
size of this location is given by the :F.scalar_type_byte:eF.. The
CHAR_IND_386 form is the same as CHAR_IND except that the location of the length
is given by a 48-bit far pointer.
The CHAR_IND_LOC form is the same as CHAR_IND except that the address of
the length is given by a location expression.
:H1.Local symbol information
The WATCOM Debugger local symbol information is contained in a special segment in the
object file. The segment name is "$$SYMBOLS" and the segment class is
"DEBSYM".
The segment is considered to be
a stream of variable length definitions, with each definition being
preceded by a length byte. A number of the definitions contain indices
of some form. These indices are standard Intel format, with 0 meaning no index,
1 to 127 is represented in one byte, 128 to 32767 in high byte/low byte
form with the top bit on in the high byte.
Character strings representing names are always placed at the
end of a definition so that their length can be calculated by subtracting
the name's start point from the length of the record. They are not
preceded by a length byte or followed by a zero byte.
:P
The first byte identifies the kind of the symbol definition that follows.
The top nibble of the byte is used to indicate the general class of the
symbol definition. The low order nibble is used
to qualify the general definition class.
:P.
Symbol definitions are used to provide the WATCOM Debugger with the location and
scoping of source language local symbols. There are two general classes
of symbol definition, one for variables and one for code.
:H2.VARIABLE (value 0x1?)
This definition is used to define the location of a data symbol.
There are 4 sub-classes.
:XMP.
MODULE	  (value 0x10) memory_location_32_pointer, type_index, name
LOCAL	  (value 0x11) address_locator, type_index, name
MODULE386 (value 0x12) memory_location_48_pointer, type_index, name
MODULE_LOC (v    0x13) address_locator, type_index, name
:eXMP.
:P.
MODULE defines either an exported, domestic, or imported
variable in the module.
It is not necessary to generate symbol information for an imported variable
since the WATCOM Debugger will look for local symbol information in the module which defines
the variable if required.
:P.
LOCAL defines a symbol that is local to a code block or procedure. The
defining block is the first one previous to this definition. Local symbols
only "exist" for the purpose of the WATCOM Debugger lookups when the program is
executing in a block which defines the symbol.
:H2.CODE (value 0x2?)
This definition is used to define an object in the code. There are 6
sub-classes.
:XMP.
BLOCK	     (value 0x20) start_offset_word, size_word,
			    parent_block_offset
NEAR_RTN     (value 0x21) <BLOCK>, pro_size_byte, epi_size_byte,
			    ret_addr_offset_word, type_index,
			    return_val_loc, #parms_byte
			    {,parm_location}, name
FAR_RTN      (value 0x22) <BLOCK>, pro_size_byte, epi_size_byte,
			    ret_addr_offset_word, type_index,
			    return_val_loc, #parms_byte
			    {,parm_location}, name
BLOCK_386    (value 0x23) start_offset_dword, size_dword,
			    parent_block_offset
NEAR_RTN_386 (value 0x24) <BLOCK_386>, pro_size_byte, epi_size_byte,
			    ret_addr_offset_dword, type_index,
			    return_val_loc, #parms_byte
			    {,parm_location}, name
FAR_RTN_386  (value 0x25) <BLOCK_386>, pro_size_byte, epi_size_byte,
			    ret_addr_offset_dword, type_index,
			    return_val_loc, #parms_byte
			    {,parm_location}, name
MEMBER_SCOPE (value 0x26) parent_block_offset, class_type_index
			    [obj_ptr_type_byte, object_loc]
:eXMP.
:P.
BLOCK is used to indicate a block of code that contains local symbol
definitions. The field :F.parent_block_offset:eF. is used to tell the WATCOM Debugger
the next
block to search for a symbol definition if it is not found in this block.
The field is set to zero if there is no parent block.
:P
NEAR_RTN and FAR_RTN are used to specify a routine definition.
Notice that the first part
is identical to a code block definition. The :F.ret_addr_offset_word:eF.
is the
offset from BP (or EBP) that the return address is located on the stack. The
:F.#parms_byte:eF. and :F.parm_location:eF.'s following are only
for those parms which
are passed in registers. The remainder of the parms are assumed to be
passed on the stack.
:P.
The MEMBER_SCOPE record is used for C++ member functions. It introduces
a scope where the the debugger looks up the fields of the class identified
by :F.class_type_index:eF. as if they were normal symbols. If the
:F.obj_ptr_type_byte:eF. and
:F.object_loc:eF. location expression portions of the record are present,
it indicates that the function has a C++ "this" pointer, and all fields of
the class structure are accessable. The location expression evaluates
to the address of the object that the member function is manipulating.
The :F.obj_ptr_type_byte:eF. contains a value from the low order nibble
of a POINTER type record. It indicates the type of `this' pointer the
routine is expecting. I.e.:
:DL.
:DTHD.Value
:DDHD.Definition
:DT.0
:DD.16-bit near pointer
:DT.1
:DD.16-bit far pointer
:DT.6
:DD.32-bit near pointer
:DT.7
:DD.32-bit far pointer
:eDL.
If the portions following the :F.class_type_index:eF. are
absent from the record, the routine is a static member function and only
has access to static data members.
:P.
To use this record, the member function's :F.parent_block_offset:eF. is
pointed at the MEMBER_SCOPE record, and the MEMBER_SCOPE's
:F.parent_block_offset:eF field is pointed at what the member function
would normally be pointing at. In effect, a new block scope has been
introduced.
:P.
The *_386 versions of the records are identical to  their 286
counterparts excepts that the :F.start_offset:eF., :F.size:eF., and
:F.ret_addr_offset:eF. fields have been widened to 32 bits.
:NOTE.
There should be a better mapping of parm number to parm location. There
is no provision for Pascal calling conventions (reversed parm order) or
other strangeness.
:P
The BLOCK definition contains a :F.start_offset_word:eF. (or
:F.start_offset_dword:eF. in a BLOCK_386). This is the offset
from a given memory location provided by NEW_BASE entries and indicates the
address of the start of executable code for the block.
:P.
All the code
location definitions are assumed to be sorted in order of increasing
end offsets (start offset + size). This ensures that the first scope that
the debugger encounters in a traversal of the symbolic information is the
closest enclosing scope.
:H2.NEW_BASE (value 0x3?)
:XMP.
ADD_PREV_SEG (value 0x30) seg_increment_word
SET_BASE     (value 0x31) memory_location_32_pointer
SET_BASE386  (value 0x32) memory_location_48_pointer
:eXMP.
:P.
For ADD_PREV_SEG,
the specified amount is added to the segment value of the code start address
of the module. The code start offset is reset to zero. All BLOCK
definitions occuring after this item are relative to the new value.
After a SET_BASE
or SET_BASE386
all BLOCK definitions are relative to the
memory location that is given by the record.
:NOTE.
Avoid the use of the ADD_PREV_SEG record. Its operation is only valid in
real mode. It is included for backwards compatiblity only.
:H0.Executable file structures
The linker is responsible for processing the debugging information contained
in the object files and some of its internal structures and appending
them to the executable file.
:P.
After linking, the executable file looks like this:
:XMP.
+-----------------------+
|			|
|	EXE file	|
|			|
+=======================+
|			|
|	 Overlays	|
|			|
+=======================+
|    Any Other Stuff	|
+=======================+<---	start of debugging information
| source language table |
+-----------------------+
| segment address table |
+-----------------------+
|			|<-\
|  section debug info	|   +-- repeated for each overlay & root
|			|<-/
+-----------------------+
|  master debug header	|
+=======================+<---	end of file
:eXMP.
:PC.
The section marked as "EXE file" is the normal executable file.
All debugging information is appended to the end of the file, after any
overlay sections or other information.
The :F.master debug header:eF. begins at a fixed offset from the end of
the file, and provides the location of the remainder of the debug information.
The :F.source language table:eF. contains the source languages used by
the program.
The :F.section debug info:eF. is repeated once for the
root and each overlay section defined in the executable. It contains all the
debugging information for all object modules defined in the root or a particular
overlay section.
The :F.section debug info:eF. is further divided into a number of
debugging information classes, these will be explained later.
All offsets in the debugging information that refer to other information
items are
relative to the start of the information,
the start of a section of information,
or the start of a class of the information. In other words, the information
is not sensitive to its location in the executable file.
:H1.Master debug header
The master debug header allows the WATCOM Debugger to verify the fact that there is
debugging information, to locate the other sections and to verify that
it is capable of handling the version of debugging information.
The master header structure is as follows:
:XMP.
struct master_dbg_header {
    unsigned_16 	signature;
    unsigned_8		exe_major_ver;
    unsigned_8		exe_minor_ver;
    unsigned_8		obj_major_ver;
    unsigned_8		obj_minor_ver;
    unsigned_16 	lang_size;
    unsigned_16 	segment_size;
    unsigned_32 	debug_size;
};
:eXMP.
:PC.
The :F.signature:eF. word contains the value 0x8386.
This is the first indication to the WATCOM Debugger that there is debugging information
present.
The :F.exe_major_ver:eF. field contains the major version number of the
executable file debugging information structures.
The major version number will change
whenever there is a modification to these structures that is not upwardly
compatible with the previous version. The current major version number is
three.
The :F.exe_minor_ver:eF. field contains the minor version number of the
executable file debugging information structures.
The minor version number increments by
one whenever there is a change to the structures which is upwardly compatible
with the previous version. The current minor version number is zero.
This means that in order for the WATCOM Debugger to process the debugging information
the following must be true:
:OL.
:LI. FILE exe debug info major version == debugger exe debug info major version
:LI. FILE exe debug info minor version <= debugger exe debug info minor version
:eOL.
:P.
The :F.obj_major_ver:eF. field contains the major version number of the
object file debugging information structures (internal format of the
types and local symbol information).
The major version number will change
whenever there is a modification to these structures that is not upwardly
compatible with the previous version. The current major version number is
one.
The :F.obj_minor_ver:eF. field contains the minor version number of the
object file debugging information structures.
The minor version number increments by
one whenever there is a change to the structures which is upwardly compatible
compatible with the previous version. The current minor version number is three.
This means that in order for the debugger to process the debugging information
the following must be true:
:OL.
:LI. FILE obj debug info major version == debugger obj debug info major version
:LI. FILE obj debug info minor version <= debugger obj debug info minor version
:eOL.
:PC. These two fields are filled in by the linker by extracting the
version information from special debug comment record in the processed object
files. If two object files in the link contain different major version
numbers, the linker should report an error or warning and not process the
type or local symbol information for the 'incorrect' file. The minor
version number placed in the master header should be the maximum of
all the minor version numbers extracted from the object files.
:P.
The :F.lang_size:eF. field contains the size of the source language table
at the beginning of the debug information.
The :F.segment_size:eF. field informs the debugger of the size, in bytes, of the
segment address table.
The field, :F.debug_size:eF.,
gives the total size of the debugging information, including the size
of the master header itself. This allows the debugger to calculate the start
of the debugging information by subtracting the value of the :F.debug_size:eF.
field from the location of the end of file.
This gives the start of the source language and segment address tables,
whose sizes are
known from the master header. Once the location of the first section of
debugging information is determined, it can be processed. Within the
section information is a indicator of its total size, which allows the debugger
to find the start of the next section, and process that as well. This
continues until all the debug sections have been processed. the debugger knows
there are no more debug sections to process when the indicated start
of a section is the same as the start of the master header.
:H1.Source language table
The source language table is merely the collection of unique source languages
used in the program. The strings are extracted from the special debug comment
records in the object files and placed in this section one after another
with zero bytes separating them.
:H1.Segment address table
The segment address table is an array of all the unique segment numbers used
by the executable. Essentially, any segment value that would appear in the
map file will be represented in the table.
:H1.Section debug information
Each :F.section debug info:eF. contains the following:
:XMP
+-----------------------+
|    section header	|
+-----------------------+
|    local symbols	|
+-----------------------+
|	 types		|
+-----------------------+
|    line numbers	|
+-----------------------+
|     module info	|
+-----------------------+
|    global symbols	|
+-----------------------+
|     address info	|
+-----------------------+
:eXMP.
:PC.
The local symbols, types and line numbers classes are demand loaded by
the debugger as it requires pieces of the classes for various modules.
The module info, global symbols, and address info classes are permanently
loaded by the debugger at the start of a debugging session.
The global symbol, module, and address info classes have no size restriction,
however there is a limit of 65536 modules per section and there are some
restrictions on how the address info class may be laid out. These restrictions
are described in the section explaining the address info class.
:H2.Section debug header
The section header class allows the debugger to determine the size of the section
information and the location of the permanently loaded classes.
The header structure is as follows:
:XMP.
struct section_dbg_header {
    unsigned_32 	mod_offset;
    unsigned_32 	gbl_offset;
    unsigned_32 	addr_offset;
    unsigned_32 	section_size;
    unsigned_16 	section_id;
};
:eXMP.
:PC.
The :F.mod_offset:eF., :F.gbl_offset:eF., and :F.addr_offset:eF.
fields are offsets, from the beginning of the section debug header
to the module info,
global symbol, and address info classes of debugging information.
The :F.section_size:eF. field is the size of the debugging information
for the section, including the section header.
The following
conditions must hold true for the debugger to recognize the debugging information
as valid:
:OL.
:LI. mod_offset < gbl_offset
:LI. gbl_offset < addr_offset
:LI. addr_offset < section_size
:eOL.
:PC.
The :F.section_id:eF. field contains the overlay number for this section.
This is zero for the root.
:H2.Local symbols class
The local symbols segments are processed normally by the linker, except that
the data in the segments is placed in this section, no relocation entries are
output for any fixups in the data and fields in the module structure are
intialized to point to the beginning and size of each object file's contribution
to the section.
:H2.Types class
The type segments are processed normally by the linker, except that
the data in the segments is placed in this section, no relocation entries are
output for any fixups in the data and fields in the module structure are
intialized to point to the beginning and size of each object file's contribution
to the section.
:H2.Line numbers class
The LINNUM records for each object file are collected and placed in this
class using an array of arrays. The top level array is the following structure:
:XMP.
struct line_segment {
    unsigned_32     segment;
    unsigned_16     num;
    line_info	    line[1];
}
:eXMP.
:PC.
The :F.segment:eF. field contains a offset, from the start of the
address info class,
to an addr_info structure (see the address info class description).
This provides
the segment value for the array of line_info's following. The next field,
:F.num:eF., provides the number of line_info's in the array. The :F.line:eF.
is a variable size array containing the following structure:
:XMP.
struct line_info {
    unsigned_16  line_number;
    unsigned_32  code_offset;
};
:eXMP.
:PC.
The :F.line_number:eF. contains the source line number whose offset
is being defined. If the top bit of the line number is on, this line number
refers to an entry in the special line number table. See the
"Special Line Number Table" section for more details.
The :F.code_offset:eF. field contains the offset from the
begining of the module for the first instruction associated with the line
number.
To get the true code address for the instruction you must add :F.code_offset:eF.
to the address given by the :F.segment:eF. field in the line_segment
structure.
All the instructions
up to the next element's :F.code_offset:eF., or the end of the
object file's code for that segment if
there is no next :F.code_offset:eF. are considered to be part of the
:F.line_number:eF. source line. Within each line_segment structure
the line_info array is assumed to be sorted
in order of ascending :F.code_offset:eF.'s.
The module structure for the
object file contains fields which indicate the start and size of the
line_segment array
within the class.
:P.
Each line_segment structure may not exceed 60K, however the total amount
of line information for a module may exceed 60K with multiple line_segment
structures and multiple entries in the demand link table (described in the
module information section).
:P.
To obtain a line number from an address, the debugger performs the following steps
:OL.
:LI.Given an address, the defining module is found from the address information
class. This allows the debugger to find and load the line number information for
that module, if it is not already loaded.
:LI. Walk down the array of line_segment structures until one with the
appropriate segment is found.
:LI. Binary search the array of line_info's until the proper one is located.
:eOL.
:H3.Special Line Number Table
The OMF line number record does not allow for more than one source file
to be referenced in an object file. This kludge gets around the restriction.
If the top bit is on in :F.line_number:eF. than that field refers to an
entry in the special line number table. The debugger then searches the
typing information for the module for a :F.CUE_TABLE:eF. record. If it
finds one, it uses the offset given to find the begining of the table
in the typing information. The table looks like this:
:XMP.
/* cue entry table */
unsigned_16 cue_count

struct {
    unsigned_16 cue;
    unsigned_16 fno;
    unsigned_16 line;
    unsigned_16 column;
} cue_entry; /* repeated cue_count times, sorted by the 'cue' field */

/* file name index table */
unsigned_16 file_count

struct {
    unsigned_16	index;
} file_name_index_entry; /* repeated file_count times */

/* file name table */
A list of zero terminated source file names
:eXMP.
:P.
To find the correct cue entry given the value in a :F.line_number:eF., search
the :F.cue_entry:eF. table for the cue which satisfies the following:
:XMP.
cue_entry[entry].cue <= (line_number & 0x7fff) < cue_entry[entry+1].cue
:eXMP.
:PC.
Once you have the cue entry, you can extract the true line number by:
:XMP.
line = cue_entry[entry].line + (line_number & 0x7fff) 
               - cue_entry[entry].cue;
:eXMP.
:PC.
The file name is found by:
:XMP.
fname_index = file_name_index_table[ cue_entry[entry].fno ]
fname =  file_name_table[ fname_index ]
:eXMP.
:PC.
The code offset and segment are found in the :F.line_info:eF and 
:F.line_segment:eF structures as usual.
:H2.Module information class
The module information class is built from the linker's list of object
files that it processes to build the executable file, which are either
specified on the linker command line or extracted from libraries.
All the modules are implicitly given an index number by their order in
the class. These index numbers start at zero and are used by other classes
to identify individual modules.
The module structure contains the following fields:
:XMP.
struct mod_info {
    unsigned_16 language;
    demand_info locals;
    demand_info types;
    demand_info lines;
    unsigned_8	name[1];
};
:eXMP.
:PC.
The :F.language:eF. field contains an offset, from the start of the source
language table to the string of the source language for this module.
The :F.name:eF. field is a variable length array of characters
with the first element of the array being the length of the name. The remaining
characters identify the source file the compiler used to generate the object
file (e.g. "C:\DEV\WV\C\DBGMAIN.C"). The source file name is obtained from
the THEADR record of the object file.
the debugger uses the file name part of the
file specification as its "module name". The remaining fields,
:F.locals:eF., :F.types:eF., and :F.lines:eF. are a structure
type which define the location and size of this module's demand
loaded information
from those classes. The structure contains these fields:
:XMP.
struct demand_info {
    unsigned_32 	offset;
    unsigned_16 	num_entries;
};
:eXMP.
:PC.
The :F.offset:eF. field contains the offset from the beginning of the
debugging information section to first entry in the demand link table
containing the information for that particular demand load class. The
:F.num_entries:eF. field gives the number of contiguous entries in the
demand link table that are present for the module's demand load information
of that particular class.
:P.
The demand link table consists of an array of unsigned_32 offsets, which
are relative from the debugging information section, to the individual
demand info class data blocks. The array is in ascending order of offsets
so that the debugger may calculate the size of a particular demand load data
block by subtracting the offset of the next data block from the offset of
the current data block. This implies that there is an extra entry at the
end of the table whose offset points to the end of the final demand load
data block so that the debugger always has a 'next' link entry to calculate
size of a data block with. The size of each individual block may not exceed
60K. A picture may be useful here to show how all the pieces fit together:
:XMP.
module info
class
+--------+
|        |          demand link
   ...              table             demand info
|        |          +--------+        data block 
|	 |          |        |        +------+
+--------+             ...       +--->|      |
| offset | ---+     |        |   |      ...
+--------+    |     +--------+   |    |      |
|#entries|    +---->| offset |---+    +------+
+--------+          +--------+
|        |          |        |
   ...                 ...
|	 |          |        |  
+--------+          +--------+        demand info 
                    | offset |---+    data block
                    +--------+   |    +------+
		    |        |   +--->|      |
		       ...              ...
                    |        |        |      |
                    +--------+        +------+
:eXMP.
:PC.
When the debugger wishes to look something up in a demand load class for
a module. It uses the offset in the mod_info structure to locate the
array entry in the demand link table which has the offset for the first
info data block. It then loads the first block and searches it for the
information. If the information is not present in that block, it moves to
the next entry in the demand link table and repeats the above process. This
continues until all the entries for that particular class of the module
(identified by the :F.num_entries:eF. field in the mod_info structure)
have been examined, or the information is located.
:H2.Global symbols class
All PUBDEF records processed by the linker create entries in this class.
The fields in the structure are:
:XMP.
struct gbl_info {
    addr48_ptr	    addr;
    unsigned_16     mod_index;
    unsigned_8	    kind;
    unsigned_8	    name[1];
};
:eXMP.
:PC.
The :F.addr:eF. field contains the location in memory associated with
this symbol. The value placed in this field is the same that the linker
places in the map file (i.e. unrelocated, as if the executable loads
at location 0:0). The field contains a	48 bit value
(32 bit offset followed by a 16 bit segment).
The  :F.mod_index:eF. field is an index which identifies the
module which defines the symbol (i.e. contained the [L]PUBDEF record).
The :F.kind:eF. gives rudimentary typing information for the symbol. It
consists of the following set of bits:
:XMP.
BIT: 7 6 5 4 3 2 1 0
     |       | | | |
     |       | | | +--- STATIC symbol
     |       | | +----- DATA symbol
     |       | +------- CODE symbol
     +-------+--------- unused
:eXMP.
Bit zero is 1 if the global was defined by a
LPUBDEF record and 0 if it was defined by a PUBDEF record. LPUBDEF
symbols are generated by the code generator for static symbols, so this
allows a debugger to see static symbols even when no compiler debug switches
are being used.
Bit one is 1 if the producer of the information is able to determine that
the symbol is a data symbol. Bit two is one if the producer is able to
determine that the symbol is a code symbol. Both bits may be zero if the
producer is unable to determine whether the symbol is a code or data item.
The final field, :F.name:eF. is a variable length array, with the first
character indicating the length of the name, and the remaining characters
being the actual name of the symbol.
:H2.Address information class
The address information class allows the debugger, given a memory address, to
determine the module which defines that memory address. The linker builds
this class from the SEGDEF and GRPDEF records in the object files that
it processes.
The class consists of an array of structures with the following fields:
:XMP.
struct seg_info {
    addr48_ptr	    addr;
    unsigned_16     num;
    addr_info	    sects[1];
};
:eXMP.
:PC.
The :F.addr:eF. field identifies the start of a segment in memory.
This field contains the unrelocated value of the segment starting address
(i.e. as if the executable had been loaded at 0:0).
The the low order 15 bits of the next field, :F.num:eF. tells 
how many of the :F.sects:eF. entries
there are in the structure. 
The top bit of the field is a one when the segment belongs to "NonSect".
"NonSect" is the overlay section which holds all program data that is not
in the root or an overlay section. Typically this consists of DGROUP and
FAR_DATA segments. NonSect always is located at the highest address
of all sections. It is preloaded by the overlay manager and is never
moved. If the segment does not belong to NonSect, the top bit of the
:F.num:eF. field is zero.
The :F.sects:eF. field is a variable size
array of structures. This addr_info structure contains the following fields:
:XMP.
struct addr_info {
    unsigned_32     size;
    unsigned_16     mod_index;
};
:eXMP.
:PC.
The :F.mod_index:eF. field indicates the module in the module information
class which defines this piece of the segment.
The :F.size:eF. field identifies how large a piece of the segment specified
by the seg_info structure belongs to the module. The starting address
of the segment piece is given by adding all the previous size fields in the
:F.sects:eF. array to
the original starting address in the seg_info structure.
:P.
The size of a seg_info structure may not exceed 60K. If a single physical
segment would have more :F.sects:eF. than would fit into this restriction
(:F.num:eF. greater than 10238), it should be split into two separate
seg_info structures.
:P
To identify the module that defines a location in memory, the debugger does the
following:
:OL.
:LI. Walk down the array of seg_info structures until one is found with
the same segment address as the location that is being identified. If
no such seg_info is found, or the starting offset of the
segment is greater than the offset of the memory location,
then there is no defining module.
:LI. Walk down the array of addr_info's in the seg_info structure until
an entry is found whose starting offset is less than or equal to the memory
location offset and whose ending offset is greater than the memory location
offset. If there is no such entry, there is no defining module.
:LI. Otherwise, the :F.mod_offset:eF. field of the addr_info entry
is added to the beginning of the module information class, which gives
a pointer to the module structure that defines the memory location.
:eOL.
:eGDOC
