:CLFNM.WC&lpref.HashDict<Key,Value>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.HashDict<Key,Value>
:HFILE.wchash.h
:CLSS.
The &cls. is a templated class used to store objects in a dictionary.
Dictionaries store values with an associated key, which may be of any type.
One example of a dictionary used in everyday life is the phone book.
The phone numbers are the data values, and the customer
name is the key.
An example of a specialized dictionary is a vector, where the
key value is the integer index.

:P.
As an element is looked up or inserted into the dictionary, the associated
key is hashed.
Hashing converts the key into a numeric index value
which is used to locate the value.
The storage area referenced by the hash value is usually called a bucket.
If more than one key results in the same hash, the values associated with
the keys are placed in a list stored in the bucket.
The equality operator of the key's type is used to locate the key-value
pairs.

:P.
In the description of each member function, the text
.MONO Key
is used to indicate the template parameter defining
.if &lpref. eq Val .do begin
the type of the indices used to store data in the dictionary.
.do end
.el .do begin
the type of the indices pointed to by the pointers stored in the dictionary.
.do end
The text
.MONO Value
is used to indicate the template parameter defining
.if &lpref. eq Val .do begin
the type of the data stored in the dictionary.
.do end
.el .do begin
the type of the data pointed to by the pointers stored in the dictionary.
.do end

:P.
The constructor for the &cls.
requires a hashing function, which given a reference to
.MONO Key,
returns an
.MONO unsigned
value.
The returned value modulo the number of buckets determines the
bucket into which the key-value pair will be located.
The return values of the hash function
can be spread over the entire range of unsigned numbers.
The hash function return value
must be the same for values which are equivalent by the
equivalence operator for
.MONO Key.

.if &lpref. eq Val .do begin
:P.
Values are copied into the dictionary, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value dictionaries should not be used to store objects of a base class if any
derived types of different sizes would be stored in the dictionary, or if the
destructor for a derived class must be called.
.do end
.el .do begin
:P.
Note that pointers to the key values are stored in the dictionary.
Destructors are not called on the keys pointed to.
The key values pointed to in the dictionary should not be changed such
that the equivalence to the old value is modified.
.do end

:P.
:INCLUDE file='_EXPT_BC'.

.if &lpref. eq Val .do begin
:HDG.Requirements of Key and Value
.do end
.el .do begin
:HDG.Requirements of Key
.do end

The &cls requires
.MONO Key
to have:
.if &lpref. eq Val .do begin
:P.
A default constructor (
.MONO Key::Key()
).
:P.
A well defined copy constructor (
.MONO Key::Key( const Key & )
).
:P.
A well defined assignment operator (
.MONO Key & operator =( const Key & )
).
.do end
:P.
A well defined equivalence operator with constant parameters
.br
(
.MONO int operator ==( const Key & ) const
).

.if &lpref. eq Val .do begin
:P.
The &cls requires
.MONO Value
to have:
:P.
A default constructor (
.MONO Value::Value()
).
:P.
A well defined copy constructor (
.MONO Value::Value( const Value & )
).
:P.
A well defined assignment operator (
.MONO Value & operator =( const Value & )
).
.do end

:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFN index='WC&lpref.HashDict' .WC&lpref.HashDict( unsigned (*hash_fn)( const Key & ), unsigned = WC_DEFAULT_HASH_SIZE );
:MFN index='WC&lpref.HashDict' .WC&lpref.HashDict( unsigned (*hash_fn)( const Key & ), unsigned = WC_DEFAULT_HASH_SIZE, void * (*user_alloc)( size_t size ), void (*user_dealloc)( void *old, size_t size ) );
:MFN index='WC&lpref.HashDict' .WC&lpref.HashDict( const WC&lpref.HashDict & );
:MFN index='~~WC&lpref.HashDict' .virtual ~~WC&lpref.HashDict();
:MFN index='bitHash'        .static unsigned bitHash( const void *, size_t );
:MFN index='buckets'        .unsigned buckets() const;
:MFN index='clear'          .void clear();
.if &lpref eq Ptr .do begin
:MFN index='clearAndDestroy'.void clearAndDestroy();
.do end
.if &lpref eq Val .do begin
:MFN index='contains'       .int contains( const Key & ) const;
:MFN index='entries'        .unsigned entries() const;
:MFN index='find'           .int find( const Key &, Value & ) const;
:MFN index='findKeyAndValue' .int findKeyAndValue( const Key &, Key &, Value & ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Key, Value, void * ), void * );
:MFN index='insert'         .int insert( const Key &, const Value & );
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='remove'         .int remove( const Key & );
.do end
.if &lpref eq Ptr .do begin
:MFN index='contains'       .int contains( const Key * ) const;
:MFN index='entries'        .unsigned entries() const;
:MFN index='find'            .Value * find( const Key * ) const;
:MFN index='findKeyAndValue' .Value * findKeyAndValue( const Key *, Key * & ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Key *, Value *, void * ) , void * );
:MFN index='insert'         .int insert( Key *, Value * );
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='remove'         .Value * remove( const Key * );
.do end
:MFN index='resize'         .void resize( unsigned );
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
.if &lpref. eq Val .do begin
:MFN index='operator []'     .Value & operator []( const Key & );
:MFN index='operator []'     .const Value & operator []( const Key & ) const;
.do end
.el .do begin
:MFN index='operator []'     .Value * & operator []( const Key & );
:MFN index='operator []'     .const Value * & operator []( const Key & ) const;
.do end
:MFN index='operator ='      .WC&lpref.HashDict & operator =( const WC&lpref.HashDict & );
:MFN index='operator =='     .int operator ==( const WC&lpref.HashDict & ) const;
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashDict<Key,Value>' fmt='ctor' prot='public'.WC&lpref.HashDict
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashDict( unsigned (*hash_fn)( const Key & ),
:SNPFLF          .            unsigned = WC_DEFAULT_HASH_SIZE );
:eSNPL.
:SMTICS.
The &fn. creates an &obj. with no entries and with the number of buckets
in the second optional parameter, which defaults to the constant
.MONO WC_DEFAULT_HASH_SIZE
(currently defined as 101).
The number of buckets specified must be greater than zero, and will be forced
to at least one.
If the hash dictionary object can be created, but an allocation failure
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
is used to determine which bucket each key-value pair will be assigned.
If no hash function exists, the
static member function
.MONO bitHash
is available to help create one.

:RSLTS.
The &fn. creates an initialized &obj. with the specified number of buckets and
hash function.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashDict
:SAL typ='fun'.bitHash
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashDict<Key,Value>' fmt='ctor' prot='public'.WC&lpref.HashDict
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashDict( unsigned (*hash_fn)( const Key & ),
:SNPFLF          .            unsigned = WC_DEFAULT_HASH_SIZE,
:SNPFLF          .            void * (*user_alloc)( size_t ),
:SNPFLF          .            void (*user_dealloc)( void *, size_t ) );
:eSNPL.
:SMTICS.
Allocator and deallocator functions are specified
for use when entries are inserted and removed from the hash dictionary.
The semantics of this constructor are the same as the constructor without
the memory management functions.
:P.
The allocation function must return a
zero if it cannot perform the allocation.
The deallocation function is passed the size as well
as the pointer to the data.
Your allocation system may take advantage of the characteristic that
the allocation function will always be called with the same size
value for any particular instantiation of a hash dictionary.
To determine the size of
the objects that the memory management functions will be
required to allocate and free, the following macro may be used:
.br
.MONO WC&lpref.HashDictItemSize( Key, Value )
:RSLTS.
The &fn. creates an initialized &obj. with the specified number of buckets and
hash function.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashDict
:SAL typ='fun'.bitHash
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashDict<Key,Value>' fmt='ctor' prot='public'.WC&lpref.HashDict
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.HashDict( const WC&lpref.HashDict & );
:eSNPL.
:SMTICS.
The &fn. is the copy constructor for the &cls.:PERIOD.
The new dictionary is created with the same number of buckets, hash function,
all values or pointers stored in the dictionary, and the exception trap states.
If the hash dictionary object can be created, but an allocation failure
occurs when creating the buckets, the
table will be created with zero buckets.
If there is not enough memory to copy all of
the values in the dictionary, then only some will be copied,
and the number of entries will correctly reflect the number copied.
If all of the elements cannot be copied, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
:RSLTS.
The &fn. creates an &obj. which is a copy of the passed dictionary.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashDict
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.HashDict<Key,Value>' fmt='dtor' prot='public'.~~WC&lpref.HashDict
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.HashDict();
:eSNPL.
:SMTICS.
The &fn. is the destructor for the &cls.:PERIOD.
If the number of dictionary elements is not zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the dictionary elements are cleared using the
.MONO clear
member function.
.if &lpref. eq Ptr .do begin
The objects which the dictionary elements point to are not deleted unless the
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
unsigned my_hash_fn( const int & key ) {
    return( WCValHashDict<int,String>::bitHash( &key, sizeof( int ) );
}
WCValHashDict<int,String> data_object( &my_hash_fn );
:eXMPL.
.do end
:RSLTS.
The &fn. returns an unsigned value which can be used as the basis of
a user defined hash function.
:SALSO.
:SAL typ='fun'.WC&lpref.HashDict
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.buckets
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='buckets'   .unsigned buckets const;
:eSNPL.
:SMTICS.
The &fn. is used to find the number of buckets contained in
the &obj.:PERIOD.
:RSLTS.
The &fn. returns the number of buckets in the dictionary.
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
The &fn. is used to clear the dictionary so that it has no entries.
The number of buckets remain unaffected.
.if &lpref. eq Val .do begin
Elements stored in the dictionary are destroyed using
the destructors of
.MONO Key
and of
.MONO Value.
.do end
.el .do begin
Objects pointed to by the dictionary elements are not deleted.
.do end
The dictionary object is not destroyed and re-created by this function, so
the object destructor is not invoked.
:RSLTS.
The &fn. clears the dictionary to have no elements.
:SALSO.
:SAL typ='fun'.~~WC&lpref.HashDict
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
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='clearAndDestroy'   .void clearAndDestroy();
:eSNPL.
:SMTICS.
The &fn. is used to clear the dictionary and delete
the objects pointed to by the dictionary elements.
The dictionary object is not destroyed and re-created by this function, so
the dictionary object destructor is not invoked.
:RSLTS.
The &fn. clears the dictionary by deleting the objects pointed to by the
dictionary elements.
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
:SNPF index='contains'  .int contains( const Key & ) const;
.do end
.el .do begin
:SNPF index='contains'  .int contains( const Key * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn.
returns non-zero if an element with the specified key is stored in
the dictionary, or zero if there is no equivalent element.
:INCLUDE file='_keyequ.gml'
:RSLTS.
The &fn. returns a non-zero value if the
.MONO Key
is found in the dictionary.
:SALSO.
:SAL typ='fun'.find
:SAL typ='fun'.findKeyAndValue
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
stored in the dictionary.
:RSLTS.
The &fn. returns the number of elements in the dictionary.
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
:SNPF index='find'      .int find( const Key &, Value & ) const;
.do end
.el .do begin
:SNPF index='find'      .Value * find( const Key * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to find an element with an equivalent key in the dictionary.
If an equivalent element is found,
.if &lpref. eq Val .do begin
a non-zero value
.do end
.el .do begin
a pointer to the element
.MONO Value
.do end
is returned.
.if &lpref. eq Val .do begin
The reference to a
.MONO Value
passed as the second argument is assigned the found element's
.MONO Value.
.do end
Zero is returned if the element is not found.
:INCLUDE file='_keyequ.gml'
:RSLTS.
The element equivalent to the passed key is located in the dictionary.
:SALSO.
:SAL typ='fun'.findKeyAndValue
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.findKeyAndValue
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='findKeyAndValue' .int findKeyAndValue( const Key &, Key &, Value & ) const;
.do end
.el .do begin
:SNPF index='findKeyAndValue'.Value * findKeyAndValue( const Key *,
:SNPFLF                      .                         Key &, Value & ) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to find an element in the dictionary with an key
equivalent to the first parameter.
If an equivalent element is found,
.if &lpref. eq Val .do begin
a non-zero value
.do end
.el .do begin
a pointer to the element
.MONO Value
.do end
is returned.
The reference to a
.MONO Key
passed as the second parameter is assigned the found element's key.
.if &lpref. eq Val .do begin
The reference to a
.MONO Value
passed as the third argument is assigned the found element's
.MONO Value.
.do end
Zero is returned if the element is not found.
:INCLUDE file='_keyequ.gml'
:RSLTS.
The element equivalent to the passed key is located in the dictionary.
:SALSO.
:SAL typ='fun'.findKeyAndValue
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.forAll
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='forAll'    .void forAll(
:SNPFLF                 .      void (*user_fn)( Key, Value, void * ),
:SNPFLF                 .      void * );
.do end
.el .do begin
:SNPF index='forAll'    .void forAll(
:SNPFLF                 .      void (*user_fn)( Key *, Value *, void * ),
:SNPFLF                 .      void * );
.do end
:eSNPL.
:SMTICS.
The &fn. causes the user supplied function to be invoked for every key-value
pair in the dictionary.
The user function has the prototype
:XMPL.
.if &lpref. eq Val .do begin
void user_func( Key key, Value value, void * data );
.do end
.el .do begin
void user_func( Key * key, Value * value, void * data );
.do end
:eXMPL.
As the elements are visited, the user function is invoked with the
.MONO Key
and
.MONO Value
components of the element passed as the first two parameters.
The second parameter of the
.MONO forAll
function is passed as the third parameter to the user function.
This value can be used to pass any appropriate data from the main code to the
user function.
:RSLTS.
The elements in the dictionary are all visited, with the user function
being invoked for each one.
:SALSO.
:SAL typ='fun'.find
:SAL typ='fun'.findKeyAndValue
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
.if &lpref. eq Val .do begin
:SNPF index='insert'       .int insert( const Key &, const Value & );
.do end
.el .do begin
:SNPF index='insert'       .int insert( Key *, Value * );
.do end
:eSNPL.
:SMTICS.
The &fn. inserts
a key and value into the dictionary, using the hash function on the key to
determine to which bucket it should be stored.
If allocation of the
node to store the key-value pair fails, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, the insert will not be completed.
:P.
At some point, the number of buckets initially selected may be too small
for the number of elements inserted.
The resize of the dictionary can be controlled by the insertion mechanism
by using
.MONO WC&lpref.HashDict
as a base class, and providing an insert member function to
do a resize when appropriate.
This insert could then call
.MONO WC&lpref.HashDict::insert
to insert the element.
Note that copy constructors and assignment operators are not
inherited in your class, but you can provide the following inline
definitions (assuming that the class inherited
from WC&lpref.HashDict is named MyHashDict):
:XMPL.
inline MyHashDict( const MyHashDict &orig ) : WC&lpref.HashDict( orig ) {};
inline MyHashDict &operator=( const MyHashDict &orig ) {
    return( WC&lpref.HashDict::operator=( orig ) );
}
:eXMPL.
:RSLTS.
The &fn. inserts a key and value into the dictionary.
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
The &fn. is used to determine if the dictionary is empty.
:RSLTS.
The &fn. returns zero if it contains at least one entry,
non-zero if the dictionary is empty.
:SALSO.
:SAL typ='fun'.buckets
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.operator []
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='operator []'     . &lret. & operator[]( const Key & );
:eSNPL.
:SMTICS.
.MONO operator []
is the dictionary index operator.  A reference to the object stored
in the dictionary with the given
.MONO Key
is returned.
If no equivalent element is found, then a new
key-value pair is created with the specified
.MONO Key
value, and initialized with the default constructor.
The returned reference can then be assigned to,
so that insertions can be made with the operator.
.if &lpref. eq Val .do begin
:XMPL
WCValHashDict<int,String> data_object( &my_hash_fn );
data_object[ 5 ] = "Hello";
:eXMPL.
.do end
If an allocation error occurs while inserting a new key-value pair,
then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, then a reference to address
zero will be returned.
This will result in a run-time error on systems which trap address zero
references.
:RSLTS.
The &fn. returns a reference to the element at the given key value.
If the key does not exist, a reference to a created element is returned.
The result of the operator may be assigned to.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.operator []
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='operator []'     .const Value & operator[]( const Key & ) const;
.do end
.el .do begin
:SNPF index='operator []'     .Value * const & operator[]( const Key * ) const;
.do end
:eSNPL.
:SMTICS.
.MONO operator []
is the dictionary index operator.  A constant reference to the object stored
in the dictionary with the given
.MONO Key
is returned.
If no equivalent element is found, then the
.MONO index_range
.ix 'index_range' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, then a reference to address
zero will be returned.
This will result in a run-time error on systems which trap address zero
references.
:RSLTS.
The &fn. returns a constant reference to the element at the given key value.
The result of the operator may not be assigned to.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.index_range
:eSALSO.
:eLIBF.

:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                    .#include <wchash.h>
:SNPFLF                    .public:
:SNPF index='operator ='   .WC&lpref.HashDict & operator =( const WC&lpref.HashDict & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the &cls.:PERIOD.
The left hand side dictionary is first cleared using the
.MONO clear
member function, and then the right hand side dictionary is copied.
The hash function, exception trap states, and all of the dictionary
elements are copied.
If an allocation failure occurs when creating the buckets, the
table will be created with zero buckets, and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If there is not enough memory to copy all of
the values or pointers in the dictionary, then only some will be copied, and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
The number of entries will correctly reflect the number copied.
:RSLTS.
The &fn. assigns the left hand side dictionary to be a copy of the
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
:SNPF index='operator =='.int operator ==( const WC&lpref.HashDict & ) const;
:eSNPL.
:SMTICS.
The &fn. is the equivalence operator for the &cls.:PERIOD.
Two dictionary objects are equivalent if they are the same object and share the
same address.
:RSLTS.
A TRUE (non-zero) value is returned if the left hand side and right
hand side dictionary are the same object.  A FALSE (zero) value is returned
otherwise.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.remove
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='remove'    .int remove( const Key & );
.do end
.el .do begin
:SNPF index='remove'    .Value * remove( const Key * );
.do end
:eSNPL.
:SMTICS.
The &fn. is used to remove the specified element from the dictionary.
If an equivalent element is found,
.if &lpref. eq Val .do begin
a non-zero
.do end
.el .do begin
the pointer
.do end
value is returned.
Zero is returned if the element is not found.
:INCLUDE file='_keyequ.gml'
:RSLTS.
The element is removed from the dictionary if it found.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.resize
:SNPL.
:SNPFLF                 .#include <wchash.h>
:SNPFLF                 .public:
:SNPF index='resize'    .void resize( unsigned );
:eSNPL.
:SMTICS.
The &fn. is used to change the number of buckets contained in the dictionary.
If the new number is larger than the previous dictionary size, then the
hash function will be used on all of the stored elements to determine
which bucket they should be stored into.
Entries are not destroyed or created in the process of being moved.
If there is not enough memory to resize the dictionary, the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled, and the dictionary will contain
the number of buckets it contained before the resize.
If the new number is zero, then the
.MONO zero_buckets
.ix 'zero_buckets' 'exception'
exception is thrown if it is enabled, and no resize will be performed.
The dictionary is guaranteed to contain the same number of entries after
the resize.
:RSLTS.
The dictionary is resized to the new number of buckets.
:SALSO.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.zero_buckets
:eSALSO.
:eLIBF.
