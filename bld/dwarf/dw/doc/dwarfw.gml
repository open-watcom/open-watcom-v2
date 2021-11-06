.chap Debugging Information
.*
.np
The include file "dw.h" should be included to access the DW library.
.*
.section Data Types
.*
.np
The following types are defined in "dwcnf.h" and may be redefined
if the entire library is to be recompiled ("dw.h" automatically
includes "dwcnf.h").
.begnote
.notehd1 Type
.notehd2 Description
.note dw_sym_handle
Has a client defined meaning; the DW library will pass these
back to the client in
.id CLIRelocs
for
.id DW_W_STATIC
, and
.id DW_W_SEGMENT
.period
.note dw_targ_addr
The contents of
.id dw_targ_addr
is unimportant to the DW library; it is only used for
.id sizeof( dw_targ_addr )
.period
A
.id dw_targ_addr
is the type that will be emitted for relocations to run-time addresses.
.note dw_targ_seg
This is the size of the quantity that
.id DW_W_SEGMENT
emits.
.note dw_addr_offset
The type used for offsets from some base address.
For example, the
.id start_scope
parameter to typing routines, or the
.id addr
parameter to line number information.
The code assumes this is an unsigned integer type.
.note dw_addr_delta
An integer type that can hold the largest possible
difference between the
.id addr
parameter for two subsequent calls to
.id DWLineNum
.period
.note dw_linenum
A line number.
It must be an unsigned integer type.
.note dw_linenum_delta
dw_linenum_delta is a type that can hold the largest possible
difference between two adjacent line
numbers passed to
.id DWLineNum
or
.id DWReference
.period
.note dw_column
A column number.
It must be an unsigned integer type.
.note dw_column_delta
Type that can hold the largest possible difference between two
adjacent column numbers passed to
.id DWReference
.period
.note dw_size_t
Used for sizes of various things such as block constants
(i.e. for DWAddConstant) and the
.id size
parameter to
.id CLIWrite
.period
.note dw_uconst
An unsigned integer type that can hold the largest possible
unsigned integer constant.
.note dw_sconst
A signed integer type that can hold the largest possible
signed integer constant.
.note dw_sectnum
Enumerated type that can hold all defined Dwarf sections,
passed to client functions
.note dw_out_offset
A integer type that can hold the largest possible section offset
passed to
.id CLISeek
and returned by
.id CLITell
.note dw_reloc_type
A integer type that can hold the all relocation type
.endnote
.*
.section Initialization and Finalization
.*
.np
In the following functions, unless specified otherwise all strings
are assumed to be null-terminated.
.np
The DW library does not assume that a pointer passed to it is valid
beyond the function call used to pass it.
For example, you can pass the address of an auto-buffer that contains a
string.
.np
All names passed to the DW library should be unmangled.
.np
The
.id cli
parameter required for all DW functions except
.id DWInit
is assumed to be a valid value returned by a call to
.id DWInit
.period
.np
Currently DWENTRY is defined to be nothing.
It was created in case there is ever a need to put the DW library into
a DLL.
.beglevel
.*
.section DWInit
.*
.dwfunc dw_client DWENTRY DWInit( dw_init_info *info )
.np
Initialization for a compilation unit.
Return an unique client id.
This function will call client functions passed to it, so any client function
initialization must be done before the call to DWInit.
.millust begin
typedef struct {
    void          (*reloc)( dw_sectnum, uint, ... );
    void          (*write)( dw_sectnum, const void *, dw_size_t );
    void          (*seek)( dw_sectnum, dw_out_offset, int );
    dw_out_offset (*tell)( dw_sectnum );
    void          *(*alloc)( size_t );
    void          (*free)( void * );
} dw_funcs;

typedef struct {
    dw_lang       language;
    uint_8        compiler_options;
    const char    *producer_name;
    jmp_buf       exception_handler;
    dw_funcs      funcs;
} dw_init_info;
.millust end
.begnote
.notehd1 Member
.notehd2 Description
.note language
Language used.
.begnote $setptnt 24
.notehd1 Constant
.notehd2 Language
.note DWLANG_C
ISO/ANSI C
.note DWLANG_CPP
C++
.note DWLANG_FORTRAN
FORTRAN77
.endnote
.note compile_options
Compilation option, which is a combination of bits:
.begnote $setptnt 20
.notehd1 Bit
.notehd2 Description
.note DW_CM_BROWSER
The library generates the debugging information for the class
browser.
.note DW_CM_DEBUGGER
The library generates the debugging information for the debugger.
.note DW_CM_UPPER
For FORTRAN - The compiler converts all identifier names to upper
case.
.note DW_CM_LOWER
For FORTRAN - The compiler converts all identifier names to lower
case.
.endnote
.note producer
A string that identifies the compiler.
.note exception_handler
If the library ends up in a situation which it can't handle (can we say bug ;-)
) this jmp_buf will be called with a non-zero value.
This is a fatal exit, and the client should not call any of the DW functions.
.np
FIXME: The library is currently not very good at cleaning up memory in these situations.
.note funcs
These functions are described in a later section.
The initialization routines may call any of them; so any initialization
necessary for these routines must be done before DWInit is called.
.endnote
.np
The details of the above functions are discussed in Part 3.
.*
.section DWFini
.*
.dwfunc void DWENTRY DWFini( dw_client cli )
.np
Finalize the debugging information generator.
This routine must be called last.
It frees any structures that the DW library required, and flushes all the
debugging information.
.*
.section DWBeginCompileUnit
.*
.dwfunc dw_handle DWENTRY DWBeginCompileUnit( dw_client cli, const char *source_filename, const char *directory, dw_loc_handle segment, const unsigned offset_size )
.np
This function is called some time after
.id DWInit
.period
The only other DW functions that can be called in between are those
dealing with location expressions.
.begnote
.notehdr Parameter
.note source_filename
Name of the source file.
.note directory
Compilation directory.
.note segment
A location expression who's result is the code segment portion
of the low_pc and high_pc.
.note offset_size
The size in bytes of the offset portion of an address in this compile unit
.endnote
.np
The following CLIRelocs will be required:
.begnote
.note DW_W_HIGH_PC
.note DW_W_LOW_PC
.note DW_W_SECTION_POS
.note DW_W_UNIT_SIZE
.endnote
.*
.section DWEndCompileUnit
.*
.dwfunc void DWENTRY DWEndCompileUnit( dw_client cli )
.np
This function pairs up with
.id DWBeginCompileUnit
.period
After this, until the next
.id DWBeginCompileUnit
, the only valid calls are those made to location expression routines
(or
.id DWFini
).
.*
.endlevel
.*
.chap Ordering Considerations
.*
.np
In general the DW routines are called in an order that matches the
order of the declarations during the source program.
The sole exception to this are the Macro information routines.
Since it is possible to have a separate preprocessor pass, the library assumes
that these routines can be called before any of the other routines.
That is why the macro routines have a separate mechanism for specifying file
and line number.
.*
.chap Macro Information
.*
.beglevel
.*
.section DWMacStartFile
.*
.dwfunc void DWENTRY DWMacStartFile( dw_client cli, dw_linenum line, const char *name )
.np
Subsequent DWMac calls refer to the named file.
.*
.section DWMacEndFile
.*
.dwfunc void DWENTRY DWMacEndFile( dw_client cli )
.np
End the current included file.
.*
.section DWMacDef
.*
.dwfunc dw_macro DWENTRY DWMacDef( dw_client cli, dw_linenum line, const char *name )
.np
Defines a macro.
.id name
is the name of the macro.
A
.id dw_macro
is returned and must be used in a subsequent call
to
.id DWMacFini
(and possibly
.id DWMacParam
).
.*
.section DWMacParam
.*
.dwfunc void DWENTRY DWMacParam( dw_client cli, dw_macro mac, const char *name )
.np
Adds a parameter to the macro definition
.id mac
.period
.id name
is the name of the parameter with no leading or trailing white-space.
The order of parameters must be the same as they appear in the source
program.
.*
.section DWMacFini
.*
.dwfunc void DWENTRY DWMacFini( dw_client cli, dw_macro mac, const char *def )
.np
Finishes the macro definition
.id mac
.period
.id def
is the definition string.
.*
.section DWMacUnDef
.*
.dwfunc void DWENTRY DWMacUnDef( dw_client cli, dw_linenum line, const char *name )
.np
Undefines the macro named
.id name
.period
.*
.section DWMacUse
.*
.dwfunc void DWENTRY DWMacUse( dw_client cli, dw_linenum line, const char *name )
.np
Indicate where the macro named
.id name
is used.
.*
.endlevel
.*
.chap File and Line-Number Management
.*
.beglevel
.*
.section DWSetFile
.*
.dwfunc void DWENTRY DWSetFile( dw_client cli, const char *file )
.np
Specifies the current file.
The default is for the source_filename parameter from the dw_init_info
to be the current file.
.*
.section DWLineNum
.*
.dwfunc void DWLineNum( dw_client cli, uint info, dw_linenum line, uint col, dw_addr_offset addr )
.np
Sets the current source line number and machine address.
The line numbers information of all instructions, not just
declarations, are stored by this routine.
Note that all source line numbers are relative to the beginning of
their corresponding source file.
So the line number of the first line of an included file is one.
.begnote
.notehdr Parameter
.note info
The information about the line, which is established by the
combination of the following bits:
.begnote $setptnt 20
.notehd1 Bit
.notehd2 Description
.note DW_LN_DEFAULT
There is no special information about the line.
.note DW_LN_STMT
The line is the beginning of a statement.
.note DW_LN_BLK
The line is the beginning of a block.
.endnote
.note line
Source line number, numbered beginning with one on the first line
of the file.
.note col
Source column number, which begins at 1.
.note addr
Address of instruction relative to the beginning of the compilation unit.
If it is at all possible the client should call
.id DWLineNum
with increasing addrs.
The line parameter does not have to be increasing.
The size of the emitted line number information is smaller if increasing
addrs are used (There is also an implementation limitation that the maximum
decrease of addr between two calls is 32768).
.endnote
.*
.section DWDeclFile
.*
.dwfunc void DWENTRY DWDeclFile( dw_client cli, const char *name )
.np
Subsequent declarations are from the file named
.id name
.period
.*
.section DWDeclPos
.*
.dwfunc void DWENTRY DWDeclPos( dw_client cli, dw_linenum line, dw_column column )
.np
The next declaration occurs at the indicated line and column in the
source file set by the last call to
.id DWDeclFile
.period
Note that the position is only used for the immediate next declaration.
If there are multiple declarations on the same line, then multiple calls
should be made.
.*
.section DWReference
.*
.dwfunc void DWENTRY DWReference( dw_client cli, dw_linenum line, dw_column column, dw_handle dependant )
.np
Indicate that in the source code there is a reference to the dependant.
This reference is attributed to the current scope of debugging information.
(i.e., if it is done inside a structure, then the structure is considered
to be the "referencer").
.*
.endlevel
.*
.chap Location Expression Routines
.*
.np
Many functions require a
.id dw_loc_handle
.period
These are handles for expressions that the debugger will evaluate.
A
.id dw_loc_handle
can be either a single expression, or a list
of expressions created by
.id DWListFini
.period
The BROWSER is only interested in whether a location expression is present or
not; so when creating BROWSER output the client may create an empty location
expression and use that wherever appropriate.
.np
The expressions are evaluated on a stack machine, with operations
described later.
In some cases the stack will be initially empty, in other cases (such
as when calculating the address of a structure field) some base address
will be on the stack.
.np
A location expression is limited to roughly 64K.
Since each op-code is a single byte, this shouldn't pose much of a limitation
(famous last words).
The destination of the branch instructions
.id DW_LOC_BRA
and
.id DW_LOC_SKIP
must be within 32K of the current instruction (This is a limitation of
the DWARF format, not a limitation of the DW library).
.*
.beglevel
.*
.section DWLocInit
.*
.dwfunc dw_loc_id DWENTRY DWLocInit( dw_client cli )
.np
First function called to create a location expression for a symbol.
An unique
.id dw_loc_id
is returned to the front end.
.*
.section DWLocNewLabel
.*
.dwfunc dw_loc_label DWENTRY DWLocNewLabel( dw_client cli, dw_loc_id loc )
.np
Create a label for the location expression being built in
.id loc
.period
This label can be used for forward or backward references by
.id DW_LOC_SKIP
and
.id DW_LOC_BRA
.period
.*
.section DWLocSetLabel
.*
.dwfunc void DWENTRY DWLocSetLabel( dw_client cli, dw_loc_id loc, dw_loc_label label )
.np
Give the label
.id label
the address of the next operation emitted into the location expression
.id loc
.period
.*
.section DWLocReg
.*
.dwfunc void DWENTRY DWLocReg( dw_client cli, dw_loc_id loc, uint reg )
.np
This 'operation' informs the debugger that the value it seeks is in
the register named by
.id reg
.period
.np
FIXME: need to define the possible values of
.id reg
.period
.*
.section DWLocStatic
.*
.dwfunc void DWENTRY DWLocStatic( dw_client cli, dw_loc_id loc, dw_sym_handle sym )
.np
This operation pushes the address of
.id sym
on the stack.
.*
.section DWLocSegment
.*
.dwfunc void DWENTRY DWLocSegment( dw_client cli, dw_loc_id loc, dw_sym_handle sym )
.np
This operation pushes the segment of the address of
.id sym
on the stack.
.*
.section DWLocConstU
.*
.dwfunc void DWENTRY DWLocConstU( dw_client cli, dw_loc_id loc, dw_uconst value )
.np
Pushes an atom which is has an unsigned constant value
.id value
.period
.*
.section DWLocConstS
.*
.dwfunc void DWENTRY DWLocConstS( dw_client cli, dw_loc_id loc, dw_sconst value )
.np
Pushes an atom which is has a signed constant value
.id value
.period
.*
.section DWLocOp0
.*
.dwfunc void DWENTRY DWLocOp0( dw_client cli, dw_loc_id loc, dw_loc_op op )
.np
Performs one of the operations listed below.
.begnote $break $setptnt 24
.notehd1 Operation
.notehd2 Description
.note DW_LOC_ABS
It pops the top stack entry and pushes its absolute value.
.note DW_LOC_AND
It pops the top two stack values, performs the logical AND
operation on the two, and pushes the result.
.note DW_LOC_DEREF
It pops the top stack entry and treats it as an address.
The value retrieved from that address is pushed.
The size of data retrieved from the dereferenced address is an
addressing unit.
.note DW_LOC_DIV
It pops the top two stack values, divides the former second entry
by the former top of the stack using signed division, and pushes the
result.
.note DW_LOC_DROP
It pops the value at the top of the stack.
.note DW_LOC_DUP
It duplicates the value at the top of the stack.
.note DW_LOC_EQ
Pop two entries from stack, push 1 if they are equal; push 0 otherwise.
.note DW_LOC_GE, DW_LOC_GT, DW_LOC_LE, DW_LOC_LT
These operation pop the top two stack values, compare the former
top of stack from the former second entry, and pushes 1 onto stack if
the comparison is true, 0 if it is false.
The comparisons are signed comparison.
.note DW_LOC_MINUS
It pops the top two stack values, subtracts the former top of the
stack from the former second entry, and pushes the result.
.note DW_LOC_MOD
It pops the top two stack values, and pushes the result of the
calculation: former second stack entry modulo the former top of the
stack.
.note DW_LOC_MUL
It pops the top two stack values, multiplies them together, and
pushes the result.
.note DW_LOC_NE
Pop two entries from stack, push 0 if they are equal; push 1 otherwise.
.note DW_LOC_NEG
It pops the top value and pushes its negation.
.note DW_LOC_NOP
A placeholder; has no side-effects.
.note DW_LOC_NOT
It pops the top value and pushes its logical complement.
.note DW_LOC_OR
It pops the top two stack entries, performs the logical OR
operation on them, and pushes the result.
.note DW_LOC_OVER
It duplicates the entry currently second in the stack at the top of
the stack.
.note DW_LOC_PLUS
It pops the top two stack entries, and pushes their sum.
.note DW_LOC_ROT
It rotates the first three stack entries.
The entry at the top of the stack becomes the third entry, the second
entry becomes the top, and the third entry becomes the second.
.note DW_LOC_SHL
It pops the top two stack entries, shifts the former second entry
left by the number of bits specified by the former top of the stack,
and pushes the result.
.note DW_LOC_SHR
It pops the top two stack entries, shifts the former second entry
right (logically) by the number of bits specified by the former top of
the stack, and pushes the result.
.note DW_LOC_SHRA
It pops the top two stack entries, shifts the former second entry
right (arithmetically) by the number of bits specified by the former
top of the stack, and pushes the result.
.note DW_LOC_SWAP
It swaps the top two stack entries.
.note DW_LOC_XDEREF
It provides an extended dereference mechanism.
The entry at the top of the stack is treated as an address.
The second stack entry is treated as an "address space identifier" for
those architectures that support multiple address spaces.
The top two stack elements are popped, a data item is retrieved through
an implementation-defined address calculation and pushed as the new
stack top.
The size of data retrieved is an addressing unit.
.note DW_LOC_XOR
It pops the top two stack entries, performs the logical
EXCLUSIVE-OR operation on them, and pushes the result.
.endnote
.*
.section DWLocOp
.*
.dwfunc void DWENTRY DWLocOp( dw_client cli, dw_loc_id loc, dw_loc_op op, ... )
.np
Performs one of the following operations:
.begnote $break $setptnt 24
.notehd1 Operation
.notehd2 Description
.note DW_LOC_BRA
It is followed by a dw_loc_label operand.
This operation pops the top stack entry, if the value is not zero, then
jump to the label.
.note DW_LOC_BREG
Followed by two operands, the first is a register, and the second
is an dw_sconst to add to the value in the register.
The result is pushed onto the stack.
.note DW_LOC_FBREG
Takes one dw_sconst parameter which is added to the value calculated
by the frame_base_loc parameter to the current subroutine, then pushed
on the stack.
.note DW_LOC_PICK
It is followed by a uint operand which is an index.
The stack entry with the specified index (0 through 255, inclusive; 0
means the top) is pushed on the stack.
.note DW_LOC_PLUS_UCONST
It is followed an dw_sconst operand.
It pops the top stack entry, adds it to the operand and pushes the
result.
.note DW_LOC_SKIP
It is followed by a dw_loc_label operand.
Control is transferred immediately to this label.
.endnote
.*
.section DWLocFini
.*
.dwfunc dw_loc_handle DWENTRY DWLocFini( dw_client cli, dw_loc_id loc )
.np
Ends the location expression for a symbol, and returns a handle that
may be passed to other DW routines.
.*
.section DWListInit
.*
.dwfunc dw_list_id DWENTRY DWListInit( dw_client cli )
.np
First function called to create a location list for a symbol.
.*
.section DWListEntry
.*
.dwfunc void DWENTRY DWListEntry( dw_client cli, dw_list_id id, dw_sym_handle beg, dw_sym_handle end, dw_loc_handle loc )
.np
Define an entry in the location list.
.begnote
.notehdr Parameter
.note beg
A beginning address.
This address is relative to the base address of the compilation unit
referencing this location list.
It marks the beginning of the range over which the location is valid.
.note end
A ending address.
This address is relative to the base address of the compilation unit
referencing this location list.
It marks the first address past the end of the range over which the
location is valid.
Overlapping ranges are possible and are interpreted to mean that the
value may be found in one of many places during the overlap.
A CLIReloc for
.id DW_W_LABEL
will be made for each dw_sym_handle.
.note loc
A location expression describing the location of the object over
the range specified by the beginning and end addresses.
.endnote
.*
.section DWListFini
.*
.dwfunc dw_loc_handle DWENTRY DWListFini( dw_client cli, dw_list_id )
.np
Finishes the creation of the location list.
.*
.section DWLocTrash
.*
.dwfunc void DWENTRY DWLocTrash( dw_client cli, dw_loc_handle loc )
.np
Frees the memory associated with the location expression or list loc.
A location expression/list can be created and
used over and over again until it is
freed by calling this function.
.*
.endlevel
.*
.chap Typing Information
.*
.np
Unless otherwise noted, calls to these functions emit debugging
information immediately.
The DWARF format requires that debugging information appear in the same order
as it does in the source code.
So, for example, a structure's fields must be created in the same order that
they appear in the source program.
.np
Some of the following functions have common parameters.
Here is the documentation for these common parameters:
.begnote
.notehdr Parameter
.note char *name
A null-terminated type name.
i.e., "struct foobar {}" has the name foobar.
If this parm is NULL then no name is emitted.
.note dw_addr_offset start_scope
This is the offset from the low_pc value for the enclosing block that
the declaration occurs at.
This is most commonly 0.
.note uint flags
Some routines have additional flags available here; but unless otherwise noted,
the following are always available:
.begnote
.notehd1 Flag
.notehd2 Description
.note DW_DECLARATION
The object is a declaration, not a definition
.note DW_FLAG_PRIVATE
The object has the C++ private attribute.
.note DW_FLAG_PROTECTED
The object has the C++ protected attribute.
.note DW_FLAG_PUBLIC
The object has the C++ public attribute.
.endnote
.endnote
.*
.beglevel
.*
.section DWFundamental
.*
.dwfunc dw_handle DWENTRY DWFundamental( dw_client cli, char * name, unsigned fund_idx, unsigned size )
.np
Get a handle for a fundamental type.
.id fund_idx
is one of the following:
.begnote
.note DW_FT_ADDRESS
.note DW_FT_BOOLEAN
.note DW_FT_COMPLEX_FLOAT
.note DW_FT_FLOAT
.note DW_FT_SIGNED
.note DW_FT_SIGNED_CHAR
.note DW_FT_UNSIGNED
.note DW_FT_UNSIGNED_CHAR
.endnote
.np
For convenience, DW_FT_MIN, and DW_FT_MAX are defined.
A valid fundamental type is in the range DW_FT_MIN <= ft < DW_FT_MAX.
The DW library will always return the same handle when called with the same
fundamental type (so the client does not need to save fundamental type
handles).
.begnote
.notehdr Parameter
.note name
The name of the type being defined.
.note size
The size in bytes of the type being defined.
.endnote
.*
.section DWModifier
.*
.dwfunc dw_handle DWENTRY DWModifier( dw_client cli, dw_handle base_type, uint modifiers )
.np
Specifies a modifier to a type.
.id base_type
is the base type to be modified with the modifier
.id modifier
.period
The available modifiers are:
.begnote $setptnt 20
.notehd1 Modifier Constant
.notehd2 Description
.note DW_MOD_CONSTANT
The object is a constant
.note DW_MOD_VOLATILE
The object is volatile.
.note DW_MOD_NEAR
The object is a near object.
.note DW_MOD_FAR
The object is a far object.
.note DW_MOD_HUGE
The object is a huge object.
.note DW_MOD_FAR16
The object is a far16 object.
.endnote
.*
.section DWTypedef
.*
.dwfunc dw_handle DWENTRY DWTypedef( dw_client cli, dw_handle base_type, const char *name, dw_addr_offset start_scope, uint flags )
.np
This function gives a name to a type.
The
.id name
must not be NULL.
The flag value
.id DW_FLAG_DECLARATION
is not allowed.
.*
.section DWPointer
.*
.dwfunc dw_handle DWENTRY DWPointer( dw_client cli, dw_handle base_type, uint flags )
.np
Declares a pointer type.
.begnote
.notehdr Parameter
.note base_type
The pointed-at type.
.note flags
Only the following flags are available:
.begnote
.notehd1 Flags
.notehd2 Description
.note DW_FLAG_REFERENCE
Declare a pointer that is dereferenced automatically.
.note DW_PTR_TYPE_NORMAL
A normal pointer (i.e. a model dependant pointer).
.note DW_PTR_TYPE_NEAR16
A near 16-bit pointer.
.note DW_PTR_TYPE_FAR16
A far 16-bit pointer.
.note DW_PTR_TYPE_HUGE
A huge 16-bit pointer.
.note DW_PTR_TYPE_NEAR32
A near 32-bit pointer.
.note DW_PTR_TYPE_FAR32
A far 32-bit pointer.
.endnote
.endnote
.*
.section DWString
.*
.dwfunc dw_handle DWENTRY DWString( dw_client cli, dw_loc_handle string_length, dw_size_t byte_size, const char *name, dw_addr_offset start_scope, uint flags )
.np
Declares a type to be a block of characters.
.begnote
.notehdr Parameter
.note string_length
If this parameter is non-NULL then it is a location expression that
the debugger executes to get the address where the length of the string
is stored in the program.
In this case the
.id byte_size
parameter describes the number of bytes to be retrieved at the location
calculated.
If
.id byte_size
is 0, then the debugger will use
.id sizeof( long )
.period
.note byte_size
If
.id string_length
is NULL then this parameter is the number of bytes in the string.
Otherwise see
.id string_length
.period
.endnote
.*
.section DWMemberPointer
.*
.dwfunc dw_handle DWENTRY DWMemberPointer( dw_client cli, dw_handle containing_struct, dw_loc_handle use_location, dw_handle base_type, const char *name, unsigned flags )
.np
Declares a C++ pointer type to a data or function member of a class
or structure.
.begnote
.notehdr Parameter
.note containing_struct
A handle to the class or struct to whose members objects of this type
may point.
.note use_location
This refers to the location expression which describes how to get
to the member it points to from the beginning of the entire class.
It expects the base address of the structure/class object to be on the
stack before the debugger starts to execute the location description.
.note base_type
The type of the member to which this object may point to.
.endnote
.*
.endlevel
.*
.section Array Types
.*
.beglevel
.*
.section DWBeginArray
.*
.dwfunc dw_handle DWENTRY DWBeginArray( dw_client cli, dw_handle elt_type, uint stride_size, const char *name, dw_addr_offset scope, uint flags )
.np
Begin the declaration of an array.
This function call must be followed by calls to
.id DWArrayDimension
and
.id DWEndArray
.period
.begnote
.notehdr Parameter
.note elt_type
Handle for the type of the elements of this array.
.note stride_size
If this value is non-zero then it indicates the number of bits of
each element of the array (Useful if the number of bits used to store
an element in the array is different from the number of bits used to
store an individual element of type
.id elt_type
).
.endnote
.*
.section DWArrayDimension
.*
.dwfunc void DWENTRY DWArrayDimension( dw_client cli, const dw_dim_info *info )
.np
Add a dimension to the previously started array.
This function must be called for each dimension in the order that the
dimensions appear in the source program.
.id info
points to an instance of the following structure:
.millust begin
typedef struct {
    dw_handle   index_type;
    dw_uconst   lo_data;
    dw_uconst   hi_data;
} dw_dim_info;
.millust end
.begnote
.notehd1 Field
.notehd2 Description
.note hi_bound_fmt
This is similar to
.id lo_bound_fmt
but describes the high bound of this dimension.
.note index_type
This is the handle of the type of the indicies for this dimension.
.note lo_data
The low bound of the array.
.note hi_data
The upper bound of the array.
.endnote
.*
.section DWEndArray
.*
.dwfunc void DWENTRY DWEndArray( dw_client cli, dw_handle array_hdl, dw_handle elt_type, uint stride_size, const char *name, dw_addr_offset scope, uint flags )
.np
This finishes the writing of the record to describe the array
A sufficient number of calls to
.id DWArrayDimension
must have been made before
.id DWEndArray
is called.
.*
.endlevel
.*
.section Structure Types
.*
.beglevel
.*
.section DWStruct
.*
.dwfunc dw_handle DWENTRY DWStruct( dw_client cli, uint kind )
.np
Create a handle for a structure type that will be defined later.
This handle can be used for other DW routines even before
.id DWBeginStruct
has been called.
.begnote
.notehd1 Kind
.notehd2 Description
.note DW_ST_CLASS
A C++ class type.
.note DW_ST_STRUCT
A structure type.
.note DW_ST_UNION
A union type.
.endnote
.*
.section DWBeginStruct
.*
.dwfunc void DWENTRY DWBeginStruct( dw_client cli, dw_handle struct_hdl, dw_size_t size, const char *name, dw_addr_offset scope, uint flags )
.np
Begin the declaration of the structure reserved by a call to
.id DWStruct
.period
This function begins a nesting of the debugging information.
Subsequent calls, up to the corresponding
.id DWEndStruct
call, to the DW library become children of this structure.
i.e., this function marks the beginning of the scope of the structure
definition.
.begnote
.notehdr Parameter
.note struct_hdl
A dw_handle returned by a call to
.id DWStruct
.period
.note size
If this is non-zero it indicates the number of bytes required to
hold an element of this structure including any padding bytes.
.endnote
.*
.section DWAddFriend
.*
.dwfunc void DWENTRY DWAddFriend( dw_client cli, dw_handle friend )
.np
Add
.id friend
as a friend to the current structure.
.*
.section DWAddInheritance
.*
.dwfunc dw_handle DWENTRY DWAddInheritance( dw_client cli, dw_handle ancestor, dw_loc_handle loc, uint flags )
.np
Indicate the the current structure inherits from another structure.
.begnote
.notehdr Parameter
.note ancestor
The handle of the ancestor to be inherited.
.note loc
A location expression that describes the location of the beginning
of the data members contributed to the entire class by the ancestor
relative to the beginning of the address of the data members of the
entire class.
.note flags
In addition to the common values of
.id flags
, the flag
.id DW_FLAG_VIRTUAL
may be supplied to indicate that the inheritance serves as a virtual base
class.
As well, the flag
.id DW_FLAG_DECLARATION
is not allowed here.
.endnote
.*
.section DWAddField
.*
.dwfunc dw_handle DWENTRY DWAddField( dw_client cli, dw_handle field_hdl, dw_loc_handle loc, const char *name, uint flags )
.np
Add a data member to a structure.
.begnote
.notehdr Parameter
.note field_hdl
The dw_handle of the type of this field.
.note loc
A location expression which expects the base address of the structure
to be pushed on the stack and calculates the base address of this field.
If the structure is a union type, then this parameter may be NULL.
If this is a static data member of a class then this parameter may be NULL
if the actual definition of the parameter is outside the class.
.note flags
The additional flag
.id DW_FLAG_STATIC
may be used to indicate a static structure member.
.endnote
.*
.section DWAddBitField
.*
.dwfunc dw_handle DWENTRY DWAddBitField( dw_client cli, dw_handle field_hdl, dw_loc_handle loc, dw_size_t byte_size, uint bit_offset, uint bit_size, const char *name, uint flags )
.np
Add a bitfield member to a structure.
.begnote
.notehdr Parameter
.note field_hdl
the dw_handle of the type of this field.
.note loc
A location expression which expects the base address of the structure
most closely containing the bit field to be pushed and the stack, and
which calculates the base address of this field.
.note byte_size
This field must be the non-zero byte size of the unit of storage
containing the bit-field.
This is required only if the storage required cannot be determined by the type
of the bit-field (i.e., padding bytes).
If the size can be determined by the type of the bit-field, then this value
may be 0.
.note bit_offset
The number of bits to the left of the leftmost (most significant);
bit of the bit field value.
.note bit_size
The number of bits occupied by this bit-field value.
.endnote
.*
.section DWEndStruct
.*
.dwfunc void DWENTRY DWEndStruct( dw_client cli )
.np
End the current structure.
Client must ensure proper Begin/End matching.
.*
.endlevel
.*
.section Enumeration Types
.*
.beglevel
.*
.section DWBeginEnumeration
.*
.dwfunc dw_handle DWENTRY DWBeginEnumeration( dw_client cli, dw_size_t byte_size, const char *name, dw_addr_offset scope, uint flags )
.np
Begin the definition of an enumerated type.
.id byte_size
is the number of bytes required to hold an instance of this enumeration.
This call must be followed by calls to
.id DWAddConstant
and
.id DWEndEnumeration
.period
No other DW calls may be made before the call to
.id DWEndEnumeration
.period
The DWARF standard requires that the constants be defined in
.id reverse
order to which they appear in the source program.
.*
.section DWAddConstant
.*
.dwfunc void DWENTRY DWAddConstant( dw_client cli, dw_uconst value, const char *name )
.np
Add the constant
.id value
(that is
.id byte_size
bytes large as determined by the parameter to
.id DWBeginEnumeration
); with the name
.id name
to the current enumeration.
.*
.section DWEndEnumeration
.*
.dwfunc void DWENTRY DWEndEnumeration( dw_client cli )
.np
Finish the current enumeration.
.*
.endlevel
.*
.section Subroutine Type Declarations
.*
.np
These function calls deal with declarations of subroutines.
That is, their prototypes, or for use in creating function pointers.
.*
.beglevel
.*
.section DWBeginSubroutineType
.*
.dwfunc dw_handle DWENTRY DWBeginSubroutineType( dw_client cli, dw_handle return_type, const char *name, dw_addr_offset scope, uint flags )
.np
Begin the nested declaration of the subroutine type.
All calls to the DW library after this, until
.id DWEndSubroutineType
are in the scope of the declaration of the subroutine type
(i.e., if it's a prototyped C function, then declarations before
.id DWEndSubroutineType
are similar to declarations inside the prototype).
Parameters for this type are declared using the entries
.id DWAddParmToSubroutineType
and
.id DWAddEllipsisToSubroutineType
.period
.begnote
.notehdr Parameter
.note return_type
If the function is void, this parameter must be NULL.
Otherwise it is a handle for the return type of the subroutine.
.note flags
In addition to the standard flags,
.id DW_FLAG_PROTOTYPED
indicates that the declaration of the subroutine type was prototyped
in the source code.
As well, the "address class" set of flags used in
.id DWPointer
are also allowed here (e.g.
.id DW_TYPE_FAR16
etc.)
.endnote
.*
.section DWEndSubroutineType
.*
.dwfunc void DWENTRY DWEndSubroutineType( dw_client cli )
.np
The client must ensure that proper Begin/End matching is done.
.*
.endlevel
.*
.section Lexical Blocks
.*
.beglevel
.*
.section DWBeginLexicalBlock
.*
.dwfunc dw_handle DWENTRY DWBeginLexicalBlock( dw_client cli, dw_loc_handle segment, const char *name )
.np
Begin a new lexical scope.
.id name
may be NULL indicating an un-named scope.
Two CLIReloc calls will made, one for
.id DW_W_LOW_PC
and one for
.id DW_W_HIGH_PC
which indicate the first byte of the scope, and the first byte beyond the end
of the scope.
.id segment
if non-null is an expression that evaluates to the segment this block is in.
.*
.section DWEndLexicalBlock
.*
.dwfunc void DWENTRY DWEndLexicalBlock( dw_client cli )
.np
End a lexical scope.
As usual, the client must ensure that Begin/End pairs match.
.*
.endlevel
.*
.section Common Blocks
.*
.beglevel
.*
.section DWBeginCommonBlock
.*
.dwfunc dw_handle DWENTRY DWBeginCommonBlock( dw_client cli, dw_loc_handle loc, dw_loc_handle segment, const char *name, unsigned flag )
.np
Begin the declarations for the common block named
.id name
and located at
.id loc
.period
.id segment
if non-null indicates which segment the common block is in.
The only flag that is valid for the
.id flag
parameter is
.id DW_FLAG_DECLARATION
.period
.*
.section DWEndCommonBlock
.*
.dwfunc void DWENTRY DWEndCommonBlock( dw_client cli )
.np
End of declarations for the common block.
.*
.section DWIncludeCommonBlock
.*
.dwfunc dw_handle DWENTRY DWIncludeCommonBlock( dw_client cli, dw_handle common_block )
.np
Used in the subroutine scope that references the common block.
.*
.endlevel
.*
.section Subroutines
.*
.beglevel
.*
.section DWBeginInlineSubroutine
.*
.dwfunc dw_handle DWENTRY DWBeginInlineSubroutine( dw_client cli, dw_handle out_of_line, dw_loc_handle ret_addr, dw_loc_handle segment )
.np
Begin a definition of a particular instance of an inlined subroutine.
.id out_of_line
is a handle to the "out of line" instance of the subroutine
(i.e., a handle from a
.id DWBeginSubroutine
call that had the DW_FLAG_OUT_OF_LINE flag).
Each instance of the inlined subroutine must have it's own copies of entries
describing parameters to that subroutine and it's local variables.
.id ret_addr
gives the location of the return address (if any).
.id segment
if non-null indicates which segment the expansion occurs in.
.*
.section DWBeginSubroutine
.*
.dwfunc dw_handle DWENTRY DWBeginSubroutine( dw_client cli, dw_call_type call_type, dw_handle return_type, dw_loc_handle return_addr_loc, dw_loc_handle frame_base_loc, dw_loc_handle structure_loc, dw_handle member_hdl, dw_loc_handle segment, const char *name, dw_addr_offset start_scope, uint flags )
.np
Begin a declaration/definition of a subroutine or entry point.
This begins a nesting of the debugging information, and must be
followed by calls to
.id DWFormalParameter
et al to declare the parameters, types, and variables for this subroutine.
Unless
.id DW_FLAG_DECLARATION
is set, this will require a
.id DW_W_LOW_PC
and/or a
.id DW_W_HIGH_PC
.period
.begnote
.notehdr Parameter
.note call_type
Not currently used, but should be one of:
.begnote
.note DW_SB_NEAR_CALL
.note DW_SB_FAR_CALL
.note DW_SB_FAR16_CALL
.endnote
.note return_type
Handle for the return type.
Must be NULL for void-type subroutines.
.note return_addr_loc
If non-NULL then this is a location expression that calculates the
address of memory that stores the return address.
.note frame_base_loc
If non-NULL then this is a location expression that describes the
"frame base" for the subroutine or entry point (If the frame base
changes during the subroutine, it might be desirable
for local variables to be calculated from the frame base, and then
use a location list for the frame base).
.note structure_loc
For member functions of structure types, this calculates the
address of the slot for the function within the virtual function table
for the enclosing class or structure.
.note member_hdl
If this is a definition of a member function occuring outside the body
of the structure type, then this is the handle for the type definition
of the structure.
.note segment
If non-null then this is a location expression that evaluates to the
segment for this subprogram.
.endnote
.np
The following additional flags are available:
.begnote
.note flag
description
.note DW_FLAG_PROTOTYPED
The function was declared with ANSI-C style prototyping, as opposed
to K&R-C style parameter lists.
.note DW_FLAG_ARTIFICIAL
The function was created by the compiler (i.e. not explicitly declared in
any of the user's source files)
.note DW_FLAG_VIRTUAL
This is a virtual subroutine.
.note DW_FLAG_PURE_VIRTUAL
This is a pure virtual subroutine.
.note DW_FLAG_MAIN
For Fortran PROGRAM-type subroutines.
.note DW_SUB_STATIC
A file static subroutine or function.
Also used for a static member function, and for nested subroutine declarations.
.note DW_SUB_ENTRY
A FORTRAN Entry point.
DW requires only a DW_W_LOW_PC for this type of function.
.note DW_FLAG_WAS_INLINED
The function was generated inline by the compiler.
.note DW_FLAG_DECLARED_INLINED
The function was declared inline by the user.
.endnote
.*
.section DWEndSubroutine
.*
.dwfunc void DWENTRY DWEndSubroutine( dw_client cli )
.np
End the current nesting of
.id DWBeginSubroutine
or
.id DWBeginInlineSubroutine
.period
.*
.section DWFormalParameter
.*
.dwfunc dw_handle DWENTRY DWFormalParameter( dw_client cli, dw_handle parm_type, dw_loc_handle parm_loc, dw_loc_handle segment, const char *name, uint default_value_type, ... )
.np
Declare a formal parameter to the current function.
.begnote
.note parm_type
The type of the parameter.
.note parm_loc
A location description that yields the address of the parameter.
May be NULL indicating unknown address.
.note segment
A location expression that yields the segment of the parameter.
May be NULL indicating the default segment.
.note default_value_type
One of the following:
.begnote
.note DW_DEFAULT_NONE
There is no default value for this parameter.
.note DW_DEFAULT_FUNCTION
The default value for this parameter is returned by a function
with no args, that is specified by a CLIReloc for DW_W_DEFAULT_FUNCTION.
.note DW_DEFAULT_STRING
The default value is a null-terminated string that is specified
as an extra parameter to this
.id DWFormalParameter
.period
.note DW_DEFAULT_BLOCK
The default value is a constant block of data that is specified
by extra "const void *" and "dw_size_t"  parameters to
.id DWFormalParameter
.period
.endnote
.note ...
Extra parameters depend on the
.id default_value_type
.period
.endnote
.*
.section DWEllipsis
.*
.dwfunc dw_handle DWENTRY DWEllipsis( dw_client cli )
.np
Indicate that the current subroutine has unspecified parameters.
Used for "..." in C.
.*
.section DWLabel
.*
.dwfunc dw_handle DWENTRY DWLabel( dw_client cli, dw_loc_handle segment, const char *name, dw_addr_offset start_scope )
.np
Declare a label inside a subroutine.
.id start_scope
will usually be 0, but is here for future compatibility.
A CLIReloc for
.id DW_W_LABEL
will be made.
.id segment
if non-null indicates which segment the label belongs to.
.*
.section DWVariable
.*
.dwfunc dw_handle DWENTRY DWVariable( dw_client cli, dw_handle type, dw_loc_handle loc, dw_handle member_of, dw_loc_handle segment, const char *name, dw_addr_offset start_scope, uint flags )
.np
Declare a variable.
.begnote
.note type
The type of this variable.
.note loc
A location expression yielding the address of this variable.
.note member_of
If this is the definition of a static data member then this
is the handle to the structure type.
Otherwise this is NULL.
.note segment
If this is non-null then it evaluates to the segment the variable
is in.
.note flags
If
.id DW_FLAG_GLOBAL
is set then this is a global variable.
Otherwise it is a local variable.
File static variables in C and C++ are considered local variables.
If
.id DW_FLAG_ARTIFICIAL
is set then this is a variable that has been created by the compiler.
.endnote
.*
.section DWConstant
.*
.dwfunc dw_handle DWENTRY DWConstant( dw_client cli, dw_handle type, const void *value, dw_size_t len, dw_handle member_of, const char *name, dw_addr_offset start_scope, uint flags )
.np
Declare a named constant.
.begnote
.note type
The type of this constant.
.note value
Pointer to the value for this constant.
.note len
The length of this constant.
If len is 0, then value is considered to be a null-terminated string.
.note member_of
If this is the definition of a constant member of a structure type,
then this is the handle to the structure type.
Otherwise it is NULL.
.endnote
.*
.section DWAddress
.*
.dwfunc void DWENTRY DWAddress( dw_client cli, uint_32 len )
.np
DWARF builds a table of all the addresses attributed to a compilation unit.
The client calls this function to add addresses to this table.
.id len
is the length of this address range.
The base of the address range is filled in by a CLIReloc for
.id DW_W_ARANGE_ADDR
.period
.*
.section DWPubname
.*
.dwfunc void DWENTRY DWPubname( dw_client cli, dw_handle hdl, const char *name )
.np
These are used to speed up the debugger.
This should be called for any name that has global scope.
.id hdl
is the handle for the debugging entry that declares/defines the
.id name
.period
.*
.endlevel
.*
.chap Required Client Routines
.*
.np
The debugging information has several sections indicated by the
following enumerated type:
.begnote $setptnt 22
.notehd1 Constant
.notehd2 Description
.note DW_DEBUG_INFO
This section is called
.book .debug_info
, which stores all the debugging information entries.
.note DW_DEBUG_PUBNAMES
This section is called
.book .debug_pubnames
, which stores a table consisting of object name information that is used in
lookup by Name.
.note DW_DEBUG_ARANGES
This section is called
.book .debug_aranges
, which stores a table consisting of object address information that is used
in lookup by Address.
.note DW_DEBUG_LINE
This section is called
.book .debug_line
, which stores the line number information generated for the compilation units.
.note DW_DEBUG_LOC
This section is called
.book .debug_loc
, which stores the location lists information.
.note DW_DEBUG_ABBREV
This section is called
.book .debug_abbrev
, which stores abbreviation declarations.
.note DW_DEBUG_MACINFO
This section is called
.book .debug_macinfo
, which stores macro information.
.note DW_DEBUG_REF
This section is called
.book .WATCOM_references
, which contains information about the symbols of every instructions in
the source files.
.note DW_DEBUG_MAX
Defined for convenience; it is the number of sections.
.endnote
.*
.section Performance Considerations
.*
.np
The DW library does it's best to try and group CLIWrite operations together
into one larger CLIWrite, and to try and avoid using CLISeek.
But the library does not go out of it's way to provide this massaging of output.
The client should attempt to buffer the data itself.
CLISeek is most often called on the DW_DEBUG_INFO, and the DW_DEBUG_LOC sections.
The other sections may have one CLISeek performed at the DWFini stage, and
the seek will be to the zero offset.
The client might wish to optimize performance for only the DW_DEBUG_INFO and
the DW_DEBUG_LOC sections.
.*
.beglevel
.*
.section CLISeek
.*
.dwfunc void CLISeek( uint section, long offset, uint mode )
.np
Repositions the pointer in
.id section
so that subsequent output occurs at the new pointer.
.begnote
.notehd1 Mode
.notehd2 Description
.note DW_SEEK_SET
The position is set to the absolute location
.id offset
.period
.note DW_SEEK_CUR
.id offset
is added to the current position.
.note DW_SEEK_END
The position is set to
.id offset
bytes from the current end of
.id section
.period
.endnote
.*
.section CLITell
.*
.dwfunc long CLITell( uint section )
.np
Return the offset of the next byte to be written to the section.
.*
.section CLIReloc
.*
.dwfunc void CLIReloc( uint section, uint reloc_type, ... )
.np
Even when writing BROWSER information, relocations such as DW_W_LOC_PC
may be asked for.
This is because the DWARF format requires the presence of certain fields to
indicate something specific about a record.
For example, if a subroutine record doesn't have a low pc then it is assumed
to be a declaration of the subroutine rather than a definition.
.begnote
.notehdr Parameter
.note section
The section to write a relocation entry to.
.note reloc_type
The type of the relocation, as follows:
.begnote
.note DW_W_LOW_PC
Emit a dw_targ_addr.
Used by various entry points to get the low pc address of an object.
.note DW_W_HIGH_PC
Emit a dw_targ_addr.
Used by various entry points to get the high pc address of an object.
.note DW_W_STATIC
Emit a dw_targ_addr.
This relocation has an extra parameter of type dw_sym_handle.
This parameter is the target of the relocation; the offset
of the symbol should be generated.
This is used any time a location expression
involving a
.id DWLocStatic
is generated.
.note DW_W_SEGMENT
Emit a dw_segment.
This relocation has an extra parameter of type dw_sym_handle.
It indicates that the segment portion of the address of the symbol
should be generated.
This is used any time a location expression
involving a
.id DWLocSegment
operation is generated.
.note DW_W_LABEL
Emit a dw_targ_addr.
Used by
.id DWLabel
.period
.note DW_W_SECTION_POS
Emit a uint_32.
This relocation has an extra parameter of type uint called
.id targ_sect
.period
.id targ_sect
parameter is the number of a section for which the current
offset is the target of the relocation.
The relocation is emitted into
.id section
.period
.note DW_W_DEFAULT_FUNCTION
Emit a dw_targ_addr.
Used by
.id DWFormalParameter
.period
.note DW_W_ARANGE_ADDR
Emit a dw_targ_addr.
Used by
.id DWAddress
.period
.note DW_W_UNIT_SIZE
Emit an uint_32 that is the number of bytes of code in the current
compilation unit.
.note DW_W_MAX
Defined for convenience.
This enumerated type starts at 0 and goes to DW_W_MAX.
.endnote
.endnote
.*
.section CLIWrite
.*
.dwfunc void CLIWrite( uint section, const void *block, size_t len )
.np
Writes out the debugging information.
.begnote
.notehdr Parameter
.note section
The section to which the debugging information is written.
.note block
Points to the debugging information block.
.note len
Length of the debugging information block.
.endnote
.*
.section CLIAlloc
.*
.dwfunc void *CLIAlloc( size_t size )
.np
Allocates a memory block of size
.id size
for the library and returns its address.
This function cannot return NULL.
.*
.section CLIFree
.*
.dwfunc void CLIFree( void *blk )
.np
Free the block pointed by
.id blk
.period
.*
.endlevel
.*
.chap Examples
.*
.np
.id This section needs a major rewrite.
.np
The example below shows what functions should be called in order to
store the debugging information for this C program.
.np
N.B.
In this example, for all the CLIWrite() calls, only the section id is
accurate.
Also for all DWLineNum() calls, the advances in machine instruction
address are inaccurate.
.np
test.c:
.millust begin
1 #include <stdlib.h>
2 int    a;
3 typedef near char NCHAR;
4 void main()
5 {
6    NCHAR    b;
7    b := 5;
8 }
.millust end

Functions called by the client and the DWARF library.
.np
Client:
.millust begin
    cli_id = DWInit( DW_LANG_C89, DW_CM_DEBUGGER, "test.c",
		     "c:\mydir", 0x123, 1, CLILoc, CLIType,
		     CLIName, CLIWrite, CLIAlloc, CLIFree );
.millust end
DWARF Library:
.millust begin
    /* Initialize the .debug_line section */
    CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );

    /* Initialize the .debug_abbrevs section */
    CLIWrite( DW_DEBUG_ABBREVS, 0, &info, 50, block );

    /* Initialize the .debug_pubnames section */
    CLIWrite( DW_DEBUG_PUBNAMES, 0, &info, 50, block );

    /* Initialize the .debug_aranges section */
    CLIWrite( DW_DEBUG_ARANGES, 0, &info, 50, block );

    /* Write all strings to the string table */
    CLIWrite( DW_DEBUG_STR, 0, &info, 17, block );
.millust end
Client:
.np
.'clisrc #include <stdlib.h>
.millust begin
    DWLineNum( cli_id, DW_LN_STMT|DW_LN_BLK, 1, 1, 0 );
    DWIncl( id, "stdlib.h" );
    ...Function calls for "stdlib.h"...
    DWInclFini( cli_id );
.millust end
DWARF Library:
.millust begin
    CLIWrite( DW_DEBUG_LINE, 0, &info, 28, block );
    CLIWrite( DW_DEBUG_INFO, 30, &info, 12, block );
.millust end
Client:
.np
.'clisrc int   a;
.millust begin
    DWLineNum( cli_id, DW_LN_STMT, 1, 1, 4 );
    a_dw_handle = DWModSym( cli_id, a_cg_handle, DW_SM_VAR,
		    DW_SM_GLO|DW_SM_FILE, DW_SM_NULL );
.millust end
DWARF Library:
.millust begin
    name = CLIName( a_cg_handle );
    /* It returns the string "a". */
    type = CLIType( a_cg_handle );
    /* It returns DW_FT_INTEGER. */
    loc = CLILoc( a_cg_handle );
    CLIWrite( DW_DEBUG_LINE, 0, &info, 28, block );
    CLIWrite( DW_DEBUG_INFO, 0, &info, 24, block );
    CLIWrite( DW_DEBUG_PUBNAMES, 0, &info, 12, block );
.millust end
Inside CLILoc():
.millust begin
    loc_id = DWLocInt();
    DWLocAtom( cli_id, a_cg_handle, DW_LOC_STATIC );
    /* The actual address will be filled in by the client when
	the debugging information is written to the object file.*/
    a_loc_hd = DWLocFini( loc_id );
    return a_loc_hd;
.millust end
Client:
.np
.'clisrc typedef near char NCHAR;
.millust begin
    DWLineNum( cli_id, DW_LN_STMT, 1, 1, 14 );
    mod_handle = DWMod( cli_id, DW_FT_CHAR, DW_MOD_NEAR );
    nchar_handle = DWModSym( cli_id, nchar_cg_handle,
	    DW_SM_TYPEDEF, DW_SM_NULL, DW_SM_NULL );
.millust end
DWARF Library:
.millust begin
    name = CLIName( nchar_cg_handle );
    /* It returns the string "NCHAR". */
    type = CLIType( nchar_cg_handle );
    /* It returns mod_handle. */
    CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );
    CLIWrite( DW_DEBUG_INFO, 0, &info, 24, block );
.millust end
Client:
.np
.'clisrc void main()
.millust begin
    DWLineNum( cli_id, DW_LN_DEFAULT, 1, 1, 23 );
    pro_handle = DWBegProc( cli_id, DW_SB_NEAR_CALL, DW_FT_VOID,
		    ret_loc_hd, DW_LOC_NULL,
		    DW_SB_GLOBAL_SUB|DW_SB_FUNC_PROTOTYPE );
.millust end
In order to get ret_loc_ad:
.millust begin
    loc_id = DWLocInit();
    DWLocAtom( cli_id, some_cg_handle, DW_LOC_STATIC );
    /* Assume that the return address of main() is stored
	in a symbol with some_cg_handle as its handle.
	The actual address will be filled in by the
	client when the debugging information is written
	to the object file.                               */
    ret_loc_ad = DWLocFini( cli_id );
.millust end
DWARF Library:
.millust begin
    CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );
.millust end
Client:
.np
.'clisrc {
.millust begin
    DWLineNum( cli_id, DW_LN_BLK, 1, 1, 0 );
.millust end
DWARF Library:
.millust begin
    CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );
.millust end
Client:
.np
.'clisrc NCHAR b;
.millust begin
    DWLineNum( cli_id, DW_LN_STMT, 1, 1, 10 );
    b_handle = DWModSym( cli_id, b_cg_handle, DW_SM_VAR,
		    DW_SM_NULL, DW_SM_LOC|DW_SM_ROUT );
.millust end
DWARF Library:
.millust begin
    loc = CLILoc( b_cg_handle );
    name = CLIName( b_cg_handle );
    /* It returns the string "b". */
    type = CLIType( b_cg_handle );
    /* It returns nchar_handle. */
    CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );
.millust end
Inside CLILoc():
.millust begin
    loc_id = DWLocInt();
    DWLocAtom( cli_id, b_cg_handle, DW_LOC_STACK );
    /* The offset from stack frame base will be filled in by
	the client when the debugging information is written
	to the object file.   */
    b_loc_hd = DWLocFini( loc_id );
    return b_loc_hd;
.millust end
Client:
.np
.'clisrc b := 5;
.millust begin
    DWLineNum( cli_id, DW_LN_STMT, 1, 4, 14 );
.millust end
DWARF Library:
.millust begin
    CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );
.millust end
Client:
.np
.'clisrc }
.millust begin
    DWLineNum( cli_id, DW_LN_DEFAULT, 1, 1, 4 );
    DWEndProc( cli_id, pro_handle );
    main_handle = DWModSym( cli_id, main_cg_handle, DW_SM_SUB,
		    DW_SM_NULL, DW_SM_NULL );
.millust end
DWARF Library:
.millust begin
    name = CLIName( main_cg_handle );
    /* It returns the string "main" */
    type = CLIType( main_cg_handle );
    /* It returns pro_handle */
    CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );
    CLIWrite( DW_DEBUG_INFO, -50, &info, 86, block );
    CLIWrite( DW_DEBUG_REF, 0, &info, 12, block );
    CLIWrite( DW_DEBUG_PUBNAMES, 0, &info, 12, block );
    /* For the global object "main" */
.millust end
Client:
.millust begin
    DWFini( cli_id );
.millust end
DWARF Library:
.millust begin
    CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );
    CLIWrite( DW_DEBUG_INFO, -120, &info, 54, block );
.millust end
.*
.chap Revision History
.*
.begnote
.note Draft 5
Changed the arguments to a number of the function calls for use with
draft 5 of dwarf.
.note Draft 6
Changed the arguments to a number of the function calls for use with
draft 6 of dwarf.
.endnote
