.func _m_empty
.synop begin
#include <mmintrin.h>
void  _m_empty(void);
.synop end
.desc begin
The
.id &func.
function empties the multimedia state.
The values in the Multimedia Tag Word (TW) are set to empty
(i.e., all ones).
This will indicate that no Multimedia registers are in use.
.np
This function is useful for applications that mix floating-point (FP)
instructions with multimedia instructions.
Intel maps the multimedia registers onto the floating-point registers.
For this reason, you are discouraged from intermixing MM code and FP
code.
The recommended way to write an application with FP instructions and
MM instructions is:
.begbull
.bull
Split the FP code and MM code into two separate instruction streams
such that each stream contains only instructions of one type.
.bull
Do not rely on the contents of FP/MM registers across transitions
from one stream to the other.
.bull
Leave the MM state empty at the end of an MM stream using the
.id &func.
function.
.bull
Similarly, leave the FP stack empty at the end of an FP stream.
.endbull
.desc end
.return begin
The
.id &func.
function does not return a value.
.return end
.see begin
.im seemmfun &function.
.see end
.ix 'MMX detection'
.ix 'CPUID'
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

long featureflags( void );

#pragma aux featureflags = \
    ".586"          \
    "mov eax,1"     \
    "cpuid"         \
    "mov eax,edx"   \
    modify [eax ebx ecx edx]
.exmp break
#define MM_EXTENSION 0x00800000
.exmp break
void main( void )
{
    if( featureflags() & MM_EXTENSION ) {
    /*
        sequence of code that uses Multimedia functions
        .
        .
        .
    */
.exmp break
        _m_empty();
    }
.exmp break
    /*
        sequence of code that uses floating-point
        .
        .
        .
    */
}
.exmp end
.class Intel
.system
