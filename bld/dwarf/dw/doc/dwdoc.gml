:PSC proc='rita'.
**RTA**wgmlstd.rta
:ePSC.
:LAYOUT.
:HEADING
threshold = 3
:eLAYOUT.
:GDOC.
:FRONTM.
:TITLEP.
:TITLE.SPECIFICATION FOR THE DWARF WRITING LIBRARY
:TITLE.Draft #6
:DATE.
:eTITLEP.
:BODY.
:H0.Debugging Information
:p.The include file "dw.h" should be included to access the DW library.

:H1.Data Types
:p.The following types are defined in "dwcnf.h" and may be redefined
if the entire library is to be recompiled.  ("dw.h" automatically
includes "dwcnf.h".);
:DL.
:DTHD.Type
:DDHD.Description
:DT.dw_sym_handle
:DD.Has a client defined meaning; the DW library will pass these
back to the client in :hp2.CLIRelocs:ehp2. for :hp2.DW_W_STATIC:ehp2.,
and :hp2.DW_W_SEGMENT:ehp2..
:DT.dw_targ_addr
:DD.The contents of :hp2.dw_targ_addr:ehp2. is unimportant to the
DW library; it is only used for :hp2.sizeof( dw_targ_addr ):ehp2..
A :hp2.dw_targ_addr:ehp2. is the type that will be emitted for
relocations to run-time addresses.
:DT.dw_targ_seg
:DD.This is the size of the quantity that :hp2.DW_W_SEGMENT:ehp2. emits.
:DT.dw_addr_offset
:DD.The type used for offsets from some base address.
For example, the :hp2.start_scope:ehp2. parameter to typing routines,
or the :hp2.addr:ehp2. parameter to line number information.
The code assumes this is an unsigned integer type.
:DT.dw_addr_delta
:DD.An integer type that can hold the largest possible
difference between the :hp2.addr:ehp2. parameter for two subsequent
calls to :hp2.DWLineNum:ehp2..
:DT.dw_linenum
:DD.A line number.  It must be an unsigned integer type.
:DT.dw_linenum_delta
:DD.dw_linenum_delta is a type that can hold the largest possible
difference between two adjacent line
numbers passed to :hp2.DWLineNum:ehp2. or :hp2.DWReference:ehp2..
:DT.dw_column
:DD.A column number.  It must be an unsigned integer type.
:DT.dw_column_delta
:DD.dw_column_delta is a type that can hold the largest possible
difference between two adjacent column numbers
passed to :hp2.DWReference:ehp2..
:DT.dw_size_t
:DD.Used for sizes of various things such as block constants
(i.e. for DWAddConstant) and the :hp2.size:ehp2. parameter to
:hp2.CLIWrite:ehp2..
:DT.dw_uconst
:DD.An unsigned integer type that can hold the largest possible
unsigned integer constant.
:DT.dw_sconst
:DD.A signed integer type that can hold the largest possible
signed integer constant.
:edl.

:H1.Initialization and Finalization

:P.In the following functions, unless specified otherwise all strings
are assumed to be null-terminated.
:P.The DW library does not assume that a pointer passed to it is valid
beyond the function call used to pass it.
For example, you can pass the address of an auto-buffer that contains a
string.
:P.All names passed to the DW library should be unmangled.
:P.The :HP2.cli:eHP2. parameter required for all DW functions except
:HP2.DWInit:eHP2. is assumed to be a valid value returned by a call to
:HP2.DWInit:eHP2..
:P.Currently DWENTRY is defined to be nothing.
It was created in case there is ever a need to put the DW library into
a DLL.
:H2.dw_client DWENTRY DWInit( dw_init_info *info );
:P.Initialization for a compilation unit.
Return an unique client id.  This function will call client functions
passed to it, so any client function initialization must be done before
the call to DWInit.
:XMP.
:SF font=4.
typedef struct {
    void		(*reloc)( uint, uint, ... );
    void		(*write)( uint, const void *, dw_size_t );
    void		(*seek)( uint, long, uint );
    long		(*tell)( uint );
    void *		(*alloc)( size_t );
    void		(*free)( void * );
} dw_funcs;

typedef struct {
    dw_lang		language;
    uint_8		compiler_options;
    const char *	producer_name;
    jmp_buf		exception_handler;
    dw_funcs		funcs;
} dw_init_info;
:eSF.
:eXMP.
:DL.
:DTHD.Member
:DDHD.Description
:DT.language
:DD.Language used.
:DL tsize=24.
:DTHD.Constant
:DDHD.Language
:DT.DWLANG_C
:DD.ISO/ANSI C
:DT.DWLANG_CPP
:DD.C++
:DT.DWLANG_FORTRAN
:DD.FORTRAN77
:eDL.
:DT.compile_options
:DD.Compilation option, which is a combination of bits:
:DL tsize=20.
:DTHD.Bit
:DDHD.Description
:DT.DW_CM_BROWSER
:DD.The library generates the debugging information for the class
browser.
:DT.DW_CM_DEBUGGER
:DD.The library generates the debugging information for the debugger.
:DT.DW_CM_UPPER
:DD.For FORTRAN - The compiler converts all identifier names to upper
case.
:DT.DW_CM_LOWER
:DD.For FORTRAN - The compiler converts all identifier names to lower
case.
:eDL.
:DT.producer
:DD.A string that identifies the compiler.
:DT.exception_handler
:DD.If the library ends up in a situation which it can't handle (can
we say
bug ;-) ) this jmp_buf will be called with a non-zero value.
This is a fatal exit, and the client should not call any of the DW
functions.  (FIXME: The library is currently not very good at cleaning
up memory in these situations.);
:DT.funcs
:DD.These functions are described in a later section.
The initialization routines may call any of them; so any initialization
necessary for these routines must be done before DWInit is called.
:eDL.
:P.The details of the above functions are discussed in Part 3.

:H2.void DWENTRY DWFini( dw_client cli );
:P.Finalize the debugging information generator.  This routine must be
called last.  It frees any structures that the DW library required, and
flushes all the debugging information.

:H2.dw_handle DWENTRY DWBeginCompileUnit( dw_client cli, const char *source_filename, const char *directory, dw_loc_handle segment, const unsigned offset_size );
:P.This function is called some time after :hp2.DWInit:ehp2..
The only other DW functions that can be called in between are those
dealing with location expressions.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.source_filename
:DD.Name of the source file.
:DT.directory
:DD.Compilation directory.
:DT.segment
:DD.A location expression who's result is the code segment portion
of the low_pc and high_pc.
:DT.offset_size
:DD.The size in bytes of the offset portion of an address in this compile unit
:eDL.
:P.The following CLIRelocs will be required:
:sL.
:LI.DW_W_HIGH_PC
:LI.DW_W_LOW_PC
:LI.DW_W_SECTION_POS
:LI.DW_W_UNIT_SIZE
:eSL.

:H2.void DWENTRY DWEndCompilationUnit( dw_client cli );
:P.This function pairs up with :hp2.DWBeginCompilationUnit:ehp2..
After this, until the next :hp2.DWBeginCompilationUnit:ehp2., the
only valid calls are those made to location expression routines
(or :hp2.DWFini:ehp2.).

:H0.Ordering Considerations
:p.In general the DW routines are called in an order that matches the
order of the declarations during the source program.  The sole exception
to this are the Macro information routines.  Since it is possible to
have a separate preprocessor pass, the library assumes that these
routines can be called before any of the other routines.  That is
why the macro routines have a separate mechanism for specifying
file and line number.

:H0.Macro Information
:H2.void DWENTRY DWMacStartFile( dw_client cli, dw_linenum line, const char *name );
:p.Subsequent DWMac calls refer to the named file.

:H2.void DWENTRY DWMacEndFile( dw_client cli );
:p.End the current included file.

:H2.dw_macro DWENTRY DWMacDef( dw_client cli, dw_linenum line, const char *name );
:P.Defines a macro.
:HP2.name:eHP2. is the name of the macro.
A :HP2.dw_macro:eHP2. is returned and must be used in a subsequent call
to :HP2.DWMacFini:eHP2. (and possibly :HP2.DWMacParam:eHP2.).

:H2.void DWENTRY DWMacParam( dw_client cli, dw_macro mac, const char *name );
:P.Adds a parameter to the macro definition :HP2.mac:eHP2..
:HP2.name:eHP2. is the name of the parameter with no leading or
trailing white-space.
The order of parameters must be the same as they appear in the source
program.

:H2.void DWENTRY DWMacFini( dw_client cli, dw_macro mac, const char *def );
:P.Finishes the macro definition :HP2.mac:eHP2..
:HP2.def:eHP2. is the definition string.

:H2.void DWENTRY DWMacUnDef( dw_client cli, dw_linenum line, const char *name );
:P.Undefines the macro named :HP2.name:eHP2..

:H2.void DWENTRY DWMacUse( dw_client cli, dw_linenum line, const char *name );
:p.Indicate where the macro named :hp2.name:ehp2. is used.

:H0.File and Line-Number Management

:H2.void DWENTRY DWSetFile( dw_client cli, const char *file );
:P.Specifies the current file.
The default is for the source_filename parameter from the dw_init_info
to be the current file.

:H2.void DWLineNum( dw_client cli, uint info, dw_linenum line, uint col, dw_addr_offset addr );
:P.Sets the current source line number and machine address.
The line numbers information of all instructions, not just
declarations, are stored by this routine.
Note that all source line numbers are relative to the beginning of
their corresponding source file.
So the line number of the first line of an included file is one.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.info
:DD.The information about the line, which is established by the
combination of the following bits:
:DL tsize=20.
:DTHD.Bit
:DDHD.Description
:DT.DW_LN_DEFAULT
:DD.There is no special information about the line.
:DT.DW_LN_STMT
:DD.The line is the beginning of a statement.
:DT.DW_LN_BLK
:DD.The line is the beginning of a block.
:eDL.
:DT.line
:DD.Source line number, numbered beginning with one on the first line
of the file.
:DT.col
:DD.Source column number, which begins at 1.
:DT.addr
:DD.Address of instruction relative to the beginning of the compilation
unit.  If it is at all possible the client should call :hp2.DWLineNum:ehp2.
with increasing addrs.	The line parameter does not have to be
increasing.  The size of the emitted line number information is smaller
if increasing addrs are used.  (There is also an implementation
limitation that the maximum decrease of addr between two calls is 32768.);
:eDL.

:H2.void DWENTRY DWDeclFile( dw_client cli, const char *name );
:p.Subsequent declarations are from the file named :hp2.name:ehp2..

:H2.void DWENTRY DWDeclPos( dw_client cli, dw_linenum line, dw_column column );
:p.The next declaration occurs at the indicated line and column in the
source file set by the last call to :hp2.DWDeclFile:ehp2..  Note that
the position is only used for the immediate next declaration.  If there
are multiple declarations on the same line, then multiple calls should
be made.

:H2.void DWENTRY DWReference( dw_client cli, dw_linenum line, dw_column column, dw_handle dependant );
:P.Indicate that in the source code there is a reference to the dependant.
This reference is attributed to the current scope of debugging information.
(i.e., if it is done inside a structure, then the structure is considered
to be the "referencer").

:H0.Location Expression Routines
:P.Many functions require a :HP2.dw_loc_handle:eHP2..
These are handles for expressions that the debugger will evaluate.
A :HP2.dw_loc_handle:eHP2. can be either a single expression, or a list
of expressions created by :HP2.DWListFini:eHP2..  The BROWSER is only
interested in whether a location expression is present or not; so
when creating BROWSER output the client may create an empty location
expression and use that wherever appropriate.
:P.The expressions are evaluated on a stack machine, with operations
described later.
In some cases the stack will be initially empty, in other cases (such
as when calculating the address of a structure field) some base address
will be on the stack.
:P.A location expression is limited to roughly 64K.  Since each op-code
is a single byte, this shouldn't pose much of a limitation (famous
last words).  The destination of the branch instructions
:hp2.DW_LOC_BRA:ehp2. and :hp2.DW_LOC_SKIP:ehp2. must be within
32K of the current instruction.  (This is a limitation of the DWARF
format, not a limitation of the DW library.);

:H2.dw_loc_id DWENTRY DWLocInit( dw_client cli );
:P.First function called to create a location expression for a symbol.
An unique :HP2.dw_loc_id:eHP2. is returned to the front end.

:H2.dw_loc_label DWENTRY DWLocNewLabel( dw_client cli, dw_loc_id loc );
:P.Create a label for the location expression being built in :hp2.loc:ehp2..
This label can be used for forward or backward references by
:hp2.DW_LOC_SKIP:ehp2. and :hp2.DW_LOC_BRA:ehp2..

:H2.void DWENTRY DWLocSetLabel( dw_client cli, dw_loc_id loc, dw_loc_label label );
:P.Give the label :hp2.label:ehp2. the address of the next operation
emitted into the location expression :hp2.loc:ehp2..

:H2.void DWENTRY DWLocReg( dw_client cli, dw_loc_id loc, uint reg );
:P.This 'operation' informs the debugger that the value it seeks is in
the register named by :HP2.reg:eHP2..
FIXME: need to define the possible values of :HP2.reg:eHP2..

:H2.void DWENTRY DWLocStatic( dw_client cli, dw_loc_id loc, dw_sym_handle sym );
:P.This operation pushes the address of :hp2.sym:ehp2. on the stack.

:H2.void DWENTRY DWLocSegment( dw_client cli, dw_loc_id loc, dw_sym_handle sym );
:P.This operation pushes the segment of the address
of :hp2.sym:ehp2. on the stack.

:H2.void DWENTRY DWLocConstU( dw_client cli, dw_loc_id loc, dw_uconst value );
:P.Pushes an atom which is has an unsigned constant value :HP2.value:eHP2..

:H2.void DWENTRY DWLocConstS( dw_client cli, dw_loc_id loc, dw_sconst value );
:P.Pushes an atom which is has a signed constant value :HP2.value:eHP2..

:H2.void DWENTRY DWLocOp0( dw_client cli, dw_loc_id loc, dw_optype op );
:p.Performs one of the operations listed below.
:DL tsize=24 break.
:DTHD.Operation
:DDHD.Description
:DT.DW_LOC_ABS
:DD.It pops the top stack entry and pushes its absolute value.
:DT.DW_LOC_AND
:DD.It pops the top two stack values, performs the logical AND
operation on the two, and pushes the result.
:DT.DW_LOC_DEREF
:DD.It pops the top stack entry and treats it as an address.
The value retrieved from that address is pushed.
The size of data retrieved from the dereferenced address is an
addressing unit.
:DT.DW_LOC_DIV
:DD.It pops the top two stack values, divides the former second entry
by the former top of the stack using signed division, and pushes the
result.
:DT.DW_LOC_DROP
:DD.It pops the value at the top of the stack.
:DT.DW_LOC_DUP
:DD.It duplicates the value at the top of the stack.
:DT.DW_LOC_EQ
:DD.Pop two entries from stack, push 1 if they are equal; push 0 otherwise.
:DT.DW_LOC_GE, DW_LOC_GT, DW_LOC_LE, DW_LOC_LT
:DD.These operation pop the top two stack values, compare the former
top of stack from the former second entry, and pushes 1 onto stack if
the comparison is true, 0 if it is false.
The comparisons are signed comparison.
:DT.DW_LOC_MINUS
:DD.It pops the top two stack values, subtracts the former top of the
stack from the former second entry, and pushes the result.
:DT.DW_LOC_MOD
:DD.It pops the top two stack values, and pushes the result of the
calculation: former second stack entry modulo the former top of the
stack.
:DT.DW_LOC_MUL
:DD.It pops the top two stack values, multiplies them together, and
pushes the result.
:DT.DW_LOC_NE
:DD.Pop two entries from stack, push 0 if they are equal; push 1 otherwise.
:DT.DW_LOC_NEG
:DD.It pops the top value and pushes its negation.
:DT.DW_LOC_NOP
:DD.A placeholder; has no side-effects.
:DT.DW_LOC_NOT
:DD.It pops the top value and pushes its logical complement.
:DT.DW_LOC_OR
:DD.It pops the top two stack entries, performs the logical OR
operation on them, and pushes the result.
:DT.DW_LOC_OVER
:DD.It duplicates the entry currently second in the stack at the top of
the stack.
:DT.DW_LOC_PLUS
:DD.It pops the top two stack entries, and pushes their sum.
:DT.DW_LOC_ROT
:DD.It rotates the first three stack entries.
The entry at the top of the stack becomes the third entry, the second
entry becomes the top, and the third entry becomes the second.
:DT.DW_LOC_SHL
:DD.It pops the top two stack entries, shifts the former second entry
left by the number of bits specified by the former top of the stack,
and pushes the result.
:DT.DW_LOC_SHR
:DD.It pops the top two stack entries, shifts the former second entry
right (logically) by the number of bits specified by the former top of
the stack, and pushes the result.
:DT.DW_LOC_SHRA
:DD.It pops the top two stack entries, shifts the former second entry
right (arithmetically) by the number of bits specified by the former
top of the stack, and pushes the result.
:DT.DW_LOC_SWAP
:DD.It swaps the top two stack entries.
:DT.DW_LOC_XDEREF
:DD.It provides an extended dereference mechanism.
The entry at the top of the stack is treated as an address.
The second stack entry is treated as an "address space identifier" for
those architectures that support multiple address spaces.
The top two stack elements are popped, a data item is retrieved through
an implementation-defined address calculation and pushed as the new
stack top.
The size of data retrieved is an addressing unit.
:DT.DW_LOC_XOR
:DD.It pops the top two stack entries, performs the logical
EXCLUSIVE-OR operation on them, and pushes the result.
:eDL.

:H2.void DWENTRY DWLocOp( dw_client cli, dw_loc_id loc, dw_optype op, ... );
:p.Performs one of the following operations:
:DL tsize=24 break.
:DTHD.Operation
:DDHD.Description
:DT.DW_LOC_BRA
:DD.It is followed by a dw_loc_label operand.
This operation pops the top stack entry, if the value is not zero, then
jump to the label.
:DT.DW_LOC_BREG
:DD.Followed by two operands, the first is a register, and the second
is an dw_sconst to add to the value in the register.  The result is pushed
onto the stack.
:DT.DW_LOC_FBREG
:DD.Takes one dw_sconst parameter which is added to the value calculated
by the frame_base_loc parameter to the current subroutine, then pushed
on the stack.
:DT.DW_LOC_PICK
:DD.It is followed by a uint operand which is an index.
The stack entry with the specified index (0 through 255, inclusive; 0
means the top) is pushed on the stack.
:DT.DW_LOC_PLUS_UCONST
:DD.It is followed an dw_sconst operand.
It pops the top stack entry, adds it to the operand and pushes the
result.
:DT.DW_LOC_SKIP
:DD.It is followed by a dw_loc_label operand.  Control is transferred
immediately to this label.
:eDL.

:H2.dw_loc_handle DWENTRY DWLocFini( dw_client cli, dw_loc_id loc );
:P.Ends the location expression for a symbol, and returns a handle that
may be passed to other DW routines.

:H2.dw_list_id DWENTRY DWListInit( dw_client cli );
:P.First function called to create a location list for a symbol.

:H2.void DWENTRY DWListEntry( dw_client cli, dw_list_id id, dw_sym_handle beg, dw_sym_handle end, dw_loc_handle loc );
:P.Define an entry in the location list.
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.beg
:DD.A beginning address.
This address is relative to the base address of the compilation unit
referencing this location list.
It marks the beginning of the range over which the location is valid.
:DT.end
:DD.A ending address.
This address is relative to the base address of the compilation unit
referencing this location list.
It marks the first address past the end of the range over which the
location is valid.
Overlapping ranges are possible and are interpreted to mean that the
value may be found in one of many places during the overlap.
A CLIReloc for
:hp2.DW_W_LABEL:ehp2. will be made for each dw_sym_handle.
:DT.loc
:DD.A location expression describing the location of the object over
the range specified by the beginning and end addresses.
:eDL.

:H2.dw_loc_handle DWENTRY DWListFini( dw_client cli, dw_list_id );
:P.Finishes the creation of the location list.

:H2.void DWENTRY DWLocTrash( dw_client cli, dw_loc_handle loc );
:P.Frees the memory associated with the location expression or list loc.
A location expression/list can be created and
used over and over again until it is
freed by calling this function.

:H0.Typing Information
:p.Unless otherwise noted, calls to these functions emit debugging
information immediately.  The DWARF format requires that debugging
information appear in the same order as it does in the source code.
So, for example, a structure's fields must be created in the same
order that they appear in the source program.
:P.Some of the following functions have common parameters.  Here is the
documentation for these common parameters:
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.char *name
:DD.A null-terminated type name.  i.e., "struct foobar {}" has the name
foobar.  If this parm is NULL then no name is emitted.
:DT.dw_addr_offset start_scope
:DD.This is the offset from the low_pc value for the enclosing block that
the declaration occurs at.  This is most commonly 0.
:DT.uint flags
:DD.Some routines have additional flags available here; but unless otherwise
noted, the following are always available:
:DL.
:DTHD.Flag
:DDHD.Description
:DT.DW_DECLARATION
:DD.The object is a declaration, not a definition
:DT.DW_FLAG_PRIVATE
:DD.The object has the C++ private attribute.
:DT.DW_FLAG_PROTECTED
:DD.The object has the C++ protected attribute.
:DT.DW_FLAG_PUBLIC
:DD.The object has the C++ public attribute.
:EDL.
:EDL.

:H2.dw_handle DWENTRY DWFundamental( dw_client cli, char * name, unsigned fund_idx, unsigned size );
:p.Get a handle for a fundamental type.  fund_idx is one of the following:
:sl.
:LI.DW_FT_ADDRESS
:LI.DW_FT_BOOLEAN
:LI.DW_FT_COMPLEX_FLOAT
:LI.DW_FT_FLOAT
:LI.DW_FT_SIGNED
:LI.DW_FT_SIGNED_CHAR
:LI.DW_FT_UNSIGNED
:LI.DW_FT_UNSIGNED_CHAR
:esl.
:P.For convenience, DW_FT_MIN, and DW_FT_MAX are defined.  A valid
fundamental type is in the range DW_FT_MIN <= ft < DW_FT_MAX.  The DW
library will always return the same handle when called with the same
fundamental type (so the client does not need to save fundamental
type handles).
:DL.
:DTHD.Parameters
:DDHD.Description
:DT.name
:DD.The name of the type being defined.
:DT.size
:DD.The size in bytes of the type being defined.
:eDL.

:H2.dw_handle DWENTRY DWModifier( dw_client cli, dw_handle base_type, uint modifiers );
:P.Specifies a modifier to a type.
:HP2.base_type:eHP2. is the base type to be modified with the modifier
:HP2.modifier:eHP2..
The available modifiers are:
:DL tsize=20.
:DTHD.Modifier Constant
:DDHD.Description
:DT.DW_MOD_CONSTANT
:DD.The object is a constant
:DT.DW_MOD_VOLATILE
:DD.The object is volatile.
:DT.DW_MOD_NEAR
:DD.The object is a near object.
:DT.DW_MOD_FAR
:DD.The object is a far object.
:DT.DW_MOD_HUGE
:DD.The object is a huge object.
:DT.DW_MOD_FAR16
:DD.The object is a far16 object.
:eDL.

:H2.dw_handle DWENTRY DWTypedef( dw_client cli, dw_handle base_type, const char *name, dw_addr_offset start_scope, uint flags );
:P.This function gives a name to a type.  The :hp2.name:ehp2. must not be NULL.
The flag value :hp2.DW_FLAG_DECLARATION:ehp2. is not allowed.

:H2.dw_handle DWENTRY DWPointer( dw_client cli, dw_handle base_type, uint flags );
:P.Declares a pointer type.
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.base_type
:DD.The pointed-at type.
:DT.flags
:DD.Only the following flags are available:
:DL.
:DTHD.Flags
:DDHD.Description
:DT.DW_FLAG_REFERENCE
:DD.Declare a pointer that is dereferenced automatically.
:DT.DW_PTR_TYPE_NORMAL
:dd.A normal pointer (i.e. a model dependant pointer).
:DT.DW_PTR_TYPE_NEAR16
:dd.A near 16-bit pointer.
:DT.DW_PTR_TYPE_FAR16
:dd.A far 16-bit pointer.
:DT.DW_PTR_TYPE_HUGE
:dd.A huge 16-bit pointer.
:DT.DW_PTR_TYPE_NEAR32
:dd.A near 32-bit pointer.
:DT.DW_PTR_TYPE_FAR32
:dd.A far 32-bit pointer.
:eDL.
:eDL.

:H2.dw_handle DWENTRY DWString( dw_client cli, dw_loc_handle string_length, dw_size_t byte_size, const char *name, dw_addr_offset start_scope, uint flags );
:P.Declares a type to be a block of characters.
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.string_length
:DD.If this parameter is non-NULL then it is a location expression that
the debugger executes to get the address where the length of the string
is stored in the program.  In this case the :hp2.byte_size:ehp2.
parameter describes the number of bytes to be retrieved at the location
calculated.  If :hp2.byte_size:ehp2. is 0, then the debugger will
use :hp2.sizeof( long ):ehp2..
:DT.byte_size
:DD.If :hp2.string_length:ehp2. is NULL then this parameter is the number
of bytes in the string.  Otherwise see :hp2.string_length:ehp2..
:eDL.

:H2.dw_handle DWENTRY DWMemberPointer( dw_client cli, dw_handle containing_struct, dw_loc_handle use_location, dw_handle base_type, const char *name, unsigned flags );
:P.Declares a C++ pointer type to a data or function member of a class
or structure.
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.containing_struct
:DD.A handle to the class or struct to whose members objects of this type
may point.
:DT.use_location
:DD.This refers to the location expression which describes how to get
to the member it points to from the beginning of the entire class.
It expects the base address of the structure/class object to be on the
stack before the debugger starts to execute the location description.
:DT.base_type
:DD.The type of the member to which this object may point to.
:eDL.

:H1.Array Types

:H2.dw_handle DWENTRY DWBeginArray( dw_client cli, dw_handle elt_type, uint stride_size, const char *name, dw_addr_offset scope, uint flags );
:P.Begin the declaration of an array. This function call must be followed by
calls to :hp2.DWArrayDimension:ehp2. and :hp2.DWEndArray:ehp2..
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.elt_type
:DD.Handle for the type of the elements of this array.
:DT.stride_size
:DD.If this value is non-zero then it indicates the number of bits of
each element of the array.  (Useful if the number of bits used to store
an element in the array is different from the number of bits used to
store an individual element of type :hp2.elt_type:ehp2..);
:eDL.

:H2.void DWENTRY DWArrayDimension( dw_client cli, const dw_dim_info *info );
:P.Add a dimension to the previously started array.  This function must
be called for each dimension in the order that the dimensions appear in
the source program.  :hp2.info:ehp2. points to an instance of the following
structure:
:XMP.
:SF font=4.
typedef struct {
    dw_handle	index_type;
    dw_uconst	lo_data;
    dw_uconst	hi_data;
} dw_dim_info;
:eSF.
:eXMP.
:DL.
:DTHD.Field
:DDHD.Description
:DT.hi_bound_fmt
:DD.This is similar to :hp2.lo_bound_fmt:ehp2. but describes the
high bound of this dimension.
:DT.index_type
:DD.This is the handle of the type of the indicies for this dimension.
:DT.lo_data
:DD.The low bound of the array.
:DT.hi_data
:DD.The upper bound of the array.
:eDL.

:H2.void DWENTRY DWEndArray( dw_client cli, dw_handle array_hdl, dw_handle elt_type, uint stride_size, const char *name, dw_addr_offset scope, uint flags );
:P.This finishes the writing of the record to describe the array
A sufficient number of calls to :hp2.DWArrayDimension:ehp2. must have been
made before :hp2.DWEndArray:ehp2. is called.

:H1.Structure Types

:H2.dw_handle DWENTRY DWStruct( dw_client cli, uint kind );
:P.Create a handle for a structure type that will be defined later.
This handle can be used for other DW routines even before
:hp2.DWBeginStruct:ehp2. has been called.
:DL.
:DTHD.Kind
:DDHD.Description
:DT.DW_ST_CLASS
:DD.A C++ class type.
:DT.DW_ST_STRUCT
:DD.A structure type.
:DT.DW_ST_UNION
:DD.A union type.
:eDL.

:h2.void DWENTRY DWBeginStruct( dw_client cli, dw_handle struct_hdl, dw_size_t size, const char *name, dw_addr_offset scope, uint flags );
:p.Begin the declaration of the structure reserved by a call to
:hp2.DWStruct:ehp2..  This function begins a nesting of the debugging
information.  Subsequent calls, up to the corresponding
:hp2.DWEndStruct:ehp2. call, to the DW library become children
of this structure. i.e., this function marks the beginning of the
scope of the structure definition.
:dl.
:dthd.Parameter
:ddhd.Description
:dt.struct_hdl
:dd.A dw_handle returned by a call to :hp2.DWStruct:ehp2..
:dt.size
:dd.If this is non-zero it indicates the number of bytes required to
hold an element of this structure including any padding bytes.
:edl.

:h2.void DWENTRY DWAddFriend( dw_client cli, dw_handle friend );
:p.Add :hp2.friend:ehp2. as a friend to the current structure.

:h2.dw_handle DWENTRY DWAddInheritance( dw_client cli, dw_handle ancestor, dw_loc_handle loc, uint flags );
:p.Indicate the the current structure inherits from another structure.
:dl.
:dthd.Parameter
:ddhd.Description
:dt.ancestor
:dd.The handle of the ancestor to be inherited.
:dt.loc
:dd.A location expression that describes the location of the beginning
of the data members contributed to the entire class by the ancestor
relative to the beginning of the address of the data members of the
entire class.
:dt.flags
:dd.In addition to the common values of :hp2.flags:ehp2., the flag
:hp2.DW_FLAG_VIRTUAL:ehp2. may be supplied to indicate that the
inheritance serves as a virtual base class.
As well, the flag :hp2.DW_FLAG_DECLARATION:ehp2. is not allowed here.
:edl.

:h2.dw_handle DWENTRY DWAddField( dw_client cli, dw_handle field_hdl, dw_loc_handle loc, const char *name, uint flags );
:p.Add a data member to a structure.
:dl.
:dthd.Parameter
:ddhd.Description
:dt.field_hdl
:dd.The dw_handle of the type of this field.
:dt.loc
:dd.A location expression which expects the base address of the structure
to be pushed on the stack and calculates the base address of this field.
If the structure is a union type, then this parameter may be NULL.  If
this is a static data member of a class then this parameter may be NULL
if the actual definition of the parameter is outside the class.
:dt.flags
:dd.The additional flag :hp2.DW_FLAG_STATIC:ehp2. may be used to indicate
a static structure member.
:edl.

:h2.dw_handle DWENTRY DWAddBitField( dw_client cli, dw_handle field_hdl, dw_loc_handle loc, dw_size_t byte_size, uint bit_offset, uint bit_size, const char *name, uint flags );
:p.Add a bitfield member to a structure.
:dl.
:dthd.Parameter
:ddhd.Description
:dt.field_hdl
:dd.the dw_handle of the type of this field.
:dt.loc
:dd.A location expression which expects the base address of the structure
most closely containing the bit field to be pushed and the stack, and
which calculates the base address of this field.
:dt.byte_size
:dd.This field must be the non-zero byte size of the unit of storage
containing the bit-field.  This is required only if the storage
required cannot be determined by the type of the bit-field (i.e.,
padding bytes).  If the size can be determined by the type of the
bit-field, then this value may be 0.
:dt.bit_offset
:dd.The number of bits to the left of the leftmost (most significant);
bit of the bit field value.
:dt.bit_size
:dd.The number of bits occupied by this bit-field value.
:edl.

:H2.void DWENTRY DWEndStruct( dw_client cli );
:p.End the current structure.  Client must ensure proper Begin/End
matching.

:H1.Enumeration Types

:H2.dw_handle DWENTRY DWBeginEnumeration( dw_client cli, dw_size_t byte_size, const char *name, dw_addr_offset scope, uint flags );
:P.Begin the definition of an enumerated type.	:hp2.byte_size:ehp2. is
the number of bytes required to hold an instance of this enumeration.
This call must be followed by calls to :hp2.DWAddConstant:ehp2. and
:hp2.DWEndEnumeration:ehp2..  No other DW calls may be made before
the call to :hp2.DWEndEnumeration:ehp2..  The DWARF standard requires
that the constants be defined in :hp1.reverse:ehp1. order to which they
appear in the source program.

:h2.void DWENTRY DWAddConstant( dw_client cli, dw_uconst value, const char *name );
:p.Add the constant :hp2.value:ehp2. (that is :hp2.byte_size:ehp2. bytes
large as determined by the parameter to :hp2.DWBeginEnumeration:ehp2.);
with the name :hp2.name:ehp2. to the current enumeration.

:h2.void DWENTRY DWEndEnumeration( dw_client cli );
:p.Finish the current enumeration.


:h1.Subroutine Type Declarations
:p.These function calls deal with declarations of subroutines.	That is,
their prototypes, or for use in creating function pointers.
:h2.dw_handle DWENTRY DWBeginSubroutineType( dw_client cli, dw_handle return_type, const char *name, dw_addr_offset scope, uint flags );
:p.Begin the nested declaration of the subroutine type.  All calls
to the DW library after this, until :hp2.DWEndSubroutineType:ehp2.
are in the scope of the declaration of the subroutine type.  (i.e.,
if it's a prototyped C function, then declarations before
:hp2.DWEndSubroutineType:ehp2. are similar to declarations
inside the prototype.)	Parameters for this type are declared using
the entries :hp2.DWAddParmToSubroutineType:ehp2. and
:hp2.DWAddEllipsisToSubroutineType:ehp2..
:dl.
:dthd.Parameter
:ddhd.Description
:dt.return_type
:dd.If the function is void, this parameter must be NULL.  Otherwise
it is a handle for the return type of the subroutine.
:dt.flags
:dd.In addition to the standard flags, :hp2.DW_FLAG_PROTOTYPED:ehp2.
indicates that the declaration of the subroutine type was prototyped
in the source code.  As well, the "address class" set of flags used in
:hp2.DWPointer:ehp2. are also allowed here (e.g. :hp2.DW_TYPE_FAR16:ehp2. etc.)
:edl.

:h2.void DWENTRY DWEndSubroutineType( dw_client cli );
:p.The client must ensure that proper Begin/End matching is done.

:h1.Lexical Blocks
:h2.dw_handle DWENTRY DWBeginLexicalBlock( dw_client cli, dw_loc_handle segment, const char *name );
:p.Begin a new lexical scope.  :hp2.name:ehp2. may be NULL indicating
an un-named scope.  Two CLIReloc calls will made, one for
:hp2.DW_W_LOW_PC:ehp2. and one for :hp2.DW_W_HIGH_PC:ehp2. which
indicate the first byte of the scope, and the first byte beyond the end
of the scope.  :hp2.segment:ehp2. if non-null is an expression that
evaluates to the segment this block is in.

:h2.void DWENTRY DWEndLexicalBlock( dw_client cli );
:p.End a lexical scope.  As usual, the client must ensure that
Begin/End pairs match.

:h1.Common Blocks
:h2.dw_handle DWENTRY DWBeginCommonBlock( dw_client cli, dw_loc_handle loc, dw_loc_handle segment, const char *name, unsigned flag );
:p.Begin the declarations for the common block named :hp2.name:ehp2. and
located at :hp2.loc:ehp2..  :hp2.segment:ehp2. if non-null indicates which
segment the common block is in.  The only flag that is valid for the
:hp2.flag:ehp2. parameter is :hp2.DW_FLAG_DECLARATION:ehp2..

:h2.void DWENTRY DWEndCommonBlock( dw_client cli );
:p.End of declarations for the common block.

:h2.dw_handle DWENTRY DWIncludeCommonBlock( dw_client cli, dw_handle common_block );
:p.Used in the subroutine scope that references the common block.

:h1.Subroutines
:h2.dw_handle DWENTRY DWBeginInlineSubroutine( dw_client cli, dw_handle out_of_line, dw_loc_handle ret_addr, dw_loc_handle segment );
:p.Begin a definition of a particular instance of an inlined
subroutine.  :hp2.out_of_line:ehp2. is a handle to the "out of line"
instance of the subroutine (i.e., a handle from a :hp2.DWBeginSubroutine:ehp2.
call that had the DW_FLAG_OUT_OF_LINE flag).  Each instance of the
inlined subroutine must have it's own copies of entries describing
parameters to that subroutine and it's local variables.
:hp2.ret_addr:ehp2. gives the location of the return address (if any).
:hp2.segment:ehp2. if non-null indicates which segment the expansion
occurs in.

:h2.dw_handle DWENTRY DWBeginSubroutine( dw_client cli, dw_call_type call_type, dw_handle return_type, dw_loc_handle return_addr_loc, dw_loc_handle frame_base_loc, dw_loc_handle structure_loc, dw_handle member_hdl, dw_loc_handle segment, const char *name, dw_addr_offset start_scope, uint flags );
:p.Begin a declaration/definition of a subroutine or entry point.
This begins a nesting of the debugging information, and must be
followed by calls to :hp2.DWFormalParameter:ehp2. et al to declare
the parameters, types, and variables for this subroutine.  Unless
:hp2.DW_FLAG_DECLARATION:ehp2. is set, this will require a
:hp2.DW_W_LOW_PC:ehp2. and/or a :hp2.DW_W_HIGH_PC:ehp2..
:dl.
:dthd.Parameter
:ddhd.Description
:dt.call_type
:dd.Not currently used, but should be one of:
:sl.
:li.DW_SB_NEAR_CALL
:li.DW_SB_FAR_CALL
:li.DW_SB_FAR16_CALL
:esl.
:dt.return_type
:dd.Handle for the return type.  Must be NULL for void-type subroutines.
:dt.return_addr_loc
:dd.If non-NULL then this is a location expression that calculates the
address of memory that stores the return address.
:dt.frame_base_loc
:dd.If non-NULL then this is a location expression that describes the
"frame base" for the subroutine or entry point.  (If the frame base
changes during the subroutine, it might be desirable
for local variables to be calculated from the frame base, and then
use a location list for the frame base.);
:dt.structure_loc
:dd.For member functions of structure types, this calculates the
address of the slot for the function within the virtual function table
for the enclosing class or structure.
:dt.member_hdl
:dd.If this is a definition of a member function occuring outside the body
of the structure type, then this is the handle for the type definition
of the structure.
:dt.segment
:dd.If non-null then this is a location expression that evaluates to the
segment for this subprogram.
:edl.
:p.The following additional flags are available:
:dl.
:dt.flag
:dd.description
:dt.DW_FLAG_PROTOTYPED
:dd.The function was declared with ANSI-C style prototyping, as opposed
to K&R-C style parameter lists.
:dt.DW_FLAG_ARTIFICIAL
:dd.The function was created by the compiler (i.e. not explicitly declared in
any of the user's source files)
:dt.DW_FLAG_VIRTUAL
:dd.This is a virtual subroutine.
:dt.DW_FLAG_PURE_VIRTUAL
:dd.This is a pure virtual subroutine.
:dt.DW_FLAG_MAIN
:dd.For Fortran PROGRAM-type subroutines.
:dt.DW_SUB_STATIC
:dd.A file static subroutine or function.  Also used for a static
member function, and for nested subroutine declarations.
:dt.DW_SUB_ENTRY
:dd.A FORTRAN Entry point.  DW requires only a DW_W_LOW_PC for this
type of function.
:dt.DW_FLAG_WAS_INLINED
:dd.The function was generated inline by the compiler.
:dt.DW_FLAG_DECLARED_INLINED
:dd.The function was declared inline by the user.
:edl.

:h2.void DWENTRY DWEndSubroutine( dw_client cli );
:p.End the current nesting of :hp2.DWBeginSubroutine:ehp2. or
:hp2.DWBeginInlineSubroutine:ehp2..

:h2.dw_handle DWENTRY DWFormalParameter( dw_client cli, dw_handle parm_type, dw_loc_handle parm_loc, dw_loc_handle segment, const char *name, uint default_value_type, ... );
:p.Declare a formal parameter to the current function.
:dl.
:dt.parm_type
:dd.The type of the parameter.
:dt.parm_loc
:dd.A location description that yields the address of the parameter.
May be NULL indicating unknown address.
:dt.segment
:dd.A location expression that yields the segment of the parameter.
May be NULL indicating the default segment.
:dt.default_value_type
:dd.One of the following:
:dl.
:dt.DW_DEFAULT_NONE
:dd.There is no default value for this parameter.
:dt.DW_DEFAULT_FUNCTION
:dd.The default value for this parameter is returned by a function
with no args, that is specified by a CLIReloc for DW_W_DEFAULT_FUNCTION.
:dt.DW_DEFAULT_STRING
:dd.The default value is a null-terminated string that is specified
as an extra parameter to this :hp2.DWFormalParameter:ehp2..
:dt.DW_DEFAULT_BLOCK
:dd.The default value is a constant block of data that is specified
by extra "const void *" and "dw_size_t"  parameters to
:hp2.DWFormalParameter:ehp2..
:edl.
:dt....
:dd.Extra parameters depend on the :hp2.default_value_type:ehp2..
:edl.

:h2.dw_handle DWENTRY DWEllipsis( dw_client cli );
:p.Indicate that the current subroutine has unspecified parameters.
Used for "..." in C.

:h2.dw_handle DWENTRY DWLabel( dw_client cli, dw_loc_handle segment, const char *name, dw_addr_offset start_scope );
:p.Declare a label inside a subroutine.  :hp2.start_scope:ehp2. will
usually be 0, but is here for future compatibility.  A CLIReloc for
:hp2.DW_W_LABEL:ehp2. will be made.  :hp2.segment:ehp2. if non-null
indicates which segment the label belongs to.

:h2.dw_handle DWENTRY DWVariable( dw_client cli, dw_handle type, dw_loc_handle loc, dw_handle member_of, dw_loc_handle segment, const char *name, dw_addr_offset start_scope, uint flags );
:p.Declare a variable.
:dl.
:dt.type
:dd.The type of this variable.
:dt.loc
:dd.A location expression yielding the address of this variable.
:dt.member_of
:dd.If this is the definition of a static data member then this
is the handle to the structure type.  Otherwise this is NULL.
:dt.segment
:dd.If this is non-null then it evaluates to the segment the variable
is in.
:dt.flags
:dd.If :hp2.DW_FLAG_GLOBAL:ehp2. is set then this is a global
variable.  Otherwise it is a local variable.  File static variables
in C and C++ are considered local variables.
If :hp2.DW_FLAG_ARTIFICIAL:ehp2. is set then this is a variable that has
been created by the compiler.
:edl.

:h2.dw_handle DWENTRY DWConstant( dw_client cli, dw_handle type, const void *value, dw_size_t len, dw_handle member_of, const char *name, dw_addr_offset start_scope, uint flags );
:p.Declare a named constant.
:dl.
:dt.type
:dd.The type of this constant.
:dt.value
:dd.Pointer to the value for this constant.
:dt.len
:dd.The length of this constant.  If len is 0, then value is considered
to be a null-terminated string.
:dt.member_of
:dd.If this is the definition of a constant member of a structure type,
then this is the handle to the structure type.	Otherwise it is NULL.
:edl.

:h2.void DWENTRY DWAddress( dw_client cli, uint_32 len );
:p.DWARF builds a table of all the addresses attributed to a compilation
unit.  The client calls this function to add addresses to this table.
:hp2.len:ehp2. is the length of this address range.  The base of the
address range is filled in by a CLIReloc for :hp2.DW_W_ARANGE_ADDR:ehp2..

:h2.void DWENTRY DWPubname( dw_client cli, dw_handle hdl, const char *name );
:p.These are used to speed up the debugger.  This should be called for any
name that has global scope.  :hp2.hdl:ehp2. is the handle for the debugging
entry that declares/defines the :hp2.name:ehp2..

:H0.Required Client Routines
:p.The debugging information has several sections indicated by the
following enumerated type:
:DL tsize=22.
:DTHD.Constant
:DDHD.Description
:DT.DW_DEBUG_INFO
:DD.This section is called :SF font=4..debug_info:eSF., which stores
all the debugging information entries.
:DT.DW_DEBUG_PUBNAMES
:DD.This section is called :SF font=4..debug_pubnames:eSF., which
stores a table consisting of object name information that is used in
lookup by Name.
:DT.DW_DEBUG_ARANGES
:DD.This section is called :SF font=4..debug_aranges:eSF., which stores
a table consisting of object address information that is used in lookup
by Address.
:DT.DW_DEBUG_LINE
:DD.This section is called :SF font=4..debug_line:eSF., which stores
the line number information generated for the compilation units.
:DT.DW_DEBUG_LOC
:DD.This section is called :SF font=4..debug_loc:eSF., which stores the
location lists information.
:DT.DW_DEBUG_ABBREV
:DD.This section is called :SF font=4..debug_abbrev:eSF., which stores
abbreviation declarations.
:DT.DW_DEBUG_MACINFO
:DD.This section is called :SF font=4..debug_macinfo:eSF., which stores
macro information.
:DT.DW_DEBUG_REF
:DD.This section is called :SF font=4..WATCOM_references:eSF., which
contains information about the symbols of every instructions in the
source files.
:DT.DW_DEBUG_MAX
:DD.Defined for convenience; it is the number of sections.
:eDL.

:H1.Performance Considerations
:p.The DW library does it's best to try and group CLIWrite operations
together into one larger CLIWrite, and to try and avoid using CLISeek.
But the library does not go out of it's way to provide this massaging
of output.  The client should attempt to buffer the data itself.  CLISeek
is most often called on the DW_DEBUG_INFO, and the
DW_DEBUG_LOC sections.	The other sections may have one CLISeek performed
at the DWFini stage, and the seek will be to the zero offset.  The
client might wish to optimize performance for only the DW_DEBUG_INFO
and the DW_DEBUG_LOC sections.

:H2.void CLISeek( uint section, long offset, uint mode );
:P.Repositions the pointer in :hp2.section:ehp2. so that subsequent
output occurs at the new pointer.
:DL.
:dthd.Mode
:ddhd.Description
:dt.DW_SEEK_SET
:dd.The position is set to the absolute location :hp2.offset:ehp2..
:dt.DW_SEEK_CUR
:dd.:hp2.offset:ehp2. is added to the current position.
:dt.DW_SEEK_END
:dd.The position is set to :hp2.offset:ehp2. bytes from the current end
of :hp2.section:ehp2.
:edl.

:h2.long CLITell( uint section );
:P.Return the offset of the next byte to be written to the section.

:h2.void CLIReloc( uint section, uint reloc_type, ... );
:p.Even when writing BROWSER information, relocations such as DW_W_LOC_PC
may be asked for.  This is because the DWARF format requires the presence
of certain fields to indicate something specific about a record.  For
example, if a subroutine record doesn't have a low pc then it is assumed
to be a declaration of the subroutine rather than a definition.
:dl.
:dt.section
:dd.The section to write a relocation entry to.
:dt.reloc_type
:dd.The type of the relocation, as follows:
:DL.
:dt.DW_W_LOW_PC
:dd.Emit a dw_targ_addr.
Used by various entry points to get the low pc address of an object.
:dt.DW_W_HIGH_PC
:dd.Emit a dw_targ_addr.
Used by various entry points to get the high pc address of an object.
:dt.DW_W_STATIC
:dd.Emit a dw_targ_addr.
This relocation has an extra parameter of type dw_sym_handle.
This parameter is the target of the relocation; the offset
of the symbol should be generated.
This is used any time a location expression
involving a :hp2.DWLocStatic:ehp2. is generated.
:dt.DW_W_SEGMENT
:dd.Emit a dw_segment.
This relocation has an extra parameter of type dw_sym_handle.
It indicates that the segment portion of the address of the symbol
should be generated.
This is used any time a location expression
involving a :hp2.DWLocSegment:ehp2. operation is generated.
:dt.DW_W_LABEL
:dd.Emit a dw_targ_addr.
Used by :hp2.DWLabel:ehp2..
:dt.DW_W_SECTION_POS
:dd.Emit a uint_32.
This relocation has an extra parameter of type uint called
:hp2.targ_sect:ehp2..  :hp2.targ_sect:ehp2.
parameter is the number of a section for which the current
offset is the target of the relocation.  The relocation
is emitted into :hp2.section:ehp2..
:dt.DW_W_DEFAULT_FUNCTION
:dd.Emit a dw_targ_addr.
Used by :hp2.DWFormalParameter:ehp2..
:dt.DW_W_ARANGE_ADDR
:dd.Emit a dw_targ_addr.
Used by :hp2.DWAddress:ehp2..
:dt.DW_W_UNIT_SIZE
:dd.Emit an uint_32 that is the number of bytes of code in the current
compilation unit.
:DT.DW_W_MAX
:dd.Defined for convenience.  This enumerated type starts at 0 and
goes to DW_W_MAX.
:eDL.
:edl.

:H2.void CLIWrite( uint section, const void *block, size_t len );
:P.Writes out the debugging information.
:DL.
:DTHD.Parameter
:DDHD.Description
:DT.section
:DD.The section to which the debugging information is written.
:DT.block
:DD.Points to the debugging information block.
:DT.len
:DD.Length of the debugging information block.
:eDL.

:H2.void *CLIAlloc( size_t size );
:P.Allocates a memory block of size :HP2.size:eHP2. for the library and
returns its address.
This function cannot return NULL.

:H2.void CLIFree( void *blk );
:P.Free the block pointed by :HP2.blk:eHP2..

:H0.Examples
:p.:hp2.This section needs a major rewrite.:ehp2.
:P.The example below shows what functions should be called in order to
store the debugging information for this C program.
:P.N.B.
In this example, for all the CLIWrite() calls, only the section id is
accurate.
Also for all DWLineNum() calls, the advances in machine instruction
address are inaccurate.
:xmp.
test.c:
:sf font=4.

1 #include <stdlib.h>

2 int	 a;

3 typedef near char NCHAR;

4 void main();
5 {
6    NCHAR    b;

7    b := 5;
8 }

:esf.
Functions called by the client and the DWARF library:

Client:
:sf font=4.

	cli_id = DWInit( DW_LANG_C89, DW_CM_DEBUGGER, "test.c",
			 "c:\mydir", 0x123, 1, CLILoc, CLIType,
			 CLIName, CLIWrite, CLIAlloc, CLIFree );
:esf.
:exmp.

DWARF Library:

:xmp.
:sf font=4.
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
:esf.
:exmp.

Client:

:xmp.
:sf font=4.
#include <stdlib.h>

	DWLineNum( cli_id, DW_LN_STMT|DW_LN_BLK, 1, 1, 0 );
	DWIncl( id, "stdlib.h" );
	...Function calls for "stdlib.h"...
	DWInclFini( cli_id );

:esf.
:exmp.
DWARF Library:
:xmp.
:sf font=4.

	CLIWrite( DW_DEBUG_LINE, 0, &info, 28, block );
	CLIWrite( DW_DEBUG_INFO, 30, &info, 12, block );

:esf.
:exmp.
Client:

:xmp.
:sf font=4.
int   a;

	DWLineNum( cli_id, DW_LN_STMT, 1, 1, 4 );
	a_dw_handle = DWModSym( cli_id, a_cg_handle, DW_SM_VAR,
			DW_SM_GLO|DW_SM_FILE, DW_SM_NULL );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	name = CLIName( a_cg_handle );
		/* It returns the string "a". */
	type = CLIType( a_cg_handle );
		/* It returns DW_FT_INTEGER. */
	loc = CLILoc( a_cg_handle );
	CLIWrite( DW_DEBUG_LINE, 0, &info, 28, block );
	CLIWrite( DW_DEBUG_INFO, 0, &info, 24, block );
	CLIWrite( DW_DEBUG_PUBNAMES, 0, &info, 12, block );

:esf.
:exmp.
Inside CLILoc():

:xmp.
:sf font=4.
	loc_id = DWLocInt();
	DWLocAtom( cli_id, a_cg_handle, DW_LOC_STATIC );
	/* The actual address will be filled in by the client when
	   the debugging information is written to the object file.*/
	a_loc_hd = DWLocFini( loc_id );
	return a_loc_hd;

:esf.
:exmp.
Client:

:xmp.
:sf font=4.
typedef near char NCHAR;

	DWLineNum( cli_id, DW_LN_STMT, 1, 1, 14 );
	mod_handle = DWMod( cli_id, DW_FT_CHAR, DW_MOD_NEAR );
	nchar_handle = DWModSym( cli_id, nchar_cg_handle,
		DW_SM_TYPEDEF, DW_SM_NULL, DW_SM_NULL );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	name = CLIName( nchar_cg_handle );
		/* It returns the string "NCHAR". */
	type = CLIType( nchar_cg_handle );
		/* It returns mod_handle. */
	CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );
	CLIWrite( DW_DEBUG_INFO, 0, &info, 24, block );

:esf.
:exmp.
Client:
:xmp.
:sf font=4.

void main();

	DWLineNum( cli_id, DW_LN_DEFAULT, 1, 1, 23 );
	pro_handle = DWBegProc( cli_id, DW_SB_NEAR_CALL, DW_FT_VOID,
			ret_loc_hd, DW_LOC_NULL,
			DW_SB_GLOBAL_SUB|DW_SB_FUNC_PROTOTYPE );

:esf.
:exmp.
In order to get ret_loc_ad:
:xmp.
:sf font=4.

	loc_id = DWLocInit();
	DWLocAtom( cli_id, some_cg_handle, DW_LOC_STATIC );
		/* Assume that the return address of main() is stored
		   in a symbol with some_cg_handle as its handle.
		   The actual address will be filled in by the
		   client when the debugging information is written
		   to the object file.				     */
	ret_loc_ad = DWLocFini( cli_id );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );

:esf.
:exmp.
Client:

:xmp.
:sf font=4.
{
	DWLineNum( cli_id, DW_LN_BLK, 1, 1, 0 );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );

:esf.
:exmp.
Client:

:xmp.
:sf font=4.
NCHAR b;

	DWLineNum( cli_id, DW_LN_STMT, 1, 1, 10 );
	b_handle = DWModSym( cli_id, b_cg_handle, DW_SM_VAR,
			DW_SM_NULL, DW_SM_LOC|DW_SM_ROUT );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	loc = CLILoc( b_cg_handle );
	name = CLIName( b_cg_handle );
		/* It returns the string "b". */
	type = CLIType( b_cg_handle );
		/* It returns nchar_handle. */
	CLIWrite( DW_DEBUG_LINE, 0, &info, 20, block );

:esf.
:exmp.
Inside CLILoc():

:xmp.
:sf font=4.
	loc_id = DWLocInt();
	DWLocAtom( cli_id, b_cg_handle, DW_LOC_STACK );
	/* The offset from stack frame base will be filled in by
	   the client when the debugging information is written
	   to the object file.	 */
	b_loc_hd = DWLocFini( loc_id );
	return b_loc_hd;

:esf.
:exmp.
Client:
:xmp.
:sf font=4.

b := 5;
	DWLineNum( cli_id, DW_LN_STMT, 1, 4, 14 );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );

:esf.
:exmp.
Client:
:xmp.
:sf font=4.

}
	DWLineNum( cli_id, DW_LN_DEFAULT, 1, 1, 4 );
	DWEndProc( cli_id, pro_handle );
	main_handle = DWModSym( cli_id, main_cg_handle, DW_SM_SUB,
			DW_SM_NULL, DW_SM_NULL );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	name = CLIName( main_cg_handle );
		/* It returns the string "main" */
	type = CLIType( main_cg_handle );
		/* It returns pro_handle */
	CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );
	CLIWrite( DW_DEBUG_INFO, -50, &info, 86, block );
	CLIWrite( DW_DEBUG_REF, 0, &info, 12, block );
	CLIWrite( DW_DEBUG_PUBNAMES, 0, &info, 12, block );
		/* For the global object "main" */

:esf.
:exmp.
Client:
:xmp.
:sf font=4.

	DWFini( cli_id );

:esf.
:exmp.
DWARF Library:

:xmp.
:sf font=4.
	CLIWrite( DW_DEBUG_LINE, 0, &info, 24, block );
	CLIWrite( DW_DEBUG_INFO, -120, &info, 54, block );
:esf.
:exmp.

:H0.Revision History
:DL.
:DTHD.Draft
:DDHD.Description
:DT.Draft 5
:DD.Changed the arguments to a number of the function calls for use with
draft 5 of dwarf.
:DT.Draft 6
:DD.Changed the arguments to a number of the function calls for use with
draft 6 of dwarf.
:edl.

:BACKM.
:eGDOC.
