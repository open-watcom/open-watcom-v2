.func setjmp
#include <setjmp.h>
int setjmp( jmp_buf env );
.ixfunc2 'Non-local Jumps' &func
.funcend
.desc begin
The &func function saves its calling environment in its
.kw jmp_buf
argument, for subsequent use by the
.kw longjmp
function.
.np
In some cases, error handling can be implemented by using &func to record
the point to which a return will occur following an error.
When an error is detected in a called function, that function uses
.kw longjmp
to jump back to the recorded position.
The original function which called &func must still be active (it cannot
have returned to the function which called it).
.np
Special care must be exercised to ensure that any side effects that
are left undone (allocated memory, opened files, etc.) are
satisfactorily handled.
.desc end
.return begin
The &func function returns zero when it is initially called.
The return value will be non-zero if the return is the result of a call
to the
.kw longjmp
function.
An
.mono if
statement is often used to handle these two returns.
When the return value is zero, the initial call to &func
has been made; when the return value is non-zero, a return from a
.kw longjmp
has just occurred.
.return end
.see begin
.seelist setjmp longjmp
.see end
.exmp begin
#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

rtn()
  {
    printf( "about to longjmp\n" );
    longjmp( env, 14 );
  }
.exmp break
void main()
  {
    int ret_val = 293;

    if( 0 == ( ret_val = setjmp( env ) ) ) {
      printf( "after setjmp %d\n", ret_val );
      rtn();
      printf( "back from rtn %d\n", ret_val );
    } else {
      printf( "back from longjmp %d\n", ret_val );
    }
  }
.exmp output
after setjmp 0
about to longjmp
back from longjmp 14
.exmp end
.class ANSI
.system
