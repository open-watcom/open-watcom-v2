:CLFNM.WCIterExcept
:CMT.========================================================================
:LIBF fmt='hdr'.WCIterExcept
:HFILE.wcexcept.h
:CLSS.
The &cls. provides the exception handling for the container iterators.
If you have compiled your code with exception handling enabled,
the C++ exception processing can be used to catch errors.
Your source file must be compiled with the exception handling
compile switch for C++ exception processing to occur.
The iterators will attempt to set the class into a reasonable
state if there is an error and exception handling is not enabled, or
if the trap for the specific error has not been enabled by your program.
:P.
By default, no exception traps are enabled and no exceptions will be thrown.
Exception traps are enabled by setting the exception state with the
.MONO exceptions
member function.
:P.
The
.MONO wcexcept.h
header file is included by the header files for each of the iterator classes.
There is normally no need to explicitly include the
.MONO wcexcept.h
header file, but no errors will result if it is included.
This class is inherited as part of the base construction for each
of the iterators.
You do not need to derive from it directly.
:HDG.Public Enumerations
The following enumeration typedefs are declared in the public interface:
:MTYPL.
:MTYP index='wciter_state' .typedef int wciter_state;
:eMTYPL.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c'.WCIterExcept();
:MFCD cd_idx='d'.virtual ~~WCIterExcept();
:MFN index='exceptions'         .wciter_state exceptions() const;
:MFN index='exceptions'         .wciter_state exceptions( wciter_state );
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WCIterExcept
:SNPL.
:SNPFLF                    .#include <wcexcept.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCIterExcept();
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj.:PERIOD.
:P.
The &fn. is used implicitly by the compiler
when it generates a constructor for a derived class.
:CMT. It is automatically used by the iterator classes,
:CMT. and should not be required in any user derived classes.
:RSLTS.
The &fn. produces an initialized &obj. with no exception traps enabled.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~WCIterExcept
:SNPL.
:SNPFLF                   .#include <wcexcept.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual ~~WCIterExcept();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit.
The call to the &fn. is inserted implicitly by the compiler
at the point where the object derived from
.MONO &cl_name.
goes out of scope.
:RSLTS.
The object derived from
.MONO &cl_name.
is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.exceptions
:SNPL.
:SNPFLF               .#include <wcexcept.h>
:SNPFLF               .public:
:MFN index='exceptions' .wciter_state exceptions() const;
:MFN index='exceptions' .wciter_state exceptions( wciter_state set_flags );
:eSNPL.
:SMTICS.
The &fn. queries and/or sets the bits that control which exceptions
are enabled for the iterator class.
Each bit corresponds to an exception, and is set if the exception is enabled.
The first form of the &fn. returns the current settings of the
exception bits.
The second form of the function sets the exception bits to those
specified by :HP1.set_flags:eHP1.:PERIOD.
:RSLTS.
The current exception bits are returned.
If a new set of bits are being set, the returned value is the old
set of exception bits.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mtyp' prot='public'.wciter_state
:SNPL.
:SNPFLF                       .#include <wcexcept.h>
:SNPFLF                       .public:
:SNPFLF                  .enum wciterstate {
:SNPT index='all_fine'   .all_fine    = 0x0000,  // - no errors
:SNPT index='check_none' .check_none  = all_fine,// - disable all exceptions
:SNPT index='undef_iter' .undef_iter  = 0x0001,  // - position is undefined
:SNPT index='undef_item' .undef_item  = 0x0002,  // - iterator item is undefined
:SNPT index='iter_range' .iter_range  = 0x0004,  // - advance value is bad
:SNPFLF                  .// value to use to check for all errors
:SNPT index='check_all'  .check_all= (undef_iter|undef_item|iter_range)
:SNPFLF                  .};
:SNPT index='wciter_state'.typedef int wciter_state;
:eSNPL.
:SMTICS.
The type
:MONO.WCIterExcept::wciterstate
is a set of bits representing the current state of the iterator.
The &fn. represents the same set of bits, but uses an
:MONO.int
to represent the values, thereby avoiding problems made possible by the
compiler's ability to use smaller types for enumerations.
All uses of these bits should use the &fn.:PERIOD.
:P.
The bit values defined by the &fn. can be read and set by the
member function
:MONO.exceptions
:CONT.,
.ix 'exceptions' 'function'
which is used to control exception handling.
:P.
The
.MONO WCIterExcept::undef_iter
.ix 'undef_iter' 'exception'
bit setting traps the use of the iterator when the position within
the container object is undefined.
Trying to operate on an iterator with no associated container object,
increment an iterator which is after the last element,
or decrement an iterator positioned before the first element
is an undefined operation.
:P.
The
.MONO WCIterExcept::undef_item
.ix 'undef_item' 'exception'
bit setting traps an attempt to obtain the current element of the iterator
when the iterator has no associated container object, or is positioned either
before or after the container elements.  The
.MONO undef_item
exception can be thrown only by the
.MONO key
and
.MONO value
dictionary iterator member functions, and the
.MONO current
member function for non-dictionary iterators.
.MONO
:P.
The
.MONO WCIterExcept::iter_range
.ix 'iter_range' 'exception'
bit setting traps an attempt to use a iteration count value that would
place the iterator more than one element past the end or before the
beginning of the container elements.  The
.MONO iter_range
exception can be thrown only by the
.MONO operator +=
and
.MONO operator -=
operators.
:eLIBF.
