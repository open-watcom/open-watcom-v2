:CLFNM.WC&lpref.SkipListDict<Key,Value>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.SkipListDict<Key,Value>
:HFILE.wcskip.h
:CLSS.
The &cls. is a templated class used to store objects in a dictionary.
Dictionaries store values with an associated key, which may be of any type.
One example of a dictionary used in everyday life is the phone book.
The phone numbers are the data values, and the customer
name is the key.
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
The iterator classes for skip lists have the same function and operator
interface as the hash iterators classes.
See the chapter on hash iterators for more information.

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
:P.
A well defined operator less than with constant parameters
.br
(
.MONO int operator <( const Key & ) const
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
:MFN index='WC&lpref.SkipListDict' .WC&lpref.SkipListDict( unsigned = WCSkipListDict_PROB_QUARTER, unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS );
:MFN index='WC&lpref.SkipListDict' .WC&lpref.SkipListDict( unsigned = WCSkipListDict_PROB_QUARTER, unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS, void * (*user_alloc)( size_t size ), void (*user_dealloc)( void *old, size_t size ) );
:MFN index='WC&lpref.SkipListDict' .WC&lpref.SkipListDict( const WC&lpref.SkipListDict & );
:MFN index='~~WC&lpref.SkipListDict' .virtual ~~WC&lpref.SkipListDict();
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
.do end
.if &lpref eq Ptr .do begin
:MFN index='contains'       .int contains( const Key * ) const;
:MFN index='entries'        .unsigned entries() const;
:MFN index='find'            .Value * find( const Key * ) const;
:MFN index='findKeyAndValue' .Value * findKeyAndValue( const Key *, Key * & ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Key *, Value *, void * ), void * );
:MFN index='insert'         .int insert( Key *, Value * );
.do end
:MFN index='isEmpty'        .int isEmpty() const;
.if &lpref eq Val .do begin
:MFN index='remove'         .int remove( const Key & );
.do end
.el .do begin
:MFN index='remove'         .Value * remove( const Key * );
.do end
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
.if &lpref. eq Val .do begin
:MFN index='operator []'     .Value & operator []( const Key & );
:MFN index='operator []'     .const Value & operator []( const Key & ) const;
.do end
.el .do begin
:MFN index='operator []'     .Value * & operator []( const Key * );
:MFN index='operator []'     .Value * const & operator []( const Key * ) const;
.do end
:MFN index='operator ='      .WC&lpref.SkipListDict & operator =( const WC&lpref.SkipListDict & );
:MFN index='operator =='     .int operator ==( const WC&lpref.SkipListDict & ) const;
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListDict<Key,Value>' fmt='ctor' prot='public'.WC&lpref.SkipListDict
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipListDict( unsigned = WCSKIPLIST_PROB_QUARTER,
:SNPFLF          .            unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS );
:eSNPL.
:SMTICS.
The &fn. creates an &obj. with no entries.
The first optional parameter, which defaults to the constant
.MONO WCSKIPLIST_PROB_QUARTER,
determines the probability of having a certain number of pointers in each
skip list node.
The second optional parameter, which defaults to the constant
.MONO WCDEFAULT_SKIPLIST_MAX_PTRS,
determines the maximum number of pointers that are allowed in any
skip list node.
.MONO WCDEFAULT_SKIPLIST_MAX_PTRS
is the maximum effective value of the second parameter.
If an allocation failure occurs while creating the skip list, the
.MONO out_of_memory
exception is thrown if the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled.

:RSLTS.
The &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListDict<Key,Value>' fmt='ctor' prot='public'.WC&lpref.SkipListDict
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipListDict( unsigned = WCSKIPLIST_PROB_QUARTER,
:SNPFLF          .            unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS,
:SNPFLF          .            void * (*user_alloc)( size_t ),
:SNPFLF          .            void (*user_dealloc)( void *, size_t ) );
:eSNPL.
:SMTICS.
Allocator and deallocator functions are specified
for use when entries are inserted and removed from the list dictionary.
The semantics of this constructor are the same as the constructor without
the memory management functions.
:P.
The allocation function must return a
zero if it cannot perform the allocation.
The deallocation function is passed the size as well
as the pointer to the data.
Your allocation system may take advantage of the characteristic that
the allocation function will always be called with the same size
value for any particular instantiation of a list dictionary.
To determine the size of
the objects that the memory management functions will be
required to allocate and free, the following macro may be used:
.br
.MONO WC&lpref.SkipListDictItemSize( Key, Value, num_of_pointers )
:RSLTS.
The &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListDict<Key,Value>' fmt='ctor' prot='public'.WC&lpref.SkipListDict
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipListDict( const WC&lpref.SkipListDict & );
:eSNPL.
:SMTICS.
The &fn. is the copy constructor for the &cls.:PERIOD.
The new skip list is created with the same probability and maximum pointers,
all values or pointers stored in the list, and the exception trap states.
If there is not enough memory to copy all of
the values, then only some will be copied,
and the number of entries will correctly reflect the number copied.
If all of the elements cannot be copied, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
:RSLTS.
The &fn. creates an &obj. which is a copy of the passed dictionary.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListDict<Key,Value>' fmt='dtor' prot='public'.~~WC&lpref.SkipListDict
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.SkipListDict();
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
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
:SNPF index='clear'     .void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the dictionary so that it has no entries.
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
:SNPFLF                 .#include <wcskip.h>
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
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
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
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
:SNPF index='entries'   .unsigned entries() const;
:eSNPL.
:SMTICS.
The &fn. is used to return the current number of elements
stored in the dictionary.
:RSLTS.
The &fn. returns the number of elements in the dictionary.
:SALSO.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.find
:SNPL.
:SNPFLF                 .#include <wcskip.h>
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
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='findKeyAndValue' .int findKeyAndValue( const Key &,
:SNPFLF                       .                     Key &, Value & ) const;
.do end
.el .do begin
:SNPF index='findKeyAndValue'.Value * findKeyAndValue( const Key *, Key * & ) const;
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
:SNPFLF                 .#include <wcskip.h>
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
:SNPFLF                    .#include <wcskip.h>
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
a key and value into the dictionary.
If allocation of the
node to store the key-value pair fails, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, the insert will not be completed.
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
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
:SNPF index='isEmpty'   .int isEmpty() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if the dictionary is empty.
:RSLTS.
The &fn. returns zero if it contains at least one entry,
non-zero if the dictionary is empty.
:SALSO.
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.operator []
:SNPL.
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='operator []'     . Value & operator[]( const Key & );
.do end
.el .do begin
:SNPF index='operator []'     . Value * & operator[]( const Key * );
.do end
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
:SNPFLF                 .#include <wcskip.h>
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
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPF index='operator ='   .WC&lpref.SkipListDict & operator =( const WC&lpref.SkipListDict & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the &cls.:PERIOD.
The left hand side dictionary is first cleared using the
.MONO clear
member function, and then the right hand side dictionary is copied.
The new skip list is created with the same probability and maximum pointers,
all values or pointers stored in the list, and the exception trap states.
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
:SNPFLF          .#include <wcskip.h>
:SNPFLF          .public:
:SNPF index='operator =='.int operator ==( const WC&lpref.SkipListDict & ) const;
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
:SNPFLF                 .#include <wcskip.h>
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
