/*
 *  Changelist 31737
 *  Should cause an error because the pragma attempts to define code for an already existing function
 *  
 *  Uses 16 bit pragma for 386 and i86 test
 */

int foo(int i) { return i; }

#pragma aux foo = \
    "mov  ax,1"  \
    parm caller [ax]

int main(void)
{
    return foo(5);
}
