.gt id delete
.gt eid delete
.gt op delete
.gt eop delete
.gt rp delete
.gt erp delete
.gt or delete
.gt prgbeg delete
.gt prgend delete
.*
.dm @id begin
:HP2.&*.
.dm @id end
.gt id add @id cont
.*
.dm @eid begin
:eHP2.&*
.dm @eid end
.gt eid add @eid cont
.*
.dm @op begin
:HP2.[:eHP2.&*
.dm @op end
.gt op add @op cont
.*
.dm @eop begin
:HP2.]:eHP2.&*
.dm @eop end
.gt eop add @eop cont
.*
.dm @rp begin
:HP2.{:eHP2.&*
.dm @rp end
.gt rp add @rp cont
.*
.dm @erp begin
:HP2.}:eHP2.&*
.dm @erp end
.gt erp add @erp cont
.*
.dm @or begin
:HP2.|:eHP2.&*
.dm @or end
.gt or add @or cont
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.dm @prgbeg begin
*$pragma&*
.dm @prgbeg end
.gt prgbeg add @prgbeg cont
.*
.dm @prgend begin
.dm @prgend end
.gt prgend add @prgend cont texterror tagnext
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.dm @prgbeg begin
#pragma&*.
.dm @prgbeg end
.gt prgbeg add @prgbeg cont
.*
.dm @prgend begin
:HP2.[:eHP2.;:HP2.]:eHP2.
.dm @prgend end
.gt prgend add @prgend cont texterror tagnext
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="pragma" value="*$pragma".
:set symbol="epragma" value="".
:set symbol="function" value="subprogram".
:set symbol="functions" value="subprograms".
:set symbol="ufunction" value="Subprogram".
:set symbol="ufunctions" value="Subprograms".
:set symbol="short_int" value="INTEGER*2".
:set symbol="long_int" value="INTEGER*4".
:set symbol="int" value="INTEGER".
:set symbol="char" value="INTEGER*1".
:set symbol="ushort_int" value="INTEGER*2".
:set symbol="ulong_int" value="INTEGER*4".
:set symbol="uint" value="INTEGER".
:set symbol="uchar" value="INTEGER*1".
:set symbol="double" value="DOUBLE PRECISION".
:set symbol="single" value="REAL".
:set symbol="alias_name" value="WC".
:set symbol="other_cmp" value="&company C".
.if '&machine' eq '8086' .do begin
:set symbol="winopt" value="windows".
.do end
:set symbol="pragcont" value="c".
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="pragma" value="#pragma".
:set symbol="epragma" value="~;".
:set symbol="function" value="function".
:set symbol="functions" value="functions".
:set symbol="ufunction" value="Function".
:set symbol="ufunctions" value="Functions".
:set symbol="short_int" value="short int".
:set symbol="long_int" value="long int".
:set symbol="int" value="int".
:set symbol="char" value="char".
:set symbol="ushort_int" value="unsigned short int".
:set symbol="ulong_int" value="unsigned long int".
:set symbol="uint" value="unsigned int".
:set symbol="uchar" value="unsigned char".
:set symbol="double" value="double".
:set symbol="single" value="float".
.if '&machine' eq '8086' .do begin
:set symbol="alias_name" value="MS_C".
:set symbol="other_cmp" value="Microsoft C".
:set symbol="winopt" value="zW".
.do end
.if '&machine' eq '80386' .do begin
:set symbol="alias_name" value="HIGH_C".
:set symbol="other_cmp" value="MetaWare High C".
.do end
:set symbol="pragcont" value="".
.do end
.*
.if '&machine' eq '80386' .do begin
:set symbol="ax" value="eax".
:set symbol="bx" value="ebx".
:set symbol="cx" value="ecx".
:set symbol="dx" value="edx".
:set symbol="di" value="edi".
:set symbol="si" value="esi".
:set symbol="sp" value="esp".
:set symbol="bp" value="ebp".
:set symbol="axup" value="EAX".
:set symbol="bxup" value="EBX".
:set symbol="cxup" value="ECX".
:set symbol="dxup" value="EDX".
:set symbol="diup" value="EDI".
:set symbol="siup" value="ESI".
:set symbol="spup" value="ESP".
:set symbol="bpup" value="EBP".
:set symbol="reg32" value="EAX".
:set symbol="intsize" value="4".
:set symbol="machint" value="int".
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="machint" value="INTEGER".
:set symbol="arg_2_regs" value="DOUBLE PRECISION".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="machint" value="int".
:set symbol="arg_2_regs" value="double".
.do end
.do end
.if '&machine' eq '8086' .do begin
:set symbol="ax" value="ax".
:set symbol="bx" value="bx".
:set symbol="cx" value="cx".
:set symbol="dx" value="dx".
:set symbol="di" value="di".
:set symbol="si" value="si".
:set symbol="sp" value="sp".
:set symbol="bp" value="bp".
:set symbol="axup" value="AX".
:set symbol="bxup" value="BX".
:set symbol="cxup" value="CX".
:set symbol="dxup" value="DX".
:set symbol="diup" value="DI".
:set symbol="siup" value="SI".
:set symbol="spup" value="SP".
:set symbol="bpup" value="BP".
:set symbol="reg32" value="pair DX:AX".
:set symbol="intsize" value="2".
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="machint" value="INTEGER*2".
:set symbol="arg_2_regs" value="INTEGER".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="machint" value="int".
:set symbol="arg_2_regs" value="long int".
.do end
.do end
.if '&machine' eq '8086' .do begin
:set symbol="pragref" value="prg86".
:set symbol="praglib" value="prgl86".
:set symbol="pragttl" value="16-bit Pragmas".
:set symbol="pragx87" value="prg87".
:set symbol="pragenbl" value="prgen16".
:set symbol="pragdsbl" value="prgds16".
.do end
.el .do begin
:set symbol="pragref" value="prg386".
:set symbol="praglib" value="prgl386".
:set symbol="pragttl" value="32-bit Pragmas".
:set symbol="pragx87" value="prg387".
:set symbol="pragenbl" value="prgen32".
:set symbol="pragdsbl" value="prgds32".
.do end
.*
.chap *refid=&pragref. &pragttl.
.*
.im pragstx
.*
.if &e'&dohelp eq 1 .do begin
.   .if '&machine' eq '8086' .do begin
.   .   .helppref 16-bit:
.   .do end
.   .el .do begin
.   .   .helppref 32-bit:
.   .do end
.do end
.*
.pc
The following classes of pragmas are supported.
.begbull
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
pragmas that specify options
.do end
.bull
pragmas that specify default libraries
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
pragmas that describe the way structures are stored in memory
.do end
.bull
pragmas that provide auxiliary information used for
.if '&cmpclass' eq 'load-n-go' .do begin
argument passing.
.do end
.el .do begin
code generation
.do end
.endbull
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.section Using Pragmas to Specify Options
.*
.np
.ix 'pragma options'
.ix 'options' 'using pragmas'
Currently, the following options can be specified with pragmas:
.ix 'unreferenced option'
.ix 'check_stack option'
.begnote
.note unreferenced
.ix 'unreferenced option'
.ix 'options' 'unreferenced'
The "unreferenced" option controls the way &cmpname handles unused
symbols.
For example,
.millust begin
#pragma on (unreferenced);
.millust end
.pc
will cause &cmpname to issue warning messages for all unused symbols.
This is the default.
Specifying
.millust begin
#pragma off (unreferenced);
.millust end
.pc
will cause &cmpname to ignore unused symbols.
Note that if the warning level is not high enough, warning messages
for unused symbols will not be issued even if "unreferenced" was
specified.
.note check_stack
.ix 'check_stack option'
.ix 'options' 'check_stack'
The "check_stack" option controls the way stack overflows are to be
handled.
For example,
.millust begin
#pragma on (check_stack);
.millust end
.pc
will cause stack overflows to be detected and
.millust begin
#pragma off (check_stack);
.millust end
.pc
will cause stack overflows to be ignored.
When "check_stack" is on, &cmpname will generate a run-time call
to a stack-checking routine at the start of every routine compiled.
This run-time routine will issue an error if a stack overflow occurs
when invoking the routine.
The default is to check for stack overflows.
Stack overflow checking is particularly useful when functions are
invoked recursively.
Note that if the stack overflows and stack checking has been
suppressed, unpredictable results can occur.
.np
If a stack overflow does occur during execution and you are sure that
your program is not in error
(i.e. it is not unnecessarily recursing), you must increase the stack
size.
This is done by linking your application again and specifying the
"STACK" option to the &lnkname with a larger stack size.
.np
It is also possible to specify more than one option in a pragma as
illustrated by the following example.
.millust begin
#pragma on (check_stack unreferenced);
.millust end
.note reuse_duplicate_strings (C only)
.ix 'reuse_duplicate_strings option'
.ix 'options' 'reuse_duplicate_strings'
(C Only) The "reuse_duplicate_strings" option controls the way
&cmpcname handles identical strings in an expression.
For example,
.millust begin
#pragma on (reuse_duplicate_strings);
.millust end
.pc
will cause &cmpcname to reuse identical strings in an expression.
This is the default.
Specifying
.millust begin
#pragma off (reuse_duplicate_strings);
.millust end
.pc
will cause &cmpcname to generate additional copies of the identical
string.
The following example shows where this may be of importance to the way
the application behaves.
.exam begin
#include <stdio.h>

#pragma off (reuse_duplicate_strings)


void poke( char *, char * );

void main()
  {
    poke( "Hello world\n", "Hello world\n" );
  }

void poke( char *x, char *y )
  {
    x[3] = 'X';
    printf( x );
    y[4] = 'Y';
    printf( y );
  }
/*
Default output:
HelXo world
HelXY world
*/
.exam end
.endnote
.*
.do end
.*
.section *refid=&praglib Using Pragmas to Specify Default Libraries
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.np
.ix 'pragmas' 'specifying default libraries'
.ix 'default libraries' 'using pragmas'
Default libraries are specified in special object module records.
Library names are extracted from these special records by the
&lnkname..
When unresolved references remain after processing all object modules
specified in linker "FILE" directives, these default libraries are
searched after all libraries specified in linker "LIBRARY" directives
have been searched.
.np
By default, that is if no library pragma is specified, the &cmpname
compiler generates, in the object file defining the main program,
default libraries corresponding to the memory model and floating-point
model used to compile the file.
For example, if you have compiled the source file containing
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '8086' .do begin
the main program for the medium memory model and the floating-point
calls floating-point model, the libraries "clibm" and "mathm" will be
.do end
.if '&machine' eq '80386' .do begin
the main program for the flat memory model and the floating-point
calls floating-point model, the libraries "clib3r" and "math3r" will
be
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&machine' eq '8086' .do begin
the main program for the medium memory model and the floating-point
calls floating-point model, the library "flibm" will be
.do end
.if '&machine' eq '80386' .do begin
the main program for the flat memory model and the floating-point
calls floating-point model, the library "flib" will be
.do end
.do end
placed in the object file.
.do end
.if '&cmpclass' eq 'load-n-go' .do begin
.np
The library pragma can be used to specify object libraries that are to
be searched by &product when resolving references to undefined
symbols.
.ix '&ccmdup options' 'link'
These libraries will only be searched when the "link" compiler option
has been specified.
.do end
.el .do begin
.np
If you wish to add your own default libraries to this list, you can do
so with a library pragma.
.do end
Consider the following example.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#pragma library (mylib);
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma library mylib
.millust end
.do end
.pc
The name "mylib" will be added to the list of default libraries
.if '&cmpclass' eq 'load-n-go' .do begin
searched by &product when resolving undefined references.
.do end
.el .do begin
specified in the object file.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
If the library specification contains characters such as '&pc', ':' or
',' (i.e., any character not allowed in a C identifier), you must
enclose it in double quotes as in the following example.
.millust begin
.if '&target' eq 'QNX' .do begin
#pragma library ("&pathnam.&libdir.&pc.graph.lib");
.do end
.el .do begin
#pragma library ("&pathnam.&libdir16.&pc.dos&pc.graph.lib");
#pragma library ("&pathnam.&libdir32.&pc.dos&pc.graph.lib");
.do end
.millust end
.do end
.np
If you wish to specify more than one library in a library pragma you
must separate them with spaces as in the following example.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
.if '&target' eq 'QNX' .do begin
#pragma library (mylib "&pathnam.&libdir.&pc.graph.lib");
.do end
.el .do begin
#pragma library (mylib "&pathnam.&libdir16.&pc.dos&pc.graph.lib");
#pragma library (mylib "&pathnam.&libdir32.&pc.dos&pc.graph.lib");
.do end
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
.if '&target' eq 'QNX' .do begin
*$pragma library mylib &pathnam.&libdir.&pc.graph.lib
.do end
.el .do begin
*$pragma library mylib &pathnam.&libdir16.&pc.dos&pc.graph.lib
*$pragma library mylib &pathnam.&libdir32.&pc.dos&pc.graph.lib
.do end
.millust end
.if '&cmpclass' ne 'load-n-go' .do begin
.np
If no libraries are specified as in the following example,
.millust begin
*$pragma library
.millust end
.pc
the run-time libraries corresponding to the memory and floating-point models
used to compile the file will be generated.
.do end
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section The ALIAS Pragma (C Only)
.*
.np
.ix 'pragmas' 'alias'
.ix 'alias pragma'
The "alias" pragma can be used to emit alias records in the object file,
causing the linker to substitute references to a specified symbol with
references to another symbol. Either identifiers or names (strings) may
be used. Strings are used verbatim, while names corresponding to identifiers
are derived as appropriate for the kind and calling convention of the symbol.
The following describes the form of the "alias" pragma.
.mbox begin
:prgbeg. alias ( :id.alias:eid., :id.subst:eid. ) :prgend.
.mbox end
.synote
.note alias
is either a name or an identifier of the symbol to be aliased.
.note subst
is either a name or an identifier of the symbol that references to
.id alias
will be replaced with.
.endnote
.np
Consider the following example.
.millust begin
extern int var;

void fn( void )
{
    var = 3;
}

#pragma alias ( var, "other_var" );
.millust end
.pc
Instead of
.id var
the linker will reference symbol named "other_var". Symbol
.id var
need not be defined, although "other_var" has to be.
.*
.section The ALLOC_TEXT Pragma (C Only)
.*
.np
.ix 'pragmas' 'alloc_text'
.ix 'alloc_text pragma'
The "alloc_text" pragma can be used to specify the name of the text
segment into which the generated code for a function, or a list of
functions, is to be placed.
The following describes the form of the "alloc_text" pragma.
.mbox begin
:prgbeg. alloc_text ( :id.seg_name:eid., :id.fn:eid. :rp., :id.fn:eid.:erp. ) :prgend.
.mbox end
.synote
.note seg_name
is the name of the text segment.
.note fn
is the name of a function.
.endnote
.np
Consider the following example.
.millust begin
extern int fn1(int);
extern int fn2(void);
#pragma alloc_text ( my_text, fn1, fn2 );
.millust end
.pc
The code for the functions
.id fn1
and
.id fn2
will be placed in the segment
.id my_text.
Note: function prototypes for the named functions must exist prior
to the "alloc_text" pragma.
.*
.section The CODE_SEG Pragma
.*
.np
.ix 'pragmas' 'code_seg'
.ix 'code_seg pragma'
The "code_seg" pragma can be used to specify the name of the text
segment into which the generated code for functions is to be placed.
The following describes the form of the "code_seg" pragma.
.mbox begin
:prgbeg. code_seg ( :id.seg_name:eid. :op., :id.class_name:eid.:eop. ) :prgend.
.mbox end
.synote
.note seg_name
is the name of the text segment optionally enclosed in quotes.
Also,
.mono seg_name
may be a macro as in:
.millust begin
#define seg_name "MY_CODE_SEG"
#pragma code_seg ( seg_name );
.millust end
.note class_name
is the optional class name of the text segment and may be enclosed in quotes.
Please note that in order to be recognized by the linker as code, a
class name has to end in "CODE".
Also,
.mono class_name
may be a macro as in:
.millust begin
#define class_name "MY_CODE"
#pragma code_seg ( "MY_CODE_SEG", class_name );
.millust end
.endnote
.np
Consider the following example.
.millust begin
#pragma code_seg ( my_text );

int incr( int i )
{
    return( i + 1 );
}

int decr( int i )
{
    return( i - 1 );
}
.millust end
.pc
The code for the functions
.id incr
and
.id decr
will be placed in the segment
.id my_text.
.np
To return to the default segment, do not specify any string between
the opening and closing parenthesis.
.millust begin
#pragma code_seg ();
.millust end
.*
.section The COMMENT Pragma
.*
.np
.ix 'pragmas' 'comment'
.ix 'comment pragma'
The "comment" pragma can be used to place a comment record in an
object file or executable file.
The following describes the form of the "comment" pragma.
.mbox begin
:prgbeg. comment ( :id.comment_type:eid. :op., :id."comment_string":eid.:eop. ) :prgend.
.mbox end
.synote
.note comment_type
specifies the type of comment record.
The allowable comment types are:
.begnote
.note lib
Default libraries are specified in special object module records.
Library names are extracted from these special records by the
&lnkname..
When unresolved references remain after processing all object modules
specified in linker "FILE" directives, these default libraries are
searched after all libraries specified in linker "LIBRARY" directives
have been searched.
.np
The "lib" form of this pragma offers the same features as the
"library" pragma.
See the section entitled :HDREF refid='&praglib'. for more information.
.endnote
.note "comment_string"
is an optional string literal that provides additional information
for some comment types.
.endnote
.np
Consider the following example.
.millust begin
#pragma comment ( lib, "mylib" );
.millust end
.*
.section The DATA_SEG Pragma
.*
.np
.ix 'pragmas' 'data_seg'
.ix 'data_seg pragma'
The "data_seg" pragma can be used to specify the name of the
segment into which data is to be placed.
The following describes the form of the "data_seg" pragma.
.mbox begin
:prgbeg. data_seg ( :id.seg_name:eid. :op., :id.class_name:eid.:eop. ) :prgend.
.mbox end
.synote
.note seg_name
is the name of the data segment and may be enclosed in quotes.
Also,
.mono seg_name
may be a macro as in:
.millust begin
#define seg_name "MY_DATA_SEG"
#pragma data_seg ( seg_name );
.millust end
.note class_name
is the optional class name of the data segment and may be enclosed in quotes.
Also,
.mono class_name
may be a macro as in:
.millust begin
#define class_name "MY_CLASS"
#pragma data_seg ( "MY_DATA_SEG", class_name );
.millust end
.endnote
.np
Consider the following example.
.millust begin
#pragma data_seg ( my_data );

static int i;
static int j;
.millust end
.pc
The data for
.id i
and
.id j
will be placed in the segment
.id my_data.
.np
To return to the default segment, do not specify any string between
the opening and closing parenthesis.
.millust begin
#pragma data_seg ();
.millust end
.*
.section *refid=&pragdsbl. The DISABLE_MESSAGE Pragma
.*
.np
.ix 'pragmas' 'disable_message'
.ix 'disable_message pragma'
The "disable_message" pragma disables the issuance of specified
diagnostic messages.
The form of the "disable_message" pragma is as follows.
.mbox begin
:prgbeg. disable_message ( :id.msg_num:eid. :rp., :id.msg_num:eid.:erp. ) :prgend.
.mbox end
.synote
.note msg_num
is the number of the diagnostic message.
.if &e'&dohelp eq 0 .do begin
This number corresponds to the number issued by the compiler and can
be found in the appendix entitled :HDREF refid='cmsgs'..
.do end
.el .do begin
This number corresponds to the number issued by the compiler.
.do end
Make sure to strip all leading zeroes from the message number (to
avoid interpretation as an octal constant).
.esynote
.np
See also the description of :HDREF refid='&pragenbl.'..
.*
.section The DUMP_OBJECT_MODEL Pragma (C++ Only)
.*
.np
.ix 'object model'
.ix 'pragmas' 'dump_object_model'
.ix 'dump_object_model pragma'
.ix 'class information'
The "dump_object_model" pragma causes the C++ compiler to print
information about the object model for an indicated
.if &version ge 107 .do begin
class or an enumeration name to the diagnostics file.
.ix 'enumeration' 'information'
.ix 'enumeration' 'values'
.do end
.el .do begin
class.
.do end
For class names, this information includes the offsets and sizes of
fields within the class and within base classes.
.if &version ge 107 .do begin
For enumeration names, this information consists of a list of all the
enumeration constants with their values.
.do end
.np
The general form of the "dump_object_model" pragma is as follows.
.mbox begin
:prgbeg. dump_object_model :id.class:eid. :prgend.
.if &version ge 107 .do begin
:prgbeg. dump_object_model :id.enumeration:eid. :prgend.
.do end
:id.class ::= a defined C++ class free of errors:eid.
.if &version ge 107 .do begin
:id.enumeration ::= a defined C++ enumeration name:eid.
.do end
.mbox end
.np
This pragma is designed to be used for information purposes only.
.*
.section *refid=&pragenbl. The ENABLE_MESSAGE Pragma
.*
.np
.ix 'pragmas' 'enable_message'
.ix 'enable_message pragma'
The "enable_message" pragma re-enables the issuance of specified
diagnostic messages that have been previously disabled.
The form of the "enable_message" pragma is as follows.
.mbox begin
:prgbeg. enable_message ( :id.msg_num:eid. :rp., :id.msg_num:eid.:erp. ) :prgend.
.mbox end
.synote
.note msg_num
is the number of the diagnostic message.
.if &e'&dohelp eq 0 .do begin
This number corresponds to the number issued by the compiler and can
be found in the appendix entitled :HDREF refid='cmsgs'..
.do end
.el .do begin
This number corresponds to the number issued by the compiler.
.do end
Make sure to strip all leading zeroes from the message number (to
avoid interpretation as an octal constant).
.esynote
.np
See also the description of :HDREF refid='&pragdsbl.'..
.*
.if &version ge 107 .do begin
.*
.section The ENUM Pragma
.*
.np
.ix 'pragmas' 'enum'
.ix 'enum pragma'
The "enum" pragma affects the underlying storage-definition for
subsequent
.us enum
declarations.
The forms of the "enum" pragma are as follows.
.mbox begin
:prgbeg. enum int :prgend.
:prgbeg. enum minimum :prgend.
:prgbeg. enum original :prgend.
:prgbeg. enum pop :prgend.
.mbox end
.synote
.note int
Make
.us int
the underlying storage definition (same as the "ei" compiler option).
.note minimum
Minimize the underlying storage definition (same as not specifying
the "ei" compiler option).
.note original
Reset back to the original compiler option setting (i.e., what was or
was not specified on the command line).
.note pop
Restore the previous setting.
.esynote
.np
The first three forms all push the previous setting before
establishing the new setting.
.*
.do end
.*
.section The ERROR Pragma
.*
.np
.ix 'pragmas' 'error'
.ix 'error pragma'
The "error" pragma can be used to issue an error message with the
specified text.
The following describes the form of the "error" pragma.
.mbox begin
:prgbeg. error :id."error text":eid. :prgend.
.mbox end
.synote
.note "error text"
is the text of the message that you wish to display.
.endnote
.np
You should use the ISO
.kw #error
directive rather than this pragma.
This pragma is provided for compatibility with legacy code.
The following is an example.
.millust begin
#if defined(__386__)
    ...
#elseif defined(__86__)
    ...
#else
#pragma error ( "neither __386__ or __86__ defined" );
#endif
.millust end
.*
.if &version ge 107 .do begin
.*
.section The EXTREF Pragma
.*
.np
.ix 'pragmas' 'extref'
.ix 'extref pragma'
.ix 'external references'
The "extref" pragma is used to generate a reference to an external
function or data item.
The form of the "extref" pragma is as follows.
.mbox begin
:prgbeg. extref :id.name:eid. :prgend.
.mbox end
.synote
.note name
is the name of an external function or data item.
It must be declared to be an external function or data item before
the pragma is encountered.
In C++, when
.sy name
is a function, it must not be overloaded.
.esynote
.np
This pragma causes an external reference for the function or data item
to be emitted into the object file even if that function or data item
is not referenced in the module.
The external reference will cause the linker to include the module
containing that name in the linked program or DLL.
.np
This is useful for debugging since you can cause debugging routines
(callable from within debugger) to be included into a program or DLL
to be debugged.
.np
In C++, you can also force constructors and/or destructors to be
called for a data item without necessarily referencing the data item
anywhere in your code.
.*
.do end
.*
.section The FUNCTION Pragma
.*
.np
.ix 'pragmas' 'function'
.ix 'function pragma'
Certain functions, such as those listed in the description of the "oi"
and "om" options, have intrinsic forms.
These functions are special functions that are recognized by the
compiler and processed in a special way.
For example, the compiler may choose to generate in-line code for the
function.
The intrinsic attribute for these special functions is set by
specifying the "oi" or "om" option or using an "intrinsic" pragma.
The "function" pragma can be used to remove the intrinsic attribute
for a specified list of functions.
.np
The following describes the form of the "function" pragma.
.mbox begin
:prgbeg. function ( :id.fn:eid. :rp., :id.fn:eid.:erp. ) :prgend.
.mbox end
.synote
.note fn
is the name of a function.
.endnote
.np
Suppose the following source code was compiled using the "om" option
so that when one of the special math functions is referenced, the
intrinsic form will be used.
In our example, we have referenced the function
.id sin
which does have an intrinsic form.
By specifying
.id sin
in a "function" pragma, the intrinsic attribute will be removed,
causing the function
.id sin
to be treated as a regular user-defined function.
.millust begin
#include <math.h>
#pragma function( sin );

double test( double x )
{
    return( sin( x ) );
}
.millust end



.*
.section The INCLUDE_ALIAS Pragma
.*
.np
.ix 'pragmas' 'include_alias'
.ix 'include_alias pragma'
In certain situations, it can be advantageous to remap the names of include
files. Most commonly this occurs on systems that do not support long file
names when building source code that references header files with long names.
.np
The form of the "include_alias" pragma follows.
.mbox begin
:prgbeg. include_alias ( ":id.alias_name:eid.", ":id.real_name:eid." ) :prgend.
:prgbeg. include_alias ( <:id.alias_name:eid.>, <:id.real_name:eid.> ) :prgend.
.mbox end
.synote
.note alias_name
is the name referenced in include directives in source code.
.note real_name
is the translated name that the compiler will reference instead.
.endnote
.np
The following is an example.
.millust begin
#pragma include_alias( "LongFileName.h", "lfn.h" )
#include "LongFileName.h"
.millust end
In the example, the compiler will attempt to read lfn.h when LongFileName.h
was included.
.np
Note that only simple textual substitution is performed. The aliased name
must match exactly, including double quotes or angle brackets, as well as
any directory separators. 
Also, double quotes and angle brackets may not be mixed a single pragma.
.np
The value of the predefined
.id __FILE__
symbol, as well as the filename reported in error messages, will be the true
filename after substitution was performed.
.*
.section Setting Priority of Static Data Initialization (C++ Only)
.*
.np
.ix 'pragmas' 'initialize'
.ix 'initialize pragma'
The "initialize" pragma sets the priority for initialization of
static data in the file.
This priority only applies to initialization of static data that requires
the execution of code.
For example, the initialization of a class that contains a constructor
requires the execution of the constructor.
Note that if the sequence in which initialization of static data in your
program takes place has no dependencies, the "initialize" pragma need not
be used.
.np
The general form of the "initialize" pragma is as follows.
.mbox begin
:prgbeg. initialize :op.before :or. after:eop. :id.priority:eid. :prgend.

:id.priority ::=:eid. :id.n:eid. :or. library :or. program
.mbox end
.synote
.note n
is a number representing the priority and must be in the range 0-255.
The larger the priority, the later the point at which initialization will
occur.
.esynote
.np
Priorities in the range 0-20 are reserved for the C++ compiler.
This is to ensure that proper initialization of the C++ run-time system
takes place before the execution of your program.
The "library" keyword represents a priority of 32 and can be used for class
libraries that require initialization before the program is initialized.
The "program" keyword represents a priority of 64 and is the default priority
for any compiled code.
Specifying "before" adjusts the priority by subtracting one.
Specifying "after" adjusts the priority by adding one.
.np
A source file containing the following "initialize" pragma specifies that the
initialization of static data in the file will take place before
initialization of all other static data in the program since a priority of
63 will be assigned.
.exam begin
#pragma initialize before program
.exam end
.pc
If we specify "after" instead of "before", the initialization of the
static data in the file will occur after initialization of all other static
data in the program since a priority of 65 will be assigned.
.np
Note that the following is equivalent to the "before" example
.exam begin
#pragma initialize 63
.exam end
.pc
and the following is equivalent to the "after" example.
.exam begin
#pragma initialize 65
.exam end
.pc
The use of the "before", "after", and "program" keywords are more
descriptive in the intent of the pragmas.
.np
It is recommended that a priority of 32 (the priority used when the "library"
keyword is specified) be used when developing class libraries.
This will ensure that initialization of static data defined by the class
library will take place before initialization of static data defined by the
program.
The following "initialize" pragma can be used to achieve this.
.exam begin
#pragma initialize library
.exam end
.*
.section The INLINE_DEPTH Pragma (C++ Only)
.*
.ix 'pragmas' 'inline_depth'
.ix 'inline_depth pragma'
.np
When an in-line function is called, the function call may be replaced
by the in-line expansion for that function.
This in-line expansion may include calls to other in-line functions
which can also be expanded.
The "inline_depth" pragma can be used to set the number of times this
expansion of in-line functions will occur for a call.
.np
The form of the "inline_depth" pragma is as follows.
.mbox begin
:prgbeg. inline_depth :op.(:eop. :id.n:eid. :op.):eop. :prgend.
.mbox end
.synote
.note n
is the depth of expansion.
If
.id n
is 0, no expansion will occur.
If
.id n
is 1, only the original call is expanded.
If
.id n
is 2, the original call and the in-line functions invoked by the
original function will be expanded.
The default value for
.id n
is 3.
The maximum value for
.id n
is 255.
Note that no expansion of recursive in-line functions occur unless
enabled using the "inline_recursion" pragma.
.esynote
.*
.section The INLINE_RECURSION Pragma (C++ Only)
.*
.np
.ix 'pragmas' 'inline_recursion'
.ix 'inline_recursion pragma'
The "inline_recursion" pragma controls the recursive expansion of
inline functions.
The form of the "inline_recursion" pragma is as follows.
.mbox begin
:prgbeg. inline_recursion :op.(:eop. on :or. off :op.):eop. :prgend.
.mbox end
.np
Specifying "on" will enable expansion of recursive inline functions.
The depth of expansion is specified by the "inline_depth" pragma.
The default depth is 3.
Specifying "off" suppresses expansion of recursive inline functions.
This is the default.
.*
.section The INTRINSIC Pragma
.*
.np
.ix 'pragmas' 'intrinsic'
.ix 'intrinsic pragma'
Certain functions, those listed in the description of the "oi" option,
have intrinsic forms.
These functions are special functions that are recognized by the compiler
and processed in a special way.
For example, the compiler may choose to generate in-line code for the
function.
The intrinsic attribute for these special functions is set by specifying
the "oi" option or using an "intrinsic" pragma.
.np
The following describes the form of the "intrinsic" pragma.
.mbox begin
:prgbeg. intrinsic ( :id.fn:eid. :rp., :id.fn:eid.:erp. ) :prgend.
.mbox end
.synote
.note fn
is the name of a function.
.endnote
.np
Suppose the following source code was compiled without using the "oi" option
so that no function had the intrinsic attribute.
If we wanted the intrinsic form of the
.id sin
function to be used,
we could specify the function in an "intrinsic" pragma.
.millust begin
#include <math.h>
#pragma intrinsic( sin );

double test( double x )
{
    return( sin( x ) );
}
.millust end
.*
.section The MESSAGE Pragma
.*
.np
.ix 'pragmas' 'message'
.ix 'message pragma'
The "message" pragma can be used to issue a message with the specified
text to the standard output without terminating compilation.
The following describes the form of the "message" pragma.
.mbox begin
:prgbeg. message ( :id."message text":eid. ) :prgend.
.mbox end
.synote
.note "message text"
is the text of the message that you wish to display.
.endnote
.np
The following is an example.
.millust begin
#if defined(__386__)
    ...
#else
#pragma message ( "assuming 16-bit compile" );
#endif
.millust end
.*
.section The ONCE Pragma
.*
.np
.ix 'pragmas' 'once'
.ix 'once pragma'
.ix 'optimization'
The "once" pragma can be used to indicate that the file which contains
this pragma should only be opened and processed "once".
The following describes the form of the "once" pragma.
.mbox begin
:prgbeg. once :prgend.
.mbox end
.np
Assume that the file "foo.h" contains the following text.
.exam begin
#ifndef _FOO_H_INCLUDED
#define _FOO_H_INCLUDED
#pragma once
    .
    .
    .
#endif
.exam end
.np
The first time that the compiler processes "foo.h" and encounters the
"once" pragma, it records the file's name.
Subsequently, whenever the compiler encounters a
.mono #include
statement that refers to "foo.h", it will not open the include file
again.
This can help speed up processing of
.mono #include
files and reduce the time required to compile an application.
.*
.section The PACK Pragma
.*
.np
.ix 'pragmas' 'pack'
.ix 'pack pragma'
The "pack" pragma can be used to control the way in which structures
are stored in memory.
There are 4 forms of the "pack" pragma.
.np
The following form of the "pack" pragma can be used to change the
alignment of structures and their fields in memory.
.mbox begin
:prgbeg. pack ( :id.n:eid. ) :prgend.
.mbox end
.synote
.note n
is 1, 2, 4, 8 or 16 and specifies the method of alignment.
.endnote
.np
The alignment of structure members is described in the following
table.
If the size of the member is 1, 2, 4, 8 or 16, the alignment is given
for each of the "zp" options.
If the member of the structure is an array or structure, the alignment
is described by the row "x".
.millust begin
                    zp1     zp2     zp4     zp8     zp16
sizeof(member)  \---------------------------------------
        1       |   0       0       0       0       0
        2       |   0       2       2       2       2
        4       |   0       2       4       4       4
        8       |   0       2       4       8       8
        16      |   0       2       4       8       16
        x       |   aligned to largest member
.millust end
.pc
An alignment of 0 means no alignment, 2 means word boundary,
4 means doubleword boundary, etc.
If the largest member of structure "x" is 1 byte then "x" is not
aligned.
If the largest member of structure "x" is 2 bytes then "x" is aligned
according to row 2.
If the largest member of structure "x" is 4 bytes then "x" is aligned
according to row 4.
If the largest member of structure "x" is 8 bytes then "x" is aligned
according to row 8.
If the largest member of structure "x" is 16 bytes then "x" is aligned
according to row 16.
.np
.if &version ge 107 .do begin
.if '&machine' eq '8086' .do begin
If no value is specified in the "pack" pragma, a default value of 2 is
used.
.do end
.if '&machine' eq '80386' .do begin
If no value is specified in the "pack" pragma, a default value of 8 is
used.
.do end
.do end
.el .do begin
If no value is specified in the "pack" pragma, a default value of 1 is
used.
.do end
Note that the default value can be changed with the "zp" &cmpname
compiler command line option.
.np
The following form of the "pack" pragma can be used to save the current
alignment amount on an internal stack.
.mbox begin
:prgbeg. pack ( push ) :prgend.
.mbox end
.np
The following form of the "pack" pragma can be used to save the current
alignment amount on an internal stack and set the current alignment.
.mbox begin
:prgbeg. pack ( push, :id.number:eid. ) :prgend.
.mbox end
.np
The following form of the "pack" pragma can be used to restore the
previous alignment amount from an internal stack.
.mbox begin
:prgbeg. pack ( pop ) :prgend.
.mbox end
.*
.section The READ_ONLY_FILE Pragma
.*
.np
.ix 'pragmas' 'read_only_file'
.ix 'read_only_file pragma'
.ix 'AUTODEPEND'
Explicit listing of dependencies in a makefile can often be tedious
in the development and maintenance phases of a project.
The &cmpname compiler will insert dependency information into the
object file as it processes source files so that a complete snapshot
of the files necessary to build the object file are recorded.
The "read_only_file" pragma can be used to prevent the name of the
source file that includes it from being included in the dependency
information that is written to the object file.
.np
This pragma is commonly used in system header files since they change
infrequently (and, when they do, there should be no impact on source
files that have included them).
.np
The form of the "read_only_file" pragma follows.
.mbox begin
:prgbeg. read_only_file :prgend.
.mbox end
.np
For more information on make dependencies, see the section
entitled "Automatic Dependency Detection (.AUTODEPEND)" in the
.book &product Tools User's Guide.
.*
.section The TEMPLATE_DEPTH Pragma (C++ Only)
.*
.ix 'pragmas' 'template_depth'
.ix 'template_depth pragma'
.np
The "template_depth" pragma provides a hard limit for the amount of
nested template expansions allowed so that infinite expansion can be
detected.
.np
The form of the "template_depth" pragma is as follows.
.mbox begin
:prgbeg. template_depth :op.(:eop. :id.n:eid. :op.):eop. :prgend.
.mbox end
.synote
.note n
is the depth of expansion.
If the value of
.id n
is less than 2, if will default to 2.
If
.id n
is not specified, a warning message will be issued and the default
value for
.id n
will be 100.
.esynote
.np
The following example of recursive template expansion illustrates why
this pragma can be useful.
.exam begin
#pragma template_depth(10);

template <class T>
struct S {
    S<T*> x;
};

S<char> v;
.exam end
.*
.section The WARNING Pragma (C++ Only)
.*
.np
.ix 'pragmas' 'warning'
.ix 'warning pragma'
The "warning" pragma sets the level of warning messages.
The form of the "warning" pragma is as follows.
.mbox begin
:prgbeg. warning :id.msg_num:eid. :id.level:eid. :prgend.
.mbox end
.synote
.note msg_num
is the number of the warning message.
.if &e'&dohelp eq 0 .do begin
This number corresponds to the number issued by the compiler and can
be found in the appendix entitled :HDREF refid='cppmsgs'..
.do end
.el .do begin
This number corresponds to the number issued by the compiler.
.do end
If
.id msg_num
is "*", the level of all warning messages is changed to the specified
level.
Make sure to strip all leading zeroes from the message number (to
avoid interpretation as an octal constant).
.note level
is a number from 0 to 9 and represents the level of the warning message.
When a value of zero is specified, the warning becomes an error.
.esynote
.*
.do end
.*
.section Auxiliary Pragmas
.*
.np
.ix 'pragmas' 'auxiliary'
.ix 'auxiliary pragma'
The following sections describe the capabilities provided by auxiliary
pragmas.
.if '&lang' eq 'FORTRAN 77' .do begin
.np
The backslash character ('\') is used to continue a pragma on the next
line.
Text following the backslash character is ignored.
The line continuing the pragma must start with a comment character
('c', 'C' or '*').
.do end
.*
.beglevel
.*
.section Specifying Symbol Attributes
.*
.np
.ix 'symbol attributes'
.ix 'pragmas' 'auxiliary'
.ix 'auxiliary pragma'
Auxiliary pragmas are used to describe attributes that affect
.if '&cmpclass' eq 'load-n-go' .do begin
the method used for passing arguments.
.do end
.el .do begin
code generation.
.do end
Initially, the compiler defines a default set of attributes.
Each auxiliary pragma refers to one of the following.
.autopoint
.point
.if '&cmpclass' = 'load-n-go' .do begin
a symbol (such as a subroutine or function)
.do end
.el .do begin
a symbol (such as a variable or function)
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.point
a type definition that resolves to a function type
.do end
.point
the default set of attributes defined by the compiler
.endpoint
.np
When an auxiliary pragma refers to a particular symbol, a copy of the
current set of default attributes is made and merged with the
attributes specified in the auxiliary pragma.
The resulting attributes are assigned to the specified symbol and can
only be changed by another auxiliary pragma that refers to the same
symbol.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
An example of a type definition that resolves to a function type is the
following.
.millust begin
typedef void (*func_type)();
.millust end
.pc
When an auxiliary pragma refers to a such a type definition, a copy of the
current set of default attributes is made and merged with the
attributes specified in the auxiliary pragma.
The resulting attributes are assigned to each function whose type matches
the specified type definition.
.do end
.np
When "default" is specified instead of a symbol name,
the attributes specified by the auxiliary pragma change the default
set of attributes.
The resulting attributes are used by all symbols that have not been
specifically referenced by a previous auxiliary pragma.
.np
.if '&cmpclass' ne 'load-n-go' .do begin
Note that all auxiliary pragmas are processed before code generation
begins.
.do end
Consider the following example.
.millust begin
code in which symbol x is referenced
&pragma aux y <attrs_1>&epragma
code in which symbol y is referenced
code in which symbol z is referenced
&pragma aux default <attrs_2>&epragma
&pragma aux x <attrs_3>&epragma
.millust end
.np
Auxiliary attributes are assigned to
.id x,
.id y
and
.id z
in the following way.
.autonote
.note
Symbol
.id x
is assigned the initial default attributes merged with the attributes
specified by
.id <attrs_2>
and
.id <attrs_3>.
.note
Symbol
.id y
is assigned the initial default attributes merged with the attributes
specified by
.id <attrs_1>.
.note
Symbol
.id z
is assigned the initial default attributes merged with the attributes
specified by
.id <attrs_2>.
.endnote
.*
.section Alias Names
.*
.np
.ix 'alias name (pragma)'
When a symbol referred to by an auxiliary pragma includes an alias name,
the attributes of the alias name are also assumed by the specified symbol.
.np
There are two methods of specifying alias information.
In the first method, the symbol assumes only the attributes of the
alias name; no additional attributes can be specified.
The second method is more general since it is possible to specify
an alias name as well as additional auxiliary information.
In this case, the symbol assumes the attributes of the alias name as well as
the attributes specified by the additional auxiliary information.
.np
The simple form of the auxiliary pragma used to specify an alias is as
follows.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' and '&machine' eq '80386' .do begin
.mbox begin
:prgbeg. aux ( :id.sym:eid., :op.far16:eop. :id.alias:eid. ) :prgend.
.mbox end
.do end
.el .do begin
.mbox begin
:prgbeg. aux ( :id.sym:eid., :id.alias:eid. ) :prgend.
.mbox end
.do end
.synote
.note sym
is any valid &lang identifier.
.note alias
is the alias name and is any valid &lang identifier.
.esynote
.if '&lang' eq 'C' or '&lang' eq 'C/C++' and '&machine' eq '80386' .do begin
.im prag16
.do end
.np
Consider the following example.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
&pragma aux push_args parm [] &epragma
&pragma aux ( rtn, push_args ) &epragma
.millust end
.do end
.el .do begin
.millust begin
&pragma aux value_args parm (value) &epragma
&pragma aux ( rtn, value_args ) &epragma
.millust end
.do end
.pc
The routine
.id rtn
assumes the attributes of the alias name
.id push_args
which specifies that the arguments to
.id rtn
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
are passed on the stack.
.np
Let us look at an example in which the symbol is a type definition.
.millust begin
typedef void (func_type)(int);

#pragma aux push_args parm [];
#pragma aux ( func_type, push_args );

extern func_type rtn1;
extern func_type rtn2;
.millust end
.pc
The first auxiliary pragma defines an alias name called
.id push_args
that specifies the mechanism to be used to pass arguments.
The mechanism is to pass all arguments on the stack.
The second auxiliary pragma associates the attributes specified
in the first pragma with the type definition
.id func_type.
Since
.id rtn1
and
.id rtn2
are of type
.id func_type,
arguments to either of those functions will be passed on the stack.
.do end
.el .do begin
are passed by value.
.do end
.np
The general form of an auxiliary pragma that can be used to specify an
alias is as follows.
.ix 'pragmas' 'alias name'
.mbox begin
:prgbeg. aux ( :id.alias:eid. ) :id.sym:eid. :id.aux_attrs:eid. :prgend.
.mbox end
.synote
.note alias
is the alias name and is any valid &lang identifier.
.note sym
is any valid &lang identifier.
.note aux_attrs
are attributes that can be specified with the auxiliary pragma.
.esynote
.pc
Consider the following example.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '8086' .do begin
.ix 'Microsoft' 'C calling convention'
.ix 'calling convention' 'Microsoft C'
.millust begin
#pragma aux MS_C "_*"                                  \
                 parm caller []                        \
                 value struct float struct routine [ax]\
                 modify [ax bx cx dx es];
#pragma aux (MS_C) rtn1;
#pragma aux (MS_C) rtn2;
#pragma aux (MS_C) rtn3;
.millust end
.do end
.if '&machine' eq '80386' .do begin
.ix 'MetaWare' 'High C calling convention'
.ix 'calling convention' 'MetaWare High C'
.millust begin
#pragma aux HIGH_C "*"                                 \
                   parm caller []                      \
                   value no8087                        \
                   modify [eax ecx edx fs gs];
#pragma aux (HIGH_C) rtn1;
#pragma aux (HIGH_C) rtn2;
#pragma aux (HIGH_C) rtn3;
.millust end
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux WC "*_" parm (value)
*$pragma aux (WC) rtn1
*$pragma aux (WC) rtn2
*$pragma aux (WC) rtn3
.millust end
.do end
.pc
The routines
.id rtn1,
.id rtn2
and
.id rtn3
assume the same attributes as the
alias name
.id &alias_name
which defines the calling convention used by the &other_cmp compiler.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' and '&machine' eq '80386' .do begin
Note that register ES must also be specified in the "modify" register
set when using a memory model that is not a small data model.
.do end
Whenever calls are made to
.id rtn1,
.id rtn2
and
.id rtn3,
the &other_cmp calling convention will be used.
.if '&lang' eq 'FORTRAN 77' .do begin
Note that arguments must be passed by value.
.ix 'passing arguments by value'
By default, &cmpname passes arguments by reference.
.do end
.np
Note that if the attributes of
.id &alias_name
change, only one pragma needs to be changed.
If we had not used an alias name and specified the attributes in each
of the three pragmas for
.id rtn1,
.id rtn2
and
.id rtn3,
we would have to change all three pragmas.
This approach also reduces the amount of memory required by the
compiler to process the source file.
.warn
The alias name
.id &alias_name
is just another symbol.
If
.id &alias_name
appeared in your source code, it would assume the attributes specified
in the pragma for
.id &alias_name..
.ewarn
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section Predefined Aliases
.*
.np
A number of symbols are predefined by the compiler with a set of
attributes that describe a particular calling convention.
These symbols can be used as aliases.
The following is a list of these symbols.
.*
.begnote
.note __cdecl
.ix 'alias names' '__cdecl'
.ix '__cdecl alias name'
.kwm __cdecl
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'cdecl'
.ix 'cdecl alias name'
.kwm cdecl
.do end
defines the calling convention used by Microsoft compilers.
.note __fastcall
.ix 'alias names' '__fastcall'
.ix '__fastcall alias name'
.kwm __fastcall
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'fastcall'
.ix 'fastcall alias name'
.kwm fastcall
.do end
defines the calling convention used by Microsoft compilers.
.note __fortran
.ix 'alias names' '__fortran'
.ix '__fortran alias name'
.kwm __fortran
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'fortran'
.ix 'fortran alias name'
.kwm fortran
.do end
defines the calling convention used by &company. FORTRAN compilers.
.note __pascal
.kwm __pascal
.ix 'alias names' '__pascal'
.ix '__pascal alias name'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'pascal'
.ix 'pascal alias name'
.kwm pascal
.do end
defines the calling convention used by OS/2 1.x and Windows 3.x API
functions.
.note __stdcall
.ix 'alias names' '__stdcall'
.ix '__stdcall alias name'
.kwm __stdcall
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'stdcall'
.ix 'stdcall alias name'
.kwm stdcall
.do end
.if '&machine' eq '80386' .do begin
defines a special calling convention used by the Win32 API functions.
.do end
.el .do begin
defines the calling convention used by Microsoft compilers.
.do end
.if '&machine' eq '80386' .do begin
.note __syscall
.ix 'alias names' '__syscall'
.ix '__syscall alias name'
.kwm __syscall
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'syscall'
.ix 'syscall alias name'
.kwm syscall
.do end
defines the calling convention used by the 32-bit OS/2 API functions.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' and '&machine' eq '80386' .do begin
.note __system
.ix 'alias names' '__system'
.ix '__system alias name'
.kwm __system
or
.ix 'alias names' 'system'
.ix 'system alias name'
.kwm system
are identical to
.kwm __syscall
.ct .li .
.do end
.note __watcall
.ix 'alias names' '__watcall'
.ix '__watcall alias name'
.kwm __watcall
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or
.ix 'alias names' 'watcall'
.ix 'watcall alias name'
.kwm watcall
.do end
defines the calling convention used by &company compilers.
.endnote
.pc
The following describes the attributes of the above alias names.
.*
.beglevel
.*
.section Predefined "__cdecl" Alias
.*
.millust begin
&pragma aux __cdecl "_*" \
&pragcont           parm caller [] \
&pragcont           value struct float struct routine [&ax] \
.if '&machine' eq '80386' .do begin
&pragcont           modify [eax ecx edx]
.do end
.if '&machine' eq '8086' .do begin
&pragcont           modify [ax bx cx dx es]
.do end
.millust end
.autonote Notes:
.note
All symbols are preceded by an underscore character.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The calling routine will remove the arguments from the stack.
.note
Floating-point values are returned in the same way as structures.
When a structure is returned, the called routine allocates space
for the return value and returns a pointer to the return value in register
&axup..
.if '&machine' eq '8086' .do begin
.note
Registers AX, BX, CX and DX, and segment register ES are not saved and
restored when a call is made.
.do end
.if '&machine' eq '80386' .do begin
.note
Registers EAX, ECX and EDX are not saved and restored when a call is
made.
.do end
.endnote
.*
.section Predefined "__pascal" Alias
.*
.millust begin
&pragma aux __pascal "^" \
&pragcont           parm reverse routine [] \
&pragcont           value struct float struct caller [] \
.if '&machine' eq '8086' .do begin
&pragcont           modify [ax bx cx dx es]
.do end
.if '&machine' eq '80386' .do begin
&pragcont           modify [eax ebx ecx edx]
.do end
.millust end
.autonote Notes:
.note
All symbols are mapped to upper case.
.note
Arguments are pushed on the stack in reverse order.
That is, the first argument is pushed first, the second argument is pushed
next, and so on.
The routine being called will remove the arguments from the stack.
.note
Floating-point values are returned in the same way as structures.
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space will be pushed on the stack
immediately before the call instruction.
Upon returning from the call, register &axup will contain address of
the space allocated for the return value.
.if '&machine' eq '80386' .do begin
.note
Registers EAX, EBX, ECX and EDX are not saved and restored when a call
is made.
.do end
.el .do begin
.note
Registers AX, BX, CX and DX, and segment register ES are not saved and
restored when a call is made.
.do end
.endnote
.*
.if '&machine' eq '80386' .do begin
.*
.section Predefined "__stdcall" Alias
.*
.millust begin
&pragma aux __stdcall "_*@nnn" \
&pragcont           parm routine [] \
&pragcont           value struct struct caller [] \
&pragcont           modify [eax ecx edx]
.millust end
.autonote Notes:
.note
All symbols are preceded by an underscore character.
.note
All C symbols (extern "C" symbols in C++) are suffixed by "@nnn" where
"nnn" is the sum of the argument sizes (each size is rounded up to a
multiple of 4 bytes so that char and short are size 4).
When the argument list contains "...", the "@nnn" suffix is omitted.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The called routine will remove the arguments from the stack.
.note
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space will be pushed on the stack
immediately before the call instruction.
Upon returning from the call, register EAX will contain address of
the space allocated for the return value.
Floating-point values are returned in 80x87 register ST(0).
.note
Registers EAX, ECX and EDX are not saved and restored when a call is
made.
.endnote
.*
.section Predefined "__syscall" Alias
.*
.millust begin
&pragma aux __syscall "*" \
&pragcont           parm caller [] \
&pragcont           value struct struct caller [] \
&pragcont           modify [eax ecx edx]
.millust end
.autonote Notes:
.note
Symbols names are not modified, that is, they are not adorned with
leading or trailing underscores.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The calling routine will remove the arguments from the stack.
.note
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space will be pushed on the stack
immediately before the call instruction.
Upon returning from the call, register EAX will contain address of
the space allocated for the return value.
Floating-point values are returned in 80x87 register ST(0).
.note
Registers EAX, ECX and EDX are not saved and restored when a call is
made.
.endnote
.*
.do end
.*
.if '&machine' eq '80386' .do begin
.section Predefined "__watcall" Alias (register calling convention)
.do end
.el .do begin
.section Predefined "__watcall" Alias
.do end
.*
.millust begin
&pragma aux __watcall "*_" \
.if '&machine' eq '80386' .do begin
&pragcont           parm routine [eax ebx ecx edx] \
.do end
.if '&machine' eq '8086' .do begin
&pragcont           parm routine [ax bx cx dx] \
.do end
&pragcont           value struct caller
.millust end
.autonote Notes:
.note
Symbol names are followed by an underscore character.
.note
Arguments are processed from left to right. The leftmost arguments
are passed in registers and the rightmost arguments are passed on
the stack (if the registers used for argument passing have been
exhausted). Arguments that are passed on the stack are pushed from
right to left. The calling routine will remove the arguments if any
were pushed on the stack.
.note
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space is put into &siup register. 
The called routine then places the return value there.
Upon returning from the call, register &axup will contain address of
the space allocated for the return value.
.note
Floating-point values are returned using 80x86 registers ("fpc" option)
or using 80x87 floating-point registers ("fpi" or "fpi87" option).
.note
All registers must be preserved by the called routine.
.endnote
.*
.if '&machine' eq '80386' .do begin
.section Predefined "__watcall" Alias (stack calling convention)
.*
.millust begin
&pragma aux __watcall "*" \
&pragcont           parm caller [] \
&pragcont           value no8087 struct caller \
&pragcont           modify [eax ecx edx 8087]
.millust end
.autonote Notes:
.note
All symbols appear in object form as they do in source form.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The calling routine will remove the arguments from the stack.
.note
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space will be pushed on the stack
immediately before the call instruction.
Upon returning from the call, register &axup will contain address of
the space allocated for the return value.
.note
Floating-point values are returned only using 80x86 registers.
.note
Registers EAX, ECX and EDX are not preserved by the called routine.
.note
Any local variables that are located in the 80x87 cache are not
preserved by the called routine.
.endnote
.*
.do end
.*
.endlevel
.*
.do end
.* end of if '&cmpclass' ne 'load-n-go'
.*
.section Alternate Names for Symbols
.*
.np
The following form of the auxiliary pragma can be used to describe the
mapping of a symbol from its source form to its object form.
.ix 'pragmas' 'alternate name'
.mbox begin
:prgbeg. aux :id.sym:eid. :id.obj_name:eid. :prgend.
.mbox end
.synote
.note sym
is any valid &lang identifier.
.note obj_name
is any character string enclosed in double quotes.
.esynote
.pc
When specifying
.id obj_name,
some characters have a special meaning:
.synote
.note *
is unmodified symbol name
.note ^
is symbol name converted to uppercase
.note !
is symbol name converted to lowercase
.note #
is a placeholder for "@nnn", where nnn is size of all function parameters
on the stack; it is ignored for functions with variable argument lists, or
for symbols that are not functions
.note \
next character is treated as literal
.esynote
.np
Several examples of source to object form symbol name translation follow:
.if '&lang' eq 'FORTRAN 77' .do begin
By default, the upper case version "MYRTN" or "MYVAR" is placed in the object file.
.do end
.np
In the following example, the name "MyRtn" will be replaced by
.if '&lang' eq 'FORTRAN 77' .do begin
"MYRTN_"
.do end
.el .do begin
"MyRtn_"
.do end
in the object file.
.millust begin
.if '&lang' eq 'FORTRAN 77' .do begin
&pragma aux MyRtn "^_"&epragma
.do end
.el .do begin
&pragma aux MyRtn "*_"&epragma
.do end
.millust end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.pc
This is the default for all function names.
.do end
.np
In the following example, the name "MyVar" will be replaced by
.if '&lang' eq 'FORTRAN 77' .do begin
"_MYVAR"
.do end
.el .do begin
"_MyVar"
.do end
in the object file.
.millust begin
.if '&lang' eq 'FORTRAN 77' .do begin
&pragma aux MyVar "_^"&epragma
.do end
.el .do begin
&pragma aux MyVar "_*"&epragma
.do end
.millust end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.pc
This is the default for all variable names.
.do end
.np
In the following example, the lower case version "myrtn" will be placed in
the object file.
.millust begin
&pragma aux MyRtn "!"&epragma
.millust end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
In the following example, the upper case version "MYRTN" will be placed in
the object file.
.millust begin
&pragma aux MyRtn "^"&epragma
.millust end
.do end
.*
.np
In the following example, the name "MyRtn" will be replaced by
"_MyRtn@nnn" in the object file. "nnn" represents the size of all function
parameters.
.millust begin
&pragma aux MyRtn "_*#"&epragma
.millust end
.np
In the following example, the name "MyRtn" will be replaced by
"_MyRtn#" in the object file.
.millust begin
&pragma aux MyRtn "_*\#"&epragma
.millust end
.*
.np
The default mapping for all symbols can also be changed as illustrated
by the following example.
.millust begin
.if '&lang' eq 'FORTRAN 77' .do begin
&pragma aux default "_^_"&epragma
.do end
.el .do begin
&pragma aux default "_*_"&epragma
.do end
.millust end
.pc
The above auxiliary pragma specifies that all names will be prefixed
and suffixed by an underscore character ('_').
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section Describing Calling Information
.*
.np
.ix 'calling &functions' 'near'
.ix 'calling &functions' 'far'
The following form of the auxiliary pragma can be used to describe the
way a &function is to be called.
.ix 'pragmas' 'calling information'
.ix 'pragmas' 'far'
.if '&lang' eq 'FORTRAN 77' and '&machine' eq '80386' .do begin
.ix 'pragmas' 'far16'
.do end
.ix 'pragmas' 'near'
.ix 'pragmas' '= const'
.ix 'pragmas' 'in-line assembly'
.ix 'calling information (pragma)'
.ix 'far (pragma)'
.if '&lang' eq 'FORTRAN 77' and '&machine' eq '80386' .do begin
.ix 'far16 (pragma)'
.do end
.ix 'near (pragma)'
.ix 'in-line assembly' 'in pragmas'
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.* ---------------------------------------
.if '&machine' eq '8086' .do begin
.mbox begin
:prgbeg. aux :id.sym:eid. far :prgend.
    or
:prgbeg. aux :id.sym:eid. near :prgend.
    or
:prgbeg. aux :id.sym:eid. = :id.in_line:eid. :prgend.

:id.in_line ::= { const | (:eid.seg:id. id) | (:eid.offset:id. id) | (:eid.reloff:id. id)
                    | (:eid.float:id. fpinst) | :eid.":id.asm:eid." :id.}:eid.
.mbox end
.do end
.* ---------------------------------------
.if '&machine' eq '80386' .do begin
.mbox begin
:prgbeg. aux :id.sym:eid. far :prgend.
    or
:prgbeg. aux :id.sym:eid. near :prgend.
    or
:prgbeg. aux :id.sym:eid. = :id.in_line:eid. :prgend.

:id.in_line ::= { const | (:eid.seg:id. id) | (:eid.offset:id. id) | (:eid.reloff:id. id)
                    | :eid.":id.asm:eid." :id.}:eid.
.mbox end
.do end
.* ---------------------------------------
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.* ---------------------------------------
.if '&machine' eq '8086' .do begin
.mbox begin
:prgbeg. aux :id.sym:eid. far
    or
:prgbeg. aux :id.sym:eid. near
    or
:prgbeg. aux :id.sym:eid. = :id.in_line:eid.

:id.in_line ::= { const | :eid.":id.asm:eid.":id. | (:eid.float:id. fpinst) }:eid.
.mbox end
.do end
.* ---------------------------------------
.if '&machine' eq '80386' .do begin
.mbox begin
:prgbeg. aux :id.sym:eid. far
    or
:prgbeg. aux :id.sym:eid. far16
    or
:prgbeg. aux :id.sym:eid. near
    or
:prgbeg. aux :id.sym:eid. = :id.in_line:eid.

:id.in_line ::= { const | :eid.":id.asm:eid.":id. }:eid.
.mbox end
.do end
.* ---------------------------------------
.do end
.*
.synote
.note sym
is a &function name.
.note const
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
is a valid &lang integer constant.
.note id
is any valid &lang identifier.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
is a valid &lang hexadecimal constant.
.do end
.if '&machine' eq '8086' .do begin
.note fpinst
is a sequence of bytes that forms a valid 80x87 instruction.
The keyword
.kw float
must precede
.id fpinst
so that special fixups are applied to the 80x87 instruction.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.note seg
specifies the segment of the symbol
.id id.
.note offset
specifies the offset of the symbol
.id id.
.note reloff
specifies the relative offset of the symbol
.id id
for near control transfers.
.do end
.note asm
is an assembly language instruction or directive.
.esynote
.pc
In the following example, &cmpname will generate a far call to the
&function
.id myrtn.
.millust begin
&pragma aux myrtn far&epragma
.millust end
.pc
Note that this overrides the calling sequence that would normally be
generated for a particular memory model.
In other words, a far call will be generated even if you are compiling
for a memory model with a small code model.
.np
In the following example, &cmpname will generate a near call to the
&function
.id myrtn.
.millust begin
&pragma aux myrtn near&epragma
.millust end
.pc
Note that this overrides the calling sequence that would normally be
generated for a particular memory model.
In other words, a near call will be generated even if you are
compiling for a memory model with a big code model.
.np
In the following DOS example, &cmpname will generate the sequence of
bytes following the "=" character in the auxiliary pragma whenever a
call to
.id mode4
is encountered.
.id mode4
is called an in-line &function..
.ix 'in-line &functions'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
void mode4(void);
#pragma aux mode4 =                \
    0xb4 0x00       /* mov AH,0 */ \
    0xb0 0x04       /* mov AL,4 */ \
    0xcd 0x10       /* int 10H  */ \
    modify [ AH AL ];
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux mode4 =    \
*    zb4 z00            \ mov AH,0
*    zb0 z04            \ mov AL,4
*    zcd z10            \ int 10h
*    modify [ AH AL ]
.millust end
.do end
.pc
The sequence in the above DOS example represents the following lines
of assembly language instructions.
.millust begin
mov   AH,0       ; select function "set mode"
mov   AL,4       ; specify mode (mode 4)
int   10H        ; BIOS video call
.millust end
.pc
The above example demonstrates how to generate BIOS function calls
in-line without writing an assembly language function and calling it
from your &lang program.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The C prototype for the function
.id mode4
is not necessary but is included so that we can take advantage of the
argument type checking provided by &cmpname..
.do end
.np
.ix 'in-line assembly language instructions' 'using mnemonics'
The following DOS example is equivalent to the above example but
mnemonics for the assembly language instructions are used instead of
the binary encoding of the assembly language instructions.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
void mode4(void);
#pragma aux mode4 =     \
    "mov AH,0",         \
    "mov AL,4",         \
    "int 10H"           \
    modify [ AH AL ];
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux mode4 =    \
*    "mov AH,0"         \
*    "mov AL,4"         \
*    "int 10H"          \
*    modify [ AH AL ]
.millust end
.do end
.if '&machine' eq '8086' .do begin
.np
.ix 'in-line 80x87 floating-point instructions'
If a sequence of in-line assembly language instructions contains 80x87
floating-point instructions, each floating-point instruction must be
preceded by "float".
Note that this is only required if you have specified the "fpi"
compiler option; otherwise it will be ignored.
.np
The following example generates the 80x87 "square root" instruction.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
double mysqrt(double);
#pragma aux mysqrt parm [8087] = \
    float 0xd9 0xfa /* fsqrt */;
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux mysqrt parm( value ) [8087] = \
*            float zd9fa
.millust end
.do end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
.ix 'symbolic references in in-line code sequences'
A sequence of in-line assembly language instructions may contain
symbolic references.
In the following example, a near call to the function
.id myalias
is made whenever
.id myrtn
is called.
.millust begin
extern void myalias(void);
void myrtn(void);
#pragma aux myrtn =                     \
    0xe8 reloff myalias /* near call */;
.millust end
.pc
In the following example, a far call to the function
.id myalias
is made whenever
.id myrtn
is called.
.millust begin
extern void myalias(void);
void myrtn(void);
#pragma aux myrtn =                                \
    0x9a offset myalias seg myalias /* far call */;
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' and '&machine' eq '80386' .do begin
.im prag16
.np
The file :FNAME.bsesub.fap:eFNAME. in the
:FNAME.&pathnam.&pc.src&pc.fortran&pc.os2:eFNAME. directory contains examples
of pragmas that use the
.kwm far16
attribute to describe the 16-bit VIO, KBD and MOU subsystems available in
32-bit OS/2.
.do end
.*
.beglevel
.*
.section Loading Data Segment Register
.*
.np
.ix 'loading DS before calling a &function'
An application may have been compiled so that the segment register DS
does not contain the segment address of the default data segment
(group "DGROUP").
This is usually the case if you are using a large data memory model.
Suppose you wish to call a &function that assumes that the segment
register DS contains the segment address of the default data segment.
It would be very cumbersome if you were forced to compile your
application so that the segment register DS contained the default data
segment (a small data memory model).
.np
The following form of the auxiliary pragma will cause the segment
register DS to be loaded with the segment address of the default data
segment before calling the specified &function..
.ix 'pragmas' 'loadds'
.ix 'loadds (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. parm loadds :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.np
.ix 'loading DS in prologue sequence of a &function'
Alternatively, the following form of the auxiliary pragma will cause
the segment register DS to be loaded with the segment address of the
default data segment as part of the prologue sequence for the
specified &function..
.mbox begin
:prgbeg. aux :id.sym:eid. loadds :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.*
.section Defining Exported Symbols in Dynamic Link Libraries
.*
.np
.ix 'exporting symbols in dynamic link libraries'
An exported symbol in a dynamic link library is a symbol that can be
referenced by an application that is linked with that dynamic link
library.
Normally, symbols in dynamic link libraries are exported using the
&lnkname "EXPORT" directive.
An alternative method is to use the following form of the auxiliary
pragma.
.ix 'pragmas' 'export'
.ix 'export (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. export :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.*
.if '&machine' eq '8086' .do begin
.*
.section Defining Windows Callback Functions
.*
.np
.ix 'callback functions'
When compiling a Microsoft Windows application, you must use the
"&winopt" option so that special prologue/epilogue sequences are
generated.
Furthermore, callback functions require larger prologue/epilogue
sequences than those generated when the "&winopt" compiler option is
specified.
The following form of the auxiliary pragma will cause a callback
prologue/epilogue sequence to be generated for a callback function
when compiled using the "&winopt" option.
.ix 'pragmas' 'export'
.ix 'export (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. export :prgend.
.mbox end
.synote
.note sym
is a callback function name.
.esynote
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
Alternatively, the "zw" compiler option can be used to generate callback
prologue/epilogue sequences.
However, all functions contained in a module compiled using the "zw"
option will have a callback prologue/epilogue sequence even if the
functions are not callback functions.
.do end
.*
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section Forcing a Stack Frame
.*
.np
.ix 'stack frame'
Normally, a function contains a stack frame if arguments are passed on
the stack or an automatic variable is allocated on the stack.
No stack frame will be generated if the above conditions are not satisfied.
The following form of the auxiliary pragma will force a stack frame to
be generated under any circumstance.
.ix 'pragmas' 'frame'
.ix 'frame (pragma)'
.ix 'stack frame (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. frame :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.do end
.*
.endlevel
.*
.do end
.* end of if '&cmpclass' ne 'load-n-go'
.*
.section Describing Argument Information
.*
.np
.ix 'argument list (pragma)'
.ix 'pragmas' 'describing argument lists'
Using auxiliary pragmas, you can describe the calling convention that
&cmpname is to use for calling &functions..
This is particularly useful when interfacing to &functions that have
been compiled by other compilers or &functions written in other
programming languages.
.if '&cmpclass' ne 'load-n-go' .do begin
.np
The general form of an auxiliary pragma that describes argument
passing is the following.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.mbox begin
:prgbeg. aux :id.sym:eid. parm :id.:rp. pop_info :or. :eid.reverse:id. :or. :rp.reg_set:erp. :erp.:eid. :prgend.

:id.pop_info ::=:eid. caller :or. routine
.mbox end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.cp 19
.mbox begin
:prgbeg. aux :id.sym:eid. parm :id.:rp. arg_info :or. pop_info :or. :eid.reverse:id. :rp.reg_set:erp. :erp.:eid.

:id.arg_info ::=:eid. ( :id.arg_attr:eid. :rp., :id.arg_attr:eid.:erp. )

:id.arg_attr ::=:eid. value :op.:id.v_attr:eid.:eop.
                :or. reference :op.:id.r_attr:eid.:eop.
                :or. data_reference :op.:id.d_attr:eid.:eop.

:id.v_attr ::=:eid. far :or. near :or. *1 :or. *2 :or. *4 :or. *8

:id.r_attr ::=:eid. :op.far :or. near:eop. :op.descriptor :or. nodescriptor:eop.

:id.d_attr ::=:eid. :op.far :or. near:eop.

:id.pop_info ::=:eid. caller :or. routine
.mbox end
.do end
.synote
.note sym
is a &function name.
.note reg_set
is called a register set.
The register sets specify the registers that are to be used for
argument passing.
A register set is a list of registers separated by spaces and enclosed
in square brackets.
.esynote
.do end
.*
.beglevel
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section Passing Arguments to non-FORTRAN Subprograms
.*
.np
When calling a subprogram written in a different language, it may be
necessary to provide the arguments in a form different than the
default methods used by &cmpname..
For example, C functions require scalar arguments to be passed by
value instead of by reference.
For information on the methods &cmpname uses to pass arguments, see
the chapter entitled "Assembly Language Considerations".
.np
The following form of the auxiliary pragma can be used to alter the
default calling mechanism used for passing arguments.
.if '&cmpclass' ne 'load-n-go' .do begin
.cp 15
.mbox begin
:prgbeg. aux :id.sym:eid. parm ( :id.arg_attr:eid. :rp., :id.arg_attr:eid.:erp. )

:id.arg_attr ::=:eid. value :op.:id.v_attr:eid.:eop.
                :or. reference :op.:id.r_attr:eid.:eop.
                :or. data_reference :op.:id.d_attr:eid.:eop.

:id.v_attr ::=:eid. far :or. near :or. *1 :or. *2 :or. *4 :or. *8

:id.r_attr ::=:eid. :op.far :or. near:eop. :op.descriptor :or. nodescriptor:eop.

:id.d_attr ::=:eid. :op.far :or. near:eop.
.mbox end
.do end
.el .do begin
.cp 11
.mbox begin
:prgbeg. aux :id.sym:eid. parm ( :id.arg_attr:eid. :rp., :id.arg_attr:eid.:erp. )

:id.arg_attr ::=:eid. value:id. :op.:id.v_attr:eid.:eop. :or. reference :op.:id.r_attr:eid.:eop.

:id.v_attr ::=:eid. far :or. near :or. *1 :or. *2 :or. *4 :or. *8

:id.r_attr ::=:eid. :op.far :or. near:eop.
.mbox end
.do end
.synote
.note sym
is a &function name.
.note REFERENCE
.ix 'arguments' 'passing by reference'
specifies that arguments are to be passed by reference.
For non-character arguments, the address is a pointer to the data.
For character arguments, the address is a pointer to a string descriptor.
See the chapter entitled "Assembly Language Considerations" for a description
of a string descriptor.
This is the default calling mechanism.
If "NEAR" or "FAR" is specified, a near pointer or far pointer is passed
regardless of the memory model used at compile-time.
.if '&cmpclass' ne 'load-n-go' .do begin
.np
If the "DESCRIPTOR" attribute is specified, a pointer to the string descriptor
is passed.
This is the default.
If the "NODESCRIPTOR" attribute is specified, a pointer to the the actual
character data is passed instead of a pointer to the string descriptor.
.do end
.if '&cmpclass' ne 'load-n-go' .do begin
.note DATA_REFERENCE
.ix 'arguments' 'passing by data reference'
specifies that arguments are to be passed by data reference.
For non-character items, this is identical to passing by reference.
For character items, a pointer to the actual character data (instead of the
string descriptor) is passed.
If "NEAR" or "FAR" is specified, a near pointer or far pointer is passed
regardless of the memory model used at compile-time.
.do end
.note VALUE
.ix 'arguments' 'passing by value'
specifies that arguments are to be passed by value.
Character arguments are treated specially when passed by value.
Instead of passing a pointer to a string descriptor, a pointer to the
actual character data is passed.
See the chapter entitled "Assembly Language Considerations" for a description
of a string descriptor.
.esynote
.autonote Notes:
.note
Arrays and subprograms are always passed by reference, regardless of the
argument attribute specified.
.note
When character arguments are passed by reference, the address of a string
descriptor is passed.
The string descriptor contains the address of the actual character data and
the number of characters.
When character arguments are passed by
.if '&cmpclass' ne 'load-n-go' .do begin
value or data reference,
.do end
.el .do begin
value,
.do end
the address of the actual character data is passed instead of the address
of a string descriptor.
Character arguments are passed by value by specifying the "VALUE"
.if '&cmpclass' ne 'load-n-go' .do begin
or "DATA_REFERENCE"
.do end
attribute.
If "NEAR" or "FAR" is specified, a near pointer or far pointer to the
character data is passed regardless of the memory model used at compile-time.
.note
When complex arguments are passed by value, the real part and the imaginary
part are passed as two separate arguments.
.note
When an argument is a user-defined structure and is passed by value,
a copy of the structure is made and passed as an argument.
.note
For scalar arguments, arguments of type
.bd INTEGER*1
.ct ,
.bd INTEGER*2
.ct ,
.bd INTEGER*4
ct ,
.bd REAL
or
.bd DOUBLE PRECISION
.ct , a length specification can be specified
when the "VALUE" attribute is specified to pass the argument by value.
This length specification refers to the size of the argument; the compiler
will convert the actual argument to a type that matches the size.
For example, if an argument of type
.bd REAL
is passed to a subprogram that
has an argument attribute of "VALUE*8", the argument will be converted to
.bd DOUBLE PRECISION
.ct .li .
If an argument of type
.bd DOUBLE PRECISION
is passed to a subprogram that
has an argument attribute of "VALUE*4", the argument will be converted to
.bd REAL
.ct .li .
If an argument of type
.bd INTEGER*4
is passed to a subprogram that
has an argument attribute of "VALUE*2" or VALUE*1, the argument will be
converted to
.bd INTEGER*2
or
.bd INTEGER*1
.ct .li .
If an argument of type
.bd INTEGER*2
is passed to a subprogram that
has an argument attribute of "VALUE*4 or VALUE*1", the argument will be
converted to
.bd INTEGER*4
or
.bd INTEGER*1
.ct .li .
If an argument of type
.bd INTEGER*1
is passed to a subprogram that
has an argument attribute of "VALUE*4 or VALUE*2", the argument will be
converted to
.bd INTEGER*4
or
.bd INTEGER*2
.ct .li .
.note
If the number of arguments exceeds the number of entries in the
argument-attribute list, the last attribute will be assumed for the
remaining arguments.
.endnote
.np
.if '&cmpclass' ne 'load-n-go' .do begin
Consider the following example.
.millust begin
*$pragma aux printf "*_" parm (value) caller []
      character cr/z0d/, nullchar/z00/
      call printf( 'values: %ld, %ld'//cr//nullchar,
     1             77, 31410 )
      end
.millust end
.pc
The C "printf" function is called with three arguments.
The first argument is of type
.bd CHARACTER
and is passed as a C string (address of actual data terminated by a
null character).
The second and third arguments are passed by value.
Also note that "printf" is a function that takes a variable number of
arguments, all passed on the stack (an empty register set was
specified), and that the caller must remove the arguments from the
stack.
.do end
.el .do begin
Consider the following example.
.millust begin
*$pragma aux c_function "*_" parm (value)
      call c_function( 383, 13143, 1033 )
      end
.millust end
.pc
The function
.id c_function
is a function compiled using &company C.
It is called with three arguments, all passed by value.
.do end
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section Passing Arguments in Registers
.*
.np
The following form of the auxiliary pragma can be used to specify the
registers that are to be used to pass arguments to a particular
&function..
.ix 'pragmas' 'parm'
.ix 'parm (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. parm :rp.:id.reg_set:eid.:erp. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is called a register set.
The register sets specify the registers that are to be used for
argument passing.
A register set is a list of registers separated by spaces and enclosed
in square brackets.
.esynote
.np
Register sets establish a priority for register allocation during
argument list processing.
Register sets are processed from left to right.
However, within a register set, registers are chosen in any order.
Once all register sets have been processed, any remaining arguments
are pushed on the stack.
.np
Note that regardless of the register sets specified, only certain
combinations of registers will be selected for arguments of a
particular type.
.np
Note that arguments of type
.bd &single
and
.bd &double
are always pushed on the stack when the "fpi" or "fpi87" option is
used.
.begnote $break
.note &double
Arguments of type
.bd &double
.if '&lang' eq 'FORTRAN 77' .do begin
.ct , when passed by value,
.do end
can only be passed in
.if '&machine' eq '8086' .do begin
the following register combination: AX:BX:CX:DX.
.do end
.if '&machine' eq '80386' .do begin
one of the following register pairs:
EDX:EAX, ECX:EBX, ECX:EAX, ECX:ESI, EDX:EBX, EDI:EAX, ECX:EDI,
EDX:ESI, EDI:EBX, ESI:EAX, ECX:EDX, EDX:EDI, EDI:ESI, ESI:EBX or
EBX:EAX.
.do end
For example, if the following register set was specified for a routine
having an argument of type
.bd &double
.ct ,
.if '&machine' eq '8086' .do begin
.millust begin
[AX BX SI DI]
.millust end
.do end
.if '&machine' eq '80386' .do begin
.millust begin
[EBP EBX]
.millust end
.do end
.pc
the argument would be pushed on the stack since a valid register
combination for 8-byte arguments is not contained in the register set.
Note that this method for passing arguments of type
.bd &double
is supported only when the "fpc" option is used.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Note that this argument passing method does not include the passing of
8-byte structures.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
Note that this argument passing method does not include arguments of
type
.bd COMPLEX*8
or user-defined structures whose size is 8 bytes when these arguments
are passed by value.
.do end
.note far pointer
A far pointer can only be passed in one of the following register
pairs:
DX:&axup, CX:&bxup, CX:&axup, CX:&siup, DX:&bxup, DI:&axup, CX:&diup,
DX:&siup, DI:&bxup, SI:&axup, CX:&dxup, DX:&diup, DI:&siup, SI:&bxup,
BX:&axup,
.if '&machine' eq '80386' .do begin
FS:ECX, FS:EDX, FS:EDI, FS:ESI, FS:EBX, FS:EAX,
GS:ECX, GS:EDX, GS:EDI, GS:ESI, GS:EBX, GS:EAX,
.do end
DS:&cxup, DS:&dxup, DS:&diup, DS:&siup, DS:&bxup, DS:&axup,
ES:&cxup, ES:&dxup, ES:&diup, ES:&siup, ES:&bxup or ES:&axup..
For example, if a far pointer is passed to a function with the
following register set,
.millust begin
[ES &bpup]
.millust end
.pc
the argument would be pushed on the stack since a valid register
combination for a far pointer is not contained in the register set.
.if '&lang' eq 'FORTRAN 77' .do begin
Far pointers are used to pass arguments by reference in a big data
memory model.
.do end
.if '&machine' eq '8086' .do begin
.note &long_int, &single
The only registers that will be assigned to 4-byte arguments (e.g.,
arguments of type
.bd &long_int
.ct ,
.if '&lang' eq 'FORTRAN 77' .do begin
when passed by value
.do end
.ct )
are:
DX:AX, CX:BX, CX:AX, CX:SI, DX:BX, DI:AX, CX:DI, DX:SI, DI:BX, SI:AX,
CX:DX, DX:DI, DI:SI, SI:BX and BX:AX.
For example, if the following register set was specified for a routine
with one argument of type
.bd &long_int
.ct ,
.millust begin
[ES DI]
.millust end
.pc
the argument would be pushed on the stack since a valid register
combination for 4-byte arguments is not contained in the register set.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Note that this argument passing method includes 4-byte structures.
.do end
Note that this argument passing method includes arguments of type
.bd &single
but only when the "fpc" option is used.
.do end
.note &machint
The only registers that will be assigned to &intsize.-byte arguments
(e.g., arguments of type
.bd &machint
.if '&lang' eq 'FORTRAN 77' .do begin
when passed by value or arguments passed by reference in a small data
memory model
.do end
.ct )
are:
&axup, &bxup, &cxup, &dxup, &siup and &diup..
For example, if the following register set was specified for a routine
with one argument of type
.bd &machint
.ct ,
.millust begin
[&bpup]
.millust end
.pc
the argument would be pushed on the stack since a valid register
combination for &intsize.-byte arguments is not contained in the
register set.
.if '&machine' eq '80386' .do begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Note that this argument passing method includes 4-byte structures.
.do end
Note that this argument passing method also includes arguments of type
.bd &single
but only when the "fpc" option is used.
.do end
.if '&machine' eq '80386' .do begin
.note &char, &short_int
.do end
.el .do begin
.note &char
.do end
Arguments whose size is 1 byte
.if '&machine' eq '80386' .do begin
or 2 bytes
.do end
(e.g., arguments of type
.bd &char
.if '&machine' eq '80386' .do begin
and
.bd &short_int
as well as 2-byte structures
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
when passed by value
.do end
.ct )
are promoted to &intsize bytes and are then assigned registers as if
they were &intsize.-byte arguments.
.note others
Arguments that do not fall into one of the above categories cannot be
passed in registers and are pushed on the stack.
Once an argument has been assigned a position on the stack, all
remaining arguments will be assigned a position on the stack even if
all register sets have not yet been exhausted.
.endnote
.autonote Notes:
.note
The default register set is [&axup &bxup &cxup &dxup].
.note
Specifying registers AH and AL is equivalent to specifying register AX.
Specifying registers DH and DL is equivalent to specifying register DX.
Specifying registers CH and CL is equivalent to specifying register CX.
Specifying registers BH and BL is equivalent to specifying register BX.
.if '&machine' eq '80386' .do begin
Specifying register EAX implies that register AX has been specified.
Specifying register EBX implies that register BX has been specified.
Specifying register ECX implies that register CX has been specified.
Specifying register EDX implies that register DX has been specified.
Specifying register EDI implies that register DI has been specified.
Specifying register ESI implies that register SI has been specified.
Specifying register EBP implies that register BP has been specified.
Specifying register ESP implies that register SP has been specified.
.do end
.note
If you are compiling for a memory model with a small data model,
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
or the "zdp" compiler option is specified,
.do end
any register combination containing register DS becomes illegal.
In a small data model, segment register DS must remain unchanged as it
points to the program's data segment.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Note that the "zdf" compiler option can be used to specify that
register DS does not contain that segment address of the program's
data segment.
In this case, register combinations containing register DS are legal.
.do end
.if '&machine' eq '80386' .do begin
.note
If you are compiling for the flat memory model, any register
combination containing DS or ES becomes illegal.
In a flat memory model, code and data reside in the same segment.
Segment registers DS and ES point to this segment and must remain
unchanged.
.do end
.endnote
.np
Consider the following example.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux myrtn parm (value) \
*                       [&ax &bx &cx &dx] [&bp &si]
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#pragma aux myrtn parm [&ax &bx &cx &dx] [&bp &si];
.millust end
.do end
.pc
Suppose
.id myrtn
is a routine with 3 arguments each of type
.bd &arg_2_regs
.ct .li .
.if '&lang' eq 'FORTRAN 77' .do begin
Note that the arguments are passed by value.
.do end
.autopoint
.point
The first argument will be passed in the register pair &dxup:&axup..
.point
The second argument will be passed in the register pair &cxup:&bxup..
.point
The third argument will be pushed on the stack since &bpup:&siup is
not a valid register pair for arguments of type
.bd &arg_2_regs
.ct .li .
.endpoint
.np
It is possible for registers from the second register set to be used
before registers from the first register set are used.
Consider the following example.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux myrtn parm (value) \
*                       [&ax &bx &cx &dx] [&si &di]
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#pragma aux myrtn parm [&ax &bx &cx &dx] [&si &di];
.millust end
.do end
.pc
Suppose
.id myrtn
is a routine with 3 arguments,
the first of type
.bd &int
and the second and third of type
.bd &arg_2_regs
.ct .li .
.if '&lang' eq 'FORTRAN 77' .do begin
Note that all arguments are passed by value.
.do end
.autopoint
.point
The first argument will be passed in the register &axup..
.point
The second argument will be passed in the register pair &cxup:&bxup..
.point
The third argument will be passed in the register set &diup:&siup..
.endpoint
.pc
Note that registers are no longer selected from a register set after
registers are selected from subsequent register sets, even if all
registers from the original register set have not been exhausted.
.np
An empty register set is permitted.
All subsequent register sets appearing after an empty register set are
ignored; all remaining arguments are pushed on the stack.
.autonote Notes:
.note
.ix 'arguments on the stack'
.ix 'stacking arguments'
If a single empty register set is specified, all arguments are passed
on the stack.
.note
If no register set is specified, the default register set [&axup &bxup
&cxup &dxup] is used.
.endnote
.*
.section Forcing Arguments into Specific Registers
.*
.np
It is possible to force arguments into specific registers.
Suppose you have a &function, say "mycopy", that copies data.
The first argument is the source, the second argument is the
destination, and the third argument is the length to copy.
If we want the first argument to be passed in the register &siup, the
second argument to be passed in register &diup and the third argument
to be passed in register &cxup, the following auxiliary pragma can be
used.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
void mycopy( char near *, char *, int );
#pragma aux mycopy parm [&siup] [&diup] [&cxup];
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux mycopy parm (value) \
*                        [&siup] [&diup] [&cxup]
      character*10  dst
      call mycopy( dst, '0123456789', 10 )
      ...
      end
.millust end
.do end
.pc
Note that you must be aware of the size of the arguments to ensure
that the arguments get passed in the appropriate registers.
.*
.section Passing Arguments to In-Line &ufunctions
.*
.np
.ix 'pragmas' 'in-line &functions'
.ix 'in-line &functions (pragma)'
For &functions whose code is generated by &cmpname and whose argument
list is described by an auxiliary pragma, &cmpname has some freedom in
choosing how arguments are assigned to registers.
Since the code for in-line &functions is specified by the programmer,
the description of the argument list must be very explicit.
To achieve this, &cmpname assumes that each register set corresponds
to an argument.
Consider the following DOS example of an in-line &function called
.id scrollactivepgup.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
void scrollactivepgup(char,char,char,char,char,char);
#pragma aux scrollactivepgup = \
    "mov AH,6"   \
    "int 10h"    \
    parm [ch] [cl] [dh] [dl] [al] [bh] \
    modify [ah];
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux scrollactivepgup =        \
*   "mov AH,6"                         \
*   "int 10h"                          \
*   parm (value)                       \
*        [ch] [cl] [dh] [dl] [al] [bh] \
*   modify [ah]
.millust end
.do end
.np
.ix 'BIOS call'
The BIOS video call to scroll the active page up requires the
following arguments.
.autonote
.note
The row and column of the upper left corner of the scroll window is
passed in registers CH and CL respectively.
.note
The row and column of the lower right corner of the scroll window is
passed in registers DH and DL respectively.
.note
The number of lines blanked at the bottom of the window is passed in
register AL.
.note
The attribute to be used on the blank lines is passed in register BH.
.endnote
.pc
When passing arguments, &cmpname will convert the argument so that
it fits in the register(s) specified in the register set for that
argument.
For example, in the above example, if the first argument to
.id scrollactivepgup
was called with an argument whose type was
.bd &int
.ct , it would first be converted to
.bd &char
before assigning it to register CH.
Similarly, if an in-line &function required its argument in register
&reg32 and the argument was of type
.bd &short_int
.ct , the argument would be converted to
.bd &long_int
before assigning it to register &reg32..
.np
In general, &cmpname assigns the following types to register sets.
.autonote
.note
A register set consisting of a single 8-bit register (1 byte) is
assigned a type of
.bd &uchar
.ct .li .
.note
A register set consisting of a single 16-bit register (2 bytes) is
assigned a type of
.bd &ushort_int
.ct .li .
.note
A register set consisting of
.if '&machine' eq '8086' .do begin
two 16-bit registers
.do end
.if '&machine' eq '80386' .do begin
a single 32-bit register
.do end
(4 bytes) is assigned
a type of
.bd &ulong_int
.ct .li .
.note
A register set consisting of
.if '&machine' eq '8086' .do begin
four 16-bit
.do end
.if '&machine' eq '80386' .do begin
two 32-bit
.do end
registers (8 bytes) is assigned a type of
.bd &double
.ct .li .
.endnote
.if '&lang' eq 'FORTRAN 77' .do begin
.np
If the size of an integer argument is larger than the size specified
by the register set, the argument will be truncated to the required
size.
If the size of an integer argument is smaller than the size specified
by the register set, the argument will be padded (to the left) with
zeros.
.do end
.*
.section Removing Arguments from the Stack
.*
.np
The following form of the auxiliary pragma specifies who removes
from the stack arguments that were pushed on the stack.
.ix 'pragmas' 'parm caller'
.ix 'pragmas' 'parm routine'
.ix 'parm caller (pragma)'
.ix 'parm routine (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. parm :id.(:eid.caller :or. routine:id.):eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.pc
.ix 'arguments' 'removing from the stack'
"caller" specifies that the caller will pop the arguments
from the stack; "routine" specifies that the called routine
will pop the arguments from the stack.
If "caller" or "routine" is omitted, "routine" is assumed unless the
default has been changed in a previous auxiliary pragma, in which case
the new default is assumed.
.if '&lang' eq 'FORTRAN 77' .do begin
.np
Consider the following example.
It describes the pragma required to call the C "printf" function.
.millust begin
*$pragma aux printf "*_" parm (value) caller []
      character cr/z0d/, nullchar/z00/
      call printf( 'value is %ld'//cr//nullchar,
     1             7143 )
      end
.millust end
.pc
The first argument must be passed as a C string, a pointer to the
actual character data terminated by a null character.
By default, the address of a string descriptor is passed for arguments
of type
.bd CHARACTER
.ct .li .
See the chapter entitled "Assembly Language Considerations" for more
information on string descriptors.
The second argument is of type
.bd INTEGER
and is passed by value.
Also note that "printf" is a function that takes a variable number of
arguments, all pushed on the stack (an empty register set was
specified).
.do end
.*
.section Passing Arguments in Reverse Order
.*
.np
The following form of the auxiliary pragma specifies that arguments
are passed in the reverse order.
.ix 'pragmas' 'parm reverse'
.ix 'parm reverse (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. parm reverse :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.pc
Normally, arguments are processed from left to right.
The leftmost arguments are passed in registers and the rightmost
arguments are passed on the stack (if the registers used for argument
passing have been exhausted).
Arguments that are passed on the stack are pushed from right to left.
.np
When arguments are reversed, the rightmost arguments are passed in
registers and the leftmost arguments are passed on the stack (if the
registers used for argument passing have been exhausted).
Arguments that are passed on the stack are pushed from left to right.
.np
Reversing arguments is most useful for &functions that require
arguments to be passed on the stack in an order opposite from the
default.
The following auxiliary pragma demonstrates such a &function..
.millust begin
&pragma aux rtn parm reverse []&epragma
.millust end
.*
.endlevel
.*
.section Describing &ufunction Return Information
.*
.np
.ix 'return value (pragma)'
.ix 'pragmas' 'describing return value'
Using auxiliary pragmas, you can describe the way functions are to
return values.
This is particularly useful when interfacing to functions that have
been compiled by other compilers or functions written in other
programming languages.
.np
The general form of an auxiliary pragma that describes the way a
function returns its value is the following.
.ix 'pragmas' 'value'
.ix 'pragmas' 'no8087'
.ix 'pragmas' 'struct float'
.ix 'pragmas' 'struct routine'
.ix 'pragmas' 'struct caller'
.ix 'value (pragma)'
.ix 'no8087 (pragma)'
.ix 'struct float (pragma)'
.ix 'struct routine (pragma)'
.ix 'struct caller (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value :rp.no8087 :or. :id.reg_set:eid. :or. :id.struct_info:eid.:erp. :prgend.
.millust break

:id.struct_info ::=:eid. struct :rp.float :or. struct :or. :id.(:eid.routine :or. caller:id.):eid. :or. :id.reg_set:eid.:erp.
.mbox end
.synote
.note sym
is a function name.
.note reg_set
is called a register set.
The register sets specify the registers that are to be used for
argument passing.
A register set is a list of registers separated by spaces and enclosed
in square brackets.
.esynote
.*
.beglevel
.*
.section Returning &ufunction Values in Registers
.*
.np
The following form of the auxiliary pragma can be used to specify the
registers that are to be used to return a function's value.
.ix 'pragmas' 'value'
.ix 'value (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value :id.reg_set:eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set.
.esynote
.np
Note that the method described below for returning values of type
.bd &single
or
.bd &double
is supported only when the "fpc" option is used.
.np
Depending on the type of the return value, only certain registers are
allowed in
.sy reg_set.
.begnote $break
.note 1-byte
For 1-byte return values, only the following registers are allowed:
AL, AH, DL, DH, BL, BH, CL or CH.
If no register set is specified, register AL will be used.
.note 2-byte
For 2-byte return values, only the following registers are allowed:
AX, DX, BX, CX, SI or DI.
If no register set is specified, register AX will be used.
.note 4-byte
For 4-byte return values
.if '&machine' eq '8086' .do begin
(except far pointers), only the following register pairs are allowed:
DX:AX, CX:BX, CX:AX, CX:SI, DX:BX, DI:AX, CX:DI, DX:SI, DI:BX, SI:AX,
CX:DX, DX:DI, DI:SI, SI:BX or BX:AX.
If no register set is specified, registers DX:AX will be used.
.do end
.if '&machine' eq '80386' .do begin
(including near pointers), only the following register are allowed:
EAX, EDX, EBX, ECX, ESI or EDI.
If no register set is specified, register EAX will be used.
.do end
This form of the auxiliary pragma is legal for functions of type
.bd &single
when using the "fpc" option only.
.note far pointer
For functions that return far pointers, the following register
pairs are allowed:
DX:&axup, CX:&bxup, CX:&axup, CX:&siup, DX:&bxup, DI:&axup, CX:&diup,
DX:&siup, DI:&bxup, SI:&axup, CX:&dxup, DX:&diup, DI:&siup, SI:&bxup,
BX:&axup,
.if '&machine' eq '80386' .do begin
FS:ECX, FS:EDX, FS:EDI, FS:ESI, FS:EBX, FS:EAX,
GS:ECX, GS:EDX, GS:EDI, GS:ESI, GS:EBX, GS:EAX,
.do end
DS:&cxup, DS:&dxup, DS:&diup, DS:&siup, DS:&bxup, DS:&axup,
ES:&cxup, ES:&dxup, ES:&diup, ES:&siup, ES:&bxup or ES:&axup..
If no register set is specified, the registers DX:&axup will be used.
.note 8-byte
For 8-byte return values (including functions of type
.bd &double
.ct ),
only the following register
.if '&machine' eq '8086' .do begin
combination is allowed: AX:BX:CX:DX.
If no register set is specified, the registers AX:BX:CX:DX will be
used.
.do end
.if '&machine' eq '80386' .do begin
pairs are allowed:
EDX:EAX, ECX:EBX, ECX:EAX, ECX:ESI, EDX:EBX, EDI:EAX, ECX:EDI,
EDX:ESI, EDI:EBX, ESI:EAX, ECX:EDX, EDX:EDI, EDI:ESI, ESI:EBX or
EBX:EAX.
If no register set is specified, the registers EDX:EAX will be used.
.do end
This form of the auxiliary pragma is legal for functions of type
.bd &double
when using the "fpc" option only.
.endnote
.autonote Notes:
.note
An empty register set is not allowed.
.note
If you are compiling for a memory model which has a small data model,
any of the above register combinations containing register DS becomes
illegal.
In a small data model, segment register DS must remain unchanged as
it points to the program's data segment.
.if '&machine' eq '80386' .do begin
.note
If you are compiling for the flat memory model, any
register combination containing DS or ES becomes illegal.
In a flat memory model, code and data reside in the same segment.
Segment registers DS and ES point to this segment and must remain
unchanged.
.do end
.endnote
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.section Returning Structures and Complex Numbers
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.section Returning Structures
.do end
.*
.np
Typically,
.if '&lang' eq 'FORTRAN 77' .do begin
structures and complex numbers
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
structures
.do end
are not returned in registers.
Instead, the caller allocates space on the stack for the
return value and sets register &siup to point to it.
The called routine then places the return value at the location
pointed to by register &siup..
.if '&lang' eq 'FORTRAN 77' .do begin
.np
Complex numbers are not scalars but rather an ordered pair of real
numbers.
One can also view complex numbers as a
.us structure
containing two real numbers.
.do end
.np
The following form of the auxiliary pragma can be used to specify the
register that is to be used to point to the return value.
.ix 'pragmas' 'value'
.ix 'pragmas' 'struct caller'
.ix 'pragmas' 'struct routine'
.ix 'value (pragma)'
.ix 'struct caller (pragma)'
.ix 'struct routine (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value struct :id.(:eid.caller:or.routine:id.):eid. :id.reg_set:eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set.
.esynote
.np
"caller" specifies that the caller will allocate memory for
the return value.
The address of the memory allocated for the return value is placed in
the register specified in the register set by the caller before
the function is called.
If an empty register set is specified, the address of the memory allocated
for the return value will be pushed on the stack immediately before the
call and will be returned in register &axup by the called routine.
.if '&machine' eq '8086' .do begin
It is assumed that the memory for the return value is allocated from the
stack segment (the stack segment is contained in segment register SS).
.do end
.np
"routine" specifies that the called routine will allocate
memory for the return value.
Upon returning to the caller, the register specified in the register
set will contain the address of the return value.
An empty register set is not allowed.
.np
Only the following registers are allowed in the register set:
&axup, &dxup, &bxup, &cxup, &siup or &diup..
Note that in a big data model, the address in the return register is
assumed to be in the segment specified by the value in the SS segment
register.
.np
If the size of the structure being returned is 1, 2 or 4 bytes,
it will be returned in registers.
The return register will be selected from the register set in the
following way.
.autonote
.note
A 1-byte structure will be returned in one of the following registers:
AL, AH, DL, DH, BL, BH, CL or CH.
If no register set is specified, register AL will be used.
.note
A 2-byte structure will be returned in one of the following registers:
AX, DX, BX, CX, SI or DI.
If no register set is specified, register AX will be used.
.note
A 4-byte structure will be returned in one of the following
.if '&machine' eq '8086' .do begin
register pairs: DX:AX, CX:BX, CX:AX, CX:SI, DX:BX, DI:AX, CX:DI,
DX:SI, DI:BX, SI:AX, CX:DX, DX:DI, DI:SI, SI:BX or BX:AX.
If no register set is specified, register pair DX:AX will be used.
.do end
.if '&machine' eq '80386' .do begin
registers: EAX, EDX, EBX, ECX, ESI or EDI.
If no register set is specified, register EAX will be used.
.do end
.endnote
.np
The following form of the auxiliary pragma can be used to specify that
structures whose size is 1, 2 or 4 bytes are not to be returned in
registers.
Instead, the caller will allocate space on the stack for the structure
return value and point register &siup to it.
.mbox begin
:prgbeg. aux :id.sym:eid. value struct struct :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.*
.section Returning Floating-Point Data
.*
.np
There are a few ways available for specifying how the value for a
function whose type is
.bd &single
or
.bd &double
is to be returned.
.np
The following form of the auxiliary pragma can be used to specify that
function return values whose type is
.bd &single
or
.bd &double
are not to be returned in registers.
Instead, the caller will allocate space on the stack for the
return value and point register &siup to it.
.ix 'pragmas' 'value'
.ix 'pragmas' 'struct float'
.ix 'value (pragma)'
.ix 'struct float (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value struct float :prgend.
.mbox end
.synote
.note sym
is a function name.
.esynote
.np
In other words, floating-point values are to be returned in the same
way
.if '&lang' eq 'FORTRAN 77' .do begin
complex numbers
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
structures
.do end
are returned.
.np
The following form of the auxiliary pragma can be used to specify that
function return values whose type is
.bd &single
or
.bd &double
are not to be returned in 80x87 registers when compiling with the
"fpi" or "fpi87" option.
Instead, the value will be returned in 80x86 registers.
This is the default behaviour for the "fpc" option.
Function return values whose type is
.bd &single
will be returned in
.if '&machine' eq '8086' .do begin
registers DX:AX.
.do end
.if '&machine' eq '80386' .do begin
register EAX.
.do end
Function return values whose type is
.bd &double
will be returned in registers
.if '&machine' eq '8086' .do begin
AX:BX:CX:DX.
.do end
.if '&machine' eq '80386' .do begin
EDX:EAX.
.do end
This is the default method for the "fpc" option.
.ix 'pragmas' 'value no8087'
.ix 'value no8087 (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value no8087 :prgend.
.mbox end
.synote
.note sym
is a function name.
.esynote
.np
The following form of the auxiliary pragma can be used to specify that
function return values whose type is
.bd &single
or
.bd &double
are to be returned in ST(0) when compiling with the
"fpi" or "fpi87" option.
This form of the auxiliary pragma is not legal for the "fpc" option.
.ix 'pragmas' 'value [8087]'
.ix 'value [8087] (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value [8087] :prgend.
.mbox end
.synote
.note sym
is a function name.
.esynote
.*
.endlevel
.*
.section A &ufunction that Never Returns
.*
.np
The following form of the auxiliary pragma can be used to describe a
&function that does not return to the caller.
.ix 'pragmas' 'aborts'
.ix 'aborts (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. aborts :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.cp 12
.pc
Consider the following example.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux exitrtn aborts
      ...
      call exitrtn()
      end
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#pragma aux exitrtn aborts;
extern void exitrtn(void);

void rtn()
  {
    exitrtn();
  }
.millust end
.do end
.pc
.id exitrtn
is defined to be a function that does not return.
For example, it may call
.id exit
to return to the system.
In this case, &cmpname generates a "jmp" instruction instead of a
"call" instruction to invoke
.id exitrtn.
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.section Describing How &ufunctions Use Variables in Common
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.section Describing How &ufunctions Use Memory
.do end
.*
.np
.ix 'side effects of &functions'
The following form of the auxiliary pragma can be used to describe a
&function that does not modify any
.if '&lang' eq 'FORTRAN 77' .do begin
variable that appears in a common block defined by the caller.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
memory (i.e., global or static variables) that is used directly or
indirectly by the caller.
.do end
.ix 'pragmas' 'modify nomemory'
.ix 'modify nomemory (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. modify nomemory :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.pc
Consider the following example.
.cp 15
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
      integer i
      common /blk/ i
      while( i .lt. 1000 )do
          i = i + 383
      endwhile
      call myrtn()
      i = i + 13143
      end

      block data
      common /blk/ i
      integer i/1033/
      end
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#pragma off (check_stack);

extern void myrtn(void);

int i = { 1033 };

extern Rtn() {
    while( i < 10000 ) {
        i += 383;
    }
    myrtn();
    i += 13143;
};
.millust end
.do end
.pc
To compile the above program, "rtn.&langsuff", we issue the following
command.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
&prompt.&ccmd16 rtn &sw.mm &sw.d1
&prompt.&ccmd32 rtn &sw.d1
.millust end
.pc
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
&prompt.&ccmd16 rtn &sw.oai &sw.d1
&prompt.&pcmd16 rtn &sw.oai &sw.d1
&prompt.&ccmd32 rtn &sw.oai &sw.d1
&prompt.&pcmd32 rtn &sw.oai &sw.d1
.millust end
.pc
For illustrative purposes, we omit loop optimizations from the list
of code optimizations that we want the compiler to perform.
.do end
The "d1" compiler option is specified so that the object file produced
by &cmpname contains source line information.
.np
We can generate a file containing a disassembly of
.fi rtn&obj
by issuing the following command.
.millust begin
&prompt.&discmd rtn &sw.l &sw.s &sw.r
.millust end
.pc
The "s" option is specified so that the listing file produced by the
&disname contains source lines taken from
.fi rtn.&langsuff..
The listing file
.fi rtn&lst
appears as follows.
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&machine' eq '80386' .do begin
.code begin

Module: rtn.for
Group: 'DGROUP' _DATA,LDATA,CDATA,BLK

Segment: 'FMAIN_TEXT' BYTE USE32  00000036 bytes

        integer i
        common /blk/ i
 0000  52                FMAIN           push    edx
 0001  8b 15 00 00 00 00                 mov     edx,L3

.code break
        while( i .lt. 1000 )do
 0007  81 fa e8 03 00 00 L1              cmp     edx,000003e8H
 000d  7d 08                             jge     L2

            i = i + 383
        endwhile
 000f  81 c2 7f 01 00 00                 add     edx,0000017fH
 0015  eb f0                             jmp     L1

.code break
        call myrtn()
 0017  89 15 00 00 00 00 L2              mov     L3,edx
 001d  e8 00 00 00 00                    call    MYRTN
 0022  8b 15 00 00 00 00                 mov     edx,L3

        i = i + 13143
 0028  81 c2 57 33 00 00                 add     edx,00003357H
 002e  89 15 00 00 00 00                 mov     L3,edx

        end

.code break
        block data
        common /blk/ i
        integer i/1033/
        end
 0034  5a                                pop     edx
 0035  c3                                ret

No disassembly errors

.code break
List of external symbols

Symbol
----------------
MYRTN            0000001e
------------------------------------------------------------

Segment: 'BLK' PARA USE32  00000004 bytes
 0000  09 04 00 00             L3              - ....

No disassembly errors

------------------------------------------------------------
.code break
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.if '&machine' eq '8088' .do begin
.code begin

Module: rtn.for
Group: 'DGROUP' _DATA,LDATA,CDATA,BLK

Segment: 'FMAIN_TEXT' BYTE  00000028 bytes

        integer*2 i
        common /blk/ i
 0000  52                FMAIN           push    dx
 0001  8b 16 00 00                       mov     dx,L3

.code break
        while( i .lt. 1000 )do
 0005  81 fa e8 03       L1              cmp     dx,03e8H
 0009  7d 06                             jge     L2

            i = i + 383
        endwhile
 000b  81 c2 7f 01                       add     dx,017fH
 000f  eb f4                             jmp     L1

.code break
        call myrtn()
 0011  89 16 00 00       L2              mov     L3,dx
 0015  9a 00 00 00 00                    call    far MYRTN
 001a  8b 16 00 00                       mov     dx,L3

        i = i + 13143
 001e  81 c2 57 33                       add     dx,3357H
 0022  89 16 00 00                       mov     L3,dx

        end

.code break
        block data
        common /blk/ i
        integer*2 i/1033/
        end
 0026  5a                                pop     dx
 0027  cb                                retf

No disassembly errors

.code break
List of external symbols

Symbol
----------------
MYRTN            00000016
------------------------------------------------------------

Segment: 'BLK' PARA  00000002 bytes
 0000  09 04                   L3              - ..

No disassembly errors

------------------------------------------------------------
.code break
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '80386' .do begin
.code begin

Module: rtn.c
Group: 'DGROUP' CONST,_DATA

Segment: '_TEXT' BYTE USE32  00000036 bytes

#pragma off (check_stack);

extern void myrtn(void);

int i = { 1033 };

.code break
extern Rtn() {
 0000  52                Rtn_            push    EDX
 0001  8b 15 00 00 00 00                 mov     EDX,_i

    while( i < 10000 ) {
 0007  81 fa 10 27 00 00 L1              cmp     EDX,00002710H
 000d  7d 08                             jge     L2

        i += 383;
    }
 000f  81 c2 7f 01 00 00                 add     EDX,0000017fH
 0015  eb f0                             jmp     L1

.code break
    myrtn();
 0017  89 15 00 00 00 00 L2              mov     _i,EDX
 001d  e8 00 00 00 00                    call    myrtn_
 0022  8b 15 00 00 00 00                 mov     EDX,_i

    i += 13143;
 0028  81 c2 57 33 00 00                 add     EDX,00003357H
 002e  89 15 00 00 00 00                 mov     _i,EDX

}
 0034  5a                                pop     EDX
 0035  c3                                ret

.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD USE32  00000004 bytes
 0000  09 04 00 00             _i              - ....

No disassembly errors

------------------------------------------------------------

.code end
.do end
.if '&machine' eq '8086' .do begin
.code begin

Module: rtn.c
Group: 'DGROUP' CONST,_DATA

Segment: '_TEXT' BYTE  0026 bytes

#pragma off (check_stack);

extern void MyRtn( void );

int i = { 1033 };

.code break
extern Rtn()
  {
 0000  52                Rtn_            push    DX
 0001  8b 16 00 00                       mov     DX,_i

    while( i < 10000 ) {
 0005  81 fa 10 27       L1              cmp     DX,2710H
 0009  7d 06                             jge     L2

        i += 383;
    }
 000b  81 c2 7f 01                       add     DX,017fH
 000f  eb f4                             jmp     L1

.code break
    MyRtn();
 0011  89 16 00 00       L2              mov     _i,DX
 0015  e8 00 00                          call    MyRtn_
 0018  8b 16 00 00                       mov     DX,_i

    i += 13143;
 001c  81 c2 57 33                       add     DX,3357H
 0020  89 16 00 00                       mov     _i,DX

  };
 0024  5a                                pop     DX
 0025  c3                                ret

.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD  0002 bytes
 0000  09 04                   _i              - ..

No disassembly errors

------------------------------------------------------------
.code end
.do end
.do end
.pc
Let us add the following auxiliary pragma to the source file.
.millust begin
&pragma aux myrtn modify nomemory&epragma
.millust end
.pc
If we compile the source file with the above pragma and disassemble
the object file using the &disname, we get the following listing file.
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&machine' eq '80386' .do begin
.code begin

Module: rtn.for
Group: 'DGROUP' _DATA,LDATA,CDATA,BLK

Segment: 'FMAIN_TEXT' BYTE USE32  00000030 bytes

*$pragma aux myrtn modify nomemory
        integer i
        common /blk/ i
 0000  52                FMAIN           push    edx
 0001  8b 15 00 00 00 00                 mov     edx,L3

.code break
        while( i .lt. 1000 )do
 0007  81 fa e8 03 00 00 L1              cmp     edx,000003e8H
 000d  7d 08                             jge     L2

            i = i + 383
        endwhile
 000f  81 c2 7f 01 00 00                 add     edx,0000017fH
 0015  eb f0                             jmp     L1

.code break
        call myrtn()
 0017  89 15 00 00 00 00 L2              mov     L3,edx
 001d  e8 00 00 00 00                    call    MYRTN

        i = i + 13143
 0022  81 c2 57 33 00 00                 add     edx,00003357H
 0028  89 15 00 00 00 00                 mov     L3,edx

        end

.code break
        block data
        common /blk/ i
        integer i/1033/
        end
 002e  5a                                pop     edx
 002f  c3                                ret

No disassembly errors

.code break
List of external symbols

Symbol
----------------
MYRTN            0000001e
------------------------------------------------------------

Segment: 'BLK' PARA USE32  00000004 bytes
 0000  09 04 00 00             L3              - ....

No disassembly errors

------------------------------------------------------------
.code break
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.if '&machine' eq '8086' .do begin
.code begin

Module: rtn.for
Group: 'DGROUP' _DATA,LDATA,CDATA,BLK

Segment: 'FMAIN_TEXT' BYTE  00000024 bytes

*$pragma aux myrtn modify nomemory
        integer*2 i
        common /blk/ i
 0000  52                FMAIN           push    dx
 0001  8b 16 00 00                       mov     dx,L3

.code break
        while( i .lt. 1000 )do
 0005  81 fa e8 03       L1              cmp     dx,03e8H
 0009  7d 06                             jge     L2

            i = i + 383
        endwhile
 000b  81 c2 7f 01                       add     dx,017fH
 000f  eb f4                             jmp     L1

.code break
        call myrtn()
 0011  89 16 00 00       L2              mov     L3,dx
 0015  9a 00 00 00 00                    call    far MYRTN

        i = i + 13143
 001a  81 c2 57 33                       add     dx,3357H
 001e  89 16 00 00                       mov     L3,dx

        end

.code break
        block data
        common /blk/ i
        integer*2 i/1033/
        end
 0022  5a                                pop     dx
 0023  cb                                retf

No disassembly errors

.code break
List of external symbols

Symbol
----------------
MYRTN            00000016
------------------------------------------------------------

Segment: 'BLK' PARA  00000002 bytes
 0000  09 04                   L3              - ..

No disassembly errors

------------------------------------------------------------
.code break
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '80386' .do begin
.code begin

Module: rtn.c
Group: 'DGROUP' CONST,_DATA

Segment: '_TEXT' BYTE USE32  00000030 bytes

#pragma off (check_stack);
#pragma aux myrtn modify nomemory;

.code break
extern void myrtn(void);

int i = { 1033 };

extern Rtn() {
 0000  52                Rtn_            push    EDX
 0001  8b 15 00 00 00 00                 mov     EDX,_i

    while( i < 10000 ) {
 0007  81 fa 10 27 00 00 L1              cmp     EDX,00002710H
 000d  7d 08                             jge     L2

        i += 383;
    }
 000f  81 c2 7f 01 00 00                 add     EDX,0000017fH
 0015  eb f0                             jmp     L1

.code break
    myrtn();
 0017  89 15 00 00 00 00 L2              mov     _i,EDX
 001d  e8 00 00 00 00                    call    myrtn_

    i += 13143;
 0022  81 c2 57 33 00 00                 add     EDX,00003357H
 0028  89 15 00 00 00 00                 mov     _i,EDX

}
 002e  5a                                pop     EDX
 002f  c3                                ret

.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD USE32  00000004 bytes
 0000  09 04 00 00             _i              - ....

No disassembly errors

------------------------------------------------------------

.code end
.do end
.if '&machine' eq '8086' .do begin
.code begin

Module: rtn.c
Group: 'DGROUP' CONST,_DATA

Segment: '_TEXT' BYTE  0022 bytes

#pragma off (check_stack);

extern void MyRtn( void );
#pragma aux MyRtn modify nomemory;

int i = { 1033 };

.code break
extern Rtn()
  {
 0000  52                Rtn_            push    DX
 0001  8b 16 00 00                       mov     DX,_i

    while( i < 10000 ) {
 0005  81 fa 10 27       L1              cmp     DX,2710H
 0009  7d 06                             jge     L2

        i += 383;
    }
 000b  81 c2 7f 01                       add     DX,017fH
 000f  eb f4                             jmp     L1

.code break
    MyRtn();
 0011  89 16 00 00       L2              mov     _i,DX
 0015  e8 00 00                          call    MyRtn_

    i += 13143;
 0018  81 c2 57 33                       add     DX,3357H
 001c  89 16 00 00                       mov     _i,DX

  };
 0020  5a                                pop     DX
 0021  c3                                ret

.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD  0002 bytes
 0000  09 04                   _i              - ..

No disassembly errors

------------------------------------------------------------
.code end
.do end
.do end
.pc
Notice that the value of
.id i
is in register &dxup after completion of the "while" loop.
After the call to
.id myrtn,
the value of
.id i
is not loaded from memory into a register to perform the final
addition.
The auxiliary pragma informs the compiler that
.id myrtn
does not modify any
.if '&lang' eq 'FORTRAN 77' .do begin
variable that appears in a common block defined by
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
memory (i.e., global or static variables) that is used directly or
indirectly by
.do end
.id Rtn
and hence register &dxup contains the correct value of
.id i.
.np
The preceding auxiliary pragma deals with routines that modify
.if '&lang' eq 'FORTRAN 77' .do begin
variables in common.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
memory.
.do end
Let us consider the case where routines reference
.if '&lang' eq 'FORTRAN 77' .do begin
variables in common.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
memory.
.do end
The following form of the auxiliary pragma can be used to describe a
&function that does not reference any
.if '&lang' eq 'FORTRAN 77' .do begin
variable that appears in a common block defined by the caller.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
memory (i.e., global or static variables) that is used directly or
indirectly by the caller.
.do end
.ix 'pragmas' 'parm nomemory'
.ix 'pragmas' 'modify nomemory'
.ix 'parm nomemory (pragma)'
.ix 'modify nomemory (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. parm nomemory modify nomemory :prgend.
.mbox end
.synote
.note sym
is a &function name.
.esynote
.autonote Notes:
.note
You must specify both "parm nomemory" and "modify nomemory".
.endnote
.np
Let us replace the auxiliary pragma in the above example with the
following auxiliary pragma.
.millust begin
&pragma aux myrtn parm nomemory modify nomemory&epragma
.millust end
.pc
If you now compile our source file and disassemble the object file
using &discmdup,
the result is the following listing file.
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&machine' eq '80386' .do begin
.code begin

Module: rtn.for
Group: 'DGROUP' _DATA,LDATA,CDATA,BLK

Segment: 'FMAIN_TEXT' BYTE USE32  0000002a bytes

*$pragma aux myrtn parm nomemory modify nomemory
        integer i
        common /blk/ i
 0000  52                FMAIN           push    edx
 0001  8b 15 00 00 00 00                 mov     edx,L3

.code break
        while( i .lt. 1000 )do
 0007  81 fa e8 03 00 00 L1              cmp     edx,000003e8H
 000d  7d 08                             jge     L2

            i = i + 383
        endwhile
 000f  81 c2 7f 01 00 00                 add     edx,0000017fH
 0015  eb f0                             jmp     L1

.code break
        call myrtn()
 0017  e8 00 00 00 00    L2              call    MYRTN

        i = i + 13143
 001c  81 c2 57 33 00 00                 add     edx,00003357H
 0022  89 15 00 00 00 00                 mov     L3,edx

        end

.code break
        block data
        common /blk/ i
        integer i/1033/
        end
 0028  5a                                pop     edx
 0029  c3                                ret

No disassembly errors

.code break
List of external symbols

Symbol
----------------
MYRTN            00000018
------------------------------------------------------------

Segment: 'BLK' PARA USE32  00000004 bytes
 0000  09 04 00 00             L3              - ....

No disassembly errors

------------------------------------------------------------
.code break
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.if '&machine' eq '8086' .do begin
.code begin

Module: rtn.for
Group: 'DGROUP' _DATA,LDATA,CDATA,BLK

Segment: 'FMAIN_TEXT' BYTE  00000020 bytes

*$pragma aux myrtn parm nomemory modify nomemory
        integer*2 i
        common /blk/ i
 0000  52                FMAIN           push    dx
 0001  8b 16 00 00                       mov     dx,L3

.code break
        while( i .lt. 1000 )do
 0005  81 fa e8 03       L1              cmp     dx,03e8H
 0009  7d 06                             jge     L2

            i = i + 383
        endwhile
 000b  81 c2 7f 01                       add     dx,017fH
 000f  eb f4                             jmp     L1

.code break
        call myrtn()
 0011  9a 00 00 00 00    L2              call    far MYRTN

        i = i + 13143
 0016  81 c2 57 33                       add     dx,3357H
 001a  89 16 00 00                       mov     L3,dx

        end

.code break
        block data
        common /blk/ i
        integer*2 i/1033/
        end
 001e  5a                                pop     dx
 001f  cb                                retf

No disassembly errors

.code break
List of external symbols

Symbol
----------------
MYRTN            00000012
------------------------------------------------------------

Segment: 'BLK' PARA  00000002 bytes
 0000  09 04                   L3              - ..

No disassembly errors

------------------------------------------------------------
.code break
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '80386' .do begin
.code begin

Module: rtn.c
Group: 'DGROUP' CONST,_DATA

Segment: '_TEXT' BYTE USE32  0000002a bytes

#pragma off (check_stack);
#pragma aux myrtn parm nomemory modify nomemory;

.code break
extern void myrtn(void);

int i = { 1033 };

extern Rtn() {
 0000  52                Rtn_            push    EDX
 0001  8b 15 00 00 00 00                 mov     EDX,_i

    while( i < 10000 ) {
 0007  81 fa 10 27 00 00 L1              cmp     EDX,00002710H
 000d  7d 08                             jge     L2

        i += 383;
    }
 000f  81 c2 7f 01 00 00                 add     EDX,0000017fH
 0015  eb f0                             jmp     L1

.code break
    myrtn();
 0017  e8 00 00 00 00    L2              call    myrtn_

    i += 13143;
 001c  81 c2 57 33 00 00                 add     EDX,00003357H
 0022  89 15 00 00 00 00                 mov     _i,EDX

}
 0028  5a                                pop     EDX
 0029  c3                                ret

.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD USE32  00000004 bytes
 0000  09 04 00 00             _i              - ....

No disassembly errors

------------------------------------------------------------

.code end
.do end
.if '&machine' eq '8086' .do begin
.code begin

Module: rtn.c
Group: 'DGROUP' CONST,_DATA

Segment: '_TEXT' BYTE  001e bytes

#pragma off (check_stack);

extern void MyRtn( void );
#pragma aux MyRtn parm nomemory modify nomemory;

int i = { 1033 };

.code break
extern Rtn()
  {
 0000  52                Rtn_            push    DX
 0001  8b 16 00 00                       mov     DX,_i

    while( i < 10000 ) {
 0005  81 fa 10 27       L1              cmp     DX,2710H
 0009  7d 06                             jge     L2

        i += 383;
    }
 000b  81 c2 7f 01                       add     DX,017fH
 000f  eb f4                             jmp     L1

.code break
    MyRtn();
 0011  e8 00 00          L2              call    MyRtn_

    i += 13143;
 0014  81 c2 57 33                       add     DX,3357H
 0018  89 16 00 00                       mov     _i,DX

  };
 001c  5a                                pop     DX
 001d  c3                                ret

.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD  0002 bytes
 0000  09 04                   _i              - ..

No disassembly errors

------------------------------------------------------------
.code end
.do end
.do end
.pc
Notice that after completion of the "while" loop we did not have to
update
.id i
with the value in register &dxup before calling
.id myrtn.
The auxiliary pragma informs the compiler that
.id myrtn
does not reference any
.if '&lang' eq 'FORTRAN 77' .do begin
variable that appears in a common block defined by
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
memory (i.e., global or static variables) that is used directly or
indirectly by
.do end
.id myrtn
so updating
.id i
was not necessary before calling
.id myrtn.
.keep 12
.*
.section Describing the Registers Modified by a &ufunction
.*
.np
The following form of the auxiliary pragma can be used to describe
the registers that a &function will use without saving.
.ix 'pragmas' 'modify exact'
.ix 'modify exact (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. modify :op.exact:eop. :id.reg_set:eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set.
.esynote
.pc
Specifying a register set informs &cmpname that the registers
belonging to the register set are modified by the &function..
That is, the value in a register before calling the &function is
different from its value after execution of the &function..
.np
Registers that are used to pass arguments are assumed to be modified
and hence do not have to be saved and restored by the called
&function..
Also, since the &axup register is frequently used to return a value,
it is always assumed to be modified.
If necessary, the caller will contain code to save and restore the
contents of registers used to pass arguments.
Note that saving and restoring the contents of these registers may not
be necessary if the called &function does not modify them.
The following form of the auxiliary pragma can be used to describe
exactly those registers that will be modified by the called
&function..
.ix 'pragmas' 'modify exact'
.ix 'modify exact (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. modify exact :id.reg_set:eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set.
.esynote
.pc
The above form of the auxiliary pragma tells &cmpname not to assume
that the registers used to pass arguments will be modified by the
called &function..
Instead, only the registers specified in the register set will be
modified.
This will prevent generation of the code which unnecessarily saves
and restores the contents of the registers used to pass arguments.
.np
Also, any registers that are specified in the
.mono value
register set are assumed to be unmodified unless explicitly listed in
the
.mono exact
register set.
In the following example, the code generator will not generate code to
save and restore the value of the stack pointer register since we have
told it that "GetSP" does not modify any register whatsoever.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin
unsigned GetSP(void);
#if defined(__386__)
#pragma aux GetSP = value [esp] modify exact [];
#else
#pragma aux GetSP = value [sp] modify exact [];
#endif
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin
*$ifdef __386__
*$pragma aux GetSP = value [esp] modify exact []
*$else
*$pragma aux GetSP = value [sp] modify exact []
*$endif

      program main
      integer GetSP
      print *, 'Current SP =', GetSP()
      end
.exam end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '80386' .do begin
.*
.section An Example
.*
.np
.ix 'High C calling convention'
.ix 'MetaWare' 'High C calling convention'
.ix 'calling convention' 'MetaWare High C'
As mentioned in an earlier section, the following pragma defines the
calling convention for functions compiled by MetaWare's High C
compiler.
.millust begin
#pragma aux HIGH_C "*"                                 \
                   parm caller []                      \
                   value no8087                        \
                   modify [eax ecx edx fs gs];
.millust end
.pc
Note that register ES must also be specified in the "modify" register
set when using a memory model with a non-small data model.
Let us discuss this pragma in detail.
.begnote
.note "*"
specifies that all function and variable names appear in object form
as they do in source form.
.note parm caller []
specifies that all arguments are to be passed on the stack
(an empty register set was specified) and the caller will remove
the arguments from the stack.
.note value no8087
specifies that floating-point values are to be returned using 80x86
registers and not 80x87 floating-point registers.
.note modify [eax ecx edx fs gs]
specifies that registers EAX, ECX, EDX, FS and GS are not preserved by
the called routine.
.endnote
.pc
Note that the default method of returning integer values is used;
1-byte characters are returned in register AL, 2-byte integers are
returned in register AX, and 4-byte integers are returned in
register EAX.
.do end
.if '&machine' eq '8086' .do begin
.*
.section An Example
.*
.np
As mentioned in an earlier section, the following pragma defines the
calling convention for functions compiled by Microsoft C.
.ix 'Microsoft' 'C calling convention'
.ix 'calling convention' 'Microsoft C'
.millust begin
#pragma aux MS_C "_*"                                  \
                 parm caller []                        \
                 value struct float struct routine [ax]\
                 modify [ax bx cx dx es];
.millust end
.np
Let us discuss this pragma in detail.
.begnote
.note "_*"
specifies that all function and variable names are preceded by the
underscore character (_) when translated from source form to object
form.
.note parm caller []
specifies that all arguments are to be passed on the stack
(an empty register set was specified) and the caller will remove
the arguments from the stack.
.note value struct
marks the section describing how the called routine returns structure
information.
.begnote
.note float
specifies that floating-point arguments are returned in the same way
as structures are returned.
.note struct
specifies that 1, 2 and 4-byte structures are not to be returned in
registers.
.note routine
specifies that the called routine allocates storage for the return
structure and returns with a register pointing at it.
.note [ax]
specifies that register AX is used to point to the structure return
value.
.endnote
.note modify [ax bx cx dx es]
.sp
specifies that registers AX, BX, CX, DX and ES are not preserved by
the called routine.
.endnote
.pc
Note that the default method of returning integer values is used;
1-byte characters are returned in register AL, 2-byte integers are
returned in register AX, and 4-byte integers are returned in the
register pair DX:AX.
.do end
.do end
.*
.section Auxiliary Pragmas and the 80x87
.*
.np
This section deals with those aspects of auxiliary pragmas that are
specific to the 80x87.
The discussion in this chapter assumes that one of the "fpi" or "fpi87"
options is used to compile &functions..
The following areas are affected by the use of these options.
.autopoint
.point
passing floating-point arguments to functions,
.point
returning floating-point values from functions and
.point
which 80x87 floating-point registers are allowed to be modified by
the called routine.
.endpoint
.*
.beglevel
.*
.section *refid=&pragx87. Using the 80x87 to Pass Arguments
.*
.np
By default, floating-point arguments are passed on the 80x86 stack.
The 80x86 registers are never used to pass floating-point arguments
when a &function is compiled with the "fpi" or "fpi87" option.
However, they can be used to pass arguments whose type is not
floating-point such as arguments of type "int".
.np
The following form of the auxiliary pragma can be used to describe the
registers that are to be used to pass arguments to &functions..
.ix 'pragmas' 'parm reg_set'
.ix 'parm reg_set (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. parm :rp.:id.reg_set:eid.:erp. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set.
The register set can contain 80x86 registers and/or the string "8087".
.esynote
.autonote Notes:
.note
If an empty register set is specified, all arguments, including
floating-point arguments, will be passed on the 80x86 stack.
.endnote
.pc
.ix 'passing arguments' 'in 80x87 registers'
When the string "8087" appears in a register set, it simply means that
floating-point arguments can be passed in 80x87 floating-point
registers if the source file is compiled with the "fpi" or "fpi87"
option.
Before discussing argument passing in detail, some general notes on
the use of the 80x87 floating-point registers are given.
.np
The 80x87 contains 8 floating-point registers which essentially
form a stack.
The stack pointer is called ST and is a number between 0 and 7
identifying which 80x87 floating-point register is at the top of the
stack.
ST is initially 0.
80x87 instructions reference these registers by specifying a
floating-point register number.
This number is then added to the current value of ST.
The sum (taken modulo 8) specifies the 80x87 floating-point register
to be used.
The notation ST(n), where "n" is between 0 and 7, is used to refer to
the position of an 80x87 floating-point register relative to ST.
.np
When a floating-point value is loaded onto the 80x87 floating-point
register stack, ST is decremented (modulo 8), and the value is loaded
into ST(0).
When a floating-point value is stored and popped from the 80x87
floating-point register stack, ST is incremented (modulo 8) and
ST(1) becomes ST(0).
The following illustrates the use of the 80x87 floating-point
registers as a stack, assuming that the value of ST is 4 (4 values
have been loaded onto the 80x87 floating-point register stack).
.millust begin
            +----------------+
      0     | 4th from top   |  ST(4)
            +----------------+
      1     | 5th from top   |  ST(5)
            +----------------+
      2     | 6th from top   |  ST(6)
            +----------------+
      3     | 7th from top   |  ST(7)
            +----------------+
ST -> 4     | top of stack   |  ST(0)
            +----------------+
      5     | 1st from top   |  ST(1)
            +----------------+
      6     | 2nd from top   |  ST(2)
            +----------------+
      7     | 3rd from top   |  ST(3)
            +----------------+
.millust end
.np
Starting with version 9.5, the &company compilers use all eight of the
80x87 registers as a stack.
The initial state of the 80x87 register stack is empty before a
program begins execution.
.begnote
.note Note:
For compatibility with code compiled with version 9.0 and earlier, you
can compile with the "fpr" option.
In this case only four of the eight 80x87 registers are used as a
stack.
These four registers were used to pass arguments.
The other four registers form what was called the 80x87 cache.
The cache was used for local floating-point variables.
The state of the 80x87 registers before a program began execution was
as follows.
.autopoint
.point
The four 80x87 floating-point registers that form the stack are
uninitialized.
.point
The four 80x87 floating-point registers that form the 80x87 cache are
initialized with zero.
.endpoint
.pc
Hence, initially the 80x87 cache was comprised of ST(0), ST(1), ST(2)
and ST(3).
ST had the value 4 as in the above diagram.
When a floating-point value was pushed on the stack (as is the
case when passing floating-point arguments), it became ST(0) and
the 80x87 cache was comprised of ST(1), ST(2), ST(3) and ST(4).
When the 80x87 stack was full, ST(0), ST(1), ST(2) and ST(3) formed
the stack and ST(4), ST(5), ST(6) and ST(7) formed the 80x87 cache.
Version 9.5 and later no longer use this strategy.
.endnote
.np
The rules for passing arguments are as follows.
.autonote
.note
If the argument is not floating-point, use the procedure described
earlier in this chapter.
.note
If the argument is floating-point, and a previous argument has been
assigned a position on the 80x86 stack (instead of the 80x87 stack),
the floating-point argument is also assigned a position on the
80x86 stack.
Otherwise proceed to the next step.
.note
If the string "8087" appears in a register set in the pragma, and
if the 80x87 stack is not full, the floating-point argument
is assigned floating-point register ST(0) (the top element of the
80x87 stack).
The previous top element (if there was one) is now in ST(1).
Since arguments are pushed on the stack from right to left, the
leftmost floating-point argument will be in ST(0).
Otherwise the floating-point argument is assigned a position on the
80x86 stack.
.endnote
.np
Consider the following example.
.cp 16
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$pragma aux myrtn parm (value) [8087];

      real x
      double precision y
.if '&machine' eq '8086' .do begin
      integer*2 i
.do end
.if '&machine' eq '80386' .do begin
      integer i
.do end
      integer j
      x = 7.7
      i = 7
      y = 77.77
      j = 77
      call myrtn( x, i, y, j )
      end
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#pragma aux myrtn parm [8087];

void main()
{
    float    x;
    double   y;
    int      i;
    long int j;

    x = 7.7;
    i = 7;
    y = 77.77;
    j = 77;
    myrtn( x, i, y, j );
}
.millust end
.do end
.pc
.id myrtn
is an assembly language &function that requires four arguments.
The first argument of type
.bd &single
(4 bytes), the second argument is
of type
.bd &machint
(&intsize bytes), the third argument is of type
.bd &double
(8 bytes) and the fourth argument is of type
.bd &long_int
(4 bytes).
These arguments will be passed to
.id myrtn
in the following way.
.autonote
.note
Since "8087" was specified in the register set, the first argument,
being of type
.bd &single
.ct , will be passed in an 80x87 floating-point register.
.note
The second argument will be passed on the stack since no 80x86
registers were specified in the register set.
.note
The third argument will also be passed on the stack.
Remember the following rule: once an argument is assigned a position
on the stack, all remaining arguments will be assigned a position on
the stack.
Note that the above rule holds even though there are some 80x87
floating-point registers available for passing floating-point arguments.
.note
The fourth argument will also be passed on the stack.
.endnote
.pc
Let us change the auxiliary pragma in the above example as follows.
.millust begin
&pragma aux myrtn parm [&ax 8087]&epragma
.millust end
.pc
The arguments will now be passed to
.id myrtn
in the following way.
.autonote
.note
Since "8087" was specified in the register set, the first argument,
being of type
.bd &single
will be passed in an 80x87 floating-point
register.
.note
The second argument will be passed in register &axup, exhausting the
set of available 80x86 registers for argument passing.
.note
The third argument, being of type
.bd &double
.ct , will also be passed in an
80x87 floating-point register.
.note
The fourth argument will be passed on the stack since no 80x86
registers remain in the register set.
.endnote
.keep 10
.*
.section Using the 80x87 to Return &ufunction Values
.*
.np
The following form of the auxiliary pragma can be used to describe
a &function that returns a floating-point value in ST(0).
.ix 'pragmas' 'value reg_set'
.ix 'value reg_set (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. value :id.reg_set:eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set containing the string "8087", i.e. [8087].
.esynote
.*
.section Preserving 80x87 Floating-Point Registers Across Calls
.*
.np
.if &version le 90 .do begin
As described in the section entitled "Using the 80x87 to Pass
Arguments", four of the eight 80x87 floating-point registers are used
for a &function's local variables.
These four floating-point registers are known as the 80x87 cache.
When a &function is called, the registers in the 80x87 cache must be
preserved.
.do end
.el .do begin
The code generator assumes that all eight 80x87 floating-point
registers are available for use within a &function unless the
"fpr" option is used to generate backward compatible code
(older &company compilers used four registers as a cache).
.do end
The following form of the auxiliary pragma specifies that the
floating-point registers in the 80x87 cache may be modified by the
specified &function..
.ix 'pragmas' 'modify reg_set'
.ix 'modify reg_set (pragma)'
.mbox begin
:prgbeg. aux :id.sym:eid. modify :id.reg_set:eid. :prgend.
.mbox end
.synote
.note sym
is a &function name.
.note reg_set
is a register set containing the string "8087", i.e. [8087].
.esynote
.pc
This instructs &cmpname to save any local variables that are located
in the 80x87 cache before calling the specified routine.
.*
.endlevel
.*
.endlevel
.*
.do end
.*
.if &e'&dohelp eq 1 .do begin
.   .if '&machine' eq '8086' .do begin
.   .   .helppref
.   .do end
.   .el .do begin
.   .   .helppref
.   .do end
.do end
