:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
.el .do begin
:INCLUDE file='WHELP'.
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:INCLUDE file='DEFS'.
.*
.if &e'&dohelp eq 0 .do begin
.*
.* Layout changes specific to this document
.*
.* Switch off numbering for heading
.* Set two-level contents
.* Set the same page footer text for all pages
.*
:LAYOUT.
:H1
        number_form = none
:H2
        number_form = none
        font = 8
:H3
        number_form = none
:TOC
        toc_levels=2
:BANNER
        docsect=head1
        place=botodd
:BANREGION
        refnum=1
        contents="&amp.headtxt1$."
:eBANREGION
:eBANNER
:BANNER
        docsect=body
        place=boteven
:BANREGION
        refnum=2
        contents="&amp.headtxt1$."
:eBANREGION
:eBANNER
:BANNER
        docsect=body
        place=botodd
:BANREGION
        refnum=1
        contents="&amp.headtxt1$."
:eBANREGION
:eBANNER
:eLAYOUT.
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE.&company Code Generator Interface
:CMT. :DATE.
:eTITLEP.
:TOC.
.do end
.*
:BODY.
.*
:CMT. index is disabled
.if &e'&dohelp eq 2 .do begin
:exhelp
:include file='&book..idx'
:include file='&book..tbl'
:include file='&book..kw'
.do end
.*
.chap Introduction
.*
.np
The code generator (back end) interface is a set of procedure calls.
These are divided into following category of routines.
:ZSL.
:ZLI.Code Generation (CG)
:ZLI.Data Generation (DG)
:ZLI.Miscellaneous Back End (BE)
:ZLI.Front end supplied (FE)
:ZLI.Debugger information (DB)
:ZeSL.
.*
.chap General
.*
.section cg_init_info BEInit( cg_switches switches, cg_target_switches targ_switches, uint optsize, proc_revision proc )
.*
.ix BEInit
.np
Initialize the code generator.
This must be the first routine to be called.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note switches
Select code generation options.
The options are bits, so may be combined with the bit-wise operator |.
Options apply to the entire compilation unit.
The bit values are defined below.
.note targ_switches
Target specific switches.
The bit values are defined below.
.note optsize
A number between 0 and 100.
0 means optimize for speed, 100 means optimize for size.
Anything in between selects a compromise between speed and size.
.note proc
The target hardware configuration, defined below.
.endnote
.begnote
.note Returns
Information about the code generator revision in a cg_init_info
structure, defined below.
.endnote
.begnote $break $setptnt 2i
.ix options
.notehd1 Switch
.notehd2 Definition
.note NO_OPTIMIZATION
Turn off optimizations.
.note DBG_NUMBERS
Generate line number debugging information.
.note FORTRAN_ALIASING
Assume pointers are only used for parameter passing.
.note DBG_DF
Generate debugging information in DWARF format.
.note DBG_CV
Generate debugging information in CodeView format. If neither
DBG_DF nor DBG_CV is set, debugging information (if any) is
generated in the Watcom format.
.note RELAX_ALIAS
Assume that a static/extern variable and a pointer to that same
variable are not used within the same routine.
.note DBG_LOCALS
Generate local symbol information for use by a debugger.
.note DBG_TYPES
Generate typing information for use by a debugger.
.note LOOP_UNROLLING
Turn on loop unrolling.
.note LOOP_OPTIMIZATION
Turn on loop optimizations.
.note INS_SCHEDULING
Turn on instruction scheduling.
.note MEMORY_LOW_FAILS
Allow the code generator to run out of memory without being able to
generate object code (allows the 386 compiler to use EBP as a cache
register).
.note FP_UNSTABLE_OPTIMIZATION
Allow the code generator to perform optimizations that are
mathematically correct, but are numerically unstable.
E.g.
converting division by a constant to a multiplication by the reciprocal.
.note NULL_DEREF_OK
NULL points to valid memory and may be dereferenced.
.note FPU_ROUNDING_INLINE
Inline floating-point value rounding (actually truncation) routine
when converting floating-point values to integers.
.note FPU_ROUNDING_OMIT
Omit floating-point value rounding entirely and use FPU default. Results
will not be ISO C compliant.
.note ECHO_API_CALLS
Log each call to the code generator with its arguments and return value.
Only available in debug builds.
.note OBJ_ELF
Emit ELF object files.
.note OBJ_COFF
Emit COFF object files. For Intel compilers, OMF object files will
be emitted in the absence of either switch.
.note OBJ_ENDIAN_BIG
Emit big-endian object files (COFF or ELF). If OBJ_ENDIAN_BIG is not
set, little-endian objects will be generated.
.endnote
.begnote $break $setptnt 2i
.notehd1 x86 Switch
.notehd2 Definition
.note I_MATH_INLINE
Do not check arguments for operators like O_SQRT.
This allows the compiler to use some specialty x87 instructions.
.note EZ_OMF
Generate Phar Lap EZ-OMF object files.
.note BIG_DATA
Use segmented pointers (16:16 or 16:32).
This defines TY_POINTER to be equivalent to TY_HUGE_POINTER.
.note BIG_CODE
Use inter segment (far) call and return instructions.
.note CHEAP_POINTER
Assume far objects are addressable by one segment value.
This must be used in conjunction with BIG_DATA.
It defines TY_POINTER to be equivalent to TY_FAR_POINTER.
.note FLAT_MODEL
Assume all segment registers address the same base memory.
.note FLOATING_FS
Does FS float (or is it pegged to DGROUP).
.note FLOATING_GS
Does GS float (or is it pegged to DGROUP).
.note FLOATING_ES
Does ES float (or is it pegged to DGROUP).
.note FLOATING_SS
Does SS float (or is it pegged to DGROUP).
.note FLOATING_DS
Does DS float (or is it pegged to DGROUP).
.note USE_32
Generate code into a use32 segment (versus use16).
.note INDEXED_GLOBALS
Generate all global and static variable references as an offset
past EBX.
.note WINDOWS
Generate windows prolog/epilog sequences for all routines.
.note CHEAP_WINDOWS
Generate windows prolog/epilog sequences assuming that call backs
functions are defined as __export.
.note NO_CALL_RET_TRANSFORM
Do not change a CALL followed by a RET into a JMP.
This is used for some older overlay managers that cannot handle a JMP
to an overlay.
.note CONST_IN_CODE
Generate all constant data into the code segment.
This only applies to the internal code generator data, such as floating
point constants.
The front end decides where its data goes using BESetSeg().
.note NEED_STACK_FRAME
Generate a traceable stack frame.
The first instructions will be :HP2.INC BP:eHP2. if the routine uses a
far return instruction, followed by :HP2.PUSH BP:eHP2. and :HP2.MOV
BP,SP:eHP2..
(ESP and EBP for 386 targets).
.note LOAD_DS_DIRECTLY
Generate code to load DS directly. By default, a call to __GETDS routine
is generated.
.note GEN_FWAIT_386
Generate FWAIT instructions on 386 and later CPUs. The 386 never needs
FWAIT for data synchronization, but FWAIT may still be needed for accurate
exception reporting.
.endnote
.begnote $setptnt 2i
.notehd1 RISC Switch
.notehd2 Definition
.note ASM_OUTPUT
Print final pseudo-assembly on the console. Debug builds only.
.note OWL_LOGGING
Log calls to the Object Writer Library
.note STACK_INIT
Pre-initialize stack variables to a known bit pattern.
.note EXCEPT_FILTER_USED
Set when SEH (Structured Exception Handling) is used.
.endnote
:CMT. S/370 not maintained
.if 0 eq 1 .do begin
.begnote $setptnt 2i
.notehd1 370 Switch
.notehd2 Definition
.note BIG_DATA
???
.note BIG_CODE
???
.note FLOATING_SS
???
.note ASM_OUTPUT
???
.note CHEAP_POINTER
???
.note AMODE_24
???
.note AMODE_31
???
.note RMODE_24
???
.note CODE_RENT
???
.note CODE_SPLIT
???
.note STACK_CHECK
???
.note ASM_SOURCE
???
.note ASM_LISTING
???
.note NO_ZERO_INIT
???
.note I_MATH_INLINE
???
.endnote
.do end
.ix options
.np
The supported proc_revision CPU values are:
:ZSL.
:ZLI.CPU_86
:ZLI.CPU_186
:ZLI.CPU_286
:ZLI.CPU_386
:ZLI.CPU_486
:ZLI.CPU_586
:ZeSL.
.np
The supported proc_revision FPU values are:
:ZSL.
:ZLI.FPU_NONE
:ZLI.FPU_87
:ZLI.FPU_387
:ZLI.FPU_586
:ZLI.FPU_EMU
:ZLI.FPU_E87
:ZLI.FPU_E387
:ZLI.FPU_E586
:ZeSL.
.np
The supported proc_revision WEITEK values are:
:ZSL.
:ZLI.WTK_NONE
:ZLI.WTK_1167
:ZLI.WTK_3167
:ZLI.WTK_4167
:ZeSL.
.np
The following example sets the processor revision information to
indicate a 386 with 387 and Weitek 3167.
.np
:XMP.
proc_revision proc;

SET_CPU( proc, CPU_386 );
SET_FPU( proc, FPU_387 );
SET_WTK( proc, WTK_3167 );
:eXMP.
.pc
The return value structure is defined as follows:
.np
:XMP.
typedef union   cg_init_info {
  struct {
    unsigned revision   : 10; /* contains II_REVISION */
    unsigned target     : 5;  /* has II_TARG_??? */
    unsigned is_large   : 1;  /* 1 if 16 bit host */
  } version;
  int     success;
} cg_init_info;

enum {
  II_TARG_8086,
  II_TARG_80386,
  II_TARG_STUB,
  II_TARG_CHECK,
  II_TARG_370,
  II_TARG_AXP,
  II_TARG_PPC,
  II_TARG_MIPS
};
:eXMP.
.*
.section void BEStart()
.*
.ix BEStart
.np
Start the code generator.
Must be called immediately after all calls to BEDefSeg have been made.
This restriction is relaxed somewhat for the 80(x)86 code generator.
See BEDefSeg for details.
.*
.section void BEStop()
.*
.ix BEStop
.np
Normal termination of code generator.
This must be the second last routine called.
.*
.section void BEAbort()
.*
.ix BEAbort
.np
Abnormal termination of code generator.
This must be the second last routine called.
.*
.section void BEFini()
.*
.ix BEFini
.np
Finalize the code generator.
This must be the last routine called.
.*
.section patch_handle BEPatch()
.*
.np
Allocate a patch handle which can be used to create a patchable
integer (an integer which will have a constant value provided sometime
while the codegen is handling the CGDone call).
See CGPatchNode.
.*
.section void BEPatchInteger( patch_handle hdl, signed_32 value )
.*
.np
Patch the integer corresponding to the given handle to have the
given value.
This may be called repeatedly with different values, providing
CGPatchNode has been called and BEFiniPatch has not been called.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note hdl
A patch_handle returned from an earlier invocation of BEPatch which
has had a node allocated for it via CGPatchNode.
If CGPatchNode has not been called with the handle given, the behaviour
is undefined.
.note value
A signed 32-bit integer value.
This will be the new value of the node which has been associated with
the patch handle.
.endnote
.*
.section cg_name BEFiniPatch( patch_handle hdl )
.*
.np
This must be called to free up resources used by the given handle.
After this, the handle must not be used again.
.*
.chap Segments
.*
.np
The object file produced by the code generator is composed of
various segments.
These are defined by the front end.
A program may have as many data and code segments as required by the
front end.
Each segment may be regarded as an individual file of objects, and may
be created simultaneously.
There is a current segment, selected by BESetSeg(), into which all DG
routines generate their data.
The code for each routine is generated into the segment returned by the
FESegID() call when it is passed the cg_sym_handle for the routine.
It is illegal to write data to the code segment for a routine in
between the CGProcDecl call and the CGReturn call.
.np
The following routines are used for initializing, finalizing,
defining and selecting segments.
.*
.section void BEDefSeg( segment_id segid, seg_attr attr, char *str, uint algn )
.*
.ix BEDefSeg
.ix segments
.np
Define a segment.
This must be called after BEInit and before BEStart.
For the 80(x)86 code generator, you are allowed to define additional
segments after BEStart if they are:
:OL compact.
:ZLI.Code Segments
:ZLI.PRIVATE data segments.
:eOL.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note segid
A non-negative integer used as an identifier for the segment.
It is arbitrarily picked by the front end.
.note attr
Segment attribute bits, defined below.
.note str
The name given to the segment.
.note algn
The segment alignment requirements.
The code generator will pick the next larger alignment allowed by the
object module format.
For example, 9 would select paragraph alignment.
.endnote
.begnote
.notehd1 Attribute
.notehd2 Definition
.note EXEC
This is a code segment.
.note GLOBAL
The segment is accessible to other modules.
(versus PRIVATE).
.note INIT
The segment is statically initialized.
.note ROM
The segment is read only.
.note BACK
The code generator may put its data here.
One segment must be marked with this attribute.
It may not be a COMMON, PRIVATE or EXEC segment.
If the front end requires code in the EXEC segment, the CONST_IN_CODE
switch must be passed to BEInit().
.note COMMON
All occurrences of this segment will be overlayed.
This is used for FORTRAN common blocks.
.note PRIVATE
The segment is non combinable.
This is used for far data items.
.note GIVEN_NAME
Normally, the back end feels free to prepend or append strings to
the segment name passed in by the front end.
This allows a naive front end to specify a constant set of segment
names, and have the code generator mangle them in such a manner that
they work properly in concert with the set of cg_switches that have
been specified (e.g.
prepending the module name to the code segments when BIG_CODE is
specified on the x86).
When GIVEN_NAME is specified, the back end outputs the segment name to
the object file exactly as given.
.note THREAD_LOCAL
Segment contains thread local data. Such segments may need special
handling in executable modules.
.endnote
.*
.section segment_id BESetSeg( segment_id segid )
.*
.ix BESetSeg
.ix segments
.np
Select the current segment for data generation routines.
Code for a routine is always output into the segment returned by
FESegID when it is passed the routine symbol handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note segid
Selects the current segment.
.endnote
.begnote
.note Returns
The previous current segment.
.endnote
:NOTE.
When emitting data into an EXEC or BACK segment, be aware that
the code generator is at liberty to emit code and/or back end data into
that segment anytime you make a call to a code generation routine
(CG*).
Do NOT expect data items to be contiguous in the segment if you have
made an intervening CG* call.
.*
.section segment_id BEGetSeg( void )
.*
.ix BEGetSeg
.ix segments
.np
Return the current segment for generation routines.
.begnote
.note Returns
The current segment.
.endnote
.*
.section void BEFlushSeg( segment_id segid )
.*
.ix BEFlushSeg
.ix segments
.np
BEFlushSeg informs the back end that no more code/data will be
generated in the specified segment.
For code segments, it must be called after the CGReturn() for the final
function which is placed in the segment.
This causes the code generator to flush all pending information
associated with the segment and allows the front end to free all the
back handles for symbols which were referenced by the code going into
the segment.
(The FORTRAN compiler uses this since each function has its own symbol
table which is thrown out at the end of the function).
.begnote
.notehd1 Parameter
.notehd2 Definition
.note segid
The code segment id.
.endnote
.*
.chap Labels
.*
.ix label, code
.np
The back end uses a :HP2.label_handle:eHP2. for flow of control.
Each :HP2.label_handle:eHP2. is a unique code label.
These labels may only be used for flow of control.
In order to define a label in a data segment, a :HP2.back_handle:eHP2.
must be used.
.*
.section label_handle BENewLabel()
.*
.ix BENewLabel
.ix label, code
.np
Allocate a new control flow label.
.begnote
.note Returns
A new label_handle.
.endnote
.*
.section void BEFiniLabel( label_handle lbl )
.*
.ix BEFiniLabel
.ix label, code
.np
Indicate that a label_handle will not be used by the front end
anymore.
This allows the back end to free some memory at some later stage.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note lbl
A label_handle
.endnote
.*
.chap *refid=bck Back Handles
.*
.ix 'back handle'
.ix label, data
.np
A :HP2.back_handle:eHP2. is the front end's handle for a code
generator symbol table entry.
A :HP2.cg_sym_handle:eHP2. is the code generator's handle for a front
end symbol table entry.
The back end may call FEBack, passing in any cg_sym_handle that has
been passed to it.
The front end must allocate a back_handle via BENewBack if one does not
exist.
Subsequent calls to FEBack should return the same back_handle.
This mechanism is used so that the back end does not have to do symbol
table searches.
For example:
.np
:XMP.
back_handle FEBack( SYMPOINTER sym )
{
    if( sym->back == NULL ) {
        sym->back = BENewBack( sym );
    }
    return( sym->back );
}
:eXMP.
.pc
It is the responsibility of the front end to free each back_handle,
via BEFreeBack, when it frees the corresponding cg_sym_handle entry.
.np
A back_handle for a symbol having automatic or register storage
duration (auto back_handle) may not be freed until CGReturn is called.
A back_handle for a symbol having static storage duration, (static
back_handle) may not be freed until BEStop is called or until after a
BEFlushSeg is done for a segment and the back_handle will never be
referenced by any other function.
.np
The code generator will not require a back handle for symbols which
are not defined in the current compilation unit.
.np
The front end must define the location of all symbols with static
storage duration by passing the appropriate back_handle to DGLabel.
It must also reserve the correct amount of space for that variable
using DGBytes or DGUBytes.
.np
The front end may also allocate an back_handle with static storage
duration that has no cg_sym_handle associated with it (anonymous
back_handle) by calling BENewBack(NULL).
These are useful for literal strings.
These must also be freed after calling BEStop.
.*
.section back_handle BENewBack( cg_sym_handle sym )
.*
.ix BENewBack
.ix 'back handle'
.np
Allocate a new back_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
The front end symbol handle to be associated with the back_handle.
It may be NULL.
.endnote
.begnote
.note Returns
A new back_handle.
.endnote
.*
.section void BEFiniBack( back_handle bck )
.*
.ix BEFiniBack
.ix 'back handle'
.np
Indicate that :HP2.bck:eHP2. will never be passed to the back end
again, except to BEFreeBack.
This allows the code generator to free some memory at some later stage.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note bck
A back_handle.
.endnote
.*
.section void BEFreeBack( back_handle bck )
.*
.ix BEFreeBack
.ix 'back handle'
.np
Free the back_handle :HP2.bck:eHP2..
See the preamble in this section for restrictions on freeing a
back_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note bck
A back_handle.
.endnote
.*
.chap Type definitions
.*
.ix typing
.np
Base types are defined as constants.
All other types (structures, arrays, unions, etc) are simply defined by
their length.
The base types are:
.begnote $setptnt 1.5i
.notehd1 Type
.notehd2 C type
.note TY_UINT_1
unsigned char
.note TY_INT_1
signed char
.note TY_UINT_2
unsigned short
.note TY_INT_2
signed short
.note TY_UINT_4
unsigned long
.note TY_INT_4
signed long
.note TY_UINT_8
unsigned long long
.note TY_INT_8
signed long long
.note TY_LONG_POINTER
far *
.note TY_HUGE_POINTER
huge *
.note TY_NEAR_POINTER
near *
.note TY_LONG_CODE_PTR
(far *)()
.note TY_NEAR_CODE_PTR
(near *)()
.note TY_SINGLE
float
.note TY_DOUBLE
double
.note TY_LONG_DOUBLE
long double
.note TY_INTEGER
int
.note TY_UNSIGNED
unsigned int
.note TY_POINTER
*
.note TY_CODE_PTR
(*)()
.note TY_BOOLEAN
The result of a comparison or flow operator.
May also be used as an integer.
.note TY_DEFAULT
Used to indicate default conversion
.note TY_NEAR_INTEGER
The result of subtracting 2 near pointers
.note TY_LONG_INTEGER
The result of subtracting 2 far pointers
.note TY_HUGE_INTEGER
The result of subtracting 2 huge pointers
.endnote
There are two special constants.
.begnote
.note TY_FIRST_FREE
The first user definable type
.note TY_LAST_FREE
The last user definable type.
.endnote
.*
.section void BEDefType( cg_type what, uint align, unsigned_32 len )
.*
.ix BEDefType
.ix typing
.np
Define a new type to the code generator.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note what
An integral value greater than or equal to TY_FIRST_FREE and less
then or equal to TY_LAST_FREE, used as the type identifier.
.note align
Currently ignored.
.note len
The length of the new type.
.endnote
.*
.section void BEAliasType( cg_type what, cg_type to )
.*
.ix BEAliasType
.ix typing
.np
Define a type to be an alias for an existing type.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note what
Will become an alias for an existing type.
.note to
An existing type.
.endnote
.*
.section unsigned_32 BETypeLength( cg_type type )
.*
.ix BETypeLength
.ix typing
.np
Return the length of a previously defined type, or a base type.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note type
A previously defined type.
.endnote
.begnote
.note Returns
The length associated with the type.
.endnote
.*
.section uint BETypeAlign( cg_type type )
.*
.ix BETypeAlign
.ix typing
.np
Return the alignment requirements of a type.
This is always 1 for x86 and 370 machines.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note type
A previously defined type.
.endnote
.begnote
.note Returns
The alignment requirements of :HP2.type:eHP2. as declared in
BEDefType, or for a base type, as defined by the machine architecture.
.endnote
.*
.chap Procedure Declarations
.*
.section void CGProcDecl( cg_sym_handle name, cg_type type )
.*
.ix CGProcDecl
.ix procedures
.ix routines
.ix functions
.np
Declare a new procedure.
This must be the first routine to be called when generating each
procedure.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The front end symbol table entry for the procedure.
A back_handle will be requested.
.note type
The return type of the procedure.
Use TY_INTEGER for void functions.
.endnote
.*
.section void CGParmDecl( cg_sym_handle name, cg_type type )
.*
.ix CGParmDecl
.ix procedures
.ix routines
.ix functions
.np
Declare a new parameter to the current function.
The calls to this function define the order of the parameters.
This function must be called immediately after calling CGProcDecl.
Parameters are defined in left to right order, as defined by the
procedure prototype.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The symbol table entry for the parameter.
.note type
The type of the parameter.
.endnote
.*
.section label_handle CGLastParm()
.*
.ix CGLastParm
.np
End a parameter declaration section.
This function must be called after the last parameter has been
declared.
Prior to this function, the only calls the front-end is allowed to make
are CGParmDecl and CGAutoDecl.
.*
.section void CGAutoDecl( cg_sym_handle name, cg_type type )
.*
.ix CGAutoDecl
.np
Declare an automatic variable.
.np
This routine may be called at any point in the generation of a
function between the calls to CGProcDecl and CGReturn, but must be
called before :HP2.name:eHP2. is passed to CGFEName.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The symbol table entry for the variable.
.note type
The type of the variable.
.endnote
.*
.section temp_handle CGTemp( cg_type type )
.*
.ix CGTemp
.ix Variables
.ix Temporaries
.np
Yields a temporary with procedure scope.
This can be used for things such as iteration counts for FORTRAN do
loops, or a variable in which to store the return value of a function.
This routine should be used :HP2.only if necessary:eHP2..
It should be used when the front end requires a temporary which
persists across a flow of control boundary.
Other temporary results are handled by the expression trees.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note type
The type of the new temporary.
.endnote
.begnote
.note Returns
A temp_handle which may be passed to CGTempName.
This will be freed and invalidated by the back end when CGReturn is
called.
.endnote
.*
.chap Expressions
.*
.ix expressions
.np
Expression processing involves building an expression tree in the
back end, using calls to CG routines.
There are routines to generate leaf nodes, binary and unary nodes, and
others.
These routines return a handle for a node in a back end tree structure,
called a :HP2.cg_name:eHP2..
This handle must be exactly once in a subsequent call to a CG routine.
A tree may be built in any order, but a cg_name is invalidated by a
call to any CG routine with return type void.
The exception to this rule is CGTrash.
.np
There is no equivalent of the C address of operator.
All leaf nodes generated for symbols, via CGFEName, CGBackName and
CGTempName, yield the address of that symbol, and it is the
responsibility of the front end to use an indirection operator to get
its value.
The following operators are available:
.ix operators
.begnote $setptnt 1.5i
.notehd1 0-ary Operator
.notehd2 C equivalent
.note O_NOP
N/A
.endnote
.begnote $setptnt 1.5i
.notehd1 Unary Operator
.notehd2 C equivalent
.note O_UMINUS
-x
.note O_COMPLEMENT
~x
.note O_POINTS
(*x)
.note O_CONVERT
x=y
.note O_ROUND
Do not use!
.note O_LOG
log(x)
.note O_COS
cos(x)
.note O_SIN
sin(x)
.note O_TAN
tan(x)
.note O_SQRT
sqrt(x)
.note O_FABS
fabs(x)
.note O_ACOS
acos(x)
.note O_ASIN
asin(x)
.note O_ATAN
atan(x)
.note O_COSH
cosh(x)
.note O_SINH
sinh(x)
.note O_TANH
tanh(x)
.note O_EXP
exp(x)
.note O_LOG10
log10(x)
.note O_PARENTHESIS
This operator represents the "strong" parenthesises of FORTRAN and
C.
It prevents the back end from performing certain mathematically
correct, but floating point incorrect optimizations.
E.g.
in the expression "(a*2.4)/2.0", the back end is not allowed constant
fold the expression into "a*1.2".
.endnote
.begnote $setptnt 1.5i
.notehd1 Binary Operator
.notehd2 C equivalent
.note O_PLUS
+
.note O_MINUS
-
.note O_TIMES
*
.note O_DIV
/
.note O_MOD
%
.note O_AND
&
.note O_OR
|
.note O_XOR
^
.note O_RSHIFT
>>
.note O_LSHIFT
<<
.note O_COMMA
,
.note O_TEST_TRUE
( x & y ) != 0
.note O_TEST_FALSE
( x & y ) == 0
.note O_EQ
==
.note O_NE
!=
.note O_GT
>
.note O_LE
<=
.note O_LT
<
.note O_GE
>=
.note O_POW
pow( x, y )
.note O_ATAN2
atan2( x, y )
.note O_FMOD
fmod( x, y )
.note O_CONVERT
See below.
.endnote
.np
The binary O_CONVERT operator is only available on the x86 code
generator.
It is used for based pointer operations (the result type of the
CGBinary call must be a far pointer type).
It effectively performs a MK_FP operation with the left hand side
providing the offset portion of the address, and the right hand side
providing the segment value.
If the right hand side expression is the address of a symbol, or the
type of the expression is a far pointer, then the segment value for the
symbol, or the segment value of the expression is used as the segment
value after the O_CONVERT operation.
.begnote $setptnt 1.5i
.notehd1 Short circuit operators
.notehd2 C equivalent
.note O_FLOW_AND
&&
.note O_FLOW_OR
||
.note O_FLOW_NOT
!
.endnote
.begnote $setptnt 1.5i
.notehd1 Control flow operators
.notehd2 C equivalent
.note O_GOTO
goto label;
.note O_LABEL
label:;
.note O_IF_TRUE
if( x ) goto label;
.note O_IF_FALSE
if( !(x) ) goto label;
.note O_INVOKE_LABEL
GOSUB (Basic)
.note O_LABEL_RETURN
RETURN (Basic)
.endnote
.ix typing
.ix conversions
.np
The type passed into a CG routine is used by the back end as the
type for the resulting node.
If the node is an operator node (CGBinary, CGUnary) the back end will
convert the operands to the result type before performing the
operation.
If the type TY_DEFAULT is passed, the code generator will use default
conversion rules to determine the resulting type of the node.
These rules are the same as the ANSI C value preserving rules, with the
exception that characters are not promoted to integers before doing
arithmetic operations.
.np
For example, if a node of type TY_UINT_2 and a node of type TY_INT_4
are to be added, the back end will automatically convert the operands
to TY_INT_4 before performing the addition.
The resulting node will have type TY_INT_4.
.*
.chap Leaf Nodes
.*
.section cg_name CGInteger( signed_32 val, cg_type type )
.*
.ix CGInteger
.ix integers
.np
Create an integer constant leaf node.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note val
The integral value.
.note type
An integral type.
.endnote
.*
.section cg_name CGInt64( signed_64 val, cg_type type )
.*
.ix CGInt64
.ix integers
.np
Create an 64-bit integer constant leaf node.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note val
The 64-bit integer value.
.note type
An integral type.
.endnote
.*
.section cg_name CGFloat( char *num, cg_type type )
.*
.ix CGFloat
.ix 'floating point constant'
.np
Create a floating-point constant leaf node.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note num
A NULL terminated E format string.
(-1.23456E-102)
.note type
A floating point type.
.endnote
.*
.section cg_name CGFEName( cg_sym_handle sym, cg_type type )
.*
.ix CGFEName
.ix variables
.np
Create a leaf node representing the address of the back_handle
associated with :HP2.sym:eHP2..
If sym represents an automatic variable or a parameter, CGAutoDecl or
CGParmDecl must be called before this routine is first used.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
The front end symbol.
.note type
The type to be associated with the value of the symbol.
.endnote
.*
.section cg_name CGBackName( back_handle bck, cg_type type )
.*
.ix CGBackName
.ix variables
.ix label, data
.np
Create a leaf node which represents the address of the back_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note bck
A back handle.
.note type
The type to be associated with the :HP2.value:eHP2. of the symbol.
.endnote
.*
.section cg_name CGTempName( temp_handle temp, cg_type type )
.*
.ix CGTempName
.ix temporaries
.np
Create a leaf node which yields the address of the temp_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note temp
A temp_handle.
.note type
The type to be associated with the :HP2.value:eHP2. of the symbol.
.endnote
.*
.chap Assignment Operations
.*
.section cg_name CGAssign( cg_name dest, cg_name src, cg_type type )
.*
.ix CGAssign
.ix assignment
.np
Create an assignment node.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note dest
The destination address.
.note src
The source value.
.note type
The type to which the destination address points.
.endnote
.begnote
.note Returns
The value of the right hand side.
.endnote
.*
.section cg_name CGLVAssign( cg_name dest, cg_name src, cg_type type )
.*
.ix CGLVAssign
.ix assignment
.np
Like CGAssign, but yields the address of the destination.
.*
.section cg_name CGPreGets( cg_op op, cg_name dest, cg_name src, cg_type type )
.*
.ix CGPreGets
.ix assignment
.np
Used for the C expressions a += b, a /= b.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
The arithmetic operator to be used.
.note dest
The address of the destination.
.note src
The value of the right hand side.
.note type
The type to which the destination address points.
.endnote
.begnote
.note Returns
The value of the left hand side.
.endnote
.*
.section cg_name CGLVPreGets( cg_op op, cg_name dest, cg_name src, cg_type type )
.*
.ix CGLVPreGets
.ix assignment
.np
Like CGPreGets, but yields the address of the destination.
.*
.section cg_name CGPostGets( cg_op op, cg_name dest, cg_name src, cg_type type )
.*
.ix CGPostGets
.ix assignment
.np
Used for the C expressions a++, a--.
No automatic scaling is done for pointers.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
The operator.
.note dest
The address of the destination
.note src
The value of the increment.
.note type
The type of the destination.
.endnote
.begnote
.note Returns
The value of the left hand side before the operation occurs.
.endnote
.*
.chap Arithmetic/logical operations
.*
.section cg_name CGBinary( cg_op op, cg_name left, cg_name right, cg_type type )
.*
.ix CGBinary
.ix expressions
.np
Binary operations.
No automatic scaling is done for pointer operations.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
The operator.
.note left
The value of the left hand side.
.note right
The value of the right hand side.
.note type
The result type.
.endnote
.begnote
.note Returns
The value of the result.
.endnote
.*
.section cg_name CGUnary( cg_op op, cg_name name, cg_type type )
.*
.ix CGUnary
.ix expressions
.np
Unary operations.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
The operator.
.note name
The value of operand.
.note type
The result type.
.endnote
.begnote
.note Returns
The value of the result.
.endnote
.*
.section cg_name CGIndex( cg_name name, cg_name by, cg_type type, cg_type ptype )
.*
.ix CGIndex
.np
Obsolete.
Do not use.
.*
.chap Procedure calls
.*
.section call_handle CGInitCall( cg_name name, cg_type type, cg_sym_handle aux_info )
.*
.ix CGInitCall
.ix procedures
.ix Routines
.ix functions
.np
Initiate a procedure call.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The address of the routine to call.
.note type
The return type of the routine.
.note aux_info
A handle which the back end may passed to FEAuxInfo to determine
the attributes of the call.
.endnote
.begnote
.note Returns
A :HP2.call_handle:eHP2. to be passed to the following routines.
.endnote
.*
.section void CGAddParm( call_handle call, cg_name name, cg_type type )
.*
.ix CGAddParm
.ix procedures
.ix routines
.ix functions
.np
Add a parameter to a call_handle.
The order of parameters is defined by the order in which they are
passed to this routine.
Parameters should be added in right to left order, as defined by the
procedure call.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note call
A call_handle.
.note name
The value of the parameter.
.note type
The type of the parameter.
This type will be passed to FEParmType to determine the actual type to
be used when passing the parameter.
For instance, characters are usually passes as integers in C.
.endnote
.*
.section cg_name CGCall( call_handle call )
.*
.np
Turn a call_handle into a cg_name by performing the call.
This may be immediately followed by an optional addition operation, to
reference a field in a structure return value.
An indirection operator must immediately follow, even if the function
has no return value.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note call
A call_handle.
.endnote
.begnote
.note Returns
The address of the function return value.
.endnote
.*
.chap Comparison/short-circuit operations
.*
.section cg_name CGCompare( cg_op op, cg_name left, cg_name right, cg_type type )
.*
.ix CGCompare
.ix 'short circuit operations'
.ix 'boolean expresssions'
.np
Compare two values.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
The comparison operator.
.note left
The value of the left hand side.
.note right
The value of the right hand side.
.note type
The type to which to convert the operands to before performing
comparison.
.endnote
.begnote
.note Returns
A TY_BOOLEAN cg_name, which may be passed to a control flow CG
routine, or used in an expression as an integral value.
.endnote
.*
.chap Control flow operations
.*
.section cg_name CGFlow( cg_op op, cg_name left, cg_name right )
.*
.ix 'short circuit operations'
.ix 'boolean expresssions'
.ix CGFlow
.np
Perform short-circuit boolean operations.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
An operator.
.note left
A TY_BOOLEAN or integral cg_name.
.note right
A TY_BOOLEAN or integral cg_name, or NULL if op is O_FLOW_NOT.
.endnote
.begnote
.note Returns
A TY_BOOLEAN cg_name.
.endnote
.*
.section cg_name CGChoose( cg_name sel, cg_name n1, cg_name n2, cg_type type )
.*
.ix CGChoose
.ix 'short circuit operations'
.ix 'boolean expresssions'
.np
Used for the C expression :HP2.sel:eHP2. ?
:HP2.n1:eHP2. : :HP2.n2:eHP2..
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sel
A TY_BOOLEAN or integral cg_name used as the selector.
.note n1
The value to return if :HP2.sel:eHP2. is non-zero.
.note n2
The value to return if :HP2.sel:eHP2. is zero.
.note type
The type to which convert the result.
.endnote
.begnote
.note Returns
The value of :HP2.n1:eHP2. or :HP2.n2:eHP2. depending upon the
truth of :HP2.sel:eHP2..
.endnote
.*
.section cg_name CGWarp( cg_name before, label_handle label, cg_name after )
.*
.ix CGWarp
.ix 'statement functions'
.ix FORTRAN
.np
To be used for FORTRAN statement functions.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note before
An arbitrary expression tree to be evaluated before
:HP2.label:eHP2. is called.
This is used to assign values to statement function arguments, which
are usually temporaries allocated with CGTemp.
.note label
A label_handle to invoke via O_CALL_LABEL.
.note after
An arbitrary expression tree to be evaluated after :HP2.label:eHP2.
is called.
This is used to retrieve the statement function return value.
.endnote
.begnote
.note Returns
The value of :HP2.after:eHP2..
This can be passed to CGEval, to guarantee that nested statement
functions are fully evaluated before their parameter variables are
reassigned, as in f(1,f(2,3,4),5).
.endnote
.*
.section void CG3WayControl( cg_name expr, label_handle lt, label_handle eq, label_handle gt )
.*
.ix CG3WayControl
.ix 'arithmetic if'
.ix FORTRAN
.np
Used for the FORTRAN arithmetic if statement.
Go to label :HP2.lt:eHP2., :HP2.eq:eHP2. or :HP2.gt:eHP2. depending on
whether :HP2.expr:eHP2. is less than, equal to, or greater than zero.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note expr
The selector value.
.note lt
A label_handle.
.note eq
A label_handle.
.note gt
A label_handle.
.endnote
.*
.section void CGControl( cg_op op, cg_name expr, label_handle lbl )
.*
.ix CGControl
.ix 'control flow'
.np
Generate conditional and unconditional flow of control.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note op
a control flow operator.
.note expr
A TY_BOOLEAN expression if op is O_IF_TRUE or O_IF_FALSE.
NULL otherwise.
.note lbl
The target label.
.endnote
.*
.section void CGBigLabel( back_handle lbl )
.*
.ix CGBigLabel
.ix 'control flow'
.np
Generate a label which may be branched to from a nested procedure or
used in NT structured exception handling.
Don't use this call unless you *really*, *really* need to.
It kills a lot of optimizations.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note lbl
A back_handle.
There must be a front end symbol associated with this back handle.
.endnote
.*
.section void CGBigGoto( back_handle value, int level )
.*
.ix CGBigGoto
.ix 'control flow'
.np
Generate a branch to a label in an outer procedure.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note lbl
A back_handle.
There must be a front end symbol associated with this back handle.
.note level
The lexical level of the target label.
.endnote
.*
.chap Select and Switch statements.
.*
.np
The select routines are used as follows.
CGSelOther should always be used even if there is no otherwise/default
case.
.np
:XMP.
end_label = BENewLabel();

sel_label = BENewLabel();
CGControl( O_GOTO, NULL, sel_label );
sel_handle = CGSelInit();

case_label = BENewLabel();
CGControl( O_LABEL, NULL, case_label );
CGSelCase( sel_handle, case_label, case_value );

   ... generate code associated with "case_value" here.

CGControl( O_GOTO, NULL, end_label ); // or else, fall through
other_label = BENewLabel();
CGControl( O_LABEL, NULL, other_label );
CGSelOther( sel_handle, other_label );

    ... generate "otherwise" code here

CGControl( O_GOTO, NULL, end_label ); // or else, fall through

CGControl( O_LABEL, NULL, sel_label );
CGSelect( sel_handle );

CGControl( O_LABEL, NULL, end_label );
:eXMP.
.*
.section sel_handle CGSelInit()
.*
.ix CGSelInit
.ix 'control flow'
.np
Create a sel_handle.
.begnote
.note Returns
A sel_handle to be passed to the following routines.
.endnote
.*
.section void CGSelCase( sel_handle sel, label_handle lbl, signed_32 val )
.*
.ix CGSelCase
.ix 'control flow'
.np
Add a single value case to a select or switch.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sel
A sel_handle obtained from CGSelInit().
.note lbl
The label to be associated with the case value.
.note val
The case value.
.endnote
.*
.section void CGSelRange( sel_handle s, signed_32 lo, signed_32 hi, label_handle lbl )
.*
.ix CGSelRange
.ix 'control flow'
.np
Add a range of values to a select.
All values are eventually converted into unsigned types to generate the
switch code, so lo and hi must have the same sign.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note s
A sel_handle obtained from CGSelInit().
.note lo
The lower bound of the case range.
.note hi
The upper bound of the case range.
.note lbl
The label to be associated with the case value.
.endnote
.*
.section void CGSelOther( sel_handle s, label_handle lbl )
.*
.ix CGSelOther
.ix 'control flow'
.np
Add the otherwise case to a select.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note s
A sel_handle.
.note lbl
The label to be associated with the otherwise case.
.endnote
.*
.section void CGSelect( sel_handle s, cg_name expr )
.*
.ix CGSelect
.ix 'control flow'
.np
Add the select expression to a select statement and generate code.
This must be the last routine called for a given select statement.
It invalidates the sel_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note s
A sel_handle.
.note expr
The value we are selecting.
.endnote
.*
.section void CGSelectRestricted( sel_handle s, cg_name expr, cg_switch_type allowed )
.*
.np
Identical to CGSelect, except that only switch generation techniques
corresponding to the set of allowed methods will be considered when
determining how to produce code.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note s
A sel_handle.
.note expr
The value we are selecting.
.note allowed
The allowed methods of generating code.
Must be a combination (non-empty) of the following bits:
:ZSL.
:ZLI.CG_SWITCH_SCAN
:ZLI.CG_SWITCH_BSEARCH
:ZLI.CG_SWITCH_TABLE
:ZeSL.
.endnote
.*
.chap Other
.*
.section void CGReturn( cg_name name, cg_type type )
.*
.ix CGReturn
.ix procedures
.ix routines
.ix functions
.np
Return from a function.
This is the last routine that may be called in any routine.
Multiple return statements must be implemented with assignments to a
temporary variable (CGTemp) and a branch to a label generated just
before this routine call.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The value of the return value, or NULL.
.note type
The type of the return value.
Use TY_INTEGER for void functions.
.endnote
.*
.section cg_name CGEval( cg_name name )
.*
.ix CGEval
.ix expressions
.np
Evaluate this expression tree now and assign its value to a leaf
node.
Used to force the order of operations.
This should only be used if necessary.
Normally, the expression trees adequately define the order of
operations.
This usually used to force the order of parameter evaluation.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The tree to be evaluated.
.endnote
.begnote
.note Returns
A leaf node containing the value of the tree.
.endnote
.*
.section void CGDone( cg_name name )
.*
.ix expressions
.ix CGDone
.np
Generate the tree and throw away the resultant value.
For example, CGAssign yields a value which may not be needed, but must
be passed to this routine to cause the tree to be generated.
This routine invalidates all cg_name handles.
After this routine has returned, any pending inline function expansions
will have been performed.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The cg_name to be generated/discarded.
.endnote
.*
.section void CGTrash( cg_name name )
.*
.ix expressions
.ix CGTrash
.np
Like CGDone, but used for partial expression trees.
This routine does not cause all existing cg_names to become invalid.
.*
.section cg_type CGType( cg_name name )
.*
.ix typing
.ix CGType
.np
Returns the type of the given cg_name.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
A cg_name.
.endnote
.begnote
.note Returns
The type of the cg_name.
.endnote
.*
.section cg_name *CGDuplicate( cg_name name )
.*
.ix expressions
.ix CGDuplicate
.np
Create two copies of a cg_name.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The cg_name to be duplicated.
.endnote
.begnote
.note Returns
A pointer to an array of two new cg_names, each representing the
same value as the original.
These should be copied out of the array immediately since subsequent
calls to CGDuplicate will overwrite the array.
.endnote
.*
.section cg_name CGBitMask( cg_name name, byte start, byte len, cg_type type )
.*
.ix CGBitMask
.ix expressions
.ix 'bit fields'
.np
Yields the address of a bit field.
This address may not really be used except with an indirection operator
or as the destination of an assignment operation.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The address of the integral variable containing the bit field.
.note start
The position of the least significant bit of the bit field.
0 indicates the least significant bit of the host data type.
.note len
The length of the bit field in bits.
.note type
The integral type of the value containing the bit field.
.endnote
.begnote
.note Returns
The address of the bit field.
To reference field2 in the following C structure for a little endian target,
use start=4, len=5, and type=TY_INT_2. For a big endian target, start=7.
.endnote
.np
:XMP.
typedef struct {
    short field1 : 4;
    short field2 : 5;
    short field3 : 7;
}
:eXMP.
.*
.section cg_name CGVolatile( cg_name name )
.*
.ix CGVolatile
.ix expressions
.ix volatile
.np
Indicate that the given address points to a volatile location.
This back end does not remember this information beyond this node in
the expression tree.
If an address points to a volatile location, the front end must call
this routine each time that address is used.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note name
The address of the volatile location.
.endnote
.begnote
.note Returns
A new cg_name representing the same value as name.
.endnote
.*
.section cg_name CGCallback( cg_callback func, void *ptr )
.*
.np
When a callback node is inserted into the tree, the code generator
will call the given function with the pointer as a parameter when it
turns the node into an instruction.
This can be used to retrieve order information about the placement of
nodes in the instruction stream.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note func
This is a pointer to a function which is compatible with the C type
"void (*)(void *)".
This function will be called with the second parameter to this function
as it's only parameter sometime during the execution of the CGDone call.
.note ptr
This will be a parameter to the function given as the first
parameter.
.endnote
.*
.section cg_name CGPatchNode( patch_handle hdl, cg_type type )
.*
.np
This prepares a leaf node to hold an integer constant which will be
provided sometime during the execution of the CGDone call by means of a
BEPatchInteger() call.
It is an error to insert a patch node into the tree and not call
BEPatchInteger().
.begnote
.notehd1 Parameter
.notehd2 Definition
.note hdl
A handle for a patch allocated with BEPatch().
.note type
The actual type of the node.
Must be an integer type.
.endnote
.*
.chap Data Generation
.*
.ix data
.ix segments
.ix label, data
.np
The following routines generate a data item described at the current
location in the current segment, and increment the current location by
the size of the generated object.
.*
.section void DGLabel( back_handle bck )
.*
.ix DGLabel
.ix label, data
.np
Generate the label for a given back_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note bck
A back_handle.
.endnote
.*
.section void DGBackPtr( back_handle bck, segment_id segid, signed_32 offset, cg_type type )
.*
.ix DGBackPtr
.ix 'back handle'
.ix 'relocatable data item'
.np
Generate a pointer to the label defined by the back_handle.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note bck
A back_handle.
.note segid
The segment_id of the segment in which the label for :HP2.bck:eHP2.
will be defined if it has not already been passed to DGLabel.
.note offset
A value to be added to the generated pointer value.
.note type
The pointer type to be used.
.endnote
.*
.section void DGFEPtr( cg_sym_handle sym, cg_type type, signed_32 offset )
.*
.ix DGFEPtr
.ix 'relocatable data item'
.np
Generate a pointer to the label associated with :HP2.sym:eHP2..
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
A cg_sym_handle.
.note type
The pointer type to be used.
.note offset
A value to be added to the generated pointer value.
.endnote
.*
.section void DGInteger( unsigned_32 value, cg_type type )
.*
.ix DGInteger
.np
Generate an integer.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note value
An integral value.
.note type
The integral type to be used.
.endnote
.*
.section void DGInteger64( unsigned_64 value, cg_type type )
.*
.ix DGInteger64
.np
Generate an 64-bit integer.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note value
An 64-bit integer value.
.note type
The integral type to be used.
.endnote
.*
.section void DGFloat( char *value, cg_type type )
.*
.ix DGFloat
.ix 'floating point constant'
.np
Generate a floating-point constant.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note value
An E format string (ie: 1.2345e-134)
.note type
The floating point type to be used.
.endnote
.*
.section void DGChar( char value )
.*
.ix DGChar
.ix character
.np
Generate a character constant.
Will be translated if cross compiling.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note value
A character value.
.endnote
.*
.section void DGString( char *value, uint len )
.*
.ix DGString
.ix character
.np
Generate a character string.
Will be translated if cross compiling.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note value
Pointer to the characters to put into the segment.
It is not necessarily a null terminated string.
.note len
The length of the string.
.endnote
.*
.section void DGBytes( unsigned_32 len, byte *src )
.*
.ix DGBytes
.np
Generate raw binary data.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note src
Pointer to the data.
.note len
The length of the byte stream.
.endnote
.*
.section void DGIBytes( unsigned_32 len, byte pat )
.*
.ix DGIBytes
.np
Generate the byte :HP2.pat:eHP2., :HP2.len:eHP2. times.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note pat
The pattern byte.
.note len
The number of times to repeat the byte.
.endnote
.*
.section void DGUBytes( unsigned_32 len )
.*
.ix DGUBytes
.np
Generate :HP2.len:eHP2. undefined bytes.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note len
The size by which to increase the segment.
.endnote
.*
.section void DGAlign( uint align )
.*
.ix DGAlign
.ix segments
.np
Align the segment to an :HP2.align:eHP2. byte boundary.
Any slack bytes will have an undefined value.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note align
The desired alignment boundary.
.endnote
.*
.section unsigned_32 DGSeek( unsigned_32 where )
.*
.ix DGSeek
.ix segments
.np
Seek to a location within a segment.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note where
The location within the segment.
.endnote
.begnote
.note Returns
The current location in the segment before the seek takes place.
.endnote
.*
.section unsigned long DGTell()
.*
.ix DGTell
.ix segments
.begnote
.note Returns
The current location within the segment.
.endnote
.*
.section unsigned long DGBackTell( back_handle bck )
.*
.ix DGBackTell
.begnote
.note Returns
The location of the label within its segment.
The label must have been previously generated via DGLabel.
.endnote
.*
.chap Front End Routines
.*
.section void FEGenProc( cg_sym_handle sym )
.*
.ix FEGenProc
.ix 'inline procedures'
.ix procedures
.ix routines
.ix functions
.np
This routine will be called when the back end is generating a tree
and encounters a function call having the :HP2.call_class:eHP2.
MAKE_CALL_INLINE.
The front end must save its current state and start generating code for
:HP2.sym:eHP2..
FEGenProc calls may be nested if the code generator encounters an
inline within the code for an inline function.
The front end should maintain a state stack.
It is up to the front end to prevent infinite recursion.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
The cg_sym_handle of the function to be generated.
.endnote
.*
.section back_handle FEBack( cg_sym_handle sym )
.*
.ix FEBack
.ix 'back handle'
.np
Return, and possibly allocate using BENewBack, a back handle for
sym.
See the example under :HDREF refid='bck'.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym

.endnote
.begnote
.note Returns
A back_handle.
.endnote
.*
.section segment_id FESegID( cg_sym_handle sym )
.*
.ix FESegID
.ix segments
.np
Return the segment_id for symbol :HP2.sym:eHP2..
A negative value may be returned to indicate that the symbol is defined
in an unknown PRIVATE segment which has been defined in another module.
If two symbols have the same negative value returned, the back end
assumes that they are both defined in the same (unknown) segment.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
A cg_sym_handle.
.endnote
.begnote
.note Returns
A segment_id.
.endnote
.*
.section char *FEModuleName()
.*
.ix FEModuleName
.begnote
.note Returns
A null terminated string which is the name of the module being
compiled.
This is usually the file name with path and extension information
stripped.
.endnote
.*
.section char FEStackCheck( cg_sym_handle sym )
.*
.ix 'stack probes'
.ix FEStackCheck
.begnote
.note Returns
1 if stack checking required for this routine
.endnote
.*
.section unsigned FELexLevel( cg_sym_handle sym )
.*
.ix FELexLevel
.ix pascal
.begnote
.note Returns
The lexical level of routine :HP2.sym:eHP2..
This must be zero for all languages except Pascal.
In Pascal, 1 indicates the level of the main program.
Each nested procedures adds an additional level.
.endnote
.*
.section char *FEName( cg_sym_handle sym )
.*
.ix FEName
.begnote
.note Returns
A NULL terminated character string which is the name of sym.
A null string should be returned if the symbol has no name.
NULL should never be returned.
.endnote
.*
.section char *FEExtName( cg_sym_handle sym, int request )
.*
.ix FEExtName
.begnote
.note Returns
A various kind in dependency on request parameter.
.notehd1 Request parameter
.notehd2 Returns
.note EXTN_BASENAME
NULL terminated character string which is the name of sym.
A null string should be returned if the symbol has no name.
NULL should never be returned.
.note EXTN_PATTERN
NULL terminated character string which is the pattern for symbol name decoration.
'*' is replaced by symbol name.
'^' is replaced by its upper case equivalent.
'!' is replaced by its lower case equivalent.
'#' is replaced by '@nnn' where nnn is decimal number representing total size of all
function parameters.
If an '\' is present, the character following is used literally.
.note EXTN_PRMSIZE
Returns int value which represent size of all parameters when symbol is function.
.endnote
.*
.section cg_type FEParmType( cg_sym_handle func, cg_sym_handle parm, cg_type type )
.*
.ix FEParmType
.begnote
.note Returns
The type to which to promote an argument with a given type before
passing it to a procedure.
Type will be a dealiased type.
.endnote
.*
.section int FETrue()
.*
.ix FETrue
.begnote
.note Returns
The value of TRUE.
This is normally 1.
.endnote
.*
.section char FEMoreMem( size_t size )
.*
.ix FEMoreMem
.np
Release memory for the back end to use.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note size
is the amount of memory required
.endnote
.begnote
.note Returns
1 if at least :HP2.size:eHP2. bytes were released.
May always return 0 if memory is not a scarce resource in the host
environment.
.endnote
.*
.section dbg_type FEDbgType( cg_sym_handle sym )
.*
.ix FEDbgType
.begnote
.note Returns
The dbg_type handle for the symbol :HP2.sym:eHP2..
.endnote
.*
.section fe_attr FEAttr( cg_sym_handle sym )
.*
.ix FEAttr
.np
Return symbol attributes for :HP2.sym:eHP2..
These are bits combinable with the bit-wise or operator |.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
A cg_sym_handle.
.endnote
.begnote $break $setptnt 1.5i
.notehd1 Return value
.notehd2  Definition
.note FE_PROC
A procedure.
.note FE_STATIC
A static or external symbol.
.note FE_GLOBAL
Is a global (extern) symbol.
.note FE_IMPORT
Needs to be imported.
.note FE_CONSTANT
The symbol is read only.
.note FE_MEMORY
This automatic variable needs a memory location.
.note FE_VISIBLE
Accessible outside this procedure?
.note FE_NOALIAS
No pointers point to this symbol.
.note FE_UNIQUE
This symbol should have an address which is different from all
other symbols with the FE_UNIQUE attribute.
.note FE_COMMON
There might be multiple definitions of this symbol in a program,
and it should be generated in such a way that all versions of the
symbol are merged into one copy by the linker.
.note FE_ADDR_TAKEN
The symbol has had it's address taken somewhere in the program (not
necessarally visible to the code generator).
.note FE_VOLATILE
The symbol is "volatile" (in the C language sense).
.note FE_INTERNAL
The symbol is not at file scope.
.endnote
.*
.section void FEMessage( msg_class msg, void *extra )
.*
.ix FEMessage
.np
Relays information to the front end.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note msg
Defined below.
.note extra
Extra information.
The type and meaning depends on the value of :HP2.msg:eHP2. and is
indicated below.
.endnote
.begnote $break $setptnt 2i
.note MSG_INFO_FILE
.ix 'error messages'
Informational message about file.
extra (void) is ignored.
.note MSG_CODE_SIZE
Code size.
Extra (int) is the size of the generated code.
.note MSG_DATA_SIZE
Data size.
Extra (int) is the size of the generated data.
.note MSG_ERROR
A back end error message.
Extra (char *) is the error message.
.note MSG_FATAL
A fatal code generator error.
Extra (char *) is the reason for the fatal error.
The front end should issue this message and exit immediately to the
system.
.note MSG_INFO_PROC
Informational message about current procedure.
Extra (char *) is a message.
.note MSG_BAD_PARM_REGISTER
Invalid parameter register returned from FEAuxInfo.
Extra (int) is position of the offending parameter.
.note MSG_BAD_RETURN_REGISTER
Invalid return register returned from FEAuxInfo.
Extra (aux_handle) is the offending aux_handle.
.note MSG_REGALLOC_DIED
The register alloc ran out of memory.
Extra (cg_sym_handle) is the procedure which was not fully optimized.
.note MSG_SCOREBOARD_DIED
The register scoreboard ran out of memory.
Extra (cg_sym_handle) is the procedure which was not fully optimized.
.note MSG_PEEPHOLE_FLUSHED
Peep hole optimizer flushed due to lack of memory.
(void)
.note MSG_BACK_END_ERROR
BAD NEWS!
Internal compiler error.
Extra (int) is an internal error number.
.note MSG_BAD_SAVE
Invalid register modification information return from FEAuxInfo.
Extra (aux_handle) is the offending aux_handle.
.note MSG_WANT_MORE_DATA
The back end wants more data space.
Extra (int) is amount of additional memory needed to run.
(DOS real mode hosts only).
.note MSG_BLIP
Blip.
Let the world know we're still alive by printing a dot on the screen.
This is called approximately every 4 seconds during code generation.
(void)
.note MSG_BAD_LINKAGE
Cannot resolve linkage conventions.
370 only.
(sym)
.note MSG_SCHEDULER_DIED
Instruction scheduler ran out of memory.
Extra (cg_sym_handle) is the procedure which was not fully optimized.
.note MSG_NO_SEG_REGS
(Only occurs in the x86 version).
The cg_switches did not allow any segment registers to float, but the
user has requested a far pointer indirection.
Extra (cg_sym_handle) is the procedure which contained the far pointer
usage.
.note MSG_SYMBOL_TOO_LONG
Given symbol is too long and is truncated to maximum permited
length for current module output format.
Extra (cg_sym_handle) is the symbol which was truncated.
.endnote
.*
.section void *FEAuxInfo( void *extra, aux_class class )
.*
.ix FEAuxInfo
.np
relay information to back end
.begnote
.notehd1 Parameter
.notehd2 Definition
.note extra
Extra information.
Its type and meaning is determined by the value of class.
.note class
Defined below.
.endnote
.begnote $break $setptnt 2i
.notehd1 Parameters
.notehd2 Return Value
.note ( cg_sym_handle, AUX_LOOKUP )
aux_handle - given a cg_sym_handle, return an aux_handle.
.note ( aux_handle, CALL_BYTES )
byte_seq * - A pointer to bytes to be generated instead of a call,
or NULL if a call is to be generated.
.np
:XMP.
typedef struct byte_seq {
    char    length;
    char    data[ 1 ];
} byte_seq;
:eXMP.
.ix 'calling conventions'
.note ( aux_handle, CALL_CLASS )
call_class * - returns call_class of the given aux_handle.
See definitions below.
.note ( short, FREE_SEGMENT )
short - A free segment value which is free memory for the code
generator to use.
The first word at segment:0 is the size of the free memory in bytes.
(DOS real mode host only)
.note ( NULL, OBJECT_FILE_NAME )
char * - The name of the object file to be generated.
.ix 'calling conventions'
.note ( aux_handle, PARM_REGS )
hw_reg_set[] - The set of register to be used as parameters.
.note ( aux_handle, RETURN_REG )
hw_reg_set * - The return register.
This is only called if the routine is declared to have the
SPECIAL_RETURN call_class.
.note ( NULL, REVISION_NUMBER )
int - Front end revision number.
Must return II_REVISION.
.note ( aux_handle, SAVE_REGS )
hw_reg_set * - Registers which are preserved by the routine.
.note ( cg_sym_handle, SHADOW_SYMBOL )
cg_sym_handle - An alternate handle for a symbol.
Required for FORTRAN.
Usually implemented by turning on the LSB of a pointer or MSB of an
integer.
.note ( NULL, SOURCE_NAME )
char * - The name of the source file to be put into the object file.
.note ( cg_sym_handle, TEMP_LOC_NAME )
 Return one of TEMP_LOC_NO, TEMP_LOC_YES, TEMP_LOC_QUIT.
After the back end has assigned stack locations to those temporaries
which were not placed in registers, it begins to call FEAuxInfo with
this request and passes in the cg_sym_handle for each of those
temporaries.
If the front end responds with TEMP_LOC_QUIT the back end will stop
making TEMP_LOC_NAME requests.
If the front end responds with TEMP_LOC_YES the back end will then
perform a TEMP_LOC_TELL request (see next).
If the front end returns TEMP_LOC_NO the back end moves onto the next
cg_sym_handle in its list.
.note ( int, TEMP_LOC_TELL )
 Returns nothing.
The 'int' value passed in is the relative position on the stack for the
temporary identified by the cg_sym_handle passed in from the previous
TEMP_LOC_NAME.
The value for an individual temporary has no meaning, but the
difference between two of the values is the number of bytes between the
addresses of the temporaries on the stack.
.note ( void *, NEXT_DEPENDENCY )
Returns the handle of the next dependency file for which
information is available.
To start the list off, the back end passes in NULL for the dependency
file handle.
.note ( void *, DEPENDENCY_TIMESTAMP )
Given the dependency file handle from the last NEXT_DEPENDENCY
request, return pointer to an unsigned long containing a timestamp
value for the dependency file.
.note ( void *, DEPENDENCY_NAME )
Given the dependency file handle from the last NEXT_DEPENDENCY
request, return a pointer to a string containing the name for the
dependency file.
.note ( NULL, SOURCE_LANGUAGE )
Returns a pointer to a string which identifies the source language
of the pointer.
E.g.
"C" for C, "FORTRAN" for FORTRAN, "CPP" for C++.
.note ( cg_sym_handle, DEFAULT_IMPORT_RESOLVE )
Only called for imported symbols.
Returns a cg_sym_handle for another imported symbol which the reference
should be resolved to if certain conditions are met (see IMPORT_TYPE
request).
If NULL or the original cg_sym_handle is returned, there is no default
import resolution symbol.
.note ( int, UNROLL_COUNT )
Returns a user-specified unroll count, or 0 if the user did not
specify an unroll count.
The parameter is the nesting level of the loop for which the request is
being made.
Loops which are not contained inside of other loops are nesting level
1.
If this function returns a non-zero value, the loop in question will be
unrolled that many times (there will be (count + 1) copies of the body).
.endnote
.begnote $break $setptnt 2i
.notehd1 x86 Parameters
.notehd2 Return value
.note ( NULL, CODE_GROUP )
char * - The name of the code group.
.note ( aux_handle, STRETURN_REG )
hw_reg_set * - The register which points to a structure return
value.
Only called if the routine has the SPECIAL_STRUCT_RETURN attribute.
.note ( void *, NEXT_IMPORT )
void * (See notes at end) - A handle for the next symbol to
generate a reference to in the object file.
.note ( void*, IMPORT_NAME )
char * - The EXTDEF name to generate given a handle
.note ( void *, NEXT_IMPORT_S )
void * (See notes at end) - A handle for the next symbol to
generate a reference to in the object file.
.note ( void*, IMPORT_NAME_S )
Returns a cg_sym_handle. The EXTDEF name symbol reference to generate given
a handle.
.note ( void*, NEXT_LIBRARY )
void * (See notes at end) - Handle for the next library required
.note ( void*, LIBRARY_NAME )
char * - The library name to generate given a handle
.note ( NULL, DATA_GROUP )
char * - Used to name DGROUP exactly.
NULL means use no group at all.
.note ( segment_id, CLASS_NAME )
NULL - Used to name the class of a segment.
.note ( NULL, USED_8087 )
NULL - Indicate that 8087 instructions were generated.
.note ( NULL, STACK_SIZE_8087 )
int - How many 8087 registers are reserved for stack.
.note ( NULL, CODE_LABEL_ALIGNMENT )
char * - An array x, such that x[i] is the label alignment
requirements for labels nested within i loops.
.note ( NULL, PROEPI_DATA_SIZE )
int - How much stack is reserved for the prolog hook routine.
.note ( cg_sym_handle, IMPORT_TYPE )
Returns IMPORT_IS_WEAK, IMPORT_IS_LAZY, IMPORT_IS_CONDITIONAL.
If the DEFAULT_IMPORT_RESOLVE request returned a default resolution
symbol the back end then performs an IMPORT_TYPE request to determine
the type of the resolution.
IMPORT_IS_WEAK generates a weak import (the symbol is not searched for
in libraries).
IMPORT_IS_LAZY generates a lazy import (the symbol is searched for in
libraries).
IMPORT_IS_CONDITIONAL is used for eliminating unused virtual functions.
The default symbol resolution is used if none of the conditional
symbols are referenced/defined by the program.
The back end is informed of the list of conditional symbols by the
following three aux requests.
IMPORT_IS_CONDITIONAL_PURE is used for eliminating unused pure virtual
functions.
.note ( cg_sym_handle, CONDITIONAL_IMPORT )
 Returns void *.
Once the back end determines that it has a conditional import, it
performs this request to get a conditional list handle which is the
head of the list of conditional symbols.
.note ( void *, CONDITIONAL_SYMBOL )
Returns a cg_sym_handle.
Give an conditional list handle, return the front end symbol associated
with it.
.note ( void *, NEXT_CONDITIONAL )
Given an conditional list handle, return the next conditional list
handle.
Return NULL at the end of the list.
.note ( aux_handle, VIRT_FUNC_REFERENCE )
Returns void *.
When performing an indirect function call, the back end invokes
FEAuxInfo passing the aux_handle supplied with the CGInitCall.
If the indirect call is referencing a C++ virtual function, the front
end should return a magic cookie which is the head of a list of virtual
functions that might be invoked by this call.
If it is not a virtual function invocation, return NULL.
.note ( void *, VIRT_FUNC_NEXT_REFERENCE )
Returns void *.
Given the magic cookie returned by the VIRT_FUNC_REFERENCE or a
previous VIRT_FUNC_NEXT_REFRENCE, return the next magic cookie in the
list of virtual functions that might be refrenced from this indirect
call.
Return NULL if at the end of the list.
.note ( void *, VIRT_FUNC_SYM )
Returns cg_sym_handle.
Given a magic cookie from a VIRT_FUNC_REFERENCE or
VIRT_FUNC_NEXT_REFERENCE, return the cg_sym_handle for that entry in
the list of virtual functions that might be invoked.
.note ( segment_id, PEGGED_REGISTER )
Returns a pointer at a hw_reg_set or NULL.
If the pointer is non-NULL and the hw_reg_set is not EMPTY, the
hw_reg_set will indicate a segment register that is pegged (pointing)
to the given segment_id.
The code generator will use this segment register in any references to
objects in the segment.
If the pointer is NULL or the hw_reg_set is EMPTY, the code generator
uses the cg_switches to determine if a segment register is pointing at
the segment or if it will have to load one.
.endnote
:CMT. S/370 support not maintained
.if 0 eq 1 .do begin
.begnote $break $setptnt 2i
.notehd1 370 Parameters
.notehd2 Return value
.note LINKAGE_REGS
???
.note AUX_OFFSET
???
.note AUX_HAS_MAIN
???
.note ASM_NAME
???
.note DBG_NAME
???
.note CSECT_NAME
???
.endnote
.do end
.begnote $break $setptnt 2i
.notehd1 Call Class
.notehd2 Meaning
.ix 'calling conventions'
.note REVERSE_PARMS
Reverse the parameter list.
.note SUICIDAL
Routine never returns.
.note PARMS_BY_ADDRESS
Pass parameters by reference.
.note MAKE_CALL_INLINE
Call should be inline.
FEGenProc will be called for code sequence when required.
.endnote
.begnote $break $setptnt 2i
.notehd1 x86 Call Class
.notehd2 Meaning
.note FAR
Does routine require a far call/return.
.note LOAD_DS_ON_CALL
Load DS from DGROUP prior to call.
.note CALLER_POPS
Caller pops/removes parms from the stack.
.note ROUTINE_RETURN
Routine allocates structure return memory.
.note SPECIAL_RETURN
Routine has non-default return register.
.note NO_MEMORY_CHANGED
Routine modifies no visible statics.
.note NO_MEMORY_READ
Routine reads no visible statics.
.note MODIFY_EXACT
Routine modifies no parameter registers.
.note SPECIAL_STRUCT_RETURN
Routine has special struct return register.
.note NO_STRUCT_REG_RETURNS
Pass 2/4/8 byte structs on stack, as opposed to registers.
.note NO_FLOAT_REG_RETURNS
Return floats as structs.
.note INTERRUPT
Routine is an interrupt routine.
.note NO_8087_RETURNS
No return values in the 8087.
.note LOAD_DS_ON_ENTRY
Load ds with dgroup on entry.
.note DLL_EXPORT
Is routine an OS/2 export symbol?
.note FAT_WINDOWS_PROLOG
Generate the real mode windows prolog code.
.note GENERATE_STACK_FRAME
Always generate a traceable prolog.
.note EMIT_FUNCTION_NAME
Emit the function name in front of the function in the code segment.
.note GROW_STACK
Emit a call to grow the stack on entry
.note PROLOG_HOOKS
Generate a prolog hook call.
.note EPILOG_HOOKS
Generate an epilog hook call.
.note THUNK_PROLOG
Generate a thunking prolog for routines calling 16 bit code.
.note FAR16_CALL
Performs a 16:16 call in the 386 compiler.
.note TOUCH_STACK
Certain people (who shall remain nameless) have implemented an
operating system (which shall remain nameless) that can't be bothered
figuring out whether a page reference is in the stack or not.
This attribute forces the first reference to the stack (after a routine
prologue has grown it) to be through the SS register.
.endnote
:CMT. S/370 support not maintained
.if 0 eq 1 .do begin
.begnote $break $setptnt 2i
.notehd1 370 Call Class
.notehd2 Meaning
.note LINKAGE_OS
OS linkage.
.note LINKAGE_OSFUNC
Modified OS linkage.
.note LINKAGE_OSENTRY
Modified OS linkage.
.note LINKAGE_CLINK
WSL linkage.
.endnote
.do end
.*
.chap Debugging Information
.*
These routines generate information about types, symbols, etc.
.*
.section void DBLineNum( uint no )
.*
.ix DBLineNum
.np
Set the current source line number.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note no
Is the current source line number.
.endnote
.*
.section void DBModSym( cg_sym_handle sym, cg_type indirect )
.*
.ix DBModSym
.np
Define a symbol within the module (file scope).
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
is a front end symbol handle.
.note indirect
is the type of indirection needed to obtain the value
.endnote
.*
.section void DBObject( dbg_type tipe, dbg_loc loc )
.*
.ix DBObject
.np
Define a function as being a member function of a C++ class, and
identify the type of the class and the location of the object being
manipulated.
This function may only be done after the DBModSym for the function.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note tipe
is the debug type of the class that the function is a member of.
.note loc
is a location expression that evaluates to the address of the
object being manipulated by the function (the contents of the 'this'
pointer in C++).
This parameter is NULL if the routine is a static member function.
.endnote
.*
.section void DBLocalSym( cg_sym_handle sym, cg_type indirect )
.*
.ix DBLocalSym
.np
As DBModSym but for local (routine scope) symbols.
.*
.section void DBGenSym( cg_sym_handle sym, dbg_loc loc, int scoped )
.*
.ix DBGenSym
.np
Define a symbol either with module scope ('scoped' == 0) or within
the current block ('scoped' != 0).
This routine superseeds both DBLocalSym and DBModuleSym.
The 'loc' parameter is a location expression (explained later) which
allows an aribitrary sequence of operations to locate the storage for
the symbol.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note sym
is a front end symbol handle.
.note loc
the location expression which is evaluated by the debugger to
locate the lvalue of the symbol.
.note scoped
whether the symbol is file scoped or not.
.endnote
.*
.section void DBBegBlock()
.*
.ix DBBegBlock
.np
Open a new scope level.
.*
.section void DBEndBlock()
.*
.ix DBEndBlock
.np
Close the current scope level.
.*
.section dbg_type DBScalar( char *name, cg_type tipe )
.*
.ix DBScalar
.np
Defines the string :HP2.name:eHP2. to have type :HP2.tipe:eHP2..
.*
.section dbg_type DBScope( char *name )
.*
.ix DBScope
.np
define a symbol which "scopes" subsequent symbols.
In C, the keywords :HP2.enum:eHP2., :HP2.union:eHP2., :HP2.struct:eHP2.
may perform this function as in :HP2.struct foo:eHP2..
.*
.section dbg_name DBBegName( const char *name, dbg_type scope )
.*
.ix DBBegName
.np
start a type name whose type is yet undetermined
.*
.section dbg_type DBForward( dbg_name name )
.*
.ix DBForward
.np
declare a type to be a forward reference
.*
.section dbg_type DBEndName( dbg_name name, dbg_type tipe )
.*
.ix DBEndName
.np
complete the definition of a type name.
.*
.section dbg_type DBArray( dbg_type index, dbg_type base )
.*
.ix DBArray
.np
define a C array type
.*
.section dbg_type DBIntArray( unsigned_32 hi, dbg_type base )
.*
.ix DBIntArray
.np
define a C array type
.*
.section dbg_type DBSubRange( signed_32 lo, signed_32 hi, dbg_type base )
.*
.ix DBSubRange
.np
define an integer range type
.*
.section dbg_type DBPtr( cg_type ptr_type, dbg_type base )
.*
.ix DBPtr
.np
declare a pointer type
.*
.section dbg_type DBBasedPtr( cg_type ptr_type, dbg_type base, dbg_loc seg_loc )
.*
.ix DBBasedPtr
.np
declare a based pointer type.
The 'seg_loc' parameter is a location expression which evaluates to the
base address for the pointer after the indirection has been performed.
Before the location expression is evalated, the current lvalue of the
pointer symbol associated with this type is pushed onto the expression
stack (needed for based on self pointers).
.*
.section dbg_struct DBBegStruct()
.*
.ix DBBegStruct
.np
start a structure type definition
.*
.section void DBAddField( dbg_struct st, unsigned_32 off, char *nm, dbg_type base )
.*
.ix DBAddField
.np
add a field to a structure
.*
.section void DBAddBitField( dbg_struct st, unsigned_32 off, byte strt, byte len, char *nm, dbg_type base )
.*
.ix DBAddBitField
.np
add a bit field to a structure
.*
.section void DBAddLocField( dbg_struct st, dbg_loc loc, uint attr, byte strt, byte len, char *nm, dbg_type base )
.*
.ix DBAddLocField
.np
Add a field or bit field to a structure with a generalized location
expression 'loc'.
The location expression should assume the the address of the base of
the structure has already been pushed onto the debugger's evaluation
stack.
The 'attr' parameter contains a zero or more of the following
attributes or'd together:
.begnote
.notehd1 Attribute
.notehd2 Definition
.note FIELD_ATTR_INTERNAL
the field is internally generated by the compiler and would not be
normally visible to the user.
.note FIELD_ATTR_PUBLIC
the field has the C++ 'public' attribute.
.note FIELD_ATTR_PROTECTED
the field has the C++ 'protected' attribute.
.note FIELD_ATTR_PRIVATE
the field has the C++ 'private' attribute.
.endnote
:PC.If the field being described is _not_ a bit field, the 'len'
parameter should be set to zero.
.*
.section void DBAddInheritance( dbg_struct st, dbg_type inherit, dbg_loc adjust )
.*
.ix DBAddInheritance
Add the fields of an inherited structure to the current structure being
defined.
.begnote
.notehd1 Parameter
.notehd2 Definition
.note st
the dbg_struct handle for the structure currently being defined.
.note inherit
the dbg_type of a previously defined structure which is being
inherited.
.note adjust
a location expression which evaluates to a value which is the
amount to adjust the field offsets by in the inherited structure to
access them in the current structure.
The base address of the symbol associated with the structure type is
pushed onto the location expression stack before the expression is
evaluated.
.endnote
.*
.section dbg_type DBEndStruct( dbg_struct st )
.*
.ix DBEndStruct
.np
end a structure definition
.*
.section dbg_enum DBBegEnum( cg_type tipe )
.*
.ix DBBegEnum
.np
begin defining an enumerated type
.*
.section void DBAddConst( dbg_enum en, const char *nm, signed_32 val )
.*
.ix DBAddConst
.np
add a symbolic constant to an enumerated type
.*
.section void DBAddConst64( dbg_enum en, const char *nm, signed_64 val )
.*
.ix DBAddConst64
.np
add a symbolic 64-bit integer constant to an enumerated type
.*
.section dbg_type DBEndEnum( dbg_enum en )
.*
.ix DBEndEnum
.np
finish declaring an enumerated type
.*
.section dbg_proc DBBegProc( cg_type call_type, dbg_type ret )
.*
.ix DBBegProc
.np
begin the a current procedure
.*
.section void DBAddParm( dbg_proc pr, dbg_type tipe )
.*
.ix DBAddParm
.np
declare a parameter to the procedure
.*
.section dbg_type DBEndProc( proc_list *pr )
.*
.ix DBEndProc
.np
end the current procedure
.*
.section dbg_type DBFtnType( char *name, dbg_ftn_type tipe )
.*
.ix DBFtnType
.np
declare a fortran COMPLEX type
.*
.section dbg_type DBCharBlock( unsigned_32 len )
.*
.ix DBCharBlock
.np
declare a type to be a block of length :HP2.len:eHP2. characters
.*
.section dbg_type DBIndCharBlock( back_handle len, cg_type len_type, int off )
.*
.ix DBIndCharBlock
.np
declare a type to be a block of characters.
The length is found at run-time at back_handle :HP2.len:eHP2. + offset
:HP2.off:eHP2..
The integral type of the back_handle location is :HP2.len_type:eHP2.
.*
.section dbg_type DBLocCharBlock( dbg_loc loc, cg_type len_type )
.*
.ix DBLocCharBlock
.np
declare a type to be a block of characters.
The length is found at run-time at the address specified by the
location expression :HP2.loc:eHP2..
The integral type of the location is :HP2.len_type:eHP2.
.*
.section dbg_type DBFtnArray( back_handle dims, cg_type lo_bound_tipe, cg_type num_elts_tipe, int off, dbg_type base )
.*
.ix DBFtnArray
.np
define a FORTRAN array dimension slice.
:HP2.dims:eHP2. is a back handle + offset :HP2.off:eHP2. which will
point to a structure at run-time.
The structure contains the array low bound (type
:HP2.lo_bound_tipe:eHP2.) followed by the number of elements (type
:HP2.num_elts_tipe:eHP2.).
:HP2.base:eHP2. is the element type of the array.
.*
.section dbg_type DBDereference( cg_type ptr_type, dbg_type base )
.*
.ix DBDereference
.np
declare a type to need an implicit de-reference to retrieve the
value (for FORTRAN parameters)
:NOTE.
This routine has been superceded by the use of location expressions.
.*
.section dbg_loc DBLocInit( void )
.*
.ix DBLocInit
.np
create an initial empty location expression
.*
.section dbg_loc DBLocSym( dbg_loc loc, cg_sym_handle sym )
.*
.ix DBLocSym
.np
push the address of 'sym' on to the expression stack
.*
.section dbg_loc DBLocTemp( dbg_loc loc, temp_handle tmp )
.*
.ix DBLocTemp
.np
push the address of 'tmp' on to the expression stack
.*
.section dbg_loc DBLocConst( dbg_loc loc, unsigned_32 val )
.*
.ix DBLocConst
.np
push the constant 'val' on to the expression stack
.*
.section dbg_loc DBLocOp( dbg_loc loc, dbg_loc_op op, unsigned other )
.*
.ix DBLocOp
.np
perform the following list of operations on the expression stack
.begnote
.notehd1 Operation
.notehd2 Definition
.note DB_OP_POINTS
take the top of the expression stack and use it as the address in
an indirection operation.
The result type of the operation is given by the 'other' parameter
which must be a cg_type which resolves to either an unsigned_16,
unsigned_32, a 16-bit far pointer, or a 32-bit far pointer.
.note DB_OP_ZEX
zero extend the top of the stack.
The 'other' parameter is a cg_type which is either 1 byte in size or 2
bytes in size.
That size determines how much of the original top of stack value to
leave untouched.
.note DB_OP_XCHG
exchange the top of stack value with the stack entry indexed by
'other'.
.note DB_OP_MK_FP
take the top two entries on the stack.
Make the second entry the segment value and the first entry the offset
value of an address.
.note DB_OP_ADD
add the top two stack entries together.
.note DB_OP_DUP
duplicate the top stack entry.
.note DB_OP_POP
pop off (throw away) the top stack entry.
.endnote
.*
.section void DBLocFini( dbg_loc loc )
.*
.ix DBLocFini
.np
the given location expression will not be used anymore.
.*
.section unsigned DBSrcFile( char *fname )
.*
.ix DBSrcFile
.np
add the file name into the list of source files for positon info,
return handle to this name
:NOTE.
Handle 0 is reserved for base source file name and is added by BE
automaticaly during initialization.
.*
.section void DBSrcCue( unsigned fno, unsigned line, unsigned col )
.*
.ix DBSrcCue
.np
add source position info for the appropriate source file
.*
.chap Registers
.*
.ix registers
.np
The hw_reg_set type is an abstract data type capable of representing
any combination of machine registers.
It must be manipulated using the following macros.
A parameter :HP2.c:eHP2., :HP2.c1:eHP2., :HP2.c2:eHP2., etc.
indicate a register constant such as HW_EAX must be used.
Anything else must be a variable of type :HP2.hw_reg_set:eHP2..
.np
The following are used for static initialization.
:ZSL.
:ZLI.HW_D_1( c1 )
:ZLI.HW_NotD_1( c1 )
:ZLI.HW_D_2( c1, c2 )
:ZLI.HW_NotD_2( c1, c2 )
:ZLI.HW_D_3( c1, c2, c3 )
:ZLI.HW_NotD_3( c1, c2, c3 )
:ZLI.HW_D_4( c1, c2, c3, c4 )
:ZLI.HW_NotD_4( c1, c2, c3, c4 )
:ZLI.HW_D_5( c1, c2, c3, c4, c5 )
:ZLI.HW_NotD_5( c1, c2, c3, c4, c5 )
:ZLI.HW_D( c1 )
:ZLI.HW_NotD( c1 )
:ZeSL.
.np
:XMP.
hw_reg_set regs[] = {
    /* the EAX register */
    HW_D( HW_EAX ),
    /* all registers except EDX and EBX */
    HW_NotD_2( HW_EDX, HW_EBX )
};
:eXMP.
.pc
The following are to build registers dynamically.
.begnote $break $setptnt 2i
.notehd1 Macro
.notehd2 Usage
.note HW_CEqual( a, c )
Is :HP2.a:eHP2. equal to :HP2.c:eHP2.
.note HW_COvlap( a, c )
Does :HP2.a:eHP2. overlap with :HP2.c:eHP2.
.note HW_CSubset( a, c )
Is :HP2.a:eHP2. subset of :HP2.c:eHP2.
.note HW_CAsgn( dst, c )
Assign :HP2.c:eHP2. to :HP2.dst:eHP2.
.note HW_CTurnOn( dst, c )
Turn on registers :HP2.c:eHP2. in :HP2.dst:eHP2..
.note HW_CTurnOff( dst, c )
Turn off registers :HP2.c:eHP2. in :HP2.dst:eHP2..
.note HW_COnlyOn( a, c )
Turn off all registers except :HP2.c:eHP2. in :HP2.dst:eHP2..
.note HW_Equal( a, b )
Is :HP2.a:eHP2. equal to :HP2.b:eHP2.
.note HW_Ovlap( a, b )
Does :HP2.a:eHP2. overlap with :HP2.b:eHP2.
.note HW_Subset( a, b )
Is :HP2.a:eHP2. subset of :HP2.b:eHP2.
.note HW_Asgn( dst, b )
Assign :HP2.b:eHP2. to :HP2.dst:eHP2.
.note HW_TurnOn( dst, b )
Turn on registers :HP2.b:eHP2. in :HP2.dst:eHP2..
.note HW_TurnOff( dst, b )
Turn off registers :HP2.b:eHP2. in :HP2.dst:eHP2..
.note HW_OnlyOn( dst, b )
Turn off all registers except :HP2.b:eHP2. in :HP2.dst:eHP2..
.endnote
.np
The following example selects the low order 16 bits of any register.
that has a low part.
.np
:XMP.
hw_reg_set low16( hw_reg_set reg )
{
    hw_reg_set  low;

    HW_CAsgn( low, HW_EMPTY );
    HW_CTurnOn( low, HW_AX );
    HW_CTurnOn( low, HW_BX );
    HW_CTurnOn( low, HW_CX );
    HW_CTurnOn( low, HW_DX );
    if( HW_Ovlap( reg, low ) ) {
        HW_OnlyOn( reg, low );
    }
}
:eXMP.
.pc
The following register constants are defined for all targets.
.begnote
.note HW_EMPTY
The null register set.
.note HW_UNUSED
The set of unused register entries.
.note HW_FULL
All possible registers.
.endnote
.np
The following example yields the set of all valid machine registers.
.np
:XMP.
hw_reg_set reg;

HW_CAsgn( reg, HW_FULL );
HW_CTurnOff( reg, HW_UNUSED );
:eXMP.
.*
.chap Miscellaneous
.*
.np
I apologize for my lack of consistency in this document.
I use the terms function, routine, procedure interchangeably, as well
as index, subscript - select, switch - parameter, argument - etc.
I come from a multiple language background and will always be
hopelessly confused.
.np
The NEXT_IMPORT/NEXT_IMPORT_S/NEXT_LIBRARY are used as follows.
.np
:XMP.
handle = NULL;
for( ;; ) {
    handle = FEAuxInfo( handle, NEXT_IMPORT );
    if( handle == NULL )
        break;
    do_something( FEAuxInfo( handle, IMPORT_NAME ) );
}
:eXMP.
.pc
The FREE_SEGMENT request is used as follows.
.np
:XMP.
segment = 0;
for( ;; ) {
    segment = FEAuxInfo( segment, FREE_SEGMENT );
    if( segment == NULL )
        break;
    segment_size = *(short *)MK_FP( segment, 0 ) * 16;
    this_is_my_memory_now( MK_FP( segment, 0 ), segment_size );
}
:eXMP.
.pc
The main line in Pascal is defined to be lexical level 1.
Add 1 for each nested subroutine level.
C style routines are defined to be lexical level 0.
.np
The following types are defined by the code generator header files:
.ix types, predefined
.begnote
.notehd1 Utility type
.notehd2 Definition
.note bool
(unsigned char) 0 = false, non-0 = true.
.note byte
(unsigned char)
.note int_8
(signed char)
.note int_16
(signed short)
.note int_32
(signed long)
.note signed_8
(signed char)
.note signed_16
(signed short)
.note signed_32
(signed long)
.note uint
(unsigned)
.note uint_8
(unsigned char)
.note uint_16
(unsigned short)
.note uint_32
(unsigned long)
.note unsigned_8
(unsigned char)
.note unsigned_16
(unsigned short)
.note unsigned_32
(unsigned long)
.note real
(float)
.note reallong
(double)
.note pointer
(void*)
.endnote
.begnote
.notehd1 Type
.notehd2 Definition
.note aux_class
(enum) Passed as 2nd parameter to FEAuxInfo.
.note aux_handle
(void*) A handle used as 1st parameter to FEAuxInfo.
.note back_handle
(void*) A handle for a back end symbol table entry.
.note byte_seq
(struct) Passed to back end in response to CALL_BYTES FEAuxInfo
request.
.note call_class
(unsigned long) A set of combinable bits indicating the call
attributes for a routine.
.note call_handle
(void*) A handle to be used in CGInitCall, CGAddParm and CGCall.
.note cg_init_info
(union) The return value of BEInit.
.note cg_name
(void*) A handle for a back end expression tree node.
.note cg_op
(enum) An operator to be used in building expressions.
.note cg_switches
(unsigned_32) A set of combinable bits indicating the code
generator options.
.note cg_sym_handle
(uint) A handle for a front end symbol table entry.
.note cg_type
(unsigned short) A code generator type.
.note fe_attr
(enum) A set of combinable bits indicating symbol attributes.
.note hw_reg_set
(struct hw_reg_set) A structure representing a hardware register.
.note label_handle
(void*) A handle for a code generator code label.
.note linkage_regs
(struct) For 370 linkage conventions.
.note more_cg_types
(enum)
.note msg_class
(enum) The 1st parameter to FEMessage.
.note proc_revision
(enum) The 3rd parameter to BEInit.
.note seg_attr
(enum) A set of combinable bits indicate the attributes of a
segment.
.note segment_id
(int) A segment identifier.
.note sel_handle
(void*) A handle to be used in the CGSel calls.
.note temp_handle
(void*) A handle for a code generator temporary.
.endnote
.begnote
.notehd1 Misc Type
.notehd2 Definition
.note HWT
hw_reg_part
.note hw_reg_part
(unsigned)
.note dbg_enum
(void*)
.note dbg_ftn_type
(enum)
.note dbg_name
(void*)
.note dbg_proc
(void*)
.note dbg_struct
(void*)
.note dbg_type
(unsigned short)
.note predefined_cg_types
(enum)
.endnote
:APPENDIX.
.*
.chap Pre-defined macros
.*
.np
The following macros are defined by the code generator include files.
:ZSL.
:ZLI.HW_D
:ZLI.HW_D_1
:ZLI.HW_D_2
:ZLI.HW_D_3
:ZLI.HW_D_4
:ZLI.HW_D_5
:ZLI.BIG_CODE
:ZLI.BIG_DATA
:ZLI.CALLER_POPS
:ZLI.CHEAP_POINTER
:ZLI.CHEAP_WINDOWS
:ZLI.CONST_IN_CODE
:ZLI.CPU_MASK
:ZLI.C_FRONT_END
:ZLI.DBG_FWD_TYPE
:ZLI.DBG_LOCALS
:ZLI.DBG_NIL_TYPE
:ZLI.DBG_NUMBERS
:ZLI.DBG_TYPES
:ZLI.DLL_EXPORT
:ZLI.DO_FLOATING_FIXUPS
:ZLI.DO_SYM_FIXUPS
:ZLI.EMIT_FUNCTION_NAME
:ZLI.EPILOG_HOOKS
:ZLI.EZ_OMF
:ZLI.E_8087
:ZLI.FALSE
:ZLI.FAR
:ZLI.FAT_WINDOWS_PROLOG
:ZLI.FIX_SYM_OFFSET
:ZLI.FIX_SYM_RELOFF
:ZLI.FIX_SYM_SEGMENT
:ZLI.FLAT_MODEL
:ZLI.FLOATING_DS
:ZLI.FLOATING_ES
:ZLI.FLOATING_FIXUP_BYTE
:ZLI.FLOATING_FS
:ZLI.FLOATING_GS
:ZLI.FLOATING_SS
:ZLI.FORTRAN_ALIASING
:ZLI.FORTRAN_FRONT_END
:ZLI.FPU_MASK
:ZLI.FRONT_END_MASK
:ZLI.FUNCS_IN_OWN_SEGMENTS
:ZLI.GENERATE_STACK_FRAME
:ZLI.GET_CPU
:ZLI.GET_FPU
:ZLI.GET_WTK
:ZLI.GROW_STACK
:ZLI.HWREG_INCLUDED
:ZLI.HW_0
:ZLI.HW_1
:ZLI.HW_2
:ZLI.HW_3
:ZLI.HW_64
:ZLI.HW_Asgn
:ZLI.HW_CAsgn
:ZLI.HW_CEqual
:ZLI.HW_COMMA
:ZLI.HW_COnlyOn
:ZLI.HW_COvlap
:ZLI.HW_CSubset
:ZLI.HW_CTurnOff
:ZLI.HW_CTurnOn
:ZLI.HW_DEFINE_COMPOUND
:ZLI.HW_DEFINE_GLOBAL_CONST
:ZLI.HW_DEFINE_SIMPLE
:ZLI.HW_Equal
:ZLI.HW_ITER
:ZLI.HW_NotD
:ZLI.HW_NotD_1
:ZLI.HW_NotD_2
:ZLI.HW_NotD_3
:ZLI.HW_NotD_4
:ZLI.HW_NotD_5
:ZLI.HW_OnlyOn
:ZLI.HW_Op1
:ZLI.HW_Op2
:ZLI.HW_Op3
:ZLI.HW_Op4
:ZLI.HW_Op5
:ZLI.HW_Ovlap
:ZLI.HW_Subset
:ZLI.HW_TurnOff
:ZLI.HW_TurnOn
:ZLI.II_REVISION
:ZLI.INDEXED_GLOBALS
:ZLI.INS_SCHEDULING
:ZLI.INTERNAL_DBG_OUTPUT
:ZLI.INTERRUPT
:ZLI.I_MATH_INLINE
:ZLI.LAST_AUX_ATTRIBUTE
:ZLI.LAST_CGSWITCH
:ZLI.LAST_TARG_AUX_ATTRIBUTE
:ZLI.LAST_TARG_CGSWITCH
:ZLI.LOAD_DS_ON_CALL
:ZLI.LOAD_DS_ON_ENTRY
:ZLI.LOOP_OPTIMIZATION
:ZLI.MAKE_CALL_INLINE
:ZLI.MAX_POSSIBLE_REG
:ZLI.MIN_OP
:ZLI.MODIFY_EXACT
:ZLI.NEED_STACK_FRAME
:ZLI.NO_8087_RETURNS
:ZLI.NO_CALL_RET_TRANSFORM
:ZLI.NO_FLOAT_REG_RETURNS
:ZLI.NO_MEMORY_CHANGED
:ZLI.NO_MEMORY_READ
:ZLI.NO_OPTIMIZATION
:ZLI.NO_STRUCT_REG_RETURNS
:ZLI.NULL
:ZLI.NULLCHAR
:ZLI.O_FIRST_COND
:ZLI.O_FIRST_FLOW
:ZLI.O_LAST_COND
:ZLI.O_LAST_FLOW
:ZLI.PARMS_BY_ADDRESS
:ZLI.PROLOG_HOOKS
:ZLI.RELAX_ALIAS
:ZLI.REVERSE_PARMS
:ZLI.ROUTINE_RETURN
:ZLI.SEG_EXTRN_FAR
:ZLI.SET_CPU
:ZLI.SET_FPU
:ZLI.SET_WTK
:ZLI.SPECIAL_RETURN
:ZLI.SPECIAL_STRUCT_RETURN
:ZLI.STANDARD_INCLUDED
:ZLI.SUICIDAL
:ZLI.SYM_FIXUP_BYTE
:ZLI.THUNK_PROLOG
:ZLI.TRUE
:ZLI.TY_HUGE_CODE_PTR
:ZLI.USE_32
:ZLI.WINDOWS
:ZLI.WTK_MASK
:ZLI._AL
:ZLI._AX
:ZLI._BL
:ZLI._BP
:ZLI._BX
:ZLI._CG_H_INCLUDED
:ZLI._CL
:ZLI._CMS
:ZLI._CX
:ZLI._DI
:ZLI._DL
:ZLI._DX
:ZLI._HOST_INTEGER
:ZLI._OS
:ZLI._SI
:ZLI._TARG_AUX_SHIFT
:ZLI._TARG_CGSWITCH_SHIFT
:ZLI.far
:ZLI.huge
:ZLI.interrupt
:ZLI.near
:ZLI.offsetof
:ZeSL.
.*
.chap Register constants
.*
.np
The following register constants are defined for x86 targets.
:ZSL.
:ZLI.HW_AH
:ZLI.HW_AL
:ZLI.HW_BH
:ZLI.HW_BL
:ZLI.HW_CH
:ZLI.HW_CL
:ZLI.HW_DH
:ZLI.HW_DL
:ZLI.HW_SI
:ZLI.HW_DI
:ZLI.HW_BP
:ZLI.HW_SP
:ZLI.HW_DS
:ZLI.HW_ES
:ZLI.HW_CS
:ZLI.HW_SS
:ZLI.HW_ST0
:ZLI.HW_ST1
:ZLI.HW_ST2
:ZLI.HW_ST3
:ZLI.HW_ST4
:ZLI.HW_ST5
:ZLI.HW_ST6
:ZLI.HW_ST7
:ZLI.HW_FS
:ZLI.HW_GS
:ZLI.HW_AX
:ZLI.HW_BX
:ZLI.HW_CX
:ZLI.HW_DX
:ZLI.HW_EAX
:ZLI.HW_EBX
:ZLI.HW_ECX
:ZLI.HW_EDX
:ZLI.HW_ESI
:ZLI.HW_EDI
:ZLI.HW_ESP
:ZLI.HW_EBP
:ZeSL.
.np
The following registers are defined for the Alpha AXP target.
:ZSL.
:ZLI.HW_R0-HW_R31
:ZLI.HW_D0-HW_D31
:ZLI.HW_W0-HW_W31
:ZLI.HW_B0-HW_B31
:ZLI.HW_F0-HW_F31
:ZeSL.
.np
The following registers are defined for the PowerPC target.
:ZSL.
:ZLI.HW_R0-HW_R31
:ZLI.HW_Q3-HW_Q29
:ZLI.HW_D0-HW_D31
:ZLI.HW_W0-HW_W31
:ZLI.HW_B0-HW_B31
:ZLI.HW_F0-HW_F31
:ZeSL.
.np
The following registers are defined for the MIPS32 target.
:ZSL.
:ZLI.HW_R0-HW_R31
:ZLI.HW_Q2-HW_Q24
:ZLI.HW_D0-HW_D31
:ZLI.HW_W0-HW_W31
:ZLI.HW_B0-HW_B31
:ZLI.HW_F0-HW_F31
:ZLI.HW_FD0-HW_FD30
:ZeSL.
:CMT. S/370 support not maintained
.if 0 eq 1 .do begin
.np
The following registers are defined for the 370 target.
:ZSL.
:ZLI.HW_G0
:ZLI.HW_G1
:ZLI.HW_G2
:ZLI.HW_G3
:ZLI.HW_G4
:ZLI.HW_G5
:ZLI.HW_G6
:ZLI.HW_G7
:ZLI.HW_G8
:ZLI.HW_G9
:ZLI.HW_G10
:ZLI.HW_G11
:ZLI.HW_G12
:ZLI.HW_G13
:ZLI.HW_G14
:ZLI.HW_G15
:ZLI.HW_Y0
:ZLI.HW_Y2
:ZLI.HW_Y4
:ZLI.HW_Y6
:ZLI.HW_D0
:ZLI.HW_D2
:ZLI.HW_D4
:ZLI.HW_D6
:ZLI.HW_E0
:ZLI.HW_E4
:ZeSL.
.do end
.*
.chap Debugging Open Watcom Code Generator
.*
.np
If you want to use vc.dbg command, make sure you have a tmp directory in
root of used filesystem (see bld/cg/dumpio.c for details).
.np
:NOTE.
Make a s:\tmp to facilitate debugging in s:\brad :) Yeah, it's a cheap
and sleazy hack...
.np
If you need to dump something and don't know the routine to call,
try :HP2."e/s Dump":eHP2. and see what pops up...
.*
.section Instructions
.*
.np
You can get a dump of instructions for current function via :HP3.DumpRange:eHP3. anytime between :HP3.FixEdges:eHP3. and start of :HP3.GenObject:eHP3..
.np
You can dump an individual instruction via :HP3.DumpIns:eHP3.
.np
If you need live info for a basic block, find address and call :HP3.DumpABlk( block ):eHP3..
.*
.section Symbols
.*
If you need to see a list of symbols, use :HP3.DumpSymTab:eHP3..
To look at one symbol, use :HP3.DumpSym:eHP3..
.*
.section Tree Problems
.*
Find the line number of a piece of source near the problem.
Do a :HP2."bif { edx == LINENUMBER }:eHP2. :HP3.DBSrcCue:eHP3.:HP2.":eHP2. to stop near that
Go to :HP3.CGDone:eHP3. in order to see what resulting tree is (:HP3.DumpTree:eHP3.)
If there is a problem with tree, but not with API calls, do to :HP3.DBSrcCue:eHP3. as above and then break on next appropriate CG API call.
.*
.section Optimization Problems (Loopopts at all)
.*
Find the ordinal of the problem function in the file (ie 4th function)
Do a :HP2."bcnt 4:eHP2. :HP3.FixEdges:eHP3.:HP2.":eHP2. in order to stop on 4th call (for example) to :HP3.FixEdges:eHP3.
Dump instructions (using :HP3.DumpRange:eHP3.) and see if problem is in trees
If not, go to :HP3.RegAlloc:eHP3. and see if problem shows up yet
If so, binary search between :HP3.FixEdges:eHP3. and :HP3.RegAlloc:eHP3. to find optimization at fault.
.*
.section Instruction Select Problems
.*
Go to :HP3.RegAlloc:eHP3. for appropriate function (called once per function when not -od)
Find address of instruction which gets translated or handled improperly. (Look in results of :HP3.DumpRange:eHP3. for this address).
Do a :HP2."bif { eax == address }:eHP2. :HP3.ExpandIns:eHP3.:HP2.":eHP2. to look at what we do to this instruction (trace through).
.*
.section Register Allocation Problem
.*
.section Instruction Encoding Problem
.*
Go to :HP3.RegAlloc:eHP3. invocation for routine in question.
Go to :HP3.GenObject:eHP3. and call :HP3.DumpRange:eHP3..
Find address of instruction that gets encoded incorrectly, and do a :HP2."bif { eax == address }:eHP2. :HP3.GenObjCode:eHP3.:HP2.":eHP2.
Trace into :HP3.GenObjCode:eHP3. at appropriate time.
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.cd set 1
.cntents end_of_book
:eGDOC.
