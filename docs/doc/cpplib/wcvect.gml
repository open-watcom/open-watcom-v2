:CLFNM.WC&lpref.Vector<Type>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.Vector<Type>
:HFILE.wcvector.h
:CLSS.
The &cls. is a templated class used to store objects in a vector.
Vectors are similar to arrays, but vectors perform bounds checking
and can be resized.
Elements are inserted into the vector by assigning to a vector index.
:P.
The
.MONO WC&lpref.OrderedVector
and
.MONO WC&lpref.SortedVector
classes are also available.  They provide a more abstract view of the vector
and additional functionality, including finding and removing elements.

.if &lpref. eq Val .do begin
:P.
Values are copied into the vector, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value vectors should not be used to store objects of a base class if any
derived types of different sizes would be stored in the vector, or if the
destructor for a derived class must be called.
.do end

:P.
In the description of each member function, the text
.MONO Type
is used to indicate the template parameter defining

.if &lpref. eq Val .do begin
the type of the elements stored in the vector.
.do end
.el .do begin
the type pointed to by the pointers stored in the vector.
.do end

:P.
:CMT. state WCExcept is base class
:INCLUDE file='_EXPT_BC'.
:HDG.Requirements of Type
.if &lpref. eq Val .do begin

The &cls requires
.MONO Type
to have:
:P.
A default constructor (
.MONO Type::Type()
).
:P.
A well defined copy constructor (
.MONO Type::Type( const Type & )
).
:P.
The following override of
.MONO operator new()
only if
.MONO Type
overrides the global
.MONO operator new():
:XMPL
void * operator new( size_t, void *ptr ) { return( ptr ); }
:eXMPL

.do end
.el .do begin

The &cls requires nothing from
.MONO Type.

.do end
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c' .WC&lpref.Vector( size_t = 0 );
:MFCD cd_idx='c' .WC&lpref.Vector( size_t, const &lparm. );
:MFCD cd_idx='c' .WC&lpref.Vector( const WC&lpref.Vector & );
:MFCD cd_idx='d' .virtual ~~WC&lpref.Vector();
:MFN index='clear'          .void clear();
.if &lpref eq Ptr .do begin
:MFN index='clearAndDestroy'.void clearAndDestroy();
.do end
:MFN index='length'         .size_t length() const;
:MFN index='resize'         .int resize( size_t );
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
:MFN index='operator []'     .&lret. & operator []( int );
.if &lpref. eq Val .do begin
:MFN index='operator []'     .const Type & operator []( int ) const;
.do end
.el .do begin
:MFN index='operator []'     .Type * const & operator []( int ) const;
.do end
:MFN index='operator ='      .WC&lpref.Vector & operator =( const WC&lpref.Vector & );
:MFN index='operator =='     .int operator ==( const WC&lpref.Vector & ) const;
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WC&lpref.Vector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.Vector( size_t = 0 );
:eSNPL.
:SMTICS.
The &fn. creates a &obj. able to store the number of elements specified
in the optional parameter, which defaults to zero.

.if &lpref eq Val .do begin
All vector elements are initialized with
.MONO Type's
default constructor.
.do end
.el .do begin
All vector elements are initialized to NULL(0).
.do end
:P.
If the vector object cannot be fully initialized, the vector is created
with length zero.
:RSLTS.
The &fn. creates an initialized &obj. with the specified length.
:SALSO.
:SAL typ='ctor'.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WC&lpref.Vector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.Vector( size_t, const &lparm. );
:eSNPL.
:SMTICS.
The &fn. creates a &obj. able to store the number of elements specified
by the first parameter.

.if &lpref eq Val .do begin
All vector elements are initialized to the value of the second parameter
using
.MONO Type's
copy constructor.
.do end
.el .do begin
All vector elements are initialized to the pointer value given by the second
parameter.
.do end
:P.
If the vector object cannot be fully initialized, the vector is created
with length zero.
:RSLTS.
The &fn. creates an initialized &obj. with the specified length and elements
set to the given value.
:SALSO.
:SAL typ='ctor'.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WC&lpref.Vector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.Vector( const WC&lpref.Vector & );
:eSNPL.
:SMTICS.
The &fn. is the copy constructor for the &cls.:PERIOD.
The new vector is created with the same length as the given vector.
All of the vector elements and exception trap states are copied.
:P.
If the new vector cannot be fully created, it will have length zero.  The
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if enabled in the vector being copied.
:RSLTS.
The &fn. creates a &obj. which is a copy of the passed vector.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~WC&lpref.Vector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.Vector();
:eSNPL.
:SMTICS.
The &fn. is the destructor for the &cls.:PERIOD.
If the vector is not length zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the vector elements are cleared using the
.MONO clear
member function.

.if &lpref. eq Ptr .do begin
The objects which the vector elements point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
:INCLUDE file='_DTOR'.
:RSLTS.
The &fn. destroys an &obj.:PERIOD.
:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='omtyp' ocls='WCExcept'.not_empty
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='clear'     .void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the vector so that it is of zero length.

.if &lpref. eq Val .do begin
Elements stored in the vector are destroyed using
.MONO Type's
destructor.
.do end
.el .do begin
Objects pointed to by the vector elements are not deleted.
.do end

The vector object is not destroyed and re-created by this function, so
the object destructor is not invoked.
:RSLTS.
The &fn. clears the vector to have zero length and no vector elements.
:SALSO.
:SAL typ='dtor'
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.operator~b=
:eSALSO.
:eLIBF.

.if &lpref. eq Ptr .do begin
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.clearAndDestroy
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='clearAndDestroy'   .void clearAndDestroy();
:eSNPL.
:SMTICS.
The &fn. is used to clear the vector to have zero length and delete
the objects pointed to by the vector elements.
The vector object is not destroyed and re-created by this function, so
the vector object destructor is not invoked.
:RSLTS.
The &fn. clears the vector by deleting the objects pointed to by the
vector elements and makes the vector zero length.
:SALSO.
:SAL typ='fun'.clear
:eSALSO.
:eLIBF.
.do end

:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.length
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='length'    .size_t length() const;
:eSNPL.
:SMTICS.
The &fn. is used to find the number of elements which can be stored in
the &obj.:PERIOD.
:RSLTS.
The &fn. returns the length of the vector.
:SALSO.
:SAL typ='fun'.resize
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.operator []
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='operator []'     .&lret. & operator []( int );
.if &lpref. eq Val .do begin
:SNPF index='operator []'     .const Type & operator []( int ) const;
.do end
.el .do begin
:SNPF index='operator []'     .Type * const & operator []( int ) const;
.do end
:eSNPL.
:SMTICS.
.MONO operator []
is the vector index operator.  A reference to the object stored
in the vector at the given index is returned.  If a constant vector is
indexed, a reference to a constant element is returned.
The index operator of a non-constant vector is the only way to insert
an element into the vector.
:P.
If an attempt to access an element with index greater than or equal to the
length of a non-constant vector is made and the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is enabled, the exception is thrown.  If the exception is not
enabled, the vector is automatically resized using the
.MONO resize
member function to have length the index
value plus one.

.if &lpref. eq Val .do begin
New vector elements are initialized using
.MONO Type's
default constructor.
.do end
.el .do begin
New vector elements are initialized to NULL(0).
.do end

If the resize failed, and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled, the exception is thrown.  If the exception is not
enabled and the resize failed, the last element is indexed (a new
element if the vector was zero length).
If a negative value is used to index the non-constant vector and the
.MONO index_range
.ix 'index_range' 'exception'
exception is enabled, the exception is thrown.  If the exception is not
enabled and the vector is empty, the
.MONO resize_required
exception may be thrown.
:P.
An attempt to index an empty constant vector may cause one of two
exceptions to be thrown.  If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, it is thrown.  Otherwise, the
.MONO index_range
.ix 'index_range' 'exception'
exception is thrown, if enabled.
If neither exception is enabled, a first vector element is added and
indexed (so that a reference to a valid element can be returned).
:P.
Indexing with a negative value or a value greater than or equal to the
length of a constant vector causes the
.MONO index_range
.ix 'index_range' 'exception'
exception to be thrown, if enabled.
:RSLTS.
The &fn. returns a reference to the element at the given index.
If the index is invalid, a reference to the closest valid element is returned.
The result of the non-constant index operator may be assigned to.
:SALSO.
:SAL typ='fun'.resize
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPF index='operator ='   .WC&lpref.Vector & operator =( const WC&lpref.Vector & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the &cls.:PERIOD.
The left hand side vector is first cleared using the
.MONO clear
member function, and then the right hand side vector is copied.
The left hand side vector is made to have the same length as the right
hand side.
All of the vector elements and exception trap states are copied.
:P.
If the left hand side vector cannot be fully created, it will have
zero length.  The
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if enabled in the right hand side vector.
:RSLTS.
The &fn. assigns the left hand side vector to be a copy of the right hand side.
:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator ==
:SNPL.
:SNPFLF          .#include <wcvector.h>
:SNPFLF          .public:
:SNPF index='operator =='.int operator ==( const WC&lpref.Vector & ) const;
:eSNPL.
:SMTICS.
The &fn. is the equivalence operator for the &cls.:PERIOD.
Two vector objects are equivalent if they are the same object and share the
same address.
:RSLTS.
A TRUE (non-zero) value is returned if the left hand side and right
hand side vectors are the same object.  A FALSE (zero) value is returned
otherwise.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.resize
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='resize'    .int resize( size_t new_size );
:eSNPL.
:SMTICS.
The &fn. is used to change the vector size to be able to store
:HP1.new_size:eHP1. elements.
If :HP1.new_size:eHP1. is larger than the previous vector size, all
elements will be copied

.if &lpref eq Val .do begin
(using
.MONO Type's
copy constructor)
.do end

into the newly sized vector, and new elements are initialized

.if &lpref eq Val .do begin
with
.MONO Type's
default constructor.
.do end
.el .do begin
to NULL(0).
.do end
If the vector is resized to a smaller size, the first :HP1.new_size:eHP1.
elements are copied.

.if &lpref eq Val .do begin
The remaining elements are destroyed using
.MONO Type's
destructor.
.do end
.el .do begin
The objects pointed to by the remaining elements are not deleted.
.do end

:P.
If the resize cannot be performed and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled, the exception is thrown.
:RSLTS.
The vector is resized to :HP1.new_size:eHP1.:PERIOD.
A TRUE value (non-zero) is returned if the resize is successful.
A FALSE (zero) result is returned if the resize fails.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
