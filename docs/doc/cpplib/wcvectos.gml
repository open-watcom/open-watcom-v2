.*
.* generator for ordered and sorted vectors
.*
:CLFNM cl2='WC&lpref.OrderedVector<Type>'.WC&lpref.SortedVector<Type>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.SortedVector<Type>, WC&lpref.OrderedVector<Type>
:HFILE.wcvector.h
:CLSS.
The &cls. are templated classes used to store objects in a vector.
Ordered and Sorted vectors are powerful arrays which can be resized and provide
an abstract interface to insert, find and remove elements.
An ordered vector maintains the order in which elements are added, and
allows more than one copy of an element that is equivalent.
The sorted vector allow only one copy of an equivalent element, and inserts
them in a sorted order.
The sorted vector is less efficient when inserting elements, but can provide
a faster retrieval time.
:P.
Elements cannot be inserted into these vectors by assigning to a vector index.
Vectors automatically grow when necessary to insert an element if the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is not enabled.
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the template parameter defining

.if &lpref. eq Val .do begin
the type of the elements stored in the vector.
:P.
Values are copied into the vector, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value vectors should not be used to store objects of a base class if any
derived types of different sizes would be stored in the vector, or if the
destructor for a derived class must be called.
.do end
.el .do begin
the type pointed to by the pointers stored in the vector.
:P.
Note that lookups are performed on the types pointed to, not just
by comparing pointers.  Two pointer elements are equivalent if the values
they point to are equivalent.  The values pointed to do not need to
be the same object.
.do end

:P.
The
.MONO WC&lpref.OrderedVector
class stores elements in the order which they are inserted using the
.MONO insert, append, prepend
and
.MONO insertAt
member functions.  Linear searches are performed to locate entries, and
the less than operator is not required.
:P.
The
.MONO WC&lpref.SortedVector
class stores elements in ascending order.  This requires that
.MONO Type
provides
a less than operator.  Insertions are more expensive than inserting or
appending into an ordered vector, since entries must be moved to make
room for the new element.  A binary search is used to locate elements in
a sorted vector, making searches quicker than in the ordered vector.
:P.
Care must be taken when using the
.MONO WC&lpref.SortedVector
class not to change the ordering of the vector
elements.

.if &lpref. eq Val .do begin
The result returned by the index operator must not be assigned to or
modified in such a way that it is no longer equivalent to the value inserted
into the vector.
Lookups assume elements are in sorted order.
.do end
.el .do begin
An object pointed to by a vector element must not be changed so that
it is not equivalent to the value when the pointer was inserted into the
vector.
The index operator and the member functions
.MONO find, first,
and
.MONO last
all return pointers the elements pointed to by the vector elements.
Lookups assume elements are in sorted order, so you should not
use the returned pointers to change the ordering of the value
pointed to.
.do end

:P.
The
.MONO WC&lpref.Vector
class is also available.  It provides a resizable and boundary safe vector
similar to standard arrays.
:P.
:CMT. state WCExcept is base class
:INCLUDE file='_EXPT_BC'.
:HDG.Requirements of Type
Both the &cls. require
.MONO Type
to have:

.if &lpref. eq Val .do begin
:P.
A default constructor (
.MONO Type::Type()
).
:P.
A well defined copy constructor (
.MONO Type::Type( const Type & )
).
:P.
A well defined assignment operator
.br
(
.MONO Type & operator =( const Type & )
).
:P.
The following override of
.MONO operator new()
if
.MONO Type
overrides the global
.MONO operator new():
:XMPL
void * operator new( size_t, void *ptr ) { return( ptr ); }
:eXMPL
.do end

:P
A well defined equivalence operator with constant parameters
.br
(
.MONO int operator ==( const Type & ) const
).
:P.
Additionally the
.MONO WC&lpref.SortedVector
class requires
.MONO Type
to have:
:P
A well defined less than operator with constant parameters
.br
(
.MONO int operator <( const Type & ) const
).
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFN index='WC&lpref.OrderedVector' .WC&lpref.OrderedVector( size_t = WCDEFAULT_VECTOR_LENGTH, unsigned = WCDEFAULT_VECTOR_RESIZE_GROW );
:MFN index='WC&lpref.OrderedVector' .WC&lpref.OrderedVector( const WC&lpref.OrderedVector & );
:MFN index='~~WC&lpref.OrderedVector' .virtual ~~WC&lpref.OrderedVector();
:MFN index='WC&lpref.SortedVector' .WC&lpref.SortedVector( size_t = WCDEFAULT_VECTOR_LENGTH, unsigned = WCDEFAULT_VECTOR_RESIZE_GROW );
:MFN index='WC&lpref.SortedVector' .WC&lpref.SortedVector( const WC&lpref.SortedVector & );
:MFN index='~~WC&lpref.SortedVector' .virtual ~~WC&lpref.SortedVector();
:MFN index='clear'          .void clear();
.if &lpref eq Ptr .do begin
:MFN index='clearAndDestroy'.void clearAndDestroy();
.do end
:MFN index='contains'       .int contains( const &lparm. ) const;
:MFN index='entries'        .unsigned entries() const;
.if &lpref. eq Val .do begin
:MFN index='find'           .int find( const Type &, Type & ) const;
.do end
.el .do begin
:MFN index='find'           .Type * find( const Type * ) const;
.do end
:MFN index='first'          .&lret first() const;
:MFN index='index'          .int index( const &lparm. ) const;
.if &lpref. eq Val .do begin
:MFN index='insert'         .int insert( const &lparm. );
.do end
.el .do begin
:MFN index='insert'         .int insert( &lparm. );
.do end
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='last'           .&lret. last() const;
:MFN index='occurrencesOf'  .int occurrencesOf( const &lparm. ) const;
.if &lpref. eq Val .do begin
:MFN index='remove'         .int remove( const Type & );
.do end
.el .do begin
:MFN index='remove'         .Type * remove( const Type * );
.do end
:MFN index='removeAll'      .unsigned removeAll( const &lparm. );
.if &lpref. eq Val .do begin
:MFN index='removeAt'       .int removeAt( int );
:MFN index='removeFirst'    .int removeFirst();
:MFN index='removeLast'     .int removeLast();
.do end
.el .do begin
:MFN index='removeAt'       .Type * removeAt( int );
:MFN index='removeFirst'    .Type * removeFirst();
:MFN index='removeLast'     .Type * removeLast();
.do end
:MFN index='resize'         .int resize( size_t );
:eMFNL.
The following public member functions are available for the
.MONO WC&lpref.OrderedVector
class only:
:MFNL.
.if &lpref. eq Val .do begin
:MFN index='append'         .int append( const &lparm. );
:MFN index='insertAt'       .int insertAt( int, const &lparm. );
:MFN index='prepend'        .int prepend( const &lparm. );
.do end
.el .do begin
:MFN index='append'         .int append( &lparm. );
:MFN index='insertAt'       .int insertAt( int, &lparm. );
:MFN index='prepend'        .int prepend( &lparm. );
.do end
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
:MFN index='operator ='      .WC&lpref.OrderedVector & WC&lpref.OrderedVector::operator =( const WC&lpref.OrderedVector & );
:MFN index='operator ='      .WC&lpref.SortedVector & WC&lpref.SortedVector::operator =( const WC&lpref.SortedVector & );
:MFN index='operator =='     .int WC&lpref.OrderedVector::operator ==( const WC&lpref.OrderedVector & ) const;
:MFN index='operator =='     .int WC&lpref.SortedVector::operator ==( const WC&lpref.SortedVector & ) const;
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.OrderedVector<Type>' fmt='ctor' prot='public'.WC&lpref.OrderedVector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.OrderedVector( size_t = WCDEFAULT_VECTOR_LENGTH,
:SNPFLF          .            unsigned = WCDEFAULT_VECTOR_RESIZE_GROW );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.OrderedVector<Type>
constructor creates an empty
.MONO WC&lpref.OrderedVector
object able to store the number of elements specified
in the first optional parameter, which defaults to the constant
.MONO WCDEFAULT_VECTOR_LENGTH
(currently defined as 10).
If the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is not enabled, then the second optional parameter is used to
specify the value to increase the vector size
when an element is inserted
into a full vector.
If zero(0) is specified as the second parameter, any attempt to insert into
a full vector fails.
This parameter defaults to the constant
.MONO WCDEFAULT_VECTOR_RESIZE_GROW
(currently defined as 5).
:P.
If the vector object cannot be fully initialized, the vector is created
with length zero.
:RSLTS.
The
.MONO WC&lpref.OrderedVector<Type>
constructor creates an empty initialized
.MONO WC&lpref.OrderedVector
object.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.OrderedVector<Type>' fmt='ctor' prot='public'.WC&lpref.OrderedVector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.OrderedVector( const WC&lpref.OrderedVector & );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.OrderedVector<Type>
constructor is the copy constructor for the
.MONO WC&lpref.OrderedVector
class.
The new vector is created with the same length and
resize value as the passed vector.
All of the vector elements and exception trap states are copied.
:P.
If the new vector cannot be fully created, it will have length zero.  The
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if enabled in the vector being copied.
:RSLTS.
The
.MONO WC&lpref.OrderedVector<Type>
creates a
.MONO WC&lpref.OrderedVector
object which is a copy of the passed vector.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.OrderedVector<Type>' fmt='dtor' prot='public'.~~WC&lpref.OrderedVector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.OrderedVector();
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.OrderedVector<Type>
destructor is the destructor for the
.MONO WC&lpref.OrderedVector
class.
If the vector is not length zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the vector entries are cleared using the
.MONO clear
member function.

.if &lpref. eq Ptr .do begin
The objects which the vector entries point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
The call to the
.MONO WC&lpref.OrderedVector<Type>
destructor is inserted implicitly by the compiler at the point where the
.MONO WC&lpref.OrderedVector
object goes out of scope.
:RSLTS.
The
.MONO WC&lpref.OrderedVector<Type>
destructor destroys an
.MONO WC&lpref.OrderedVector
object.
:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='omtyp' ocls='WCExcept'.not_empty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SortedVector<Type>' fmt='ctor' prot='public'.WC&lpref.SortedVector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SortedVector( size_t = WCDEFAULT_VECTOR_LENGTH,
:SNPFLF          .            unsigned = WCDEFAULT_VECTOR_RESIZE_GROW );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SortedVector<Type>
constructor creates an empty
.MONO WC&lpref.SortedVector
object able to store the number of elements specified
in the first optional parameter, which defaults to the constant
.MONO WCDEFAULT_VECTOR_LENGTH
(currently defined as 10).
If the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is not enabled, then the second optional parameter is used to
specify the value to increase the vector size when an element is inserted
into a full vector.
If zero(0) is specified as the second parameter, any attempt to insert into
a full vector fails.
This parameter defaults to the constant
.MONO WCDEFAULT_VECTOR_RESIZE_GROW
(currently defined as 5).
:P.
If the vector object cannot be fully initialized, the vector is created
with length zero.
:RSLTS.
The
.MONO WC&lpref.SortedVector<Type>
constructor creates an empty initialized
.MONO WC&lpref.SortedVector
object.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SortedVector<Type>' fmt='ctor' prot='public'.WC&lpref.SortedVector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SortedVector( const WC&lpref.SortedVector & );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SortedVector<Type>
constructor is the copy constructor for the
.MONO WC&lpref.SortedVector
class.
The new vector is created with the same length and
resize value as the passed vector.
All of the vector elements and exception trap states are copied.
:P.
If the new vector cannot be fully created, it will have length zero.  The
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if enabled in the vector being copied.
:RSLTS.
The
.MONO WC&lpref.SortedVector<Type>
constructor creates a
.MONO WC&lpref.SortedVector
object which is a copy of the passed vector.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SortedVector<Type>' fmt='dtor' prot='public'.~~WC&lpref.SortedVector
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.SortedVector();
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SortedVector<Type>
destructor is the destructor for the
.MONO WC&lpref.SortedVector
class.
If the vector is not length zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the vector entries are cleared using the
.MONO clear
member function.

.if &lpref. eq Ptr .do begin
The objects which the vector entries point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
The call to the
.MONO WC&lpref.SortedVector<Type>
destructor is inserted implicitly by the compiler at the point where the
.MONO WC&lpref.SortedVector
object goes out of scope.
:RSLTS.
The
.MONO WC&lpref.SortedVector<Type>
destructor destroys an
.MONO WC&lpref.SortedVector
object.
:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='omtyp' ocls='WCExcept'.not_empty
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.OrderedVector<Type>' fmt='mfun' prot='public'.append
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='append'        .int append( const &lparm. );
.do end
.el .do begin
:SNPF index='append'        .int append( &lparm. );
.do end
:eSNPL.
:SMTICS.
The &fn. appends the passed element to be the last element in the vector.

.if &lpref. eq Val .do begin
The data stored in the vector is a copy of the data passed as a parameter.
.do end

This member function has the same semantics as the
.MONO WC&lpref.OrderedVector::insert
member function.

:P.
This function is not provided by the
.MONO WC&lpref.SortedVector
class, since all elements must be inserted in sorted order by the
.MONO insert
member function.
:P.
Several different results can occur if the vector is not large enough for
the new element.
If the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is enabled, the exception is thrown.
If the exception is not enabled, the append fails if the amount the vector
is to be grown (the second parameter to the constructor) is zero(0).
Otherwise, the vector is automatically grown by
the number of elements specified to the constructor, using the
.MONO resize
member function.
If
.MONO resize
fails, the element is not appended to the vector and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown, if enabled.
:RSLTS.
The &fn. appends an element to the
.MONO WC&lpref.OrderedVector
object.  A TRUE (non-zero) value is returned if the append is successful.
If the append fails, a FALSE (zero) value is returned.
:SALSO.
:SAL typ='fun'.insert
:SAL typ='fun'.insertAt
:SAL typ='fun'.prepend
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.resize_required
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
The &fn. is used to clear the vector so that it contains no entries, and
is zero size.

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
The &fn. clears the vector to have zero length and no entries.
:SALSO.
:SAL typ='fun'.~~WC&lpref.OrderedVector
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.operator~b=
:eSALSO.
:eLIBF.

.if &lpref. eq Ptr .do begin
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.clearAndDestroy
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
:LIBF fmt='mfun' cllong='yes' prot='public'.contains
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='contains'  .int contains( const &lparm. ) const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if a value is contained by a vector.

.if &lpref eq Ptr .do begin
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

A linear search is used by the
.MONO WC&lpref.OrderedVector
class to find the value.  The
.MONO WC&lpref.SortedVector
class uses a binary search.
:RSLTS.
The &fn. returns a TRUE (non-zero) value if the element is found in the
vector.  A FALSE (zero) value is returned if the vector does not contain
the element.
:SALSO.
:SAL typ='fun'.index
:SAL typ='fun'.find
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.entries
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='entries'   .unsigned entries() const;
:eSNPL.
:SMTICS.
The &fn. is used to find the number of elements which are stored in
the vector.
:RSLTS.
The &fn. returns the number of elements in the vector.
:SALSO.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.find
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='find'          .int find( const Type &, Type & ) const;
.do end
.el .do begin
:SNPF index='find'          .Type * find( const Type * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to find an element equivalent to

.if &lpref. eq Val .do begin
the first argument.
.do end
.el .do begin
the element passed.
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

The
.MONO WC&lpref.OrderedVector
class uses a linear search to find the element, and the
.MONO WC&lpref.SortedVector
class uses a binary search.

:RSLTS.

.if &lpref. eq Val .do begin
If an equivalent element is found, a TRUE (non-zero) value is returned, and
the second parameter is assigned the first equivalent value.  A FALSE (zero)
value is returned and the second parameter is unchanged if the element is
not in the vector.
.do end
.el .do begin
A pointer to the first equivalent element is returned.  NULL(0) is returned
if the element is not in the vector.
.do end

:SALSO.
:SAL typ='fun'.contains
:SAL typ='fun'.first
:SAL typ='fun'.index
:SAL typ='fun'.last
:SAL typ='fun'.occurrencesOf
:SAL typ='fun'.remove
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.first
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='first'     .&lret first() const;
:eSNPL.
:SMTICS.
The &fn. returns the first element in the vector.  The element is not removed
from the vector.
:P.
If the vector is empty, one of two exceptions can be thrown.  The
.MONO empty_container
.ix 'empty_container' 'exception'
exception is thrown if it is enabled.  Otherwise, if the
.MONO index_range
.ix 'index_range' 'exception'
exception is enabled, it is thrown.
If neither exception is enabled, a first element of the vector is added

.if &lpref. eq Val .do begin
with a default value.
.do end
.el .do begin
with a NULL value.
.do end

:RSLTS.
The &fn. returns the value of the first element in the vector.
:SALSO.
:SAL typ='fun'.last
:SAL typ='fun'.removeFirst
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.index
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='index'     .int index( const &lparm. ) const;
:eSNPL.
:SMTICS.
The &fn. is used find the index of the first element equivalent to the passed
element.

.if &lpref eq Ptr .do begin
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

A linear search is used by the
.MONO WC&lpref.OrderedVector
class to find the element.  The
.MONO WC&lpref.SortedVector
class uses a binary search.
:RSLTS.
The &fn. returns the index of the first element equivalent to the parameter.
If the passed value is not contained in the vector, negative one (-1) is
returned.
:SALSO.
:SAL typ='fun'.contains
:SAL typ='fun'.find
:SAL typ='fun'.insertAt
:SAL typ='fun'.operator~b[]
:SAL typ='fun'.removeAt
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='insert'        .int insert( const &lparm. );
.do end
.el .do begin
:SNPF index='insert'        .int insert( &lparm. );
.do end
:eSNPL.
:SMTICS.
The &fn. inserts the value into the vector.
.if &lpref. eq Val .do begin
The data stored in the vector is a copy of the data passed as a parameter.
.do end
:P.
The
.MONO WC&lpref.OrderedVector::insert
function inserts the value as the last element of the vector, and has
the same semantics as the
.MONO WC&lpref.OrderedVector::append
member function.
:P.
A binary search is performed to determine where the value should be inserted
for the
.MONO WC&lpref.SortedVector::insert
function.

.if &lpref. eq Ptr .do begin
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

Any elements greater than the inserted value are copied up one index

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator),
.do end

so that the new element is after all elements
with value less than or equal to it.
:P.
Several different results can occur if the vector is not large enough for
the new element.
If the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is enabled, the exception is thrown.
If the exception is not enabled, the insert fails if the amount the vector
is to be grown (the second parameter to the constructor) is zero(0).
Otherwise, the vector is automatically grown by
the number of elements specified to the constructor, using the
.MONO resize
member function.
If
.MONO resize
fails, the element is not inserted to the vector and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown, if enabled.
:RSLTS.
The &fn. inserts an element in to the vector.
A TRUE (non-zero) value is returned if the insert is successful.
If the insert fails, a FALSE (zero) value is returned.
:SALSO.
:SAL typ='fun'.append
:SAL typ='fun'.insertAt
:SAL typ='fun'.prepend
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.OrderedVector<Type>' fmt='mfun' prot='public'.insertAt
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='insertAt'      .int insertAt( int, const &lparm. );
.do end
.el .do begin
:SNPF index='insertAt'      .int insertAt( int, &lparm. );
.do end
:eSNPL.
:SMTICS.
The &fn. inserts the second argument into the vector before the element at
index given by the first argument.
If the passed index is equal to the number of entries in the vector, the
new value is appended to the vector as the last element.

.if &lpref. eq Val .do begin
The data stored in the vector is a copy of the data passed as a parameter.
.do end

All vector elements with indexes greater than or equal to the first parameter
are copied

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator)
.do end

up one index.
:P.
This function is not provided by the
.MONO WC&lpref.SortedVector
class, since all elements must be inserted in sorted order by the
.MONO insert
member function.
:P.
If the passed index is negative or greater than the number of entries in
the vector and the
.MONO index_range
.ix 'index_range' 'exception'
exception is enabled, the exception is thrown.  If the exception is not
enabled, the new element is inserted as the first element when the index is
negative, or as the last element when the index is too large.
:P.
Several different results can occur if the vector is not large enough for
the new element.
If the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is enabled, the exception is thrown.
If the exception is not enabled, the insert fails if the amount the vector
is to be grown (the second parameter to the constructor) is zero(0).
Otherwise, the vector is automatically grown by
the number of elements specified to the constructor, using the
.MONO resize
member function.
If
.MONO resize
fails, the element is not inserted into the vector and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown, if enabled.
:RSLTS.
The &fn. inserts an element into the
.MONO WC&lpref.OrderedVector
object before the element at the given index.
A TRUE (non-zero) value is returned if the insert is successful.
If the insert fails, a FALSE (zero) value is returned.
:SALSO.
:SAL typ='fun'.append
:SAL typ='fun'.insert
:SAL typ='fun'.prepend
:SAL typ='fun'.operator~b[]
:SAL typ='fun'.removeAt
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.isEmpty
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='isEmpty'   .int isEmpty() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if a vector object has any entries contained
in it.
:RSLTS.
A TRUE value (non-zero) is returned if the vector object does not have any
vector elements contained within it.  A FALSE (zero) result is returned if
the vector contains at least one element.
:SALSO.
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.last
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='last'      .&lret last() const;
:eSNPL.
:SMTICS.
The &fn. returns the last element in the vector.  The element is not removed
from the vector.
:P.
If the vector is empty, one of two exceptions can be thrown.  The
.MONO empty_container
.ix 'empty_container' 'exception'
exception is thrown if it is enabled.  Otherwise, if the
.MONO index_range
.ix 'index_range' 'exception'
exception is enabled, it is thrown.
If neither exception is enabled, a first element of the vector is added

.if &lpref. eq Val .do begin
with a default value.
.do end
.el .do begin
with a NULL value.
.do end

:RSLTS.
The &fn. returns the value of the last element in the vector.
:SALSO.
:SAL typ='fun'.first
:SAL typ='fun'.removeLast
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.occurrencesOf
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='occurrencesOf'     .int occurrencesOf( const &lparm. ) const;
:eSNPL.
:SMTICS.
The &fn. returns the number of elements contained in the vector that are
equivalent to the passed value.

.if &lpref eq Ptr .do begin
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

A linear search is used by the
.MONO WC&lpref.OrderedVector
class to find the value.  The
.MONO WC&lpref.SortedVector
class uses a binary search.
:RSLTS.
The &fn. returns the number of elements equivalent to the passed value.
:SALSO.
:SAL typ='fun'.contains
:SAL typ='fun'.find
:SAL typ='fun'.index
:SAL typ='fun'.operator~b[]
:SAL typ='fun'.removeAll
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator []
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
:P.
The
.MONO append, insert, insertAt
and
.MONO prepend
member functions are used to insert a new element into a vector, and the
.MONO remove, removeAll, removeAt, removeFirst
and
.MONO removeLast
member functions remove elements.
The index operator cannot be used to change the number of entries in the
vector.
Searches may be performed using the
.MONO find
and
.MONO index
member functions.
:P.
If the vector is empty, one of two exceptions can be thrown.  The
.MONO empty_container
.ix 'empty_container' 'exception'
exception is thrown if it is enabled.  Otherwise, if the
.MONO index_range
.ix 'index_range' 'exception'
exception is enabled, it is thrown.
If neither exception is enabled, a first element of the vector is added

.if &lpref. eq Val .do begin
with a default value.
.do end
.el .do begin
with a NULL value.
.do end

This element is added so that a reference to a valid vector element can
be returned.
:P.
If the index value is negative and the
.MONO index_range
.ix 'index_range' 'exception'
exception is enabled, the exception is thrown.
An attempt to index an element with index greater than or equal to the
number of entries in the vector will also cause the
.MONO index_range
exception to be thrown if enabled.
If the exception is not enabled, attempting to index a negative element will
index the first element in the vector, and attempting to index an element
after the last entry will index the last element.
:P.
Care must be taken when using the
.MONO WC&lpref.SortedVector
class not to change the ordering of the vector
elements.
The result returned by the index operator must not be assigned to or
modified in such a way that it is no longer equivalent (by
.MONO Type's
equivalence operator) to the value inserted into the vector.
Failure to comply may cause lookups to work incorrectly, since
the binary search algorithm assumes elements are in sorted order.
:RSLTS.
The &fn. returns a reference to the element at the given index.
If the index is invalid, a reference to the closest valid element is returned.
The result of the non-constant index operator may be assigned to.
:SALSO.
:SAL typ='fun'.append
:SAL typ='fun'.find
:SAL typ='fun'.first
:SAL typ='fun'.index
:SAL typ='fun'.insert
:SAL typ='fun'.insertAt
:SAL typ='fun'.isEmpty
:SAL typ='fun'.last
:SAL typ='fun'.prepend
:SAL typ='fun'.remove
:SAL typ='fun'.removeAt
:SAL typ='fun'.removeAll
:SAL typ='fun'.removeFirst
:SAL typ='fun'.removeLast
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator =
:SNPL.
:SNPFLF                    .#include <wcvector.h>
:SNPFLF                    .public:
:SNPF index='operator ='   .WC&lpref.OrderedVector & WC&lpref.OrderedVector::operator =( const WC&lpref.OrderedVector & );
:SNPF index='operator ='   .WC&lpref.SortedVector & WC&lpref.SortedVector::operator =( const WC&lpref.SortedVector & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the class.
The left hand side vector is first cleared using the
.MONO clear
member function, and then the right hand side vector is copied.
The left hand side vector is made to have the same length and growth amount
as the right hand side (the growth amount is the second argument passed to
the right hand side vector constructor).
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
:LIBF fmt='mfun' cllong='yes' prot='public'.operator ==
:SNPL.
:SNPFLF          .#include <wcvector.h>
:SNPFLF          .public:
:SNPF index='operator =='.int WC&lpref.OrderedVector::operator ==( const WC&lpref.OrderedVector & ) const;
:SNPF index='operator =='.int WC&lpref.SortedVector::operator ==( const WC&lpref.SortedVector & ) const;
:eSNPL.
:SMTICS.
The &fn. is the equivalence operator for the class.
Two vector objects are equivalent if they are the same object and share the
same address.
:RSLTS.
A TRUE (non-zero) value is returned if the left hand side and right
hand side vectors are the same object.  A FALSE (zero) value is returned
otherwise.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.OrderedVector<Type>' fmt='mfun' prot='public'.prepend
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='prepend'       .int prepend( const &lparm. );
.do end
.el .do begin
:SNPF index='prepend'       .int prepend( &lparm. );
.do end
:eSNPL.
:SMTICS.
The &fn. inserts the passed element to be the first element in the vector.

.if &lpref. eq Val .do begin
The data stored in the vector is a copy of the data passed as a parameter.
.do end

All vector elements contained in the vector are copied

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator)
.do end

up one index.
:P.
This function is not provided by the
.MONO WC&lpref.SortedVector
class, since all elements must be inserted in sorted order by the
.MONO insert
member function.
:P.
Several different results can occur if the vector is not large enough for
the new element.
If the
.MONO resize_required
.ix 'resize_required' 'exception'
exception is enabled, the exception is thrown.
If the exception is not enabled, the prepend fails if the amount the vector
is to be grown (the second parameter to the constructor) is zero(0).
Otherwise, the vector is automatically grown by
the number of elements specified to the constructor, using the
.MONO resize
member function.
If
.MONO resize
fails, the element is not inserted to the vector and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown, if enabled.
:RSLTS.
The &fn. prepends an element to the
.MONO WC&lpref.OrderedVector
object.  A TRUE (non-zero) value is returned if the insert is successful.
If the insert fails, a FALSE (zero) value is returned.
:SALSO.
:SAL typ='fun'.append
:SAL typ='fun'.insert
:SAL typ='fun'.insertAt
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.resize_required
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.remove
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='remove'        .int remove( const Type & );
.do end
.el .do begin
:SNPF index='remove'        .Type * remove( const Type * );
.do end
:eSNPL.
:SMTICS.
The &fn. removes the first element in the vector which is equivalent to
the passed value.

.if &lpref eq Ptr .do begin
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

All vector elements stored after the removed elements are copied

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator)
.do end

down one index.

:P.
A linear search is used by the
.MONO WC&lpref.OrderedVector
class to find the element being removed.  The
.MONO WC&lpref.SortedVector
class uses a binary search.
:RSLTS.
The &fn. removes the first element in the vector which is equivalent
to the passed value.

.if &lpref. eq Val .do begin
A TRUE (non-zero) value is returned if an equivalent element was
contained in the vector and removed.
If the vector did not contain an equivalent value, a FALSE (zero) value
is returned.
.do end
.el .do begin
The removed pointer is returned.
If the vector did not contain an equivalent value, NULL(0) is returned.
.do end
:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.find
:SAL typ='fun'.removeAll
:SAL typ='fun'.removeAt
:SAL typ='fun'.removeFirst
:SAL typ='fun'.removeLast
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.removeAll
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
:SNPF index='removeAll' .unsigned removeAll( const &lparm. );
:eSNPL.
:SMTICS.
The &fn. removes all elements in the vector which are equivalent to
the passed value.

.if &lpref eq Ptr .do begin
Note that comparisons are done on the objects pointed to, not the pointers
themselves.
.do end

All vector elements stored after the removed elements are copied

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator)
.do end

down one or more indexes to take the place of the removed elements.

:P.
A linear search is used by the
.MONO WC&lpref.OrderedVector
class to find the elements being removed.  The
.MONO WC&lpref.SortedVector
class uses a binary search.
:RSLTS.
The &fn. removes all elements in the vector which are equivalent
to the passed value.
The number of elements removed is returned.
:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.find
:SAL typ='fun'.occurrencesOf
:SAL typ='fun'.remove
:SAL typ='fun'.removeAt
:SAL typ='fun'.removeFirst
:SAL typ='fun'.removeLast
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.removeAt
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='removeAt'      .int removeAt( int );
.do end
.el .do begin
:SNPF index='removeAt'      .Type * removeAt( int );
.do end
:eSNPL.
:SMTICS.
The &fn. removes the element at the given index.
All vector elements stored after the removed elements are copied

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator)
.do end

down one index.
:P.
If the vector is empty and the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, the exception is thrown.
:P.
If an attempt to remove an element with a negative index is made and the
.MONO index_range
exception is enabled, the exception is thrown.
If the exception is not enabled, the first element is removed from the vector.
Attempting to remove an element with index greater or equal to the number
of entries in the vector also causes the
.MONO index_range
exception to be thrown if enabled.  The last element in the vector is removed
if the exception is not enabled.
:RSLTS.
The &fn. removes the element with the given index.  If the index is invalid,
the closest element to the given index is removed.

.if &lpref. eq Val .do begin
A TRUE (non-zero) value is returned if an element was removed.
If the vector was empty, FALSE (zero) value is returned.
.do end
.el .do begin
The removed pointer is returned.
If the vector was empty, NULL(0) is returned.
.do end

:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.insertAt
:SAL typ='fun'.operator~b[]
:SAL typ='fun'.remove
:SAL typ='fun'.removeAll
:SAL typ='fun'.removeFirst
:SAL typ='fun'.removeLast
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.removeFirst
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='removeFirst'   .int removeFirst();
.do end
.el .do begin
:SNPF index='removeFirst'   .Type * removeFirst();
.do end
:eSNPL.
:SMTICS.
The &fn. removes the first element from a vector.
All other vector elements are copied

.if &lpref. eq Val .do begin
(using
.MONO Type's
assignment operator)
.do end

down one index.
:P.
If the vector is empty and the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, the exception is thrown.
:RSLTS.
The &fn. removes the first element from the vector.

.if &lpref. eq Val .do begin
A TRUE (non-zero) value is returned if an element was removed.
If the vector was empty, FALSE (zero) value is returned.
.do end
.el .do begin
The removed pointer is returned.
If the vector was empty, NULL(0) is returned.
.do end

:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.first
:SAL typ='fun'.remove
:SAL typ='fun'.removeAt
:SAL typ='fun'.removeAll
:SAL typ='fun'.removeLast
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.removeLast
:SNPL.
:SNPFLF                 .#include <wcvector.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='removeLast'   .int removeLast();
.do end
.el .do begin
:SNPF index='removeLast'   .Type * removeLast();
.do end
:eSNPL.
:SMTICS.
The &fn. removes the last element from a vector.
If the vector is empty and the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, the exception is thrown.
:RSLTS.
The &fn. removes the last element from the vector.

.if &lpref. eq Val .do begin
A TRUE (non-zero) value is returned if an element was removed.
If the vector was empty, FALSE (zero) value is returned.
.do end
.el .do begin
The removed pointer is returned.
If the vector was empty, NULL(0) is returned.
.do end

:SALSO.
:SAL typ='fun'.clear
.if &lpref. eq Ptr .do begin
:SAL typ='fun'.clearAndDestroy
.do end
:SAL typ='fun'.last
:SAL typ='fun'.remove
:SAL typ='fun'.removeAt
:SAL typ='fun'.removeAll
:SAL typ='fun'.removeFirst
:eSALSO.
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
elements are copied

.if &lpref eq Val .do begin
(using
.MONO Type's
copy constructor)
.do end

into the newly sized vector, and new elements can be added using the
.MONO append, insert, insertAt,
and
.MONO prepend
member functions.
If the vector is resized to a smaller size, the first :HP1.new_size:eHP1.
elements are copied (all vector elements if the vector contained
:HP1.new_size:eHP1. or fewer elements).

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
