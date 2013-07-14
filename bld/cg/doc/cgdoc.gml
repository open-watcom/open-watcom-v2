:INCLUDE file='LYTCHG'.
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
:INCLUDE file='WNOHELP'.
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
.chap General
:P.The code generator (back end) interface is a set of procedure calls.
These are divided into following category of routines.
:SL.
:LI.Code Generation (CG)
:LI.Data Generation (DG)
:LI.Miscellaneous Back End (BE)
:LI.Front end supplied (FE)
:LI.Debugger information (DB)
:eSL.
.section cg_init_info BEInit( cg_switches switches, cg_target_switches targ_switches, uint optsize, proc_revision proc )
:I1.BEInit
:P.Initialize the code generator.
This must be the first routine to be called.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.switches
:DD.Select code generation options.
The options are bits, so may be combined with the bit-wise operator |.
Options apply to the entire compilation unit.
The bit values are defined below.
:DT.targ_switches
:DD.Target specific switches.
The bit values are defined below.
:DT.optsize
:DD.A number between 0 and 100.
0 means optimize for speed, 100 means optimize for size.
Anything in between selects a compromise between speed and size.
:DT.proc
:DD.The target hardware configuration, defined below.
:eDL.
:DL.
:DT.Returns
:DD.Information about the code generator revision in a cg_init_info
structure, defined below.
:eDL.
:DL tsize='2i' break.
:I1.options
:DTHD.Switch
:DDHD.Definition
:DT.NO_OPTIMIZATION
:DD.Turn off optimizations.
:DT.NUMBERS
:DD.Generate line number debugging information.
:DT.FORTRAN_ALIASING
:DD.Assume pointers are only used for parameter passing.
:DT.DBG_DF
:DD.Generate debugging information in DWARF format.
:DT.DBG_CV
:DD.Generate debugging information in CodeView format. If neither
DBG_DF nor DBG_CV is set, debugging information (if any) is
generated in the Watcom format.
:DT.RELAX_ALIAS
:DD.Assume that a static/extern variable and a pointer to that same
variable are not used within the same routine.
:DT.DBG_LOCALS
:DD.Generate local symbol information for use by a debugger.
:DT.DBG_TYPES
:DD.Generate typing information for use by a debugger.
:DT.LOOP_UNROLLING
:DD.Turn on loop unrolling.
:DT.LOOP_OPTIMIZATION
:DD.Turn on loop optimizations.
:DT.INS_SCHEDULING
:DD.Turn on instruction scheduling.
:DT.MEMORY_LOW_FAILS
:DD.Allow the code generator to run out of memory without being able to
generate object code (allows the 386 compiler to use EBP as a cache
register).
:DT.FP_UNSTABLE_OPTIMIZATION
:DD.Allow the code generator to perform optimizations that are
mathematically correct, but are numerically unstable.
E.g.
converting division by a constant to a multiplication by the reciprocal.
:DT.NULL_DEREF_OK
:DD.NULL points to valid memory and may be dereferenced.
:DT.FPU_ROUNDING_INLINE
:DD.Inline floating-point value rounding (actually truncation) routine
when converting floating-point values to integers.
:DT.FPU_ROUNDING_OMIT
:DD.Omit floating-point value rounding entirely and use FPU default. Results
will not be ISO C compliant.
:DT.ECHO_API_CALLS
:DD.Log each call to the code generator with its arguments and return value.
Only available in debug builds.
:DT.OBJ_ELF
:DD.Emit ELF object files.
:DT.OBJ_COFF
:DD.Emit COFF object files. For Intel compilers, OMF object files will
be emitted in the absence of either switch.
:DT.OBJ_ENDIAN_BIG
:DD.Emit big-endian object files (COFF or ELF). If OBJ_ENDIAN_BIG is not
set, little-endian objects will be generated.
:eDL.
:DL tsize='2i' break.
:DTHD.x86 Switch
:DDHD.Definition
:DT.I_MATH_INLINE
:DD.Do not check arguments for operators like O_SQRT.
This allows the compiler to use some specialty x87 instructions.
:DT.EZ_OMF
:DD.Generate Phar Lap EZ-OMF object files.
:DT.BIG_DATA
:DD.Use segmented pointers (16:16 or 16:32).
This defines TY_POINTER to be equivalent to TY_HUGE_POINTER.
:DT.BIG_CODE
:DD.Use inter segment (far) call and return instructions.
:DT.CHEAP_POINTER
:DD.Assume far objects are addressable by one segment value.
This must be used in conjunction with BIG_DATA.
It defines TY_POINTER to be equivalent to TY_FAR_POINTER.
:DT.FLAT_MODEL
:DD.Assume all segment registers address the same base memory.
:DT.FLOATING_FS
:DD.Does FS float (or is it pegged to DGROUP).
:DT.FLOATING_GS
:DD.Does GS float (or is it pegged to DGROUP).
:DT.FLOATING_ES
:DD.Does ES float (or is it pegged to DGROUP).
:DT.FLOATING_SS
:DD.Does SS float (or is it pegged to DGROUP).
:DT.FLOATING_DS
:DD.Does DS float (or is it pegged to DGROUP).
:DT.USE_32
:DD.Generate code into a use32 segment (versus use16).
:DT.INDEXED_GLOBALS
:DD.Generate all global and static variable references as an offset
past EBX.
:DT.WINDOWS
:DD.Generate windows prolog/epilog sequences for all routines.
:DT.CHEAP_WINDOWS
:DD.Generate windows prolog/epilog sequences assuming that call backs
functions are defined as __export.
:DT.NO_CALL_RET_TRANSFORM
:DD.Do not change a CALL followed by a RET into a JMP.
This is used for some older overlay managers that cannot handle a JMP
to an overlay.
:DT.CONST_IN_CODE
:DD.Generate all constant data into the code segment.
This only applies to the internal code generator data, such as floating
point constants.
The front end decides where its data goes using BESetSeg().
:DT.NEED_STACK_FRAME
:DD.Generate a traceable stack frame.
The first instructions will be :HP2.INC BP:eHP2. if the routine uses a
far return instruction, followed by :HP2.PUSH BP:eHP2. and :HP2.MOV
BP,SP:eHP2..
(ESP and EBP for 386 targets).
:DT.LOAD_DS_DIRECTLY
:DD.Generate code to load DS directly. By default, a call to __GETDS routine
is generated.
:DT.GEN_FWAIT_386
:DD.Generate FWAIT instructions on 386 and later CPUs. The 386 never needs
FWAIT for data synchronization, but FWAIT may still be needed for accurate
exception reporting.
:eDL.
:DL tsize='2i'.
:DTHD.RISC Switch
:DDHD.Definition
:DT.ASM_OUTPUT
:DD.Print final pseudo-assembly on the console. Debug builds only.
:DT.OWL_LOGGING
:DD.Log calls to the Object Writer Library
:DT.STACK_INIT
:DD.Pre-initialize stack variables to a known bit pattern.
:DT.EXCEPT_FILTER_USED
:DD.Set when SEH (Structured Exception Handling) is used.
:eDL
:CMT. S/370 not maintained
.if 0 eq 1 .do begin
:DL tsize='2i'.
:DTHD.370 Switch
:DDHD.Definition
:DT.BIG_DATA
:DD.???
:DT.BIG_CODE
:DD.???
:DT.FLOATING_SS
:DD.???
:DT.ASM_OUTPUT
:DD.???
:DT.CHEAP_POINTER
:DD.???
:DT.AMODE_24
:DD.???
:DT.AMODE_31
:DD.???
:DT.RMODE_24
:DD.???
:DT.CODE_RENT
:DD.???
:DT.CODE_SPLIT
:DD.???
:DT.STACK_CHECK
:DD.???
:DT.ASM_SOURCE
:DD.???
:DT.ASM_LISTING
:DD.???
:DT.NO_ZERO_INIT
:DD.???
:DT.I_MATH_INLINE
:DD.???
:eDL.
.do end
:I1.options
:P.The supported proc_revision CPU values are:
:SL.
:LI.CPU_86
:LI.CPU_186
:LI.CPU_286
:LI.CPU_386
:LI.CPU_486
:LI.CPU_586
:eSL.
:P.The supported proc_revision FPU values are:
:SL.
:LI.FPU_NONE
:LI.FPU_87
:LI.FPU_387
:LI.FPU_586
:LI.FPU_EMU
:LI.FPU_E87
:LI.FPU_E387
:LI.FPU_E586
:eSL.
:P.The supported proc_revision WEITEK values are:
:SL.
:LI.WTK_NONE
:LI.WTK_1167
:LI.WTK_3167
:LI.WTK_4167
:eSL.
:P.The following example sets the processor revision information to
indicate a 386 with 387 and Weitek 3167.
:XMP.
proc_revision proc;

SET_CPU( p, CPU_386 );
SET_FPU( p, FPU_387 );
SET_WTK( p, WTK_3167 );
:eXMP.
:P.The return value structure is defined as follows:
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
.section void BEStart()
:I1.BEStart
:P.Start the code generator.
Must be called immediately after all calls to BEDefSeg have been made.
This restriction is relaxed somewhat for the 80(x)86 code generator.
See BEDefSeg for details.
.section void BEStop()
:I1.BEStop
:P.Normal termination of code generator.
This must be the second last routine called.
.section void BEAbort()
:I1.BEAbort
:P.Abnormal termination of code generator.
This must be the second last routine called.
.section void BEFini()
:I1.BEFini
:P.Finalize the code generator.
This must be the last routine called.
.section patch_handle BEPatch()
:P.Allocate a patch handle which can be used to create a patchable
integer (an integer which will have a constant value provided sometime
while the codegen is handling the CGDone call).
See CGPatchNode.
.section void BEPatchInteger( patch_handle hdl, signed_32 value )
:P.Patch the integer corresponding to the given handle to have the
given value.
This may be called repeatedly with different values, providing
CGPatchNode has been called and BEFiniPatch has not been called.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.hdl
:DD.A patch_handle returned from an earlier invocation of BEPatch which
has had a node allocated for it via CGPatchNode.
If CGPatchNode has not been called with the handle given, the behaviour
is undefined.
:DT.value
:DD.A signed 32-bit integer value.
This will be the new value of the node which has been associated with
the patch handle.
:eDL.
.section cg_name BEFiniPatch( patch_handle hdl )
:P.This must be called to free up resources used by the given handle.
After this, the handle must not be used again.
.chap Segments
:P.The object file produced by the code generator is composed of
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
:P.The following routines are used for initializing, finalizing,
defining and selecting segments.
.section void BEDefSeg( segment_id id, seg_attr attr, char *str, uint algn )
:I1.BEDefSeg
:I1.segments
:P.Define a segment.
This must be called after BEInit and before BEStart.
For the 80(x)86 code generator, you are allowed to define additional
segments after BEStart if they are:
:OL compact.
:LI.Code Segments
:LI.PRIVATE data segments.
:eOL.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.id
:DD.A non-negative integer used as an identifier for the segment.
It is arbitrarily picked by the front end.
:DT.attr
:DD.Segment attribute bits, defined below.
:DT.str
:DD.The name given to the segment.
:DT.algn
:DD.The segment alignment requirements.
The code generator will pick the next larger alignment allowed by the
object module format.
For example, 9 would select paragraph alignment.
:eDL.
:DL.
:DTHD.Attribute
:DDHD.Definition
:DT.EXEC
:DD.This is a code segment.
:DT.GLOBAL
:DD.The segment is accessible to other modules.
(versus PRIVATE).
:DT.INIT
:DD.The segment is statically initialized.
:DT.ROM
:DD.The segment is read only.
:DT.BACK
:DD.The code generator may put its data here.
One segment must be marked with this attribute.
It may not be a COMMON, PRIVATE or EXEC segment.
If the front end requires code in the EXEC segment, the CONST_IN_CODE
switch must be passed to BEInit().
:DT.COMMON
:DD.All occurrences of this segment will be overlayed.
This is used for FORTRAN common blocks.
:DT.PRIVATE
:DD.The segment is non combinable.
This is used for far data items.
:DT.GIVEN_NAME
:DD.Normally, the back end feels free to prepend or append strings to
the segment name passed in by the front end.
This allows a naive front end to specify a constant set of segment
names, and have the code generator mangle them in such a manner that
they work properly in concert with the set of cg_switches that have
been specified (e.g.
prepending the module name to the code segments when BIG_CODE is
specified on the x86).
When GIVEN_NAME is specified, the back end outputs the segment name to
the object file exactly as given.
:DT.NOGROUP
:DD.Segment will not be part of DGROUP (if it is a data segment). Private
segments are never part of DGROUP, but sometimes it is useful to have
segments that are combinable with others but can be physically separate.
:DT.THREAD_LOCAL
:DD.Segment contains thread local data. Such segments may need special
handling in executable modules.
:eDL.
.section segment_id BESetSeg( segment_id seg )
:I1.BESetSeg
:I1.segments
:P.Select the current segment for data generation routines.
Code for a routine is always output into the segment returned by
FESegID when it is passed the routine symbol handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.seg
:DD.Selects the current segment.
:eDL.
:DL.
:DT.Returns
:DD.The previous current segment.
:eDL.
:NOTE.
When emitting data into an EXEC or BACK segment, be aware that
the code generator is at liberty to emit code and/or back end data into
that segment anytime you make a call to a code generation routine
(CG*).
Do NOT expect data items to be contiguous in the segment if you have
made an intervening CG* call.
.section void BEFlushSeg( segment_id seg )
:I1.BEFlushSeg
:I1.segments
:P.BEFlushSeg informs the back end that no more code/data will be
generated in the specified segment.
For code segments, it must be called after the CGReturn() for the final
function which is placed in the segment.
This causes the code generator to flush all pending information
associated with the segment and allows the front end to free all the
back handles for symbols which were referenced by the code going into
the segment.
(The FORTRAN compiler uses this since each function has its own symbol
table which is thrown out at the end of the function).
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.seg
:DD.The code segment id.
:eDL.
.chap Labels
:I1.label, code
:P.The back end uses a :HP2.label_handle:eHP2. for flow of control.
Each :HP2.label_handle:eHP2. is a unique code label.
These labels may only be used for flow of control.
In order to define a label in a data segment, a :HP2.back_handle:eHP2.
must be used.
.section label_handle BENewLabel()
:I1.BENewLabel
:I1.label, code
:P.Allocate a new control flow label.
:DL.
:DT.Returns
:DD.A new label_handle.
:eDL.
.section void BEFiniLabel( label_handle lbl )
:I1.BEFiniLabel
:I1.label, code
:P.Indicate that a label_handle will not be used by the front end
anymore.
This allows the back end to free some memory at some later stage.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.lbl
:DD.A label_handle
:eDL.
.chap *refid=bck Back Handles
:I1.back handle
:I1.label, data
:P.A :HP2.back_handle:eHP2. is the front end's handle for a code
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
:XMP.
back_handle FEBack( SYMPOINTER sym )
{
    if( sym->back == NULL ) {
        sym->back = BENewBack( sym );
    }
    return( sym->back );
}
:eXMP.
:P.It is the responsibility of the front end to free each back_handle,
via BEFreeBack, when it frees the corresponding cg_sym_handle entry.
:P.A back_handle for a symbol having automatic or register storage
duration (auto back_handle) may not be freed until CGReturn is called.
A back_handle for a symbol having static storage duration, (static
back_handle) may not be freed until BEStop is called or until after a
BEFlushSeg is done for a segment and the back_handle will never be
referenced by any other function.
:P.The code generator will not require a back handle for symbols which
are not defined in the current compilation unit.
:P.The front end must define the location of all symbols with static
storage duration by passing the appropriate back_handle to DGLabel.
It must also reserve the correct amount of space for that variable
using DGBytes or DGUBytes.
:P.The front end may also allocate an back_handle with static storage
duration that has no cg_sym_handle associated with it (anonymous
back_handle) by calling BENewBack(NULL).
These are useful for literal strings.
These must also be freed after calling BEStop.
.section back_handle BENewBack( cg_sym_handle sym )
:I1.BENewBack
:I1.back handle
:P.Allocate a new back_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.The front end symbol handle to be associated with the back_handle.
It may be NULL.
:eDL.
:DL.
:DT.Returns
:DD.A new back_handle.
:eDL.
.section void BEFiniBack( back_handle bck )
:I1.BEFiniBack
:I1.back handle
:P.Indicate that :HP2.bck:eHP2. will never be passed to the back end
again, except to BEFreeBack.
This allows the code generator to free some memory at some later stage.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.bck
:DD.A back_handle.
:eDL.
.section void BEFreeBack( back_handle bck )
:I1.BEFreeBack
:I1.back handle
:P.Free the back_handle :HP2.bck:eHP2..
See the preamble in this section for restrictions on freeing a
back_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.bck
:DD.A back_handle.
:eDL.
.chap Type definitions
:I1.typing
:P.Base types are defined as constants.
All other types (structures, arrays, unions, etc) are simply defined by
their length.
The base types are:
:DL tsize='1.5i'.
:DTHD.Type
:DDHD.C type
:DT.TY_UINT_1
:DD.unsigned char
:DT.TY_INT_1
:DD.signed char
:DT.TY_UINT_2
:DD.unsigned short
:DT.TY_INT_2
:DD.signed short
:DT.TY_UINT_4
:DD.unsigned long
:DT.TY_INT_4
:DD.signed long
:DT.TY_UINT_8
:DD.unsigned long long
:DT.TY_INT_8
:DD.signed long long
:DT.TY_LONG_POINTER
:DD.far *
:DT.TY_HUGE_POINTER
:DD.huge *
:DT.TY_NEAR_POINTER
:DD.near *
:DT.TY_LONG_CODE_PTR
:DD.(far *)()
:DT.TY_NEAR_CODE_PTR
:DD.(near *)()
:DT.TY_SINGLE
:DD.float
:DT.TY_DOUBLE
:DD.double
:DT.TY_LONG_DOUBLE
:DD.long double
:DT.TY_INTEGER
:DD.int
:DT.TY_UNSIGNED
:DD.unsigned int
:DT.TY_POINTER
:DD.*
:DT.TY_CODE_PTR
:DD.(*)()
:DT.TY_BOOLEAN
:DD.The result of a comparison or flow operator.
May also be used as an integer.
:DT.TY_DEFAULT
:DD.Used to indicate default conversion
:DT.TY_NEAR_INTEGER
:DD.The result of subtracting 2 near pointers
:DT.TY_LONG_INTEGER
:DD.The result of subtracting 2 far pointers
:DT.TY_HUGE_INTEGER
:DD.The result of subtracting 2 huge pointers
:eDL.
There are two special constants.
:DL.
:DT.TY_FIRST_FREE
:DD.The first user definable type
:DT.TY_LAST_FREE
:DD.The last user definable type.
:eDL.
.section void BEDefType( cg_type what, uint align, unsigned_32 len )
:I1.BEDefType
:I1.typing
:P.Define a new type to the code generator.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.what
:DD.An integral value greater than or equal to TY_FIRST_FREE and less
then or equal to TY_LAST_FREE, used as the type identifier.
:DT.align
:DD.Currently ignored.
:DT.len
:DD.The length of the new type.
:eDL.
.section void BEAliasType( cg_type what, cg_type to )
:I1.BEAliasType
:I1.typing
:P.Define a type to be an alias for an existing type.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.what
:DD.Will become an alias for an existing type.
:DT.to
:DD.An existing type.
:eDL.
.section unsigned_32 BETypeLength( cg_type type )
:I1.BETypeLength
:I1.typing
:P.Return the length of a previously defined type, or a base type.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.type
:DD.A previously defined type.
:eDL.
:DL.
:DT.Returns
:DD.The length associated with the type.
:eDL.
.section uint BETypeAlign( cg_type type )
:I1.BETypeAlign
:I1.typing
:P.Return the alignment requirements of a type.
This is always 1 for x86 and 370 machines.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.type
:DD.A previously defined type.
:eDL.
:DL.
:DT.Returns
:DD.The alignment requirements of :HP2.type:eHP2. as declared in
BEDefType, or for a base type, as defined by the machine architecture.
:eDL.
.chap Procedure Declarations
.section void CGProcDecl( cg_sym_handle name, cg_type type )
:I1.CGProcDecl
:I1.procedures
:I1.routines
:I1.functions
:P.Declare a new procedure.
This must be the first routine to be called when generating each
procedure.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The front end symbol table entry for the procedure.
A back_handle will be requested.
:DT.type
:DD.The return type of the procedure.
Use TY_INTEGER for void functions.
:eDL.
.section void CGParmDecl( cg_sym_handle name, cg_type type )
:I1.CGParmDecl
:I1.procedures
:I1.routines
:I1.functions
:P.Declare a new parameter to the current function.
The calls to this function define the order of the parameters.
This function must be called immediately after calling CGProcDecl.
Parameters are defined in left to right order, as defined by the
procedure prototype.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The symbol table entry for the parameter.
:DT.type
:DD.The type of the parameter.
:eDL.
.section label_handle CGLastParm()
:I1.CGLastParm
:P.End a parameter declaration section.
This function must be called after the last parameter has been
declared.
Prior to this function, the only calls the front-end is allowed to make
are CGParmDecl and CGAutoDecl.
.section void CGAutoDecl( cg_sym_handle name, cg_type type )
:I1.CGAutoDecl
:P.Declare an automatic variable.
:P.This routine may be called at any point in the generation of a
function between the calls to CGProcDecl and CGReturn, but must be
called before :HP2.name:eHP2. is passed to CGFEName.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The symbol table entry for the variable.
:DT.type
:DD.The type of the variable.
:eDL.
.section temp_handle CGTemp( cg_type type )
:I1.CGTemp
:I1.Variables
:I1.Temporaries
:P.Yields a temporary with procedure scope.
This can be used for things such as iteration counts for FORTRAN do
loops, or a variable in which to store the return value of a function.
This routine should be used :HP2.only if necessary:eHP2..
It should be used when the front end requires a temporary which
persists across a flow of control boundary.
Other temporary results are handled by the expression trees.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.type
:DD.The type of the new temporary.
:eDL.
:DL.
:DT.Returns
:DD.A temp_handle which may be passed to CGTempName.
This will be freed and invalidated by the back end when CGReturn is
called.
:eDL.
.chap Expressions
:I1.expressions
:P.Expression processing involves building an expression tree in the
back end, using calls to CG routines.
There are routines to generate leaf nodes, binary and unary nodes, and
others.
These routines return a handle for a node in a back end tree structure,
called a :HP2.cg_name:eHP2..
This handle must be exactly once in a subsequent call to a CG routine.
A tree may be built in any order, but a cg_name is invalidated by a
call to any CG routine with return type void.
The exception to this rule is CGTrash.
:P.There is no equivalent of the C address of operator.
All leaf nodes generated for symbols, via CGFEName, CGBackName and
CGTempName, yield the address of that symbol, and it is the
responsibility of the front end to use an indirection operator to get
its value.
The following operators are available:
:I1.operators
:DL tsize='1.5i'.
:DTHD.0-ary Operator
:DDHD.C equivalent
:DT.O_NOP
:DD.N/A
:eDL.
:DL tsize='1.5i'.
:DTHD.Unary Operator
:DDHD.C equivalent
:DT.O_UMINUS
:DD.-x
:DT.O_COMPLEMENT
:DD.~x
:DT.O_POINTS
:DD.(*x)
:DT.O_CONVERT
:DD.x=y
:DT.O_ROUND
:DD.Do not use!
:DT.O_LOG
:DD.log(x)
:DT.O_COS
:DD.cos(x)
:DT.O_SIN
:DD.sin(x)
:DT.O_TAN
:DD.tan(x)
:DT.O_SQRT
:DD.sqrt(x)
:DT.O_FABS
:DD.fabs(x)
:DT.O_ACOS
:DD.acos(x)
:DT.O_ASIN
:DD.asin(x)
:DT.O_ATAN
:DD.atan(x)
:DT.O_COSH
:DD.cosh(x)
:DT.O_SINH
:DD.sinh(x)
:DT.O_TANH
:DD.tanh(x)
:DT.O_EXP
:DD.exp(x)
:DT.O_LOG10
:DD.log10(x)
:DT.O_PARENTHESIS
:DD.This operator represents the "strong" parenthesises of FORTRAN and
C.
It prevents the back end from performing certain mathematically
correct, but floating point incorrect optimizations.
E.g.
in the expression "(a*2.4)/2.0", the back end is not allowed constant
fold the expression into "a*1.2".
:eDL.
:DL tsize='1.5i'.
:DTHD.Binary Operator
:DDHD.C equivalent
:DT.O_PLUS
:DD.+
:DT.O_MINUS
:DD.-
:DT.O_TIMES
:DD.*
:DT.O_DIV
:DD./
:DT.O_MOD
:DD.%
:DT.O_AND
:DD.&
:DT.O_OR
:DD.|
:DT.O_XOR
:DD.^
:DT.O_RSHIFT
:DD.>>
:DT.O_LSHIFT
:DD.<<
:DT.O_COMMA
:DD.,
:DT.O_TEST_TRUE
:DD.( x & y ) != 0
:DT.O_TEST_FALSE
:DD.( x & y ) == 0
:DT.O_EQ
:DD.==
:DT.O_NE
:DD.!=
:DT.O_GT
:DD.>
:DT.O_LE
:DD.<=
:DT.O_LT
:DD.<
:DT.O_GE
:DD.>=
:DT.O_POW
:DD.pow( x, y )
:DT.O_ATAN2
:DD.atan2( x, y )
:DT.O_FMOD
:DD.fmod( x, y )
:DT.O_CONVERT
:DD.See below.
:eDL.
:P.The binary O_CONVERT operator is only available on the x86 code
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
:DL tsize='1.5i'.
:DTHD.Short circuit operators
:DDHD.C equivalent
:DT.O_FLOW_AND
:DD.&&
:DT.O_FLOW_OR
:DD.||
:DT.O_FLOW_NOT
:DD.!
:eDL.
:DL tsize='1.5i'.
:DTHD.Control flow operators
:DDHD.C equivalent
:DT.O_GOTO
:DD.goto label;
:DT.O_LABEL
:DD.label:;
:DT.O_IF_TRUE
:DD.if( x ) goto label;
:DT.O_IF_FALSE
:DD.if( !(x) ) goto label;
:DT.O_INVOKE_LABEL
:DD.GOSUB (Basic)
:DT.O_LABEL_RETURN
:DD.RETURN (Basic)
:eDL.
:I1.typing
:I1.conversions
:P.The type passed into a CG routine is used by the back end as the
type for the resulting node.
If the node is an operator node (CGBinary, CGUnary) the back end will
convert the operands to the result type before performing the
operation.
If the type TY_DEFAULT is passed, the code generator will use default
conversion rules to determine the resulting type of the node.
These rules are the same as the ANSI C value preserving rules, with the
exception that characters are not promoted to integers before doing
arithmetic operations.
:P.For example, if a node of type TY_UINT_2 and a node of type TY_INT_4
are to be added, the back end will automatically convert the operands
to TY_INT_4 before performing the addition.
The resulting node will have type TY_INT_4.
.chap Leaf Nodes
.section cg_name CGInteger( signed_32 val, cg_type type )
:I1.CGInteger
:I1.integers
:P.Create an integer constant leaf node.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.val
:DD.The integral value.
:DT.type
:DD.An integral type.
:eDL.
.section cg_name CGInt64( signed_64 val, cg_type type )
:I1.CGInt64
:I1.integers
:P.Create an 64-bit integer constant leaf node.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.val
:DD.The 64-bit integer value.
:DT.type
:DD.An integral type.
:eDL.
.section cg_name CGFloat( char *num, cg_type type )
:I1.CGFloat
:I1.floating point constant
:P.Create a floating-point constant leaf node.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.num
:DD.A NULL terminated E format string.
(-1.23456E-102)
:DT.type
:DD.A floating point type.
:eDL.
.section cg_name CGFEName( cg_sym_handle sym, cg_type type )
:I1.CGFEName
:I1.variables
:P.Create a leaf node representing the address of the back_handle
associated with :HP2.sym:eHP2..
If sym represents an automatic variable or a parameter, CGAutoDecl or
CGParmDecl must be called before this routine is first used.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.The front end symbol.
:DT.type
:DD.The type to be associated with the value of the symbol.
:eDL.
.section cg_name CGBackName( back_handle bck, cg_type type )
:I1.CGBackName
:I1.variables
:I1.label, data
:P.Create a leaf node which represents the address of the back_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.bck
:DD.A back handle.
:DT.type
:DD.The type to be associated with the :HP2.value:eHP2. of the symbol.
:eDL.
.section cg_name CGTempName( temp_handle temp, cg_type type )
:I1.CGTempName
:I1.temporaries
:P.Create a leaf node which yields the address of the temp_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.temp
:DD.A temp_handle.
:DT.type
:DD.The type to be associated with the :HP2.value:eHP2. of the symbol.
:eDL.

.chap Assignment Operations
.section cg_name CGAssign( cg_name dest, cg_name src, cg_type type )
:I1.CGAssign
:I1.assignment
:P.Create an assignment node.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.dest
:DD.The destination address.
:DT.src
:DD.The source value.
:DT.type
:DD.The type to which the destination address points.
:eDL.
:DL.
:DT.Returns
:DD.The value of the right hand side.
:eDL.
.section cg_name CGLVAssign( cg_name dest, cg_name src, cg_type type )
:I1.CGLVAssign
:I1.assignment
:P.Like CGAssign, but yields the address of the destination.
.section cg_name CGPreGets( cg_op op, cg_name dest, cg_name src, cg_type type )
:I1.CGPreGets
:I1.assignment
:P.Used for the C expressions a += b, a /= b.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.The arithmetic operator to be used.
:DT.dest
:DD.The address of the destination.
:DT.src
:DD.The value of the right hand side.
:DT.type
:DD.The type to which the destination address points.
:eDL.
:DL.
:DT.Returns
:DD.The value of the left hand side.
:eDL.
.section cg_name CGLVPreGets( cg_op op, cg_name dest, cg_name src, cg_type type )
:I1.CGLVPreGets
:I1.assignment
:P.Like CGPreGets, but yields the address of the destination.
.section cg_name CGPostGets( cg_op op, cg_name dest, cg_name src, cg_type type )
:I1.CGPostGets
:I1.assignment
:P.Used for the C expressions a++, a--.
No automatic scaling is done for pointers.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.The operator.
:DT.dest
:DD.The address of the destination
:DT.src
:DD.The value of the increment.
:DT.type
:DD.The type of the destination.
:eDL.
:DL.
:DT.Returns
:DD.The value of the left hand side before the operation occurs.
:eDL.
.chap Arithmetic/logical operations
.section cg_name CGBinary( cg_op op, cg_name left, cg_name right, cg_type type )
:I1.CGBinary
:I1.expressions
:P.Binary operations.
No automatic scaling is done for pointer operations.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.The operator.
:DT.left
:DD.The value of the left hand side.
:DT.right
:DD.The value of the right hand side.
:DT.type
:DD.The result type.
:eDL.
:DL.
:DT.Returns
:DD.The value of the result.
:eDL.
.section cg_name CGUnary( cg_op op, cg_name name, cg_type type )
:I1.CGUnary
:I1.expressions
:P.Unary operations.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.The operator.
:DT.name
:DD.The value of operand.
:DT.type
:DD.The result type.
:eDL.
:DL.
:DT.Returns
:DD.The value of the result.
:eDL.
.section cg_name CGIndex( cg_name name, cg_name by, cg_type type, cg_type ptype )
:I1.CGIndex
:P.Obsolete.
Do not use.
.chap Procedure calls
.section call_handle CGInitCall( cg_name name, cg_type type, cg_sym_handle aux_info )
:I1.CGInitCall
:I1.procedures
:I1.Routines
:I1.functions
:P.Initiate a procedure call.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The address of the routine to call.
:DT.type
:DD.The return type of the routine.
:DT.aux_info
:DD.A handle which the back end may passed to FEAuxInfo to determine
the attributes of the call.
:eDL.
:DL.
:DT.Returns
:DD.A :HP2.call_handle:eHP2. to be passed to the following routines.
:eDL.
.section void CGAddParm( call_handle call, cg_name name, cg_type type )
:I1.CGAddParm
:I1.procedures
:I1.routines
:I1.functions
:P.Add a parameter to a call_handle.
The order of parameters is defined by the order in which they are
passed to this routine.
Parameters should be added in right to left order, as defined by the
procedure call.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.call
:DD.A call_handle.
:DT.name
:DD.The value of the parameter.
:DT.type
:DD.The type of the parameter.
This type will be passed to FEParmType to determine the actual type to
be used when passing the parameter.
For instance, characters are usually passes as integers in C.
:eDL.
.section cg_name CGCall( call_handle call )
:P.Turn a call_handle into a cg_name by performing the call.
This may be immediately followed by an optional addition operation, to
reference a field in a structure return value.
An indirection operator must immediately follow, even if the function
has no return value.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.call
:DD.A call_handle.
:eDL.
:DL.
:DT.Returns
:DD.The address of the function return value.
:eDL.
.chap Comparison/short-circuit operations
.section cg_name CGCompare( cg_op op, cg_name left, cg_name right, cg_type type )
:I1.CGCompare
:I1.short circuit operations.
:I1.boolean expresssions.
:P.Compare two values.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.The comparison operator.
:DT.left
:DD.The value of the left hand side.
:DT.right
:DD.The value of the right hand side.
:DT.type
:DD.The type to which to convert the operands to before performing
comparison.
:eDL.
:DL.
:DT.Returns
:DD.A TY_BOOLEAN cg_name, which may be passed to a control flow CG
routine, or used in an expression as an integral value.
:eDL.
.chap Control flow operations
.section cg_name CGFlow( cg_op op, cg_name left, cg_name right )
:I1.short circuit operations.
:I1.boolean expresssions.
:I1.CGFlow
:P.Perform short-circuit boolean operations.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.An operator.
:DT.left
:DD.A TY_BOOLEAN or integral cg_name.
:DT.right
:DD.A TY_BOOLEAN or integral cg_name, or NULL if op is O_FLOW_NOT.
:eDL.
:DL.
:DT.Returns
:DD.A TY_BOOLEAN cg_name.
:eDL.
.section cg_name CGChoose( cg_name sel, cg_name n1, cg_name n2, cg_type type )
:I1.CGChoose
:I1.short circuit operations.
:I1.boolean expresssions.
:P.Used for the C expression :HP2.sel:eHP2. ?
:HP2.n1:eHP2. : :HP2.n2:eHP2..
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sel
:DD.A TY_BOOLEAN or integral cg_name used as the selector.
:DT.n1
:DD.The value to return if :HP2.sel:eHP2. is non-zero.
:DT.n2
:DD.The value to return if :HP2.sel:eHP2. is zero.
:DT.type
:DD.The type to which convert the result.
:eDL.
:DL.
:DT.Returns
:DD.The value of :HP2.n1:eHP2. or :HP2.n2:eHP2. depending upon the
truth of :HP2.sel:eHP2..
:eDL.
.section cg_name CGWarp( cg_name before, label_handle label, cg_name after )
:I1.CGWarp
:I1.statement functions
:I1.FORTRAN
:P.To be used for FORTRAN statement functions.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.before
:DD.An arbitrary expression tree to be evaluated before
:HP2.label:eHP2. is called.
This is used to assign values to statement function arguments, which
are usually temporaries allocated with CGTemp.
:DT.label
:DD.A label_handle to invoke via O_CALL_LABEL.
:DT.after
:DD.An arbitrary expression tree to be evaluated after :HP2.label:eHP2.
is called.
This is used to retrieve the statement function return value.
:eDL.
:DL.
:DT.Returns
:DD.The value of :HP2.after:eHP2..
This can be passed to CGEval, to guarantee that nested statement
functions are fully evaluated before their parameter variables are
reassigned, as in f(1,f(2,3,4),5).
:eDL.
.section void CG3WayControl( cg_name expr, label_handle lt, label_handle eq, label_handle gt )
:I1.CG3WayControl
:I1.arithmetic if
:I1.FORTRAN
:P.Used for the FORTRAN arithmetic if statement.
Go to label :HP2.lt:eHP2., :HP2.eq:eHP2. or :HP2.gt:eHP2. depending on
whether :HP2.expr:eHP2. is less than, equal to, or greater than zero.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.expr
:DD.The selector value.
:DT.lt
:DD.A label_handle.
:DT.eq
:DD.A label_handle.
:DT.gt
:DD.A label_handle.
:eDL.
.section void CGControl( cg_op op, cg_name expr, label_handle lbl )
:I1.CGControl
:I1.control flow
:P.Generate conditional and unconditional flow of control.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.op
:DD.a control flow operator.
:DT.expr
:DD.A TY_BOOLEAN expression if op is O_IF_TRUE or O_IF_FALSE.
NULL otherwise.
:DT.lbl
:DD.The target label.
:eDL.
.section void CGBigLabel( back_handle lbl )
:I1.CGBigLabel
:I1.control flow
:P.Generate a label which may be branched to from a nested procedure or
used in NT structured exception handling.
Don't use this call unless you *really*, *really* need to.
It kills a lot of optimizations.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.lbl
:DD.A back_handle.
There must be a front end symbol associated with this back handle.
:eDL.
.section void CGBigGoto( back_handle value, int level )
:I1.CGBigGoto
:I1.control flow
:P.Generate a branch to a label in an outer procedure.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.lbl
:DD.A back_handle.
There must be a front end symbol associated with this back handle.
:DT.level
:DD.The lexical level of the target label.
:eDL.
.chap Select and Switch statements.
:P.The select routines are used as follows.
CGSelOther should always be used even if there is no otherwise/default
case.
:XMP.
end_label = BENewLabel();

sel_label = BENewLabel();
CGControl( O_GOTO, NULL, sel_label );
sel_handle = CGSelInit();

case_label = BENewLabel();
CGControl( O_LABEL, NULL, case_label );
CGSelCase( sel_handle, case_label, case_value );

   ... generate code associated with "case_value"  here.

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
.section sel_handle CGSelInit()
:I1.CGSelInit
:I1.control flow
:P.Create a sel_handle.
:DL.
:DT.Returns
:DD.A sel_handle to be passed to the following routines.
:eDL.
.section void CGSelCase( sel_handle sel, label_handle lbl, signed_32 val )
:I1.CGSelCase
:I1.control flow
:P.Add a single value case to a select or switch.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sel
:DD.A sel_handle obtained from CGSelInit().
:DT.lbl
:DD.The label to be associated with the case value.
:DT.val
:DD.The case value.
:eDL.
.section void CGSelRange( sel_handle s, signed_32 lo, signed_32 hi, label_handle lbl )
:I1.CGSelRange
:I1.control flow
:P.Add a range of values to a select.
All values are eventually converted into unsigned types to generate the
switch code, so lo and hi must have the same sign.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.s
:DD.A sel_handle obtained from CGSelInit().
:DT.lo
:DD.The lower bound of the case range.
:DT.hi
:DD.The upper bound of the case range.
:DT.lbl
:DD.The label to be associated with the case value.
:eDL.
.section void CGSelOther( sel_handle s, label_handle lbl )
:I1.CGSelOther
:I1.control flow
:P.Add the otherwise case to a select.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.s
:DD.A sel_handle.
:DT.lbl
:DD.The label to be associated with the otherwise case.
:eDL.
.section void CGSelect( sel_handle s, cg_name expr )
:I1.CGSelect
:I1.control flow
:P.Add the select  expression to a select statement and generate code.
This must be the last routine called for a given select statement.
It invalidates the sel_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.s
:DD.A sel_handle.
:DT.expr
:DD.The value we are selecting.
:eDL.
.section void CGSelectRestricted( sel_handle s, cg_name expr, cg_switch_type allowed )
:P.Identical to CGSelect, except that only switch generation techniques
corresponding to the set of allowed methods will be considered when
determining how to produce code.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.s
:DD.A sel_handle.
:DT.expr
:DD.The value we are selecting.
:DT.allowed
:DD.The allowed methods of generating code.
Must be a combination (non-empty) of the following bits:
:SL.
:LI.CG_SWITCH_SCAN
:LI.CG_SWITCH_BSEARCH
:LI.CG_SWITCH_TABLE
:eSL.
:eDL.
.chap Other
.section void CGReturn( cg_name name, cg_type type )
:I1.CGReturn
:I1.procedures
:I1.routines
:I1.functions
:P.Return from a function.
This is the last routine that may be called in any routine.
Multiple return statements must be implemented with assignments to a
temporary variable (CGTemp) and a branch to a label generated just
before this routine call.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The value of the return value, or NULL.
:DT.type
:DD.The type of the return value.
Use TY_INTEGER for void functions.
:eDL.
.section cg_name CGEval( cg_name name )
:I1.CGEval
:I1.expressions
:P.Evaluate this expression tree now and assign its value to a leaf
node.
Used to force the order of operations.
This should only be used if necessary.
Normally, the expression trees adequately define the order of
operations.
This usually used to force the order of parameter evaluation.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The tree to be evaluated.
:eDL.
:DL.
:DT.Returns
:DD.A leaf node containing the value of the tree.
:eDL.
.section void CGDone( cg_name name )
:I1.expressions
:I1.CGDone
:P.Generate the tree and throw away the resultant value.
For example, CGAssign yields a value which may not be needed, but must
be passed to this routine to cause the tree to be generated.
This routine invalidates all cg_name handles.
After this routine has returned, any pending inline function expansions
will have been performed.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The cg_name to be generated/discarded.
:eDL.
.section void CGTrash( cg_name name )
:I1.expressions
:I1.CGTrash
:P.Like CGDone, but used for partial expression trees.
This routine does not cause all existing cg_names to become invalid.
.section cg_type CGType( cg_name name )
:I1.typing
:I1.CGType
:P.Returns the type of the given cg_name.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.A cg_name.
:eDL.
:DL.
:DT.Returns
:DD.The type of the cg_name.
:eDL.
.section cg_name *CGDuplicate( cg_name name )
:I1.expressions
:I1.CGDuplicate
:P.Create two copies of a cg_name.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The cg_name to be duplicated.
:eDL.
:DL.
:DT.Returns
:DD.A pointer to an array of two new cg_names, each representing the
same value as the original.
These should be copied out of the array immediately since subsequent
calls to CGDuplicate will overwrite the array.
:eDL.
.section cg_name CGBitMask( cg_name name, byte start, byte len, cg_type type )
:I1.CGBitMask
:I1.expressions
:I1.bit fields
:P.Yields the address of a bit field.
This address may not really be used except with an indirection operator
or as the destination of an assignment operation.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The address of the integral variable containing the bit field.
:DT.start
:DD.The position of the least significant bit of the bit field.
0 indicates the least significant bit of the host data type.
:DT.len
:DD.The length of the bit field in bits.
:DT.type
:DD.The integral type of the value containing the bit field.
:eDL.
:DL.
:DT.Returns
:DD.The address of the bit field.
To reference field2 in the following C structure for a little endian target,
use start=4, len=5, and type=TY_INT_2. For a big endian target, start=7.
:eDL.
:XMP.
typedef struct {
    short field1 : 4;
    short field2 : 5;
    short field3 : 7;
}
:eXMP.
.section cg_name CGVolatile( cg_name name )
:I1.CGVolatile
:I1.expressions
:I1.volatile
:P.Indicate that the given address points to a volatile location.
This back end does not remember this information beyond this node in
the expression tree.
If an address points to a volatile location, the front end must call
this routine each time that address is used.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.name
:DD.The address of the volatile location.
:eDL.
:DL.
:DT.Returns
:DD.A new cg_name representing the same value as name.
:eDL.
.section cg_name CGCallback( cg_callback func, void *ptr )
:P.When a callback node is inserted into the tree, the code generator
will call the given function with the pointer as a parameter when it
turns the node into an instruction.
This can be used to retrieve order information about the placement of
nodes in the instruction stream.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.func
:DD.This is a pointer to a function which is compatible with the C type
"void (*)(void *)".
This function will be called with the second parameter to this function
as it's only parameter sometime during the execution of the CGDone call.
:DT.ptr
:DD.This will be a parameter to the function given as the first
parameter.
:eDL.
.section cg_name CGPatchNode( patch_handle hdl, cg_type type )
:P.This prepares a leaf node to hold an integer constant which will be
provided sometime during the execution of the CGDone call by means of a
BEPatchInteger() call.
It is an error to insert a patch node into the tree and not call
BEPatchInteger().
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.hdl
:DD.A handle for a patch allocated with BEPatch().
:DT.type
:DD.The actual type of the node.
Must be an integer type.
:eDL.
.chap Data Generation
:I1.data
:I1.segments
:I1.label, data
:P.The following routines generate a data item described at the current
location in the current segment, and increment the current location by
the size of the generated object.
.section void DGLabel( back_handle bck )
:I1.DGLabel
:I1.label, data
:P.Generate the label for a given back_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.bck
:DD.A back_handle.
:eDL.
.section void DGBackPtr( back_handle bck, segment_id seg, signed_32 offset, cg_type type )
:I1.DGBackPtr
:I1.back handle
:I1.relocatable data item
:P.Generate a pointer to the label defined by the back_handle.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.bck
:DD.A back_handle.
:DT.seg
:DD.The segment_id of the segment in which the label for :HP2.bck:eHP2.
will be defined if it has not already been passed to DGLabel.
:DT.offset
:DD.A value to be added to the generated pointer value.
:DT.type
:DD.The pointer type to be used.
:eDL.
.section void DGFEPtr( cg_sym_handle sym, cg_type type, signed_32 offset )
:I1.DGFEPtr
:I1.relocatable data item
:P.Generate a pointer to the label associated with :HP2.sym:eHP2..
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.A cg_sym_handle.
:DT.type
:DD.The pointer type to be used.
:DT.offset
:DD.A value to be added to the generated pointer value.
:eDL.
.section void DGInteger( unsigned_32 value, cg_type type )
:I1.DGInteger
:P.Generate an integer.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.value
:DD.An integral value.
:DT.type
:DD.The integral type to be used.
:eDL.
.section void DGInteger64( unsigned_64 value, cg_type type )
:I1.DGInteger64
:P.Generate an 64-bit integer.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.value
:DD.An 64-bit integer value.
:DT.type
:DD.The integral type to be used.
:eDL.
.section void DGFloat( char *value, cg_type type )
:I1.DGFloat
:I1.floating point constant
:P.Generate a floating-point constant.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.value
:DD.An E format string (ie: 1.2345e-134)
:DT.type
:DD.The floating point type to be used.
:eDL.
.section void DGChar( char value )
:I1.DGChar
:I1.character
:P.Generate a character constant.
Will be translated if cross compiling.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.value
:DD.A character value.
:eDL.
.section void DGString( char *value, uint len )
:I1.DGString
:I1.character
:P.Generate a character string.
Will be translated if cross compiling.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.value
:DD.Pointer to the characters to put into the segment.
It is not necessarily a null terminated string.
:DT.len
:DD.The length of the string.
:eDL.
.section void DGBytes( unsigned_32 len, byte *src )
:I1.DGBytes
:P.Generate raw binary data.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.src
:DD.Pointer to the data.
:DT.len
:DD.The length of the byte stream.
:eDL.
.section void DGIBytes( unsigned_32 len, byte pat )
:I1.DGIBytes
:P.Generate the byte :HP2.pat:eHP2., :HP2.len:eHP2. times.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.pat
:DD.The pattern byte.
:DT.len
:DD.The number of times to repeat the byte.
:eDL.
.section void DGUBytes( unsigned_32 len )
:I1.DGUBytes
:P.Generate :HP2.len:eHP2. undefined bytes.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.len
:DD.The size by which to increase the segment.
:eDL.
.section void DGAlign( uint align )
:I1.DGAlign
:I1.segments
:P.Align the segment to an :HP2.align:eHP2. byte boundary.
Any slack bytes will have an undefined value.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.align
:DD.The desired alignment boundary.
:eDL.
.section unsigned_32 DGSeek( unsigned_32 where )
:I1.DGSeek
:I1.segments
:P.Seek to a location within a segment.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.where
:DD.The location within the segment.
:eDL.
:DL.
:DT.Returns
:DD.The current location in the segment before the seek takes place.
:eDL.
.section unsigned long DGTell()
:I1.DGTell
:I1.segments
:DL.
:DT.Returns
:DD.The current location within the segment.
:eDL.
.section unsigned long DGBackTell( back_handle bck )
:I1.DGBackTell
:DL.
:DT.Returns
:DD.The location of the label within its segment.
The label must have been previously generated via DGLabel.
:eDL.
.chap Front End Routines
.section void FEGenProc( cg_sym_handle sym )
:I1.FEGenProc
:I1.inline procedures
:I1.procedures
:I1.routines
:I1.functions
:P.This routine will be called when the back end is generating a tree
and encounters a function call having the :HP2.call_class:eHP2.
MAKE_CALL_INLINE.
The front end must save its current state and start generating code for
:HP2.sym:eHP2..
FEGenProc calls may be nested if the code generator encounters an
inline within the code for an inline function.
The front end should maintain a state stack.
It is up to the front end to prevent infinite recursion.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.The cg_sym_handle of the function to be generated.
:eDL.
.section back_handle FEBack( cg_sym_handle sym )
:I1.FEBack
:I1.back handle
:P.Return, and possibly allocate using BENewBack, a back handle for
sym.
See the example under :HDREF refid='bck'.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.
:eDL.
:DL.
:DT.Returns
:DD.A back_handle.
:eDL.
.section segment_id FESegID( cg_sym_handle sym )
:I1.FESegID
:I1.segments
:P.Return the segment_id for symbol :HP2.sym:eHP2..
A negative value may be returned to indicate that the symbol is defined
in an unknown PRIVATE segment which has been defined in another module.
If two symbols have the same negative value returned, the back end
assumes that they are both defined in the same (unknown) segment.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.A cg_sym_handle.
:eDL.
:DL.
:DT.Returns
:DD.A segment_id.
:eDL.
.section char *FEModuleName()
:I1.FEModuleName
:DL.
:DT.Returns
:DD.A null terminated string which is the name of the module being
compiled.
This is usually the file name with path and extension information
stripped.
:eDL.
.section char FEStackCheck( cg_sym_handle sym )
:I1.stack probes
:I1.FEStackCheck
:DL.
:DT.Returns
:DD.1 if stack checking required for this routine
:eDL.
.section unsigned FELexLevel( cg_sym_handle sym )
:I1.FELexLevel
:I1.pascal
:DL.
:DT.Returns
:DD.The lexical level of routine :HP2.sym:eHP2..
This must be zero for all languages except Pascal.
In Pascal, 1 indicates the level of the main program.
Each nested procedures adds an additional level.
:eDL.
.section char *FEName( cg_sym_handle sym )
:I1.FEName
:DL.
:DT.Returns
:DD.A NULL terminated character string which is the name of sym.
A null string should be returned if the symbol has no name.
NULL should never be returned.
:eDL.
.section char *FEExtName( cg_sym_handle sym, int request )
:I1.FEExtName
:DL.
:DT.Returns
:DD.A various kind in dependency on request parameter.
:DTHD.Request parameter
:DDHD.Returns
:DT.EXTN_BASENAME
:DD.NULL terminated character string which is the name of sym.
A null string should be returned if the symbol has no name.
NULL should never be returned.
:DT.EXTN_PATTERN
:DD.NULL terminated character string which is the pattern for symbol name decoration.
'*' is replaced by symbol name.
'^' is replaced by its upper case equivalent.
'!' is replaced by its lower case equivalent.
'#' is replaced by '@nnn' where nnn is decimal number representing total size of all
function parameters.
If an '\' is present, the character following is used literally.
:DT.EXTN_PRMSIZE
:DD.Returns int value which represent size of all parameters when symbol is function.
:eDL.
.section cg_type FEParmType( cg_sym_handle func, cg_sym_handle parm, cg_type type )
:I1.FEParmType
:DL.
:DT.Returns
:DD.The type to which to promote an argument with a given type before
passing it to a procedure.
Type will be a dealiased type.
:eDL.
.section int FETrue()
:I1.FETrue
:DL.
:DT.Returns
:DD.The value of TRUE.
This is normally 1.
:eDL.
.section char FEMoreMem( unsigned size )
:I1.FEMoreMem
:P.Release memory for the back end to use.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.size
:DD.is the amount of memory required
:eDL.
:DL.
:DT.Returns
:DD.1 if at least :HP2.size:eHP2. bytes were released.
May always return 0 if memory is not a scarce resource in the host
environment.
:eDL.
.section dbg_type FEDbgType( cg_sym_handle sym )
:I1.FEDbgType
:DL.
:DT.Returns
:DD.The dbg_type handle for the symbol :HP2.sym:eHP2..
:eDL.
.section fe_attr FEAttr( cg_sym_handle sym )
:I1.FEAttr
:P.Return symbol attributes for :HP2.sym:eHP2..
These are bits combinable with the bit-wise or operator |.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.A cg_sym_handle.
:eDL.
:DL tsize='1.5i' break.
:DTHD.Return value
:DDHD. Definition
:DT.FE_PROC
:DD.A procedure.
:DT.FE_STATIC
:DD.A static or external symbol.
:DT.FE_GLOBAL
:DD.Is a global (extern) symbol.
:DT.FE_IMPORT
:DD.Needs to be imported.
:DT.FE_CONSTANT
:DD.The symbol is read only.
:DT.FE_MEMORY
:DD.This automatic variable needs a memory location.
:DT.FE_VISIBLE
:DD.Accessible outside this procedure?
:DT.FE_NOALIAS
:DD.No pointers point to this symbol.
:DT.FE_UNIQUE
:DD.This symbol should have an address which is different from all
other symbols with the FE_UNIQUE attribute.
:DT.FE_COMMON
:DD.There might be multiple definitions of this symbol in a program,
and it should be generated in such a way that all versions of the
symbol are merged into one copy by the linker.
:DT.FE_ADDR_TAKEN
:DD.The symbol has had it's address taken somewhere in the program (not
necessarally visible to the code generator).
:DT.FE_VOLATILE
:DD.The symbol is "volatile" (in the C language sense).
:DT.FE_INTERNAL
:DD.The symbol is not at file scope.
:eDL.
.section void FEMessage( msg_class msg, void *extra )
:I1.FEMessage
:P.Relays information to the front end.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.msg
:DD.Defined below.
:DT.extra
:DD.Extra information.
The type and meaning depends on the value of :HP2.msg:eHP2. and is
indicated below.
:eDL.
:DL tsize='2i' break.
:DT.MSG_INFO_FILE
:I1.error messages
:DD.Informational message about file.
extra (void) is ignored.
:DT.MSG_CODE_SIZE
:DD.Code size.
Extra (int) is the size of the generated code.
:DT.MSG_DATA_SIZE
:DD.Data size.
Extra (int) is the size of the generated data.
:DT.MSG_ERROR
:DD.A back end error message.
Extra (char *) is the error message.
:DT.MSG_FATAL
:DD.A fatal code generator error.
Extra (char *) is the reason for the fatal error.
The front end should issue this message and exit immediately to the
system.
:DT.MSG_INFO_PROC
:DD.Informational message about current procedure.
Extra (char *) is a message.
:DT.MSG_BAD_PARM_REGISTER
:DD.Invalid parameter register returned from FEAuxInfo.
Extra (int) is position of the offending parameter.
:DT.MSG_BAD_RETURN_REGISTER
:DD.Invalid return register returned from FEAuxInfo.
Extra (aux_handle) is the offending aux_handle.
:DT.MSG_REGALLOC_DIED
:DD.The register alloc ran out of memory.
Extra (cg_sym_handle) is the procedure which was not fully optimized.
:DT.MSG_SCOREBOARD_DIED
:DD.The register scoreboard ran out of memory.
Extra (cg_sym_handle) is the procedure which was not fully optimized.
:DT.MSG_PEEPHOLE_FLUSHED
:DD.Peep hole optimizer flushed due to lack of memory.
(void)
:DT.MSG_BACK_END_ERROR
:DD.BAD NEWS!
Internal compiler error.
Extra (int) is an internal error number.
:DT.MSG_BAD_SAVE
:DD.Invalid register modification information return from FEAuxInfo.
Extra (aux_handle) is the offending aux_handle.
:DT.MSG_WANT_MORE_DATA
:DD.The back end wants more data space.
Extra (int) is amount of additional memory needed to run.
(DOS real mode hosts only).
:DT.MSG_BLIP
:DD.Blip.
Let the world know we're still alive by printing a dot on the screen.
This is called approximately every 4 seconds during code generation.
(void)
:DT.MSG_BAD_LINKAGE
:DD.Cannot resolve linkage conventions.
370 only.
(sym)
:DT.MSG_SCHEDULER_DIED
:DD.Instruction scheduler ran out of memory.
Extra (cg_sym_handle) is the procedure which was not fully optimized.
:DT.MSG_NO_SEG_REGS
:DD.(Only occurs in the x86 version).
The cg_switches did not allow any segment registers to float, but the
user has requested a far pointer indirection.
Extra (cg_sym_handle) is the procedure which contained the far pointer
usage.
:DT.MSG_SYMBOL_TOO_LONG
:DD.Given symbol is too long and is truncated to maximum permited
length for current module output format.
Extra (cg_sym_handle) is the symbol which was truncated.
:eDL.
.section void *FEAuxInfo( void *extra, aux_class class )
:I1.FEAuxInfo
:P.relay information to back end
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.extra
:DD.Extra information.
Its type and meaning is determined by the value of class.
:DT.class
:DD.Defined below.
:eDL.
:DL tsize='2i' break.
:DTHD.Parameters
:DDHD.Return Value
:DT.( cg_sym_handle, AUX_LOOKUP )
:DD.aux_handle - given a cg_sym_handle, return an aux_handle.
:DT.( aux_handle, CALL_BYTES )
:DD.byte_seq * - A pointer to bytes to be generated instead of a call,
or NULL if a call is to be generated.
:XMP.
typedef struct byte_seq {
    char    length;
    char    data[ 1 ];
} byte_seq;
:eXMP.
:I1.calling conventions
:DT.( aux_handle, CALL_CLASS )
:DD.call_class * - returns call_class of the given aux_handle.
See definitions below.
:DT.( short, FREE_SEGMENT )
:DD.short - A free segment value which is free memory for the code
generator to use.
The first word at segment:0 is the size of the free memory in bytes.
(DOS real mode host only)
:DT.( NULL, OBJECT_FILE_NAME )
:DD.char * - The name of the object file to be generated.
:I1.calling conventions
:DT.( aux_handle, PARM_REGS )
:DD.hw_reg_set[] - The set of register to be used as parameters.
:DT.( aux_handle, RETURN_REG )
:DD.hw_reg_set * - The return register.
This is only called if the routine is declared to have the
SPECIAL_RETURN call_class.
:DT.( NULL, REVISION_NUMBER )
:DD.int - Front end revision number.
Must return II_REVISION.
:DT.( aux_handle, SAVE_REGS )
:DD.hw_reg_set * - Registers which are preserved by the routine.
:DT.( cg_sym_handle, SHADOW_SYMBOL )
:DD.cg_sym_handle - An alternate handle for a symbol.
Required for FORTRAN.
Usually implemented by turning on the LSB of a pointer or MSB of an
integer.
:DT.( NULL, SOURCE_NAME )
:DD.char * - The name of the source file to be put into the object file.
:DT.( cg_sym_handle, TEMP_LOC_NAME )
:DD. Return one of TEMP_LOC_NO, TEMP_LOC_YES, TEMP_LOC_QUIT.
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
:DT.( int, TEMP_LOC_TELL )
:DD. Returns nothing.
The 'int' value passed in is the relative position on the stack for the
temporary identified by the cg_sym_handle passed in from the previous
TEMP_LOC_NAME.
The value for an individual temporary has no meaning, but the
difference between two of the values is the number of bytes between the
addresses of the temporaries on the stack.
:DT.( void *, NEXT_DEPENDENCY )
:DD.Returns the handle of the next dependency file for which
information is available.
To start the list off, the back end passes in NULL for the dependency
file handle.
:DT.( void *, DEPENDENCY_TIMESTAMP )
:DD.Given the dependency file handle from the last NEXT_DEPENDENCY
request, return pointer to an unsigned long containing a timestamp
value for the dependency file.
:DT.( void *, DEPENDENCY_NAME )
:DD.Given the dependency file handle from the last NEXT_DEPENDENCY
request, return a pointer to a string containing the name for the
dependency file.
:DT.( NULL, SOURCE_LANGUAGE )
:DD.Returns a pointer to a string which identifies the source language
of the pointer.
E.g.
"C" for C, "FORTRAN" for FORTRAN, "CPP" for C++.
:DT.( cg_sym_handle, DEFAULT_IMPORT_RESOLVE )
:DD.Only called for imported symbols.
Returns a cg_sym_handle for another imported symbol which the reference
should be resolved to if certain conditions are met (see IMPORT_TYPE
request).
If NULL or the original cg_sym_handle is returned, there is no default
import resolution symbol.
:DT.( int, UNROLL_COUNT )
:DD.Returns a user-specified unroll count, or 0 if the user did not
specify an unroll count.
The parameter is the nesting level of the loop for which the request is
being made.
Loops which are not contained inside of other loops are nesting level
1.
If this function returns a non-zero value, the loop in question will be
unrolled that many times (there will be (count + 1) copies of the body).
:eDL.
:DL tsize='2i' break.
:DTHD.x86 Parameters
:DDHD.Return value
:DT.( NULL, CODE_GROUP )
:DD.char * - The name of the code group.
:DT.( aux_handle, STRETURN_REG )
:DD.hw_reg_set * - The register which points to a structure return
value.
Only called if the routine has the SPECIAL_STRUCT_RETURN attribute.
:DT.( void *, NEXT_IMPORT )
:DD.void * (See notes at end) - A handle for the next symbol to
generate a reference to in the object file.
:DT.( void*, IMPORT_NAME )
:DD.char * - The EXTDEF name to generate given a handle
:DT.( void *, NEXT_IMPORT_S )
:DD.void * (See notes at end) - A handle for the next symbol to
generate a reference to in the object file.
:DT.( void*, IMPORT_NAME_S )
:DD.Returns a cg_sym_handle. The EXTDEF name symbol reference to generate given
a handle.
:DT.( void*, NEXT_LIBRARY )
:DD.void * (See notes at end) - Handle for the next library required
:DT.( void*, LIBRARY_NAME )
:DD.char * - The library name to generate given a handle
:DT.( NULL, DATA_GROUP )
:DD.char * - Used to name DGROUP exactly.
NULL means use no group at all.
:DT.( segment_id, CLASS_NAME )
:DD.NULL - Used to name the class of a segment.
:DT.( NULL, USED_8087 )
:DD.NULL - Indicate that 8087 instructions were generated.
:DT.( NULL, STACK_SIZE_8087 )
:DD.int - How many 8087 registers are reserved for stack.
:DT.( NULL, CODE_LABEL_ALIGNMENT )
:DD.char * - An array x, such that x[i] is the label alignment
requirements for labels nested within i loops.
:DT.( NULL, PROEPI_DATA_SIZE )
:DD.int - How much stack is reserved for the prolog hook routine.
:DT.( cg_sym_handle, IMPORT_TYPE )
:DD.Returns IMPORT_IS_WEAK, IMPORT_IS_LAZY, IMPORT_IS_CONDITIONAL.
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
:DT.( cg_sym_handle, CONDITIONAL_IMPORT )
:DD. Returns void *.
Once the back end determines that it has a conditional import, it
performs this request to get a conditional list handle which is the
head of the list of conditional symbols.
:DT.( void *, CONDITIONAL_SYMBOL )
:DD.Returns a cg_sym_handle.
Give an conditional list handle, return the front end symbol associated
with it.
:DT.( void *, NEXT_CONDITIONAL )
:DD.Given an conditional list handle, return the next conditional list
handle.
Return NULL at the end of the list.
:DT.( aux_handle, VIRT_FUNC_REFERENCE )
:DD.Returns void *.
When performing an indirect function call, the back end invokes
FEAuxInfo passing the aux_handle supplied with the CGInitCall.
If the indirect call is referencing a C++ virtual function, the front
end should return a magic cookie which is the head of a list of virtual
functions that might be invoked by this call.
If it is not a virtual function invocation, return NULL.
:DT.( void *, VIRT_FUNC_NEXT_REFERENCE )
:DD.Returns void *.
Given the magic cookie returned by the VIRT_FUNC_REFERENCE or a
previous VIRT_FUNC_NEXT_REFRENCE, return the next magic cookie in the
list of virtual functions that might be refrenced from this indirect
call.
Return NULL if at the end of the list.
:DT.( void *, VIRT_FUNC_SYM )
:DD.Returns cg_sym_handle.
Given a magic cookie from a VIRT_FUNC_REFERENCE or
VIRT_FUNC_NEXT_REFERENCE, return the cg_sym_handle for that entry in
the list of virtual functions that might be invoked.
:DT.( segment_id, PEGGED_REGISTER )
:DD.Returns a pointer at a hw_reg_set or NULL.
If the pointer is non-NULL and the hw_reg_set is not EMPTY, the
hw_reg_set will indicate a segment register that is pegged (pointing)
to the given segment_id.
The code generator will use this segment register in any references to
objects in the segment.
If the pointer is NULL or the hw_reg_set is EMPTY, the code generator
uses the cg_switches to determine if a segment register is pointing at
the segment or if it will have to load one.
:eDL.
:CMT. S/370 support not maintained
.if 0 eq 1 .do begin
:DL tsize='2i' break.
:DTHD.370 Parameters
:DDHD.Return value
:DT.LINKAGE_REGS
:DD.???
:DT.AUX_OFFSET
:DD.???
:DT.AUX_HAS_MAIN
:DD.???
:DT.ASM_NAME
:DD.???
:DT.DBG_NAME
:DD.???
:DT.CSECT_NAME
:DD.???
:eDL.
.do end
:DL tsize='2i' break.
:DTHD.Call Class
:DDHD.Meaning
:I1.calling conventions
:DT.REVERSE_PARMS
:DD.Reverse the parameter list.
:DT.SUICIDAL
:DD.Routine never returns.
:DT.PARMS_BY_ADDRESS
:DD.Pass parameters by reference.
:DT.MAKE_CALL_INLINE
:DD.Call should be inline.
FEGenProc will be called for code sequence when required.
:eDL.
:DL tsize='2i' break.
:DTHD.x86 Call Class
:DDHD.Meaning
:DT.FAR
:DD.Does routine require a far call/return.
:DT.LOAD_DS_ON_CALL
:DD.Load DS from DGROUP prior to call.
:DT.CALLER_POPS
:DD.Caller pops/removes parms from the stack.
:DT.ROUTINE_RETURN
:DD.Routine allocates structure return memory.
:DT.SPECIAL_RETURN
:DD.Routine has non-default return register.
:DT.NO_MEMORY_CHANGED
:DD.Routine modifies no visible statics.
:DT.NO_MEMORY_READ
:DD.Routine reads no visible statics.
:DT.MODIFY_EXACT
:DD.Routine modifies no parameter registers.
:DT.SPECIAL_STRUCT_RETURN
:DD.Routine has special struct return register.
:DT.NO_STRUCT_REG_RETURNS
:DD.Pass 2/4/8 byte structs on stack, as opposed to registers.
:DT.NO_FLOAT_REG_RETURNS
:DD.Return floats as structs.
:DT.INTERRUPT
:DD.Routine is an interrupt routine.
:DT.NO_8087_RETURNS
:DD.No return values in the 8087.
:DT.LOAD_DS_ON_ENTRY
:DD.Load ds with dgroup on entry.
:DT.DLL_EXPORT
:DD.Is routine an OS/2 export symbol?
:DT.FAT_WINDOWS_PROLOG
:DD.Generate the real mode windows prolog code.
:DT.GENERATE_STACK_FRAME
:DD.Always generate a traceable prolog.
:DT.EMIT_FUNCTION_NAME
:DD.Emit the function name in front of the function in the code segment.
:DT.GROW_STACK
:DD.Emit a call to grow the stack on entry
:DT.PROLOG_HOOKS
:DD.Generate a prolog hook call.
:DT.EPILOG_HOOKS
:DD.Generate an epilog hook call.
:DT.THUNK_PROLOG
:DD.Generate a thunking prolog for routines calling 16 bit code.
:DT.FAR16_CALL
:DD.Performs a 16:16 call in the 386 compiler.
:DT.TOUCH_STACK
:DD.Certain people (who shall remain nameless) have implemented an
operating system (which shall remain nameless) that can't be bothered
figuring out whether a page reference is in the stack or not.
This attribute forces the first reference to the stack (after a routine
prologue has grown it) to be through the SS register.
:eDL.
:CMT. S/370 support not maintained
.if 0 eq 1 .do begin
:DL tsize='2i' break.
:DTHD.370 Call Class
:DDHD.Meaning
:DT.LINKAGE_OS
:DD.OS linkage.
:DT.LINKAGE_OSFUNC
:DD.Modified OS linkage.
:DT.LINKAGE_OSENTRY
:DD.Modified OS linkage.
:DT.LINKAGE_CLINK
:DD.WSL linkage.
:eDL.
.do end
.chap Debugging Information
These routines generate information about types, symbols, etc.
.section void DBLineNum( uint no )
:I1.DBLineNum
:P.Set the current source line number.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.no
:DD.Is the current source line number.
:eDL.
.section void DBModSym( cg_sym_handle sym, cg_type indirect )
:I1.DBModSym
:P.Define a symbol within the module (file scope).
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.is a front end symbol handle.
:DT.indirect
:DD.is the type of indirection needed to obtain the value
:eDL.
.section void DBObject( dbg_type tipe, dbg_loc loc )
:I1.DBObject
:P.Define a function as being a member function of a C++ class, and
identify the type of the class and the location of the object being
manipulated.
This function may only be done after the DBModSym for the function.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.tipe
:DD.is the debug type of the class that the function is a member of.
:DT.loc
:DD.is a location expression that evaluates to the address of the
object being manipulated by the function (the contents of the 'this'
pointer in C++).
This parameter is NULL if the routine is a static member function.
:eDL.
.section void DBLocalSym( cg_sym_handle sym, cg_type indirect )
:I1.DBLocalSym
:P.As DBModSym but for local (routine scope) symbols.
.section void DBGenSym( cg_sym_handle sym, dbg_loc loc, int scoped )
:I1.DBGenSym
:P.Define a symbol either with module scope ('scoped' == 0) or within
the current block ('scoped' != 0).
This routine superseeds both DBLocalSym and DBModuleSym.
The 'loc' parameter is a location expression (explained later) which
allows an aribitrary sequence of operations to locate the storage for
the symbol.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.sym
:DD.is a front end symbol handle.
:DT.loc
:DD.the location expression which is evaluated by the debugger to
locate the lvalue of the symbol.
:DT.scoped
:DD.whether the symbol is file scoped or not.
:eDL.
.section void DBBegBlock()
:I1.DBBegBlock
:P.Open a new scope level.
.section void DBEndBlock()
:I1.DBEndBlock
:P.Close the current scope level.
.section dbg_type DBScalar( char *name, cg_type tipe )
:I1.DBScalar
:P.Defines the string :HP2.name:eHP2. to have type :HP2.tipe:eHP2..
.section dbg_type DBScope( char *name )
:I1.DBScope
:P.define a symbol which "scopes" subsequent symbols.
In C, the keywords :HP2.enum:eHP2., :HP2.union:eHP2., :HP2.struct:eHP2.
may perform this function as in :HP2.struct foo:eHP2..
.section dbg_name DBBegName( const char *name, dbg_type scope )
:I1.DBBegName
:P.start a type name whose type is yet undetermined
.section dbg_type DBForward( dbg_name name )
:I1.DBForward
:P.declare a type to be a forward reference
.section dbg_type DBEndName( dbg_name name, dbg_type tipe )
:I1.DBEndName
:P.complete the definition of a type name.
.section dbg_type DBArray( dbg_type index, dbg_type base )
:I1.DBArray
:P.define a C array type
.section dbg_type DBIntArray( unsigned_32 hi, dbg_type base )
:I1.DBIntArray
:P.define a C array type
.section dbg_type DBSubRange( signed_32 lo, signed_32 hi, dbg_type base )
:I1.DBSubRange
:P.define an integer range type
.section dbg_type DBPtr( cg_type ptr_type, dbg_type base )
:I1.DBPtr
:P.declare a pointer type
.section dbg_type DBBasedPtr( cg_type ptr_type, dbg_type base, dbg_loc seg_loc )
:I1.DBBasedPtr
:P.declare a based pointer type.
The 'seg_loc' parameter is a location expression which evaluates to the
base address for the pointer after the indirection has been performed.
Before the location expression is evalated, the current lvalue of the
pointer symbol associated with this type is pushed onto the expression
stack (needed for based on self pointers).
.section dbg_struct DBBegStruct()
:I1.DBBegStruct
:P.start a structure type definition
.section void DBAddField( dbg_struct st, unsigned_32 off, char *nm, dbg_type base )
:I1.DBAddField
:P.add a field to a structure
.section void DBAddBitField( dbg_struct st, unsigned_32 off, byte strt, byte len, char *nm, dbg_type base )
:I1.DBAddBitField
:P.add a bit field to a structure
.section void DBAddLocField( dbg_struct st, dbg_loc loc, uint attr, byte strt, byte len, char *nm, dbg_type base )
:I1.DBAddLocField
:P.Add a field or bit field to a structure with a generalized location
expression 'loc'.
The location expression should assume the the address of the base of
the structure has already been pushed onto the debugger's evaluation
stack.
The 'attr' parameter contains a zero or more of the following
attributes or'd together:
:DL.
:DTHD.Attribute
:DDHD.Definition
:DT.FIELD_ATTR_INTERNAL
:DD.the field is internally generated by the compiler and would not be
normally visible to the user.
:DT.FIELD_ATTR_PUBLIC
:DD.the field has the C++ 'public' attribute.
:DT.FIELD_ATTR_PROTECTED
:DD.the field has the C++ 'protected' attribute.
:DT.FIELD_ATTR_PRIVATE
:DD.the field has the C++ 'private' attribute.
:eDL.
:PC.If the field being described is _not_ a bit field, the 'len'
parameter should be set to zero.
.section void DBAddInheritance( dbg_struct st, dbg_type inherit, dbg_loc adjust )
:I1.DBAddInheritance
Add the fields of an inherited structure to the current structure being
defined.
:DL.
:DTHD.Parameter
:DDHD.Definition
:DT.st
:DD.the dbg_struct handle for the structure currently being defined.
:DT.inherit
:DD.the dbg_type of a previously defined structure which is being
inherited.
:DT.adjust
:DD.a location expression which evaluates to a value which is the
amount to adjust the field offsets by in the inherited structure to
access them in the current structure.
The base address of the symbol associated with the structure type is
pushed onto the location expression stack before the expression is
evaluated.
:eDL.
.section dbg_type DBEndStruct( dbg_struct st )
:I1.DBEndStruct
:P.end a structure definition
.section dbg_enum DBBegEnum( cg_type tipe )
:I1.DBBegEnum
:P.begin defining an enumerated type
.section void DBAddConst( dbg_enum en, const char *nm, signed_32 val )
:I1.DBAddConst
:P.add a symbolic constant to an enumerated type
.section void DBAddConst64( dbg_enum en, const char *nm, signed_64 val )
:I1.DBAddConst64
:P.add a symbolic 64-bit integer constant to an enumerated type
.section dbg_type DBEndEnum( dbg_enum en )
:I1.DBEndEnum
:P.finish declaring an enumerated type
.section dbg_proc DBBegProc( cg_type call_type, dbg_type ret )
:I1.DBBegProc
:P.begin the a current procedure
.section void DBAddParm( dbg_proc pr, dbg_type tipe )
:I1.DBAddParm
:P.declare a parameter to the procedure
.section dbg_type DBEndProc( proc_list *pr )
:I1.DBEndProc
:P.end the current procedure
.section dbg_type DBFtnType( char *name, dbg_ftn_type tipe )
:I1.DBFtnType
:P.declare a fortran COMPLEX type
.section dbg_type DBCharBlock( unsigned_32 len )
:I1.DBCharBlock
:P.declare a type to be a block of length :HP2.len:eHP2. characters
.section dbg_type DBIndCharBlock( back_handle len, cg_type len_type, int off )
:I1.DBIndCharBlock
:P.declare a type to be a block of characters.
The length is found at run-time at back_handle :HP2.len:eHP2. + offset
:HP2.off:eHP2..
The integral type of the back_handle location is :HP2.len_type:eHP2.
.section dbg_type DBLocCharBlock( dbg_loc loc, cg_type len_type )
:I1.DBLocCharBlock
:P.declare a type to be a block of characters.
The length is found at run-time at the address specified by the
location expression :HP2.loc:eHP2..
The integral type of the location is :HP2.len_type:eHP2.
.section dbg_type DBFtnArray( back_handle dims, cg_type lo_bound_tipe, cg_type num_elts_tipe, int off, dbg_type base )
:I1.DBFtnArray
:P.define a FORTRAN array dimension slice.
:HP2.dims:eHP2. is a back handle + offset :HP2.off:eHP2. which will
point to a structure at run-time.
The structure contains the array low bound (type
:HP2.lo_bound_tipe:eHP2.) followed by the number of elements (type
:HP2.num_elts_tipe:eHP2.).
:HP2.base:eHP2. is the element type of the array.
.section dbg_type DBDereference( cg_type ptr_type, dbg_type base )
:I1.DBDereference
:P.declare a type to need an implicit de-reference to retrieve the
value (for FORTRAN parameters)
:NOTE.
This routine has been superceded by the use of location expressions.
.section dbg_loc DBLocInit( void )
:I1.DBLocInit
create an initial empty location expression
.section dbg_loc DBLocSym( dbg_loc loc, cg_sym_handle sym )
:I1.DBLocSym
push the address of 'sym' on to the expression stack
.section dbg_loc DBLocTemp( dbg_loc loc, temp_handle tmp )
:I1.DBLocTemp
push the address of 'tmp' on to the expression stack
.section dbg_loc DBLocConst( dbg_loc loc, unsigned_32 val )
:I1.DBLocConst
push the constant 'val' on to the expression stack
.section dbg_loc DBLocOp( dbg_loc loc, dbg_loc_op op, unsigned other )
:I1.DBLocOp
perform the following list of operations on the expression stack
:DL.
:DTHD.Operation
:DDHD.Definition
:DT.DB_OP_POINTS
:DD.take the top of the expression stack and use it as the address in
an indirection operation.
The result type of the operation is given by the 'other' parameter
which must be a cg_type which resolves to either an unsigned_16,
unsigned_32, a 16-bit far pointer, or a 32-bit far pointer.
:DT.DB_OP_ZEX
:DD.zero extend the top of the stack.
The 'other' parameter is a cg_type which is either 1 byte in size or 2
bytes in size.
That size determines how much of the original top of stack value to
leave untouched.
:DT.DB_OP_XCHG
:DD.exchange the top of stack value with the stack entry indexed by
'other'.
:DT.DB_OP_MK_FP
:DD.take the top two entries on the stack.
Make the second entry the segment value and the first entry the offset
value of an address.
:DT.DB_OP_ADD
:DD.add the top two stack entries together.
:DT.DB_OP_DUP
:DD.duplicate the top stack entry.
:DT.DB_OP_POP
:DD.pop off (throw away) the top stack entry.
:eDL.
.section void DBLocFini( dbg_loc loc )
:I1.DBLocFini
the given location expression will not be used anymore.
.section unsigned DBSrcFile( char *fname )
:I1.DBSrcFile
add the file name into the list of source files for positon info,
return handle to this name
:NOTE.
Handle 0 is reserved for base source file name and is added by BE
automaticaly during initialization.
.section void DBSrcCue( unsigned fno, unsigned line, unsigned col )
:I1.DBSrcCue
add source position info for the appropriate source file
.chap Registers
:I1.registers
:P.The hw_reg_set type is an abstract data type capable of representing
any combination of machine registers.
It must be manipulated using the following macros.
A parameter :HP2.c:eHP2., :HP2.c1:eHP2., :HP2.c2:eHP2., etc.
indicate a register constant such as HW_EAX must be used.
Anything else must be a variable of type :HP2.hw_reg_set:eHP2..
:P.The following are used for static initialization.
:SL.
:LI.HW_D_1( c1 )
:LI.HW_NotD_1( c1 )
:LI.HW_D_2( c1, c2 )
:LI.HW_NotD_2( c1, c2 )
:LI.HW_D_3( c1, c2, c3 )
:LI.HW_NotD_3( c1, c2, c3 )
:LI.HW_D_4( c1, c2, c3, c4 )
:LI.HW_NotD_4( c1, c2, c3, c4 )
:LI.HW_D_5( c1, c2, c3, c4, c5 )
:LI.HW_NotD_5( c1, c2, c3, c4, c5 )
:LI.HW_D( c1 )
:LI.HW_NotD( c1 )
:eSL.
:XMP.
hw_reg_set regs[] = {
    /* the EAX register */
    HW_D( HW_EAX ),
    /* all registers except EDX and EBX */
    HW_NotD_2( HW_EDX, HW_EBX )
};
:eXMP.
:P.The following are to build registers dynamically.
:DL tsize='2i' break.
:DTHD.Macro
:DDHD.Usage
:DT.HW_CEqual( a, c )
:DD.Is :HP2.a:eHP2. equal to :HP2.c:eHP2.
:DT.HW_COvlap( a, c )
:DD.Does :HP2.a:eHP2. overlap with :HP2.c:eHP2.
:DT.HW_CSubset( a, c )
:DD.Is :HP2.a:eHP2. subset of :HP2.c:eHP2.
:DT.HW_CAsgn( dst, c )
:DD.Assign :HP2.c:eHP2. to :HP2.dst:eHP2.
:DT.HW_CTurnOn( dst, c )
:DD.Turn on registers :HP2.c:eHP2. in :HP2.dst:eHP2..
:DT.HW_CTurnOff( dst, c )
:DD.Turn off registers :HP2.c:eHP2. in :HP2.dst:eHP2..
:DT.HW_COnlyOn( a, c )
:DD.Turn off all registers except :HP2.c:eHP2. in :HP2.dst:eHP2..
:DT.HW_Equal( a, b )
:DD.Is :HP2.a:eHP2. equal to :HP2.b:eHP2.
:DT.HW_Ovlap( a, b )
:DD.Does :HP2.a:eHP2. overlap with :HP2.b:eHP2.
:DT.HW_Subset( a, b )
:DD.Is :HP2.a:eHP2. subset of :HP2.b:eHP2.
:DT.HW_Asgn( dst, b )
:DD.Assign :HP2.b:eHP2. to :HP2.dst:eHP2.
:DT.HW_TurnOn( dst, b )
:DD.Turn on registers :HP2.b:eHP2. in :HP2.dst:eHP2..
:DT.HW_TurnOff( dst, b )
:DD.Turn off registers :HP2.b:eHP2. in :HP2.dst:eHP2..
:DT.HW_OnlyOn( dst, b )
:DD.Turn off all registers except :HP2.b:eHP2. in :HP2.dst:eHP2..
:eDL.
:P.The following example selects the low order 16 bits of any register.
that has a low part.
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
:P.The following register constants are defined for all targets.
:DL.
:DT.HW_EMPTY
:DD.The null register set.
:DT.HW_UNUSED
:DD.The set of unused register entries.
:DT.HW_FULL
:DD.All possible registers.
:eDL.
:P.The following example yields the set of all valid machine registers.
:XMP.
hw_reg_set reg;

HW_CAsgn( reg, HW_FULL );
HW_CTurnOff( reg, HW_UNUSED );
:eXMP.
.chap Miscellaneous
:P.I apologize for my lack of consistency in this document.
I use the terms function, routine, procedure interchangeably, as well
as index, subscript - select, switch - parameter, argument - etc.
I come from a multiple language background and will always be
hopelessly confused.
:P.The NEXT_IMPORT/NEXT_IMPORT_S/NEXT_LIBRARY are used as follows.
:XMP.
handle = NULL;
for( ;; ) {
    handle = FEAuxInfo( handle, NEXT_IMPORT );
    if( handle == NULL ) break;
    do_something( FEAuxInfo( handle, IMPORT_NAME ) );
}
:eXMP.
:P.The FREE_SEGMENT request is used as follows.
:XMP.
segment = 0;
for( ;; ) {
    segment = FEAuxInfo( segment, FREE_SEGMENT );
    if( segment == NULL ) break;
    segment_size = *(short *)MK_FP( segment, 0 ) * 16;
    this_is_my_memory_now( MK_FP( segment, 0 ), segment_size );
}
:eXMP.
:P.The main line in Pascal is defined to be lexical level 1.
Add 1 for each nested subroutine level.
C style routines are defined to be lexical level 0.
:P.The following types are defined by the code generator header files:
:I1.types, predefined
:DL.
:DTHD.Utility type
:DDHD.Definition
:DT.bool
:DD.(unsigned char) 0 = false, non-0 = true.
:DT.byte
:DD.(unsigned char)
:DT.int_8
:DD.(signed char)
:DT.int_16
:DD.(signed short)
:DT.int_32
:DD.(signed long)
:DT.signed_8
:DD.(signed char)
:DT.signed_16
:DD.(signed short)
:DT.signed_32
:DD.(signed long)
:DT.uint
:DD.(unsigned)
:DT.uint_8
:DD.(unsigned char)
:DT.uint_16
:DD.(unsigned short)
:DT.uint_32
:DD.(unsigned long)
:DT.unsigned_8
:DD.(unsigned char)
:DT.unsigned_16
:DD.(unsigned short)
:DT.unsigned_32
:DD.(unsigned long)
:DT.real
:DD.(float)
:DT.reallong
:DD.(double)
:DT.pointer
:DD.(void*)
:eDL.
:DL.
:DTHD.Type
:DDHD.Definition
:DT.aux_class
:DD.(enum) Passed as 2nd parameter to FEAuxInfo.
:DT.aux_handle
:DD.(void*) A handle used as 1st parameter to FEAuxInfo.
:DT.back_handle
:DD.(void*) A handle for a back end symbol table entry.
:DT.byte_seq
:DD.(struct) Passed to back end in response to CALL_BYTES FEAuxInfo
request.
:DT.call_class
:DD.(unsigned long) A set of combinable bits indicating the call
attributes for a routine.
:DT.call_handle
:DD.(void*) A handle to be used in CGInitCall, CGAddParm and CGCall.
:DT.cg_init_info
:DD.(union) The return value of BEInit.
:DT.cg_name
:DD.(void*) A handle for a back end expression tree node.
:DT.cg_op
:DD.(enum) An operator to be used in building expressions.
:DT.cg_switches
:DD.(unsigned_32) A set of combinable bits indicating the code
generator options.
:DT.cg_sym_handle
:DD.(uint) A handle for a front end symbol table entry.
:DT.cg_type
:DD.(unsigned short) A code generator type.
:DT.fe_attr
:DD.(enum) A set of combinable bits indicating symbol attributes.
:DT.hw_reg_set
:DD.(struct hw_reg_set) A structure representing a hardware register.
:DT.label_handle
:DD.(void*) A handle for a code generator code label.
:DT.linkage_regs
:DD.(struct) For 370 linkage conventions.
:DT.more_cg_types
:DD.(enum)
:DT.msg_class
:DD.(enum) The 1st parameter to FEMessage.
:DT.proc_revision
:DD.(enum) The 3rd parameter to BEInit.
:DT.seg_attr
:DD.(enum) A set of combinable bits indicate the attributes of a
segment.
:DT.segment_id
:DD.(int) A segment identifier.
:DT.sel_handle
:DD.(void*) A handle to be used in the CGSel calls.
:DT.temp_handle
:DD.(void*) A handle for a code generator temporary.
:eDL.
:DL.
:DTHD.Misc Type
:DDHD.Definition
:DT.HWT
:DD.hw_reg_part
:DT.hw_reg_part
:DD.(unsigned)
:DT.dbg_enum
:DD.(void*)
:DT.dbg_ftn_type
:DD.(enum)
:DT.dbg_name
:DD.(void*)
:DT.dbg_proc
:DD.(void*)
:DT.dbg_struct
:DD.(void*)
:DT.dbg_type
:DD.(unsigned short)
:DT.predefined_cg_types
:DD.(enum)
:eDL.
:APPENDIX.
.chap Pre-defined macros
:P.The following macros are defined by the code generator include files.
:SL.
:LI.HW_D
:LI.HW_D_1
:LI.HW_D_2
:LI.HW_D_3
:LI.HW_D_4
:LI.HW_D_5
:LI.BIG_CODE
:LI.BIG_DATA
:LI.CALLER_POPS
:LI.CHEAP_POINTER
:LI.CHEAP_WINDOWS
:LI.CONST_IN_CODE
:LI.CPU_MASK
:LI.C_FRONT_END
:LI.DBG_FWD_TYPE
:LI.DBG_LOCALS
:LI.DBG_NIL_TYPE
:LI.DBG_TYPES
:LI.DLL_EXPORT
:LI.DO_FLOATING_FIXUPS
:LI.DO_SYM_FIXUPS
:LI.EMIT_FUNCTION_NAME
:LI.EPILOG_HOOKS
:LI.EZ_OMF
:LI.E_8087
:LI.FALSE
:LI.FAR
:LI.FAT_WINDOWS_PROLOG
:LI.FIX_SYM_OFFSET
:LI.FIX_SYM_RELOFF
:LI.FIX_SYM_SEGMENT
:LI.FLAT_MODEL
:LI.FLOATING_DS
:LI.FLOATING_ES
:LI.FLOATING_FIXUP_BYTE
:LI.FLOATING_FS
:LI.FLOATING_GS
:LI.FLOATING_SS
:LI.FORTRAN_ALIASING
:LI.FORTRAN_FRONT_END
:LI.FPU_MASK
:LI.FRONT_END_MASK
:LI.FUNCS_IN_OWN_SEGMENTS
:LI.GENERATE_STACK_FRAME
:LI.GET_CPU
:LI.GET_FPU
:LI.GET_WTK
:LI.GROW_STACK
:LI.HWREG_INCLUDED
:LI.HW_0
:LI.HW_1
:LI.HW_2
:LI.HW_3
:LI.HW_64
:LI.HW_Asgn
:LI.HW_CAsgn
:LI.HW_CEqual
:LI.HW_COMMA
:LI.HW_COnlyOn
:LI.HW_COvlap
:LI.HW_CSubset
:LI.HW_CTurnOff
:LI.HW_CTurnOn
:LI.HW_DEFINE_COMPOUND
:LI.HW_DEFINE_GLOBAL_CONST
:LI.HW_DEFINE_SIMPLE
:LI.HW_Equal
:LI.HW_ITER
:LI.HW_NotD
:LI.HW_NotD_1
:LI.HW_NotD_2
:LI.HW_NotD_3
:LI.HW_NotD_4
:LI.HW_NotD_5
:LI.HW_OnlyOn
:LI.HW_Op1
:LI.HW_Op2
:LI.HW_Op3
:LI.HW_Op4
:LI.HW_Op5
:LI.HW_Ovlap
:LI.HW_Subset
:LI.HW_TurnOff
:LI.HW_TurnOn
:LI.II_REVISION
:LI.INDEXED_GLOBALS
:LI.INS_SCHEDULING
:LI.INTERNAL_DBG_OUTPUT
:LI.INTERRUPT
:LI.I_MATH_INLINE
:LI.LAST_AUX_ATTRIBUTE
:LI.LAST_CGSWITCH
:LI.LAST_TARG_AUX_ATTRIBUTE
:LI.LAST_TARG_CGSWITCH
:LI.LOAD_DS_ON_CALL
:LI.LOAD_DS_ON_ENTRY
:LI.LOOP_OPTIMIZATION
:LI.MAKE_CALL_INLINE
:LI.MAX_POSSIBLE_REG
:LI.MIN_OP
:LI.MODIFY_EXACT
:LI.NEED_STACK_FRAME
:LI.NO_8087_RETURNS
:LI.NO_CALL_RET_TRANSFORM
:LI.NO_FLOAT_REG_RETURNS
:LI.NO_MEMORY_CHANGED
:LI.NO_MEMORY_READ
:LI.NO_OPTIMIZATION
:LI.NO_STRUCT_REG_RETURNS
:LI.NULL
:LI.NULLCHAR
:LI.NUMBERS
:LI.O_FIRST_COND
:LI.O_FIRST_FLOW
:LI.O_LAST_COND
:LI.O_LAST_FLOW
:LI.PARMS_BY_ADDRESS
:LI.PROLOG_HOOKS
:LI.RELAX_ALIAS
:LI.REVERSE_PARMS
:LI.ROUTINE_RETURN
:LI.SEG_EXTRN_FAR
:LI.SET_CPU
:LI.SET_FPU
:LI.SET_WTK
:LI.SPECIAL_RETURN
:LI.SPECIAL_STRUCT_RETURN
:LI.STANDARD_INCLUDED
:LI.SUICIDAL
:LI.SYM_FIXUP_BYTE
:LI.THUNK_PROLOG
:LI.TRUE
:LI.TY_HUGE_CODE_PTR
:LI.USE_32
:LI.WINDOWS
:LI.WTK_MASK
:LI._AL
:LI._AX
:LI._BL
:LI._BP
:LI._BX
:LI._CG_H_INCLUDED
:LI._CL
:LI._CMS
:LI._CX
:LI._DI
:LI._DL
:LI._DX
:LI._HOST_INTEGER
:LI._OS
:LI._SI
:LI._TARG_AUX_SHIFT
:LI._TARG_CGSWITCH_SHIFT
:LI.far
:LI.huge
:LI.interrupt
:LI.near
:LI.offsetof
:eSL.
.chap Register constants
:P.The following register constants are defined for x86 targets.
:SL.
:LI.HW_AH
:LI.HW_AL
:LI.HW_BH
:LI.HW_BL
:LI.HW_CH
:LI.HW_CL
:LI.HW_DH
:LI.HW_DL
:LI.HW_SI
:LI.HW_DI
:LI.HW_BP
:LI.HW_SP
:LI.HW_DS
:LI.HW_ES
:LI.HW_CS
:LI.HW_SS
:LI.HW_ST0
:LI.HW_ST1
:LI.HW_ST2
:LI.HW_ST3
:LI.HW_ST4
:LI.HW_ST5
:LI.HW_ST6
:LI.HW_ST7
:LI.HW_FS
:LI.HW_GS
:LI.HW_AX
:LI.HW_BX
:LI.HW_CX
:LI.HW_DX
:LI.HW_EAX
:LI.HW_EBX
:LI.HW_ECX
:LI.HW_EDX
:LI.HW_ESI
:LI.HW_EDI
:LI.HW_ESP
:LI.HW_EBP
:eSL.
:P.The following registers are defined for the Alpha AXP target.
:SL.
:LI.HW_R0-HW_R31
:LI.HW_D0-HW_D31
:LI.HW_W0-HW_W31
:LI.HW_B0-HW_B31
:LI.HW_F0-HW_F31
:eSL.
:P.The following registers are defined for the PowerPC target.
:SL.
:LI.HW_R0-HW_R31
:LI.HW_Q3-HW_Q29
:LI.HW_D0-HW_D31
:LI.HW_W0-HW_W31
:LI.HW_B0-HW_B31
:LI.HW_F0-HW_F31
:eSL.
:P.The following registers are defined for the MIPS32 target.
:SL.
:LI.HW_R0-HW_R31
:LI.HW_Q2-HW_Q24
:LI.HW_D0-HW_D31
:LI.HW_W0-HW_W31
:LI.HW_B0-HW_B31
:LI.HW_F0-HW_F31
:LI.HW_FD0-HW_FD30
:eSL.
:CMT. S/370 support not maintained
.if 0 eq 1 .do begin
:P.The following registers are defined for the 370 target.
:SL.
:LI.HW_G0
:LI.HW_G1
:LI.HW_G2
:LI.HW_G3
:LI.HW_G4
:LI.HW_G5
:LI.HW_G6
:LI.HW_G7
:LI.HW_G8
:LI.HW_G9
:LI.HW_G10
:LI.HW_G11
:LI.HW_G12
:LI.HW_G13
:LI.HW_G14
:LI.HW_G15
:LI.HW_Y0
:LI.HW_Y2
:LI.HW_Y4
:LI.HW_Y6
:LI.HW_D0
:LI.HW_D2
:LI.HW_D4
:LI.HW_D6
:LI.HW_E0
:LI.HW_E4
:eSL.
.do end
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.cd set 1
.cntents end_of_book
:eGDOC.
