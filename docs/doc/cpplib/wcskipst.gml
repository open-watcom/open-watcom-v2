:CLFNM cl2='WC&lpref.SkipListSet<Type>'.WC&lpref.SkipList<Type>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.SkipList<Type>, WC&lpref.SkipListSet<Type>
:HFILE.wcskip.h
:CLSS.
&cls. are templated classes used to store objects in a skip list.
A skip list is a probabilistic alternative to balanced trees,
and provides a reasonable performance balance to insertion, search, and
deletion.
A skip list allows more than one copy of an element that is equivalent,
while the skip list set allows only one copy.
The equality operator of the element's type is used to locate the value.
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the template parameter defining
.if &lpref. eq Val .do begin
the type of the data to be stored in the list.
.do end
.el .do begin
the type of the data pointed to by the pointers stored in the list.
.do end

.if &lpref. eq Val .do begin
:P.
Values are copied into the list, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value skip lists should not be used to store objects of a base class if any
derived types of different sizes would be stored in the list, or if the
destructor for a derived class must be called.
.do end
.el .do begin
:P.
Note that pointers to the elements are stored in the list.
Destructors are not called on the elements pointed to.
The data values pointed to in the list should not be changed such
that the equivalence to the old value is modified.
.do end
:P.
The iterator classes for skip lists have the same function and operator
interface as the hash iterators classes.
See the chapter on hash iterators for more information.

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
.do end
:P.
A well defined equivalence operator
.br
(
.MONO int operator ==( const Type & ) const
).
:P.
A well defined less than operator
.br
(
.MONO int operator <( const Type & ) const
).

:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFN index='WC&lpref.SkipList' .WC&lpref.SkipList( unsigned = WCSKIPLIST_PROB_QUARTER, unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS );
:MFN index='WC&lpref.SkipList' .WC&lpref.SkipList( unsigned = WCSKIPLIST_PROB_QUARTER, unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS, void * (*user_alloc)( size_t size ), void (*user_dealloc)( void *old, size_t size ) );
:MFN index='WC&lpref.SkipList' .WC&lpref.SkipList( const WC&lpref.SkipList & );
:MFN index='~~WC&lpref.SkipList' .virtual ~~WC&lpref.SkipList();
:MFN index='WC&lpref.SkipListSet' .WC&lpref.SkipListSet( unsigned = WCSKIPLIST_PROB_QUARTER, unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS );
:MFN index='WC&lpref.SkipListSet' .WC&lpref.SkipListSet( unsigned = WCSKIPLIST_PROB_QUARTER, unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS, void * (*user_alloc)( size_t size ), void (*user_dealloc)( void *old, size_t size ) );
:MFN index='WC&lpref.SkipListSet' .WC&lpref.SkipListSet( const WC&lpref.SkipListSet & );
:MFN index='~~WC&lpref.SkipListSet' .virtual ~~WC&lpref.SkipListSet();
:MFN index='clear'          .void clear();
.if &lpref eq Ptr .do begin
:MFN index='clearAndDestroy'.void clearAndDestroy();
.do end
.if &lpref eq Val .do begin
:MFN index='contains'       .int contains( const Type & ) const;
:MFN index='entries'        .unsigned entries() const;
:MFN index='find'           .int find( const Type &, Type & ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Type, void * ), void * );
:MFN index='insert'         .int insert( const Type & );
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='remove'         .int remove( const Type & );
.do end
.if &lpref eq Ptr .do begin
:MFN index='contains'       .int contains( const Type * ) const;
:MFN index='entries'        .unsigned entries() const;
:MFN index='find'           .Type * find( const Type * ) const;
:MFN index='forall'         .void forAll( void (*user_fn)( Type *, void * ) , void * );
:MFN index='insert'         .int insert( Type * );
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='remove'         .Type * remove( const Type * );
.do end
:eMFNL.
The following public member functions are available for the
.MONO WC&lpref.SkipList
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
:MFN index='operator ='      .WC&lpref.SkipList & operator =( const WC&lpref.SkipList & );
:MFN index='operator =='     .int operator ==( const WC&lpref.SkipList & ) const;
:MFN index='operator ='      .WC&lpref.SkipListSet & operator =( const WC&lpref.SkipListSet & );
:MFN index='operator =='     .int operator ==( const WC&lpref.SkipListSet & ) const;
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListSet<Type>' fmt='ctor' prot='public'.WC&lpref.SkipListSet
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipListSet( unsigned = WCSKIPLIST_PROB_QUARTER,
:SNPFLF          .            unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SkipListSet<Type>
constructor creates a
.MONO WC&lpref.SkipListSet
object with no entries.
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
The
.MONO WC&lpref.SkipListSet<Type>
constructor creates an initialized
.MONO WC&lpref.SkipListSet
object.
:SALSO.
:SAL typ='dtor'.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListSet<Type>' fmt='ctor' prot='public'.WC&lpref.SkipListSet
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipListSet( unsigned = WCSKIPLIST_PROB_QUARTER,
:SNPFLF          .            unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS,
:SNPFLF          .            void * (*user_alloc)( size_t ),
:SNPFLF          .            void (*user_dealloc)( void *, size_t ) );
:eSNPL.
:SMTICS.
Allocator and deallocator functions are specified
for use when entries are inserted and removed from the list.
The semantics of this constructor are the same as the constructor without
the memory management functions.
:P.
The allocation function must return a
zero if it cannot perform the allocation.
The deallocation function is passed the size as well
as the pointer to the data.
Your allocation system may take advantage of the characteristic that
the allocation function will always be called with the same size
value for any particular instantiation of a skip list.
To determine the size of
the objects that the memory management functions will be
required to allocate and free, the following macro may be used:
.br
.MONO WC&lpref.SkipListSetItemSize( Type, num_of_pointers )
:RSLTS.
The
.MONO WC&lpref.SkipListSet<Type>
constructor creates an initialized
.MONO WC&lpref.SkipListSet
object.
:SALSO.
:SAL typ='dtor'.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListSet<Type>' fmt='ctor' prot='public'.WC&lpref.SkipListSet
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipListSet( const WC&lpref.SkipListSet & );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SkipListSet<Type>
constructor is the copy constructor for the
.MONO WC&lpref.SkipListSet
class.
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
The
.MONO WC&lpref.SkipListSet<Type>
constructor creates a
.MONO WC&lpref.SkipListSet
object which is a copy of the passed list.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipListSet<Type>' fmt='dtor' prot='public'.~~WC&lpref.SkipListSet
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.SkipListSet();
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SkipListSet<Type>
destructor is the destructor for the
.MONO WC&lpref.SkipListSet
class.
If the number of elements is not zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the list elements are cleared using the
.MONO clear
member function.
.if &lpref. eq Ptr .do begin
The objects which the list elements point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
The call to the
.MONO WC&lpref.SkipListSet<Type>
destructor is inserted implicitly by the compiler
at the point where the
.MONO WC&lpref.SkipListSet
object goes out of scope.
:RSLTS.
The call to the
.MONO WC&lpref.SkipListSet<Type>
destructor destroys a
.MONO WC&lpref.SkipListSet
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
:LIBF cltype='WC&lpref.SkipList<Type>' fmt='ctor' prot='public'.WC&lpref.SkipList
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipList( unsigned = WCSKIPLIST_PROB_QUARTER,
:SNPFLF          .            unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SkipList<Type>
constructor creates a
.MONO WC&lpref.SkipList
object with no entries.
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
The
.MONO WC&lpref.SkipList<Type>
constructor creates an initialized
.MONO WC&lpref.SkipList
object.
:SALSO.
:SAL typ='dtor'.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipList<Type>' fmt='ctor' prot='public'.WC&lpref.SkipList
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipList( unsigned = WCSKIPLIST_PROB_QUARTER,
:SNPFLF          .            unsigned = WCDEFAULT_SKIPLIST_MAX_PTRS,
:SNPFLF          .            void * (*user_alloc)( size_t ),
:SNPFLF          .            void (*user_dealloc)( void *, size_t ) );
:eSNPL.
:SMTICS.
Allocator and deallocator functions are specified
for use when entries are inserted and removed from the list.
The semantics of this constructor are the same as the constructor without
the memory management functions.
:P.
The allocation function must return a
zero if it cannot perform the allocation.
The deallocation function is passed the size as well
as the pointer to the data.
Your allocation system may take advantage of the characteristic that
the allocation function will always be called with the same size
value for any particular instantiation of a skip list.
To determine the size of
the objects that the memory management functions will be
required to allocate and free, the following macro may be used:
.br
.MONO WC&lpref.SkipListItemSize( Type, num_of_pointers )
:RSLTS.
The
.MONO WC&lpref.SkipList<Type>
constructor creates an initialized
.MONO WC&lpref.SkipList
object.
:SALSO.
:SAL typ='dtor'.
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipList<Type>' fmt='ctor' prot='public'.WC&lpref.SkipList
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WC&lpref.SkipList( const WC&lpref.SkipList & );
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SkipList<Type>
constructor is the copy constructor for the
.MONO WC&lpref.SkipList
class.
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
The
.MONO WC&lpref.SkipList<Type>
constructor creates a
.MONO WC&lpref.SkipList
object which is a copy of the passed list.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SkipList<Type>' fmt='dtor' prot='public'.~~WC&lpref.SkipList
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPCD cd_idx='d'.virtual ~~WC&lpref.SkipList();
:eSNPL.
:SMTICS.
The
.MONO WC&lpref.SkipList<Type>
destructor is the destructor for the
.MONO WC&lpref.SkipList
class.
If the number of elements is not zero and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
Otherwise, the list elements are cleared using the
.MONO clear
member function.
.if &lpref. eq Ptr .do begin
The objects which the list elements point to are not deleted unless the
.MONO clearAndDestroy
member function is explicitly called before the destructor is called.
.do end
The call to the
.MONO WC&lpref.SkipList<Type>
destructor is inserted implicitly by the compiler
at the point where the
.MONO WC&lpref.SkipList
object goes out of scope.
:RSLTS.
The call to the
.MONO WC&lpref.SkipList<Type>
destructor destroys a
.MONO WC&lpref.SkipList
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
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
:SNPF index='clear'     .void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the list so that it has no entries.
.if &lpref. eq Val .do begin
Elements stored in the list are destroyed using
the destructors of
.MONO Type.
.do end
.el .do begin
Objects pointed to by the list elements are not deleted.
.do end
The list object is not destroyed and re-created by this function, so
the object destructor is not invoked.
:RSLTS.
The &fn. clears the list to have no elements.
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
:LIBF fmt='mfun' cllong='yes' prot='public'.clearAndDestroy
:SNPL.
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
:SNPF index='clearAndDestroy'   .void clearAndDestroy();
:eSNPL.
:SMTICS.
The &fn. is used to clear the list and delete
the objects pointed to by the list elements.
The list object is not destroyed and re-created by this function, so
the list object destructor is not invoked.
:RSLTS.
The &fn. clears the list by deleting the objects pointed to by the
list elements, and then removing the list elements from the list.
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
:SNPF index='contains'  .int contains( const Type & ) const;
.do end
.el .do begin
:SNPF index='contains'  .int contains( const Type * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn.
returns non-zero if the element is stored in
the list, or zero if there is no equivalent element.
:INCLUDE file='_valequ.gml'
:RSLTS.
The &fn. returns a non-zero value if the element is found in the list.
:SALSO.
:SAL typ='fun'.find
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
stored in the list.
:RSLTS.
The &fn. returns the number of elements in the list.
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
:SNPF index='find'      .int find( const Type &, Type & ) const;
.do end
.el .do begin
:SNPF index='find'      .Type * find( const Type * ) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to find an element with an equivalent value in the list.
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
The element equivalent to the passed value is located in the list.
:eLIBF.

:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.forAll
:SNPL.
:SNPFLF                 .#include <wcskip.h>
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
in the list.
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
The elements in the list are all visited, with the user function
being invoked for each one.
:SALSO.
:SAL typ='fun'.find
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF                    .#include <wcskip.h>
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
a value into the list.
If allocation of the
node to store the value fails, then the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, the insert will not be completed.
:P.
With a
.MONO WC&lpref.SkipListSet,
there must be only one equivalent element in the set.
If an element equivalent to the inserted element is already
in the list set, the list set will remain unchanged, and the
.MONO not_unique
.ix 'not_unique' 'exception'
exception is thrown if it is enabled.
If the exception is not enabled, the insert will not be completed.
:RSLTS.
The &fn. inserts a value into the list.
If the insert is successful, a non-zero will returned.
A zero will be returned if the insert fails.
:SALSO.
:SAL typ='fun'.operator~b=
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCExcept'.not_unique
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
The &fn. is used to determine if the list is empty.
:RSLTS.
The &fn. returns zero if it contains at least one entry,
non-zero if the list is empty.
:SALSO.
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.SkipList<Type>' fmt='mfun' prot='public'.occurrencesOf
:SNPL.
:SNPFLF                 .#include <wcskip.h>
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
stored in the list which are equivalent to the passed value.
:INCLUDE file='_valequ.gml'
:RSLTS.
The &fn. returns the number of elements in the list which are
equivalent to the passed value.
:SALSO.
:SAL typ='fun'.entries
:SAL typ='fun'.find
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                    .#include <wcskip.h>
:SNPFLF                    .public:
:SNPF index='operator ='   .WC&lpref.SkipList & operator =( const WC&lpref.SkipList & );
:SNPF index='operator ='   .WC&lpref.SkipListSet & operator =( const WC&lpref.SkipListSet & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the &cls.:PERIOD.
The left hand side list is first cleared using the
.MONO clear
member function, and then the right hand side list is copied.
The list function, exception trap states, and all of the list
elements are copied.
If there is not enough memory to copy all of
the values or pointers in the list, then only some will be copied, and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is thrown if it is enabled.
The number of entries will correctly reflect the number copied.
:RSLTS.
The &fn. assigns the left hand side list to be a copy of the
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
:SNPF index='operator =='.int operator ==( const WC&lpref.SkipList & ) const;
:SNPF index='operator =='.int operator ==( const WC&lpref.SkipListSet & ) const;
:eSNPL.
:SMTICS.
The &fn. is the equivalence operator for the &cls.:PERIOD.
Two list objects are equivalent if they are the same object and share the
same address.
:RSLTS.
A TRUE (non-zero) value is returned if the left hand side and right
hand side list are the same object.  A FALSE (zero) value is returned
otherwise.
:eLIBF.
:CMT.======================================================================
:LIBF fmt='mfun' prot='public'.remove
:SNPL.
:SNPFLF                 .#include <wcskip.h>
:SNPFLF                 .public:
.if &lpref. eq Val .do begin
:SNPF index='remove'    .int remove( const Type & );
.do end
.el .do begin
:SNPF index='remove'    .Type * remove( const Type * );
.do end
:eSNPL.
:SMTICS.
The &fn. is used to remove the specified element from the list.
If an equivalent element is found,
.if &lpref. eq Val .do begin
a non-zero
.do end
.el .do begin
the pointer
.do end
value is returned.
Zero is returned if the element is not found.
If the list is a
.MONO WC&lpref.SkipList
and there is more than one element equivalent to the specified
element, then the last equivalent element added to the
.MONO WC&lpref.SkipList
is removed.
:INCLUDE file='_valequ.gml'
:RSLTS.
The element is removed from the list.
:eLIBF.
:CMT.======================================================================
:LIBF cltype='WC&lpref.SkipList<Type>' fmt='mfun' prot='public'.removeAll
:SNPL.
:SNPFLF                 .#include <wcskip.h>
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
specified element from the list.
Zero is returned if no equivalent elements are found.
:INCLUDE file='_valequ.gml'
:RSLTS.
All equivalent elements are removed from the list.
:eLIBF.
