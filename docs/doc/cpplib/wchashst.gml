:CLFNM cl2='WC&lpref.HashSet<Type>'.WC&lpref.HashTable<Type>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.HashTable<Type>, WC&lpref.HashSet<Type>
:HFILE.wchash.h
:CLSS.
&cls. are templated classes used to store objects in a hash.
A hash saves objects in such a way as to make it efficient to locate
and retrieve an element.
As an element is looked up or inserted into the hash, the value of the
element is hashed.
Hashing results in a numeric index which is used to locate the value.
The storage area referenced by the hash value is usually called a
bucket.
If more than one element results in the same hash, the value
associated with the hash is placed in a list stored in the bucket.
A hash table allows more than one copy of an element that is
equivalent, while the hash set allows only one copy.
The equality operator of the element's type is used to locate the
value.
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the template parameter defining
.if &lpref. eq Val .do begin
the type of the data to be stored in the hash.
.do end
.el .do begin
the type of the data pointed to by the pointers stored in the hash.
.do end

:P.
The constructor for the &cls.
requires a hashing function, which given a reference to
.MONO Type,
returns an
.MONO unsigned
value.
The returned value modulo the number of buckets determines the
bucket into which the element will be located.
The return values of the hash function
can be spread over the entire range of unsigned numbers.
The hash function return value
must be the same for values which are equivalent by the
equivalence operator for
.MONO Type.

.if &lpref. eq Val .do begin
:P.
Values are copied into the hash, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value hashes should not be used to store objects of a base class if any
derived types of different sizes would be stored in the hash, or if the
destructor for a derived class must be called.
.do end
.el .do begin
:P.
Note that pointers to the elements are stored in the hash.
Destructors are not called on the elements pointed to.
The data values pointed to in the hash should not be changed such
that the equivalence to the old value is modified.
.do end

:P.
:INCLUDE file='_EXPT_BC'.

:HDG.Requirements of Type

The &cls requires
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
A well defined assignment operator (
.MONO Type & operator =( const Type & )
).
.do end
:P.
A well defined equivalence operator with constant parameters
.br
(
.MONO int operator ==( const Type & ) const
).

:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFN index='WC&lpref.HashSet' .WC&lpref.HashSet( unsigned (*hash_fn)( const Type & ), unsigned = WC_DEFAULT_HASH_SIZE );
:MFN index='WC&lpref.HashSet' .WC&lpref.HashSet( unsigned (*hash_fn)( const Type & ), unsigned = WC_DEFAULT_HASH_SIZE, void * (*user_alloc)( size_t size ), void (*user_dealloc)( void *old, size_t size ) );
:MFN index='WC&lpref.HashSet' .WC&lpref.HashSet( const WC&lpref.HashSet & );
:MFN index='~~WC&lpref.HashSet' .virtual ~~WC&lpref.HashSet();
:MFN index='WC&lpref.HashTable' .WC&lpref.HashTable( unsigned (*hash_fn)( const Type & ), unsigned = WC_DEFAULT_HASH_SIZE );
:MFN index='WC&lpref.HashTable' .WC&lpref.HashTable( unsigned (*hash_fn)( const Type & ), unsigned = WC_DEFAULT_HASH_SIZE, void * (*user_alloc)( size_t size ), void (*user_dealloc)( void *old, size_t size ) );
:MFN index='WC&lpref.HashTable' .WC&lpref.HashTable( const WC&lpref.HashTable & );
:MFN index='~~WC&lpref.HashTable' .virtual ~~WC&lpref.HashTable();
:MFN index='bitHash'        .static unsigned bitHash( const void *, size_t );
:MFN index='buckets'        .unsigned buckets() const;
:MFN index='clear'          .void clear();
.if &lpref eq Ptr .do begin
:MFN index='clearAndDestroy'.void clearAndDestroy();
.do end
.if &lpref eq Val .do begin
:MFN index='contains'       .int contains( const Type & ) const;
.do end
.el .do begin
:MFN index='contains'       .int contains( const Type * ) const;
.do end
:MFN index='entries'        .unsigned entries() const;
.if &lpref eq Val .do begin
:MFN index='find'           .int find( const Type &, Type & ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Type, void * ), void * );
:MFN index='insert'         .int insert( const Type & );
.do end
.if &lpref eq Ptr .do begin
:MFN index='find'            .Type * find( const Type * ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Type *, void * ) , void * );
:MFN index='insert'         .int insert( Type * );
.do end
:MFN index='isEmpty'        .int isEmpty() const;
.if &lpref eq Val .do begin
:MFN index='remove'         .int remove( const Type & );
.do end
.el .do begin
:MFN index='remove'         .Type * remove( const Type * );
.do end
:MFN index='resize'         .void resize( unsigned );
:eMFNL.
The following public member functions are available for the
.MONO WC&lpref.HashTable
class only:
:MFNL.
.if &lpref eq Val .do begin
:MFN index='occurrencesOf'  .unsigned occurrencesOf( const Type & ) const;
.do end
.el .do begin
:MFN index='occurrencesOf'  .unsigned occurrencesOf( const Type * ) const;
.do end
.if &lpref eq Val .do begin
:MFN index='removeAll'      .unsigned removeAll( const Type & );
.do end
.el .do begin
:MFN index='removeAll'      .unsigned removeAll( const Type * );
.do end
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
:MFN index='operator ='      .WC&lpref.HashSet & operator =( const WC&lpref.HashSet & );
:MFN index='operator =='     .int operator ==( const WC&lpref.HashSet & ) const;
:MFN index='operator ='      .WC&lpref.HashTable & operator =( const WC&lpref.HashTable & );
:MFN index='operator =='     .int operator ==( const WC&lpref.HashTable & ) const;
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashSet<Type>' fmt='ctor' prot='public'.WC&lpref.HashSet
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashSet( unsigned (*hash_fn)( const Type & ),
:SNPFLF          .            unsigned = WC_DEFAULT_HASH_SIZE );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.HashSet<Type>
constructor creates a
.MONO WC&lpref.HashSet
object with no entries and with the number of buckets
in the second optional parameter, which defaults to the constant
.MONO WC_DEFAULT_HASH_SIZE
(currently defined as 101).
The number of buckets specified must be greater than zero, and will be forced
to at least one.
If the hash object can be created, but an allocation failure
occurs when creating the buckets, the
table will be created with zero buckets.
If the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled, then attempting to insert into a hash
table with zero buckets with throw an
.MONO out_of_memory
error.
:P.
The hash function
.MONO hash_fn
is used to determine which bucket each value will be assigned to.
If no hash function exists, the
static member function
.MONO bitHash
is available to help create one.

:RSLTS.
The
.MONO WC&lpref.HashSet<Type>
constructor creates an initialized
.MONO WC&lpref.HashSet
object with the specified number of buckets and
hash function.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashSet
:SAL typ='fun'.bitHash
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashSet<Type>' fmt='ctor' prot='public'.WC&lpref.HashSet
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashSet( unsigned (*hash_fn)( const Type & ),
:SNPFLF          .            unsigned = WC_DEFAULT_HASH_SIZE,
:SNPFLF          .            void * (*user_alloc)( size_t ),
:SNPFLF          .            void (*user_dealloc)( void *, size_t ) );
:eSNPL.
:SMTICS.
Allocator and deallocator functions are specified
for use when entries are inserted and removed from the hash.
The semantics of this constructor are the same as the constructor without
the memory management functions.
:P.
The allocation function must return a
zero if it cannot perform the allocation.
The deallocation function is passed the size as well
as the pointer to the data.
Your allocation system may take advantage of the characteristic that
the allocation function will always be called with the same size
value for any particular instantiation of a hash.
To determine the size of
the objects that the memory management functions will be
required to allocate and free, the following macro may be used:
.br
.MONO WC&lpref.HashSetItemSize( Type )
:RSLTS.
The
.MONO WC&lpref.HashSet<Type>
constructor creates an initialized
.MONO WC&lpref.HashSet
object with the specified number of buckets and
hash function.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashSet
:SAL typ='fun'.bitHash
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashSet<Type>' fmt='ctor' prot='public'.WC&lpref.HashSet
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashSet( const WC&lpref.HashSet & );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.HashSet<Type>
is the copy constructor for the
.MONO WC&lpref.HashSet
class.
The new hash is created with the same number of buckets, hash function,
all values or pointers stored in the hash, and the exception trap states.
If the hash object can be created, but an allocation failure
occurs when creating the buckets, the
hash will be created with zero buckets.
If there is not enough memory to copy all of
the values, then only some will be copied,
and the number of entries will correctly reflect the number copied.
If all of the elements cannot be copied, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
:RSLTS.
The
.MONO WC&lpref.HashSet<Type>
constructor creates a
.MONO WC&lpref.HashSet
object which is a copy of the passed hash.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashSet
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashSet<Type>' fmt='dtor' prot='public'.~~WC&lpref.HashSet
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.HashSet();
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.HashSet<Type>
destructor is the destructor for the
.MONO WC&lpref.HashSet
class.
If the number of elements is not zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the hash elements are cleared using the
.MONO clear
member function.
.if &lpref. eq Ptr .do begin
The objects which the hash elements point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
The call to the
.MONO WC&lpref.HashSet<Type>
destructor is inserted implicitly by the compiler
at the point where the
.MONO WC&lpref.HashSet
object goes out of scope.
:RSLTS.
The call to the
.MONO WC&lpref.HashSet<Type>
destructor destroys a
.MONO WC&lpref.HashSet
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
:LIBF cltype='WC&lpref.HashTable<Type>' fmt='ctor' prot='public'.WC&lpref.HashTable
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashTable( unsigned (*hash_fn)( const Type & ),
:SNPFLF          .            unsigned = WC_DEFAULT_HASH_SIZE );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.HashTable<Type>
constructor creates a
.MONO WC&lpref.HashTable
object with no entries and with the number of buckets
in the second optional parameter, which defaults to the constant
.MONO WC_DEFAULT_HASH_SIZE
(currently defined as 101).
The number of buckets specified must be greater than zero, and will be forced
to at least one.
If the hash object can be created, but an allocation failure
occurs when creating the buckets, the
table will be created with zero buckets.
If the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled, then attempting to insert into a hash
table with zero buckets with throw an
.MONO out_of_memory
error.
:P.
The hash function
.MONO hash_fn
is used to determine which bucket each value will be assigned to.
If no hash function exists, the
static member function
.MONO bitHash
is available to help create one.

:RSLTS.
The
.MONO WC&lpref.HashTable<Type>
constructor creates an initialized
.MONO WC&lpref.HashTable
object with the specified number of buckets and
hash function.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashTable
:SAL typ='fun'.bitHash
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashTable<Type>' fmt='ctor' prot='public'.WC&lpref.HashTable
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashTable( unsigned (*hash_fn)( const Type & ),
:SNPFLF          .            unsigned = WC_DEFAULT_HASH_SIZE,
:SNPFLF          .            void * (*user_alloc)( size_t ),
:SNPFLF          .            void (*user_dealloc)( void *, size_t ) );
:eSNPL.
:SMTICS.
Allocator and deallocator functions are specified
for use when entries are inserted and removed from the hash.
The semantics of this constructor are the same as the constructor without
the memory management functions.
:P.
The allocation function must return a
zero if it cannot perform the allocation.
The deallocation function is passed the size as well
as the pointer to the data.
Your allocation system may take advantage of the characteristic that
the allocation function will always be called with the same size
value for any particular instantiation of a hash.
To determine the size of
the objects that the memory management functions will be
required to allocate and free, the following macro may be used:
.br
.MONO WC&lpref.HashTableItemSize( Type )
:RSLTS.
The
.MONO WC&lpref.HashTable<Type>
constructor creates an initialized
.MONO WC&lpref.HashTable
object with the specified number of buckets and
hash function.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashTable
:SAL typ='fun'.bitHash
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashTable<Type>' fmt='ctor' prot='public'.WC&lpref.HashTable
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashTable( const WC&lpref.HashTable & );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.HashTable<Type>
is the copy constructor for the
.MONO WC&lpref.HashTable
class.
The new hash is created with the same number of buckets, hash function,
all values or pointers stored in the hash, and the exception trap states.
If the hash object can be created, but an allocation failure
occurs when creating the buckets, the
hash will be created with zero buckets.
If there is not enough memory to copy all of
the values, then only some will be copied,
and the number of entries will correctly reflect the number copied.
If all of the elements cannot be copied, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
:RSLTS.
The
.MONO WC&lpref.HashTable<Type>
constructor creates a
.MONO WC&lpref.HashTable
object which is a copy of the passed hash.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashTable
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashTable<Type>' fmt='dtor' prot='public'.~~WC&lpref.HashTable
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.HashTable();
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.HashTable<Type>
destructor is the destructor for the
.MONO WC&lpref.HashTable
class.
If the number of elements is not zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the hash elements are cleared using the
.MONO clear
member function.
.if &lpref. eq Ptr .do begin
The objects which the hash elements point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
The call to the
.MONO WC&lpref.HashTable<Type>
destructor is inserted implicitly by the compiler
at the point where the
.MONO WC&lpref.HashTable
object goes out of scope.
:RSLTS.
The call to the
.MONO WC&lpref.HashTable<Type>
destructor destroys a
.MONO WC&lpref.HashTable
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
:LIBF fmt='mfun' prot='public'.bitHash
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='bitHash'   .static unsigned bitHash( void *, size_t );
:eSNPL.
:SMTICS.
The &fn. can be used to implement a hashing function for any type.
A hashing value is generated from the value stored for the
number of specified bytes pointed to by the first parameter.
.if &lpref. eq Val .do begin
For example:
:XMPL.
unsigned my_hash_fn( const int & elem ) {
    return( WCValHashSet<int,String>::bitHash(&elem, sizeof(int));
}
WCValHashSet<int> data_object( &my_hash_fn );
:eXMPL.
.do end
:RSLTS.
The &fn. returns an unsigned value which can be used as the basis of
a user defined hash function.
:SALSO.
:SAL typ='fun'.WC&lpref.HashSet
:SAL typ='fun'.WC&lpref.HashTable
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.buckets
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='buckets'   .unsigned buckets() const;
:eSNPL.
:SMTICS.
The &fn. is used to find the number of buckets contained in
the hash object.
:RSLTS.
The &fn. returns the number of buckets in the hash.
:SALSO.
:SAL typ='fun'.resize
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='clear'     .void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the hash so that it has no entries.
The number of buckets remain unaffected.
.if &lpref. eq Val .do begin
Elements stored in the hash are destroyed using
the destructors of
.MONO Type.
.do end
.el .do begin
Objects pointed to by the hash elements are not deleted.
.do end
The hash object is not destroyed and re-created by this function, so
the object destructor is not invoked.
:RSLTS.
The &fn. clears the hash to have no elements.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashSet
:SAL typ='fun'.~~WC&lpref.HashTable
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
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='clearAndDestroy'   .void clearAndDestroy();
:eSNPL.
:SMTICS.
The &fn. is used to clear the hash and delete
the objects pointed to by the hash elements.
The hash object is not destroyed and re-created by this function, so
the hash object destructor is not invoked.
:RSLTS.
The &fn. clears the hash by deleting the objects pointed to by the
hash elements.
:SALSO.
:SAL typ='fun'.clear
:eSALSO.
:eLIBF.
.do end

:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.contains
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref eq Val .do begin
:SNPF index='contains'  .int contains( const Type & ) const;
.do end
.el .do begin
:SNPF index='contains'  .int contains( const Type * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn.
returns non-zero if the element is stored in
the hash, or zero if there is no equivalent element.
:INCLUDE file='_valequ.gml'
:RSLTS.
The &fn. returns a non-zero value if the element is found in the hash.
:SALSO.
:SAL typ='fun'.find
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.entries
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='entries'   .unsigned entries() const;
:eSNPL.
:SMTICS.
The &fn. is used to return the current number of elements
stored in the hash.
:RSLTS.
The &fn. returns the number of elements in the hash.
:SALSO.
:SAL typ='fun'.buckets
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.find
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='find'      .int find( const Type &, Type & ) const;
.do end
.el .do begin
:SNPF index='find'      .Type * find( const Type * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to find an element with an equivalent key in the hash.
If an equivalent element is found,
.if &lpref. eq Val .do begin
a non-zero value
.do end
.el .do begin
a pointer to the element
.do end
is returned.
.if &lpref. eq Val .do begin
The reference to the element
passed as the second argument is assigned the found element's value.
.do end
Zero is returned if the element is not found.
:INCLUDE file='_valequ.gml'
:RSLTS.
The element equivalent to the passed key is located in the hash.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.forAll
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='forAll'    .void forAll(
:SNPFLF                 .      void (*user_fn)( Type, void * ),
:SNPFLF                 .      void * );
.do end
.el .do begin
:SNPF index='forAll'    .void forAll(
:SNPFLF                 .      void (*user_fn)( Type *, void * ),
:SNPFLF                 .      void * );
.do end
:eSNPL.
:SMTICS.
The &fn. causes the user supplied function to be invoked for every value
in the hash.
The user function has the prototype
:XMPL.
.if &lpref. eq Val .do begin
void user_func( Type & value, void * data );
.do end
.el .do begin
void user_func( Type * value, void * data );
.do end
:eXMPL.
As the elements are visited, the user function is invoked with the
element passed as the first.
The second parameter of the
.MONO forAll
function is passed as the second parameter to the user function.
This value can be used to pass any appropriate data from the main code to the
user function.
:RSLTS.
The elements in the hash are all visited, with the user function
being invoked for each one.
:SALSO.
:SAL typ='fun'.find
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
.if &lpref. eq Val .do begin
:SNPF index='insert'       .int insert( const Type & );
.do end
.el .do begin
:SNPF index='insert'       .int insert( Type * );
.do end
:eSNPL.
:SMTICS.
The &fn. inserts
a value into the hash, using the hash function to
determine to which bucket it should be stored.
If allocation of the
node to store the value fails, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, the insert will not be completed.
:P.
With a
.MONO WC&lpref.HashSet,
there must be only one equivalent element in the set.
If an element equivalent to the inserted element is already
in the hash set, the hash set will remain unchanged, and the
.MONO not_unique
.ix 'not_unique' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, the insert will not be completed.
:P.
At some point, the number of buckets initially selected may be too small
for the number of elements inserted.
The resize of the hash can be controlled by the insertion mechanism
by using
.MONO WC&lpref.HashSet
(or
.MONO WC&lpref.HashTable
.ct )
as a base class, and providing an insert member function to
do a resize when appropriate.
This insert could then call
.MONO WC&lpref.HashSet::insert
(or
.MONO WC&lpref.HashTable::insert
.ct )
to insert the element.
Note that copy constructors and assignment operators are not
inherited in your class, but you can provide the following inline
definitions (assuming that the class inherited
from WC&lpref.HashTable is named MyHashTable):
:XMPL.
inline MyHashTable( const MyHashTable &orig )
     : WC&lpref.HashTable( orig ) {};
inline MyHashTable &operator=( const MyHashTable &orig ) {
    return( WC&lpref.HashTable::operator=( orig ) );
}
:eXMPL.
:RSLTS.
The &fn. inserts a value into the hash.
If the insert is successful, a non-zero will returned.
A zero will be returned if the insert fails.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.isEmpty
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='isEmpty'   .int isEmpty() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if the hash is empty.
:RSLTS.
The &fn. returns zero if it contains at least one entry,
non-zero if the hash is empty.
:SALSO.
:SAL typ='fun'.buckets
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.HashTable<Type>' fmt='mfun' prot='public'.occurencesOf
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='occurrencesOf' .unsigned occurrencesOf( const Type & ) const;
.do end
.el .do begin
:SNPF index='occurrencesOf' .unsigned occurrencesOf( const Type * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to return the current number of elements
stored in the hash which are equivalent to the passed value.
:INCLUDE file='_valequ.gml'
:RSLTS.
The &fn. returns the number of elements in the hash.
:SALSO.
:SAL typ='fun'.buckets
:SAL typ='fun'.entries
:SAL typ='fun'.find
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPF index='operator ='   .WC&lpref.HashSet & operator =( const WC&lpref.HashSet & );
:SNPF index='operator ='   .WC&lpref.HashTable & operator =( const WC&lpref.HashTable & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the &cls.:PERIOD.
The left hand side hash is first cleared using the
.MONO clear
member function, and then the right hand side hash is copied.
The hash function, exception trap states, and all of the hash
elements are copied.
If an allocation failure occurs when creating the buckets, the
table will be created with zero buckets, and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If there is not enough memory to copy all of
the values or pointers in the hash, then only some will be copied, and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
The number of entries will correctly reflect the number copied.
:RSLTS.
The &fn. assigns the left hand side hash to be a copy of the
right hand side.
:SALSO.
:SAL typ='fun'.clear
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator ==
:SNPL.
:SNPFLF          .#include <wchash.h>
:SNPFLF          .public:
:SNPF index='operator =='.int operator ==( const WC&lpref.HashSet & ) const;
:SNPF index='operator =='.int operator ==( const WC&lpref.HashTable & ) const;
:eSNPL.
:SMTICS.
The &fn. is the equivalence operator for the &cls.:PERIOD.
Two hash objects are equivalent if they are the same object and share the
same address.
:RSLTS.
A TRUE (non-zero) value is returned if the left hand side and right
hand side hash are the same object.  A FALSE (zero) value is returned
otherwise.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.remove
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='remove'    .int remove( const Type & );
.do end
.el .do begin
:SNPF index='remove'    .Type * remove( const Type * );
.do end
:eSNPL.
:SMTICS.
The &fn. is used to remove the specified element from the hash.
If an equivalent element is found,
.if &lpref. eq Val .do begin
a non-zero
.do end
.el .do begin
the pointer
.do end
value is returned.
Zero is returned if the element is not found.
If the hash is a table and there is more than one element equivalent
to the specified element, then the first equivalent element added to the table
is removed.
:INCLUDE file='_valequ.gml'
:RSLTS.
The element is removed from the hash if it found.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.HashTable<Type>' fmt='mfun' prot='public'.removeAll
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='removeAll' .unsigned removeAll( const Type & );
.do end
.el .do begin
:SNPF index='removeAll' .unsigned removeAll( const Type * );
.do end
:eSNPL.
:SMTICS.
The &fn. is used to remove all elements equivalent to the
specified element from the hash.
Zero is returned if no equivalent elements are found.
:INCLUDE file='_valequ.gml'
:RSLTS.
All equivalent elements are removed from the hash.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.resize
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='resize'    .void resize( unsigned );
:eSNPL.
:SMTICS.
The &fn. is used to change the number of buckets contained in the hash.
If the new number is larger than the previous hash size, then the
hash function will be used on all of the stored elements to determine
which bucket they should be stored into.
Entries are not destroyed or created in the process of being moved.
If there is not enough memory to resize the hash, the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled, and the hash will contain
the number of buckets it contained before the resize.
If the new number is zero, then the
.MONO zero_buckets
.ix 'zero_buckets' 'exception'
exception is thrown if it is enabled, and no resize will be performed.
The hash is guaranteed to contain the same number of entries after
the resize.
:RSLTS.
The hash is resized to the new number of buckets.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.zero_buckets
:eSALSO.
:eLIBF.
