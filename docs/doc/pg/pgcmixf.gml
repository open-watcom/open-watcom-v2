.chap *refid=cmixf Inter-Language calls: C and FORTRAN
.*
.np
.ix 'inter-language calls'
.ix 'mixed-language programming'
The purpose of this chapter is to anticipate common questions about
mixed-language development using &watc and &watf..
.np
The following topics are discussed in this chapter:
.begbull
.bull
Symbol Naming Convention
.bull
Argument Passing Convention
.bull
Memory Model Compatibility
.bull
Integer Type Compatibility
.bull
How do I pass integers from C to a FORTRAN function?
.bull
How do I pass integers from FORTRAN to a C function?
.bull
How do I pass a string from a C function to FORTRAN?
.bull
How do I pass a string from FORTRAN to a C function?
.bull
How do I access a FORTRAN common block from within C?
.bull
How do I call a C function that accepts a variable number of arguments?
.endbull
.*
.section Symbol Naming Convention
.*
.np
.ix 'mixed-language programming' 'symbol names'
The symbol naming convention describes how a symbol in source form is
mapped to its object form.
Because of this mapping, the name generated in the object file may
differ from its original source form.
.np
Default symbol naming conventions vary between compilers.
&watc prefixes an underscore character to the beginning of variable
names and appends an underscore to the end of function names during
the compilation process.
&watf converts symbols to upper case.
Auxiliary pragmas can be used to resolve this inconsistency.
.np
Pragmas are compiler directives which can provide several
capabilities, one of which is to provide information used for code
generation.
When calling a FORTRAN subprogram from C, we want to instruct the
compiler NOT to append the underscore at the end of the function name
and to convert the name to upper case.
This is achieved by using the following C auxiliary pragma:
.millust begin
#pragma aux ftnname "^";
.millust end
.np
The "^" character tells the compiler to convert the symbol name
"ftnname" to upper case; no underscore character will be appended.
This solves potential linker problems with "ftnname" since (by C
convention) the linker would attempt to resolve a reference to
"ftnname_".
.np
When calling C functions from FORTRAN, we need to instruct the
compiler to add the underscore at the end of the function name, and to
convert the name to lower case.
Since the FORTRAN compiler automatically converts identifiers to
uppercase, it is necessary to force the compiler to emit an equivalent
lowercase name.
Both of these things can be done with the following FORTRAN auxiliary
pragma:
.millust begin
*$pragma aux CNAME "!_"
.millust end
.np
There is another less convenient way to do this as shown in the
following:
.millust begin
*$pragma aux CNAME "cname_"
.millust end
.np
In the latter example, the case of the name in quotation marks is
preserved.
.np
Use of these pragmas resolves the naming differences, however, the
issue of argument passing must still be resolved.
.*
.section Argument Passing Convention
.*
.np
.ix 'mixed-language programming' 'argument passing'
.ix 'mixed-language programming' 'parameter passing'
In general, C uses call-by-value (passes argument values) while
FORTRAN uses call-by-reference (passes pointers to argument values).
This implies that to pass arguments to a FORTRAN subprogram we must
pass the addresses of arguments rather than their values.
C uses the "&" character to signify "address of".
.exam begin
result = ftnname( &arg );
.exam end
.np
When calling a C function from FORTRAN, the pragma used to correct the
naming conventions must also instruct the compiler that the C function
is expecting values, not addresses.
.millust begin
*$pragma aux CNAME "!_" parm (value)
.millust end
.np
The "parm (value)" addition instructs the FORTRAN compiler to pass
values, instead of addresses.
.np
Character data (strings) are an exception to the general case when
used as arguments.
In C, strings are not thought of as a whole entity, but rather as an
"array of characters".
Since strings are not considered scalar arguments, they are referenced
differently in both C and FORTRAN.
This is described in more detail in a following section.
.*
.section Memory Model Compatibility
.*
.np
.ix 'mixed-language programming' 'memory models'
While it is really not an issue with the 32-bit compilers (both use
the default "flat" memory model), it is important to know that the
default memory model used in &watf applications is the "large" memory
model ("ml") with "medium" and "huge" memory models as options.
Since the 16-bit &watc default is the "small" memory model, you must
specify the correct memory model when compiling your C/C++ code with
the 16-bit C or C++ compiler.
.*
.section Linking Considerations
.*
.np
.ix 'mixed-language programming' 'linking issues'
When both C/C++ and FORTRAN object files are combined into an
executable program or dynamic link library, it is important that you
list a least one of the FORTRAN object files first in the &lnkname
(&lnkcmdup) "FILES" directive to guarantee the proper search order
of the FORTRAN and C run-time libraries.
If you place a C/C++ object file first, you may inadvertently cause
the wrong version of run-time initialization routines to be loaded by
the linker.
.*
.section Integer Type Compatibility
.*
.np
.ix 'mixed-language programming' 'integer type'
In general, the number of bytes used to store an integer type is
implementation dependent.
In FORTRAN, the default size of an integer type is always 4 bytes,
while in C/C++, the size is architecture dependent.
The size of an "int" is 2 bytes for the 16-bit &watc compilers and 4
bytes for the 32-bit compilers while the size of a "long" is 4 bytes
regardless of architecture.
It is safest to prototype the function in C, specifying exactly what
size integers are being used.
The byte sizes are as follows:
.autopoint
.point
LONG - 4 bytes
.point
SHORT - 2 bytes
.endpoint
.np
Since FORTRAN uses a default of 4 bytes, we should specify the "long"
keyword in C for integer types.
.exam begin
long int ftnname( long int *, long int *, long int * );
.exam end
.np
In this case, "ftnname" takes three "pointers to long ints" as
arguments, and returns a "long int".
By specifying that the arguments are pointers, and not values, and by
specifying "long int" for the return type, this prototype has solved
the problems of argument passing and integer type compatibility.
.*
.section How do I pass integers from C to a FORTRAN function?
.*
.np
.ix 'mixed-language programming' 'passing integers'
The following &watc routine passes three integers to a FORTRAN
function that returns an integer value.
.millust begin
/* MIX1C.C - This C program calls a FORTRAN function to
 *           compute the max of three numbers.
 *
 * Compile/Link: wcl /ml mix1c mix1f.obj /fe=mix1
 *               wcl386  mix1c mix1f.obj /fe=mix1
 */

#include <stdio.h>

#pragma aux tmax3 "^";
long int tmax3( long int *, long int *, long int * );

.millust break
void main()
{
    long int  result;
    long int  i, j, k;

    i = -1;
    j = 12;
    k = 5;
    result = tmax3( &i, &j, &k );
    printf( "Maximum is %ld\n", result );
}
.millust end
.*
.np
The FORTRAN function:
.millust begin
* MIX1F.FOR - This FORTRAN function accepts three integer
*             arguments and returns their maximum.

* Compile: wfc[386] mix1f.for

        integer function tmax3( arga, argb, argc )
        integer arga, argb, argc

.millust break
        tmax3 = arga
        if ( argb .gt. tmax3 ) tmax3 = argb
        if ( argc .gt. tmax3 ) tmax3 = argc
        end
.millust end
.*
.section How do I pass integers from FORTRAN to a C function?
.*
.np
.ix 'mixed-language programming' 'passing integers'
The following &watf routine passes three integers to a &watc
function that returns an integer value.
.millust begin
*  MIX2F.FOR - This FORTRAN program calls a C function to
*              compute the max of three numbers.
*
* Compile/Link: wfl[386] mix2f mix2c.obj /fe=mix2

*$pragma aux tmax3 "!_" parm (value)

        program mix2f

        integer*4  tmax3
        integer*4  result
        integer*4  i, j, k

.millust break
        i = -1
        j = 12
        k = 5
        result = tmax3( i, j, k )
        print *, 'Maximum is ', result
        end
.millust end
.np
The C function "tmax3" is shown below.
.millust begin
/* MIX2C.C - This C function accepts 3 integer arguments
 *           and returns their maximum.
 *
 * Compile: wcc /ml mix2c
 *          wcc386  mix2c
 */

long int tmax3( long int arga,
                long int argb,
                long int argc )
.millust break
{
    long int   result;
    result = arga;
    if( argb > result ) result = argb;
    if( argc > result ) result = argc;
    return( result );
}
.millust end
.*
.section How do I pass a string from a C function to FORTRAN?
.*
.np
.ix 'mixed-language programming' 'passing strings'
Character strings are referenced differently in C and FORTRAN.
The C language terminates its strings with a null character as an
End-Of-String (EOS) marker.
In this case, C need not store the length of the string in memory.
FORTRAN, however, does not use any EOS marker; hence it must store
each string's length in memory.
.np
The structure FORTRAN uses to keep track of character data is called a
"string descriptor" which consists of a pointer to the character data
(2, 4, or 6 bytes, depending on the data model)
followed by an unsigned integer length (2 bytes or 4 bytes, depending
on the data model).
.millust begin
    system option   size of pointer     size of length
    ------ ------   ---------------     --------------
    16-bit /MM      16 bits             16 bits
    16-bit /ML      32 bits             16 bits
    32-bit /MF      32 bits             32 bits
    32-bit /ML      48 bits             32 bits
.millust end
.pc
In order to access character data, FORTRAN needs to have access to the
data's string descriptor.
Hence, FORTRAN expects a pointer to a string descriptor to be passed
as an argument for character data.
.np
Passing string arguments between C and FORTRAN is a simple task of
describing a struct type in C containing the two fields described
above.
The first field must contain the pointer to the character data, and
the second field must contain the length of the string being passed.
A pointer to this structure can then be passed to FORTRAN.
.*
.millust begin
* MIX3F.FOR - This FORTRAN program calls a function written
*             in C that passes back a string.
*
* Compile/Link: wfl[386] mix3f mix3c.obj /fe=mix3

        program mix3f

        character*80 sendstr
        character*80 cstring

.millust break
        cstring = sendstr()
        print *, cstring(1:lentrim(cstring))
        end
.millust end
.np
The C function "sendstr" is shown below.
.millust begin
/* MIX3C.C - This C function passes a string back to its
 *           calling FORTRAN program.
 *
 * Compile: wcc /ml mix3c
 *          wcc386  mix3c
 */

#include <string.h>

#pragma aux sendstr "^";

typedef struct descriptor {
    char          *addr;
    unsigned       len;
} descriptor;

.millust break
void sendstr( descriptor *ftn_str_desc )
{
    ftn_str_desc->addr = "This is a C string";
    ftn_str_desc->len  = strlen( ftn_str_desc->addr );
}
.millust end
.*
.section How do I pass a string from FORTRAN to a C function?
.*
.np
.ix 'mixed-language programming' 'passing strings'
By default, FORTRAN passes the address of the string descriptor when
passing strings.
If the C function knows it is being passed a string descriptor
address, then it is very similar to the above example.
If the C function is expecting normal C-type strings, then a FORTRAN
pragma can be used to pass the string correctly.
When the &watf compiler pragma to pass by value is used for strings,
then just a pointer to the string is passed.
.exam begin
*$pragma aux cname "!_" parm (value)
.exam end
.np
The following example FORTRAN mainline defines a string, and passes it
to a C function that prints it out.
.millust begin
* MIX4F.FOR - This FORTRAN program calls a function written
*             in C and passes it a string.
*
* Compile/Link: wfl[386] mix4f mix4c.obj /fe=mix4

*$pragma aux cstr "!_" parm (value)

        program mix4f

        character*80 forstring

.millust break
        forstring = 'This is a FORTRAN string'//char(0)
        call cstr( forstring )
        end
.millust end
.np
The C function:
.millust begin
/* MIX4C.C - This C function prints a string passed from
 *           FORTRAN.
 *
 * Compile: wcc /ml mix4c
 *          wcc386  mix4c
 */

#include <stdio.h>

.millust break
void cstr( char *instring )
{
    printf( "%s\n", instring );
}
.millust end
.*
.section How do I access a FORTRAN common block from within C?
.*
.np
.ix 'mixed-language programming' 'common blocks'
The following code demonstrates a technique for accessing a FORTRAN
common block in a C routine.
The C routine defines an extern struct to correspond to the FORTRAN
common block.
.*
.millust begin
* MIX5F.FOR - This program shows how a FORTRAN common
*             block can be accessed from C.
*
* Compile/Link: wfl[386] mix5f mix5c.obj /fe=mix5

        program mix5f
        external put
        common/cblk/i,j

.millust break
        i=12
        j=10
        call put
        print *, 'i = ', i
        print *, 'j = ', j
        end
.millust end
The C function:
.millust begin
/* MIX5C.C - This code shows how to access a FORTRAN
 *           common block from C.
 *
 * Compile: wcc /ml mix5c
 *          wcc386  mix5c
 */
#include <stdio.h>

#pragma aux put "^";
#pragma aux cblk "^";

.millust break
#ifdef __386__
#define FAR
#else
#define FAR far
#endif

extern struct cb {
    long int i,j;
} FAR cblk;

.millust break
void put( void )
{
    printf( "i = %ld\n", cblk.i );
    printf( "j = %ld\n", cblk.j );
    cblk.i++;
    cblk.j++;
}
.millust end
.np
For the 16-bit C compiler, the common block "cblk" is described as
.kw far
to force a load of the segment portion of the address.
Otherwise, since the object is smaller than 32K (the default data
threshold), it is assumed to be located in the DGROUP group which is
accessed through the SS segment register.
.*
.section How do I call a C function that accepts a variable number of arguments?
.*
.np
.ix 'mixed-language programming' 'variable number of arguments'
One capability that C possesses is the ability to define functions
that accept variable number of arguments.
This feature is not present, however, in the definition of the FORTRAN
77 language.
As a result, a special pragma is required to call these kinds of
functions.
.millust begin
*$pragma aux printf "!_" parm (value) caller []
.millust end
.np
The "caller" specifies that the caller will pop the arguments from the
stack.
The "[]" indicates that there are no arguments passed in registers
because the
.mono printf
function takes a variable number of arguments passed on the stack.
The following example is a FORTRAN function that uses this pragma.
It calls the
.mono printf
function to print the value 47 on the screen.
.millust begin
* MIX6.FOR - This FORTRAN program calls the C
*            printf function.

* Compile/Link: wfl[386] mix6

*$pragma aux printf "!_" parm (value) caller []

        program mix6

.millust break
        character cr/z0d/, nullchar/z00/

        call printf( 'Value is %ld.'//cr//nullchar, 47 )
        end
.millust end
.*
.np
For more information on the pragmas that are used extensively during
inter-language programming, please refer to the chapter entitled
"Pragmas" in both the
.us &watc User's Guide
and the
.us &watf User's Guide.
