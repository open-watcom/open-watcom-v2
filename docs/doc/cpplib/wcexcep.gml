:P.
This chapter describes exception handling for the container classes.
:CMT.========================================================================
:CLFNM.WCExcept
:CMT.========================================================================
:LIBF fmt='hdr'.WCExcept
:HFILE.wcexcept.h
:CLSS.
The &cls. provides the exception handling for the container classes.
If you have compiled your code with exception handling enabled,
the C++ exception processing can be used to catch errors.
Your source file must be compiled with the exception handling
compile switch for C++ exception processing to occur.
The container classes will attempt to set the container object into a reasonable
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
header file is included by the header files for each of the container classes.
There is normally no need to explicitly include the
.MONO wcexcept.h
header file, but no errors will result if it is included.
This class is inherited as a base class for each
of the containers.
You do not need to derive from it directly.
:P.
The
.MONO WCListExcept
.ix 'WCListExcept' 'class'
class (formally used by the list container classes) has been replaced by
the
.MONO WCExcept
class.
A typedef of the
.MONO WCListExcept
class to the
.MONO WCExcept
class and the
.MONO wclist_state
type to the
.MONO wc_state
type provide backward compatability with previous
versions of the list containers.
:HDG.Public Enumerations
The following enumeration typedefs are declared in the public interface:
:MTYPL.
:MTYP index='wc_state' .typedef int wc_state;
:eMTYPL.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c'.WCExcept();
:MFCD cd_idx='d'.virtual ~~WCExcept();
:MFN index='exceptions'         .wc_state exceptions() const;
:MFN index='exceptions'         .wc_state exceptions( wc_state );
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WCExcept
:SNPL.
:SNPFLF                    .#include <wcexcept.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCExcept();
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj.:PERIOD.
:P.
The &fn. is used implicitly by the compiler
when it generates a constructor for a derived class.
It is automatically used by the list container classes,
and should not be required in any user derived classes.
:RSLTS.
The &fn. produces an initialized &obj. with no exception traps enabled.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~WCExcept
:SNPL.
:SNPFLF                   .#include <wcexcept.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual ~~WCExcept();
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
:MFN index='exceptions' .wc_state exceptions() const;
:MFN index='exceptions' .wc_state exceptions( wc_state set_flags );
:eSNPL.
:SMTICS.
The &fn. queries and/or sets the bits that control which exceptions
are enabled for the list class.
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
:LIBF fmt='mtyp' prot='public'.wc_state
:SNPL.
:SNPFLF                 .#include <wcexcept.h>
:SNPFLF                 .public:
:SNPFLF                 .enum wcstate {
:SNPT index='all_fine'  .  all_fine       = 0x0000,  // - no errors
:SNPT index='check_none'.  check_none     = all_fine,// - throw no exceptions
:SNPT index='not_empty' .  not_empty      = 0x0001,  // - container not empty
:SNPT index='index_range'    .  index_range    = 0x0002,  // - index is out of range
:SNPT index='empty_container'.  empty_container= 0x0004,  // - empty container error
:SNPT index='out_of_memory'  .  out_of_memory  = 0x0008,  // - allocation failed
:SNPT index='resize_required'.  resize_required= 0x0010,  // - request needs resize
:SNPT index='not_unique'     .  not_unique     = 0x0020,  // - adding duplicate
:SNPT index='zero_buckets'   .  zero_buckets   = 0x0040,  // - resizing hash to zero
:SNPFLF                 .  // value to use to check for all errors
:SNPT index='check_all' .  check_all   = (not_empty|index_range|empty_container
:SNPFLF                 .                |out_of_memory|resize_required
:SNPFLF                 .                |not_unique|zero_buckets)
:SNPFLF                 .};
:SNPT index='wc_state'.typedef int wc_state;
:eSNPL.
:SMTICS.
The type
:MONO.WCExcept::wcstate
is a set of bits representing the current state of the container object.
The &fn. represents the same set of bits, but uses an
:MONO.int
to represent the values, thereby avoiding problems made possible by the
compiler's ability to use smaller types for enumerations.
All uses of these bits should use the &fn.:PERIOD.
:P.
The bit values defined by the &fn. can be read and set by the
:MONO.exceptions
.ix 'exceptions'
member function,
which is also used to control exception handling.
:P.
The
.MONO WCExcept::not_empty
.ix 'not_empty' 'exception'
bit setting traps the destruction of a container when the container
has at one or more entries.
If this error is not trapped, memory may not be properly released back to the
system.
:P.
The
.MONO WCExcept::index_range
.ix 'index_range' 'exception'
state setting traps an attempt to access a container item by an index value
that is either not positive or is larger than the index of the last
item in the container.
:P.
The
.MONO WCExcept::empty_container
.ix 'empty_container' 'exception'
bit setting traps an attempt to perform and invalid operation on a
container with no entries.
:P.
The
.MONO WCExcept::out_of_memory
.ix 'out_of_memory' 'exception'
bit setting traps any container class allocation failures.
If this exception is not enabled, the operation in which the allocation
failed will return a FALSE (zero) value.  Container class copy constructors
and assignment operators can also throw this exception, and if not enabled
incomplete copies may result.
:P.
The
.MONO WCExcept::resize_required
.ix 'resize_required' 'exception'
bit setting traps any vector operations which cannot be performed unless the
vector is resized to a larger size.  If this exception is not enabled, the
vector class will attempt an appropriate resize when necessary for
an operation.
:P.
The
.MONO WCExcept::not_unique
.ix 'not_unique' 'exception'
bit setting traps an attempt to add a duplicate value to a set container, or
a duplicate key to a dictionary container.  The duplicate value is not
added to the container object regardless of the exception trap state.
:P.
The
.MONO WCExcept::zero_buckets
.ix 'zero_buckets' 'exception'
bit setting traps an attempt to resize of hash container to have zero buckets.
No resize is performed whether or not the exception is enabled.
:eLIBF.
