:CLFNM cl2='WC&lpref.DList<Type>'.WC&lpref.SList<Type>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.SList<Type>, WC&lpref.DList<Type>
:HFILE.wclist.h
:CMT.:DVFML.    List classes are not derived from links, they operate on them.
:CMT.:DVFM.WCSLink
:CMT.:DVFM.WCDLink
:CMT.:eDVFML.
:CLSS.
The &cls. are the templated classes used to create objects which
are single or double linked lists.
.*
.if &lpref. eq Isv .th .do begin
The created list is intrusive,
.ix 'intrusive' 'classes'
which means that list elements which
are inserted must be created with a library supplied base class.
The class
.MONO WCSLink
provides the base class definition for single linked lists, and
should be inherited by the definition of any list item for single linked lists.
It provides the linkage that is used to traverse the list elements.
Similarly, the class
.MONO WCDLink
provides the base class definition for double lists, and
should be inherited by the definition of any list item for double lists.

.do end
.el .if &lpref. eq Val .do begin

Values are copied into the list, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value lists should not be used to store objects of a base class if any
derived types of different sizes would be stored in the list, or if the
destructor for a derived class must be called.

.do end
.*
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the type value specified as the template parameter.
.*
.if &lpref. eq Isv .do begin
.MONO Type
is the type of the list elements, derived from
.MONO WCSLink
or
.MONO WCDLink.
.do end
.el .if &lpref. eq Val .do begin
.MONO Type
is the type of the values stored in the list.
.do end
.el .do begin
The pointers stored in the list point to values of type
.MONO Type.
.do end
.*
:P.
:CMT. state WCExcept is base class
:INCLUDE file='_EXPT_BC'.
:HDG.Requirements of Type
.*
.if &lpref. eq Isv .do begin
The
.MONO WCIsvSList<Type>
class requires only that
.MONO Type
is derived from
.MONO WCSLink.
The
.MONO WCIsvDList<Type>
class requires only that
.MONO Type
is derived from
.MONO WCDLink.
.do end
.el .do begin
The &cls requires
.MONO Type
to have:
:P.
.if &lpref. eq Val .do begin
(1) a default constructor (
.MONO Type::Type()
).
:P.
(2) a well defined copy constructor (
.MONO Type::Type( const Type & )
).
:P.
(3) an equivalence operator with constant parameters
.in +10
.MONO Type::operator ==( const Type & ) const
.in -10
.do end
.el .do begin
(1) an equivalence operator with constant parameters
.in +10
.MONO Type::operator ==( const Type & ) const
.in -10
.do end
.do end
.*
.*
.if &lpref. eq Isv .th .do begin
:HDG.Private Member Functions
In an intrusive list, copying a list is undefined.
Setting the copy constructor and assignment operator as private is the
standard mechanism to ensure a copy cannot be made.  The following member
functions are declared private:
:MFNL.
:MFN index='WC&lpref.SList'     .void WC&lpref.SList( const WC&lpref.SList & );
:MFN index='WC&lpref.DList'     .void WC&lpref.DList( const WC&lpref.DList & );
:MFN index='operator ='      .WC&lpref.SList & WC&lpref.SList::operator =( const WC&lpref.SList & );
:MFN index='operator ='      .WC&lpref.DList & WC&lpref.DList::operator =( const WC&lpref.DList & );
:eMFNL.
.do end
.*
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c' .WC&lpref.SList();
.if &lpref. ne Isv .do begin
:MFCD cd_idx='c' .WC&lpref.SList( void * (*)( size_t ), void (*)( void *, size_t ));
:MFCD cd_idx='c' .WC&lpref.SList( const WC&lpref.SList & );
.do end
:MFCD cd_idx='d' .~~WC&lpref.SList();
:MFCD cd_idx='c' .WC&lpref.DList();
.if &lpref. ne Isv .do begin
:MFCD cd_idx='c' .WC&lpref.DList( void * (*)( size_t ), void (*)( void *, size_t ));
:MFCD cd_idx='c' .WC&lpref.DList( const WC&lpref.DList & );
.do end
:MFCD cd_idx='d' .~~WC&lpref.DList();
.if &lpref. eq Val .do begin
:MFN index='append'         .int append( const &lparm. );
.do end
.el .do begin
:MFN index='append'         .int append( &lparm. );
.do end
:MFN index='clear'          .void clear();
:MFN index='clearAndDestroy'.void clearAndDestroy();
:MFN index='contains'       .int contains( const &lparm. ) const;
:MFN index='entries'        .int entries() const;
:MFN index='find'           .&lret. find( int = 0 ) const;
:MFN index='findLast'       .&lret. findLast() const;
.if &lpref. eq Isv .th .do begin
:MFN index='forAll'         .void forAll( void (*)( &lret, void * ), void *);
.do end
.el .do begin
:MFN index='forAll'         .void forAll( void (*)( &lret., void * ), void *) const;
.do end
:MFN index='get'            .&lret. get( int = 0 );
:MFN index='index'          .int index( const &lparm ) const;
.if &lpref. eq Isv .do begin
:MFN index='index'          .int index( int (*)( const Type *, void * ), void * ) const;
.do end
.if &lpref. eq Val .do begin
:MFN index='insert'         .int insert( const &lparm. );
.do end
.el .do begin
:MFN index='insert'         .int insert( &lparm. );
.do end
:MFN index='isEmpty'        .int isEmpty() const;
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
.if &lpref. ne Isv .do begin
:MFN index='operator ='      .WC&lpref.SList & WC&lpref.SList::operator =( const WC&lpref.SList & );
:MFN index='operator ='      .WC&lpref.DList & WC&lpref.DList::operator =( const WC&lpref.DList & );
.do end
:MFN index='operator =='     .int WC&lpref.SList::operator ==( const WC&lpref.SList & ) const;
:MFN index='operator =='     .int WC&lpref.DList::operator ==( const WC&lpref.DList & ) const;
:eMFNL.
.if &lpref. eq Isv .th .do begin
:HDG.Sample Program Using an Intrusive List
:XMPL.
:INCLUDE file='wcldintr.cpp'.
:eXMPL.
.do end
.if &lpref. eq Val .th .do begin
:HDG.Sample Program Using a Value List
:XMPL.
:INCLUDE file='wcldval.cpp'.
:eXMPL.
.do end
.if &lpref. eq Ptr .th .do begin
:HDG.Sample Program Using a Pointer List
:XMPL.
:INCLUDE file='wcldptr.cpp'.
:eXMPL.
.do end
:eCLSS.
:eLIBF.
:CMT. for the _ALOCFNS and _DTOR2 include files
.SE lobjtype='WC&lpref.SList<Type>'
:CMT.========================================================================
:LIBF cltype='WC&lpref.SList<Type>' fmt='mfun' prot='public'.WC&lpref.SList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.SList'.WC&lpref.SList();
:eSNPL.
:SMTICS.
The &fn. creates an empty
.MONO WC&lpref.SList
object.
:RSLTS.
The &fn. produces an initialized
.MONO WC&lpref.SList
object.
:SALSO.
.if &lpref. ne Isv .do begin
:SAL typ='fun'.WC&lpref.SList
.do end
:SAL typ='fun'.~~WC&lpref.SList
:eSALSO.
:eLIBF.
.if &lpref ne Isv .do begin
:CMT.========================================================================
:LIBF cltype='WC&lpref.SList<Type>' fmt='mfun' prot='public'.WC&lpref.SList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.SList'.WC&lpref.SList( void *(*allocator)( size_t ),
:SNPFLF                     .            void (*deallocator)( void *, size_t ) );
:eSNPL.
:SMTICS.
The &fn. creates an empty
.MONO &lobjtype.
object.
The :HP1.allocator:eHP1.
function is registered to perform all memory allocations of the list
elements, and the :HP1.deallocator:eHP1.
function to perform all freeing of the list elements' memory.
:INCLUDE file='_ALOCFNS'.
:P.
The :HP1.allocator:eHP1. and :HP1.deallocator:eHP1. functions may assume
that for a list object instance, the
:HP1.allocator:eHP1. is always called with the same first argument
(the size of the memory to be allocated).
The
.MONO WCValSListItemSize(Type)
.ix 'WCValSListItemSize' 'macro'
macro returns the size of the elements which are allocated by the
:HP1.allocator:eHP1. function.
:RSLTS.
The &fn. creates an initialized
.MONO &lobjtype.
object and registers the :HP1.allocator:eHP1.
and :HP1.deallocator:eHP1. functions.
:SALSO.
:SAL typ='fun'.WC&lpref.SList
:SAL typ='fun'.~~WC&lpref.SList
:eSALSO.
:eLIBF.
.do end
:CMT.========================================================================
:LIBF cltype='WC&lpref.SList<Type>' fmt='mfun' prot='&lacc.'.WC&lpref.SList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .&lacc.:
:SNPF index='WC&lpref.DList'.void WC&lpref.SList( const WC&lpref.SList & );
:eSNPL.
:SMTICS.
The &fn. is the copy constructor for the single linked list class.
.if &lpref. eq Isv .th .do begin

Making a copy of the list object would result in a error condition,
since intrusive lists cannot share data items with other lists.

.do end
.el .do begin

All of the list elements are copied to the new list, as well as the exception
trap states, and any registered :HP1.allocator:eHP1. and :HP1.deallocator:eHP1.
functions.

.if &lpref. eq Val .do begin
.MONO Type's
copy constructor is invoked to copy the values contained by the list
elements.
.do end
.* end if Val

:P.
If all of the elements cannot be copied and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
is enabled in the list being copied, the exception is thrown.
The new list is created in a valid state, even if all of the list elements
could not be copied.
:RSLTS.
The &fn. produces a copy of the list.
:SALSO.
:SAL typ='fun'.WC&lpref.SList
:SAL typ='fun'.~~WC&lpref.SList
:SAL typ='fun'.clear
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.

.do end
.* end if not Isv
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.SList<Type>' fmt='mfun' prot='public'.~~WC&lpref.SList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.SList'.~~WC&lpref.SList();
:eSNPL.
:SMTICS.
The &fn. destroys the
.MONO WC&lpref.SList
object.
If the list is not empty and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
If the
.MONO not_empty
exception is not enabled and the list is not empty,
the list is cleared using the
.MONO clear
member function.
.se lobjtype=WC&lpref.SList
:INCLUDE file='_DTOR2'.
:RSLTS.
The
.MONO WC&lpref.SList
object is destroyed.
:SALSO.
:SAL typ='fun'.WC&lpref.SList
:SAL typ='fun'.clear
:SAL typ='fun'.clearAndDestroy
:SAL typ='omtyp' ocls='WCExcept'.not_empty
:eSALSO.
:eLIBF.
:CMT. for the _ALOCFNS and _DTOR2 include files
.SE lobjtype='WC&lpref.DList<Type>'
:CMT.========================================================================
:LIBF cltype='WC&lpref.DList<Type>' fmt='mfun' prot='public'.WC&lpref.DList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.DList'.WC&lpref.DList();
:eSNPL.
:SMTICS.
The &fn. creates an empty
.MONO WC&lpref.DList
object.
:RSLTS.
The &fn. produces an initialized
.MONO WC&lpref.DList
object.
:SALSO.
.if &lpref. ne Isv .do begin
:SAL typ='fun'.WC&lpref.DList
.do end
:SAL typ='fun'.~~WC&lpref.DList
:eSALSO.
:eLIBF.
.if &lpref ne Isv .do begin
:CMT.========================================================================
:LIBF cltype='WC&lpref.DList<Type>' fmt='mfun' prot='public'.WC&lpref.DList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.DList'.WC&lpref.DList( void *(*allocator)( size_t ),
:SNPFLF                     .            void (*deallocator)( void *, size_t ) );
:eSNPL.
:SMTICS.
The &fn. creates an empty
.MONO &lobjtype.
object.
The :HP1.allocator:eHP1.
function is registered to perform all memory allocations of the list
elements, and the :HP1.deallocator:eHP1.
function to perform all freeing of the list elements' memory.
:INCLUDE file='_ALOCFNS'.
:P.
The :HP1.allocator:eHP1. and :HP1.deallocator:eHP1. functions may assume that
for a list object instance, the
:HP1.allocator:eHP1. is always called with the same first argument
(the size of the memory to be allocated).
The
.MONO WCValDListItemSize(Type)
.ix 'WCValDListItemSize' 'macro'
macro returns the size of the elements which are allocated by the
:HP1.allocator:eHP1. function.
:RSLTS.
The &fn. creates an initialized
.MONO &lobjtype.
object and registers the :HP1.allocator:eHP1.
and :HP1.deallocator:eHP1. functions.
:SALSO.
:SAL typ='fun'.WC&lpref.DList
:SAL typ='fun'.~~WC&lpref.DList
:eSALSO.
:eLIBF.
.do end
:CMT.========================================================================
:LIBF cltype='WC&lpref.DList<Type>' fmt='mfun' prot='&lacc.'.WC&lpref.DList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .&lacc.:
:SNPF index='WC&lpref.DList'.WC&lpref.DList( const WC&lpref.DList & );
:eSNPL.
:SMTICS.
The &fn. is the copy constructor for the double linked list class.
.if &lpref. eq Isv .th .do begin

Making a copy of the list object would result in a error condition,
since intrusive lists cannot share data items with other lists.

.do end
.el .do begin

All of the list elements are copied to the new list, as well as the exception
trap states, and any registered :HP1.allocator:eHP1. and :HP1.deallocator:eHP1.
functions.

.if &lpref. eq Val .do begin
.MONO Type's
copy constructor is invoked to copy the values contained by the list
elements.
.do end
.* end if Val

:P.
If all of the elements cannot be copied and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
is enabled in the list being copied, the exception is thrown.
The new list is created in a valid state, even if all of the list elements
could not be copied.
:RSLTS.
The &fn. produces a copy of the list.
:SALSO.
:SAL typ='fun'.WC&lpref.DList
:SAL typ='fun'.~~WC&lpref.DList
:SAL typ='fun'.clear
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.

.do end
.* end if not Isv
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.DList<Type>' fmt='mfun' prot='public'.~~WC&lpref.DList
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.DList'.~~WC&lpref.DList();
:eSNPL.
:SMTICS.
The &fn. destroys the
.MONO WC&lpref.DList
object.
If the list is not empty and the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled, the exception is thrown.
If the
.MONO not_empty
exception is not enabled and the list is not empty,
the list is cleared using the
.MONO clear
member function.
.se lobjtype=WC&lpref.DList
:INCLUDE file='_DTOR2'.
:RSLTS.
The
.MONO WC&lpref.DList
object is destroyed.
:SALSO.
:SAL typ='fun'.WC&lpref.DList
:SAL typ='fun'.clear
:SAL typ='fun'.clearAndDestroy
:SAL typ='omtyp' ocls='WCExcept'.not_empty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.append
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
.if &lpref. eq Val .do begin
:SNPF index='append'  .int append( const &lparm. );
.do end
.el .do begin
:SNPF index='append'  .int append( &lparm. );
.do end
:eSNPL.
:SMTICS.
.if &lpref. ne Isv .do begin

The &fn. is used to append the data
to the end of the list.

.if &lpref. eq Val .do begin
The data stored in the list is a copy of the data passed as a parameter.
.do end

:P.
If the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled and the append fails, the exception is thrown.
:RSLTS.
The data element is appended to the end of the list.  A TRUE value
(non-zero) is returned if the append is successful.  A FALSE (zero)
result is returned if the append fails.

.do end
.el .do begin

The &fn. is used to append the list element
object to the end of the list.
The address of (a pointer to) the list element object should be passed,
not the value.
Since the linkage information is stored in the list element, it is not
possible for the element to be in more than one list, or in the same list
more than once.
:P.
The passed list element should be constructed using
the appropriate link class as a base.
.MONO WCSLink
must be used as a list element base class for single linked lists,
and
.MONO WCDLink
must be used as a list element base class for double linked lists.
:RSLTS.
The list element is appended to the end of the list and a TRUE value
(non-zero) is returned.

.do end
:SALSO.
:SAL typ='fun'.insert
.if &lpref ne Isv .do begin
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
.do end
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:SNPF index='clear'.void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the list object and set it to the state of the object
just after the initial construction.
The list object is not destroyed and re-created by this operator, so
the object destructor is not invoked.

.if &lpref. eq Val .th .do begin

:P.
The &fn. has the same sematics as the
.MONO clearAndDestroy
member function.

.do end
.el .if &lpref. eq Isv .do begin

The list elements are not cleared.
Any list items still in the list are lost unless pointed to by some
pointer object in the program code.
:P.
If any of the list elements are not allocated with
.MONO new
(local variable or global list elements), then the &fn. must be used.
When all list elements are allocated with
.MONO new,
the
.MONO clearAndDestory
member function should be used.
.do end

:RSLTS.
The &fn. resets the list object to the state of the object immediately
after the initial construction.
:SALSO.
:SAL typ='fun'.~~WC&lpref.SList
:SAL typ='fun'.~~WC&lpref.DList
:SAL typ='fun'.clearAndDestroy
:SAL typ='fun'.get
:SAL typ='fun'.operator~b=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.clearAndDestroy
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:SNPF index='clearAndDestroy'.void clearAndDestroy();
:eSNPL.
:SMTICS.
The &fn. is used to clear the list object and set it to the state
of the object just after the initial construction.
The list object is not destroyed and re-created by this operator, so
the object destructor is not invoked.

.if &lpref. eq Val .do begin

:P.
Before the list object is re-initialized, the
delete operator is called for each list element.
:RSLTS.
The &fn. resets the list object to the initial state of the object immediately
after the initial construction.

.do end
.el .do begin

.if &lpref. eq Ptr .do begin

Before the list object is re-initialized, the
the values pointed to by the list elements are deleted.

.* end if Ptr
.do end
.el .do begin
.* Isv

The link elements are deleted before the list is re-initialized.
:P.
If any elements in the list were not allocated by the
.MONO new
operator, the &fn must not be called.
The &fn destroys each list element with the destructor for
.MONO Type
even if the list element was created as an object derived from
.MONO Type,
unless
.MONO Type
has a pure virtual destructor.

.* end if Isv
.do end

:RSLTS.
The &fn. resets the list object to the initial state of the object immediately
after the initial construction and deletes the list elements.

.* end if not val
.do end
:SALSO.
:SAL typ='fun'.clear
:SAL typ='fun'.get
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.contains
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:SNPF index='contains'  .int contains( const &lparm. ) const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if a list element
object is already contained in the list.

.if &lpref. eq Isv .th .do begin
The address of (a pointer to) the list element object should be passed,
not the value.
Each list element is compared to the passed element object to determine
if it has the same address.
Note that the comparison is of the addresses of the elements, not the
contained values.

.do end
.el .do begin

Each list element is compared to the passed element using
.MONO Type's operator ==
to determine if the passed element is contained in the list.

.if &lpref eq Ptr .do begin
Note that the comparison is of the objects pointed to.
.do end

.* end if not Isv
.do end
:RSLTS.
Zero(0) is returned
if the passed list element object is not found in the list.
A non-zero result is returned if the element is found in the list.
:SALSO.
:SAL typ='fun'.find
:SAL typ='fun'.index
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.entries
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:MFN index='entries'  .int entries() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine the number of list elements contained
in the list object.
:RSLTS.
The number of entries stored in the list is returned, zero(0) is returned
if there are no list elements.
:SALSO.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.find
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:MFN index='find'     .&lret. find( int = 0 ) const;
:eSNPL.
:SMTICS.
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to a list element in the list object.
The list element is not removed from the list, so care must be
taken not to delete the element returned to you.
.do end
.el .do begin
The &fn. returns the value of a list element in the list object.
.do end
The optional parameter specifies which element to locate, and defaults
to the first element.
Since the first element of the list is the zero'th element, the last
element will be the number of list entries minus one.
:P.
If the list is empty and the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, the exception is thrown.
If the
.MONO index_range
exception is enabled,
the exception is thrown if the index value is negative or
is greater than the number of list entries minus one.
:RSLTS.
.if &lpref. eq Isv .th .do begin

A pointer to the selected list element or the closest list element is returned.
If the index value is negative, the closest list element is the first element.
The last element is the closest element if the index value is greater than the
number of list entries minus one.
A value of NULL(0) is returned if there are no elements in the list.

.do end
.el .do begin

The value of the selected list element or the closest element is returned.
If the index value is negative, the closest list element is the first element.
The last element is the closest element if the index value is greater than the
number of list entries minus one.

.if &lpref eq Val .do begin
A default initialized value is returned if there are no elements in the list.
.do end
.el .do begin
An uninitialized pointer is returned if there are no elements in the list.
.do end

.* end if not isv
.do end
:SALSO.
:SAL typ='fun'.findLast
:SAL typ='fun'.get
:SAL typ='fun'.index
:SAL typ='fun'.isEmpty
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.findLast
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:MFN index='findLast' .&lret. findLast() const;
:eSNPL.
:SMTICS.
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to the last list element in the list object.
The list element is not removed from the list, so care must be
taken not to delete the element returned to you.
.do end
.el .do begin
The &fn. returns the value of the last list element in the list object.
.do end
:P.
If the list is empty, one of two exceptions can be thrown.
If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, it is thrown.
The
.MONO index_range
.ix 'index_range' 'exception'
exception is thrown if it is enabled and the
.MONO empty_container
exception is not enabled.
:RSLTS.
.if &lpref. eq Isv .th .do begin
A pointer to the last list element is returned.
A value of NULL(0) is returned if there are no elements in the list.
.do end
.el .do begin
The value of the last list element is returned.

.if &lpref eq Val .do begin
A default initialized value is returned if there are no elements in the list.
.do end
.el .do begin
An uninitialized pointer is returned if there are no elements in the list.
.do end

.do end
:SALSO.
:SAL typ='fun'.find
:SAL typ='fun'.get
:SAL typ='fun'.isEmpty
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.forAll
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
.if &lpref. eq Isv .do begin
:MFN index='forAll'   .void forAll( void (*fn)( &lret., void * ), void *);
.do end
.el .do begin
:MFN index='forAll'   .void forAll( void (*)( &lret., void * ), void *) const;
.do end
:eSNPL.
:SMTICS.
The &fn. is used to cause the function :HP1.fn:eHP1. to be invoked
for each list element.
The :HP1.fn:eHP1. function should have the prototype
:XMPL.
void (*fn)( &lret., void * )
:eXMPL.
:P.
The first parameter of :HP1.fn:eHP1. shall accept
.if &lpref. eq Isv .th .do begin
a pointer to
.do end
.el .do begin
the value of
.do end
the list element currently active.
The second argument passed to :HP1.fn:eHP1. is the second argument of
the
.MONO forAll
function.
This allows a callback function to be defined which can accept
data appropriate for the point at which the
.MONO forAll
function is invoked.
:SALSO.
:SAL typ='cls'.WC&lpref.ConstSListIter
:SAL typ='cls'.WC&lpref.ConstDListIter
:SAL typ='cls'.WC&lpref.SListIter
:SAL typ='cls'.WC&lpref.DListIter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:MFN index='get'      .&lret. get( int = 0 );
:eSNPL.
:SMTICS.

.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to a list element in the list object.
.do end
.el .do begin
The &fn. returns the value of the list element in the list object.
.do end

The list element is also removed from the list.
The optional parameter specifies which element to remove, and defaults
to the first element.
Since the first element of the list is the zero'th element, the last
element will be the number of list entries minus one.
:P.
If the list is empty and the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, the exception is thrown.
If the
.MONO index_range
exception trap is enabled,
the exception is thrown if the index value is negative or
is greater than the number of list entries minus one.
:RSLTS.
.if &lpref. eq Isv .th .do begin

A pointer to the selected list element or the closest list element is removed
and returned.
If the index value is negative, the closest list element is the first element.
The last element is the closest element if the index value is greater than the
number of list entries minus one.
A value of NULL(0) is returned if there are no elements in the list.

.do end
.el .do begin

The value of the selected list element or the closest element is removed
and returned.
If the index value is negative, the closest list element is the first element.
The last element is the closest element if the index value is greater than the
number of list entries minus one.

.if &lpref eq Val .do begin
A default initialized value is returned if there are no elements in the list.
.do end
.el .do begin
An uninitialized pointer is returned if there are no elements in the list.
.do end

.* end if not isv
.do end
:SALSO.
:SAL typ='fun'.clear
:SAL typ='fun'.clearAndDestroy
:SAL typ='fun'.find
:SAL typ='fun'.index
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.index
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:SNPF index='index'  .int index( const &lparm. ) const;
:eSNPL.
:SMTICS.
The &fn. is used to determine the index of the first list element equivalent to
the passed element.

.if &lpref. eq Isv .th .do begin
The address of (a pointer to) the list element object should be passed,
not the value.
Each list element is compared to the passed element object to determine
if it has the same address.
Note that the comparison is of the addresses of the elements, not the
contained values.

.do end
.el .do begin

Each list element is compared to the passed element using
.MONO Type's operator ==
until the passed element is found, or all list elements have been checked.

.if &lpref eq Ptr .do begin
Note that the comparison is of the objects pointed to.
.do end

.* end if not Isv
.do end
:RSLTS.
The index of the first element equivalent to the passed element is returned.
If the passed element is not in the list, negative one (-1) is returned.
:SALSO.
:SAL typ='fun'.contains
:SAL typ='fun'.find
:SAL typ='fun'.get
:eSALSO.
:eLIBF.

.if &lpref. eq Isv .do begin
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.index
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:SNPF index='index'   .int index( int (*test_fn)( const Type *, void * ),
:SNPFLF               .           void * ) const;
:eSNPL.
:SMTICS.
The &fn. is used to determine the index of the first list element for which
the supplied :HP1.test_fn:eHP1. function returns true.  The :HP1.test_fn:eHP1.
function must have the prototype:
:XMPL.
int (*test_fn)( const Type *, void * );
:eXMPL.
:P.
Each list element is passed in turn to the :HP1.test_fn:eHP1. function
as the first argument.  The second parameter passed is the second argument
of the
.MONO index
function.  This allows the :HP1.test_fn:eHP1. callback function to
accept data appropriate for the point at which the
.MONO index
function is invoked.
The supplied :HP1.test_fn:eHP1. shall return a TRUE (non-zero) value when the
index of the passed element is desired.
Otherwise, a FALSE (zero) value shall be returned.
:RSLTS.
The index of the first list element for which the :HP1.test_fn:eHP1.
function returns non-zero is returned.
If the :HP1.test_fn:eHP1. function returns zero for all list elements,
negative one (-1) is returned.
:SALSO.
:SAL typ='fun'.contains
:SAL typ='fun'.find
:SAL typ='fun'.get
:eSALSO.
:eLIBF.
.do end

:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
.if &lpref. eq Val .do begin
:SNPF index='insert'    .int insert( const &lparm. );
.do end
.el .do begin
:SNPF index='insert'    .int insert( &lparm. );
.do end
:eSNPL.
:SMTICS.
.if &lpref. ne Isv .th .do begin

The &fn. is used to insert the data
as the first element of the list.

.if &lpref. eq Val .do begin
The data stored in the list is a copy of the data passed as a parameter.
.do end

:P.
If the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception is enabled and the insert fails, the exception is thrown.
:RSLTS.
The data element is inserted into the beginning of the list.  A TRUE value
(non-zero) is returned if the insert is successful.  A FALSE (zero)
result is returned if the insert fails.

.do end
.el .do begin

The &fn. is used to insert the list element
object to the beginning of the list.
The address of (a pointer to) the list element object should be passed,
not the value.
Since the linkage information is stored in the list element, it is not
possible for the element to be in more than one list, or in the same list
more than once.
:P.
The passed list element should be constructed using
the appropriate link class as a base.
.MONO WCSLink
must be used as a list element base class for single linked lists,
and
.MONO WCDLink
must be used as a list element base class for double linked lists.
:RSLTS.
The list element is inserted as the first element of the list and a TRUE value
(non-zero) is returned.

.do end
:SALSO.
:SAL typ='fun'.append
.if &lpref ne Isv .do begin
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
.do end
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.isEmpty
:SNPL.
:SNPFLF               .#include <wclist.h>
:SNPFLF               .public:
:MFN index='isEmpty'  .int isEmpty() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if a list object has any list elements
contained in it.
:RSLTS.
A TRUE value (non-zero) is returned
if the list object does not have any list elements contained within it.
A FALSE (zero) result is returned if the list contains at least one element.
:SALSO.
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='&lacc.'.operator =
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .&lacc.:
:SNPF index='operator ='.WC&lpref.SList & WC&lpref.SList::operator =( const WC&lpref.SList & );
:SNPF index='operator ='.WC&lpref.DList & WC&lpref.DList::operator =( const WC&lpref.DList & );
:eSNPL.
:SMTICS.
The &fn. is the assignment operator for the class.
.if &lpref. eq Isv .th .do begin

Since making a copy of the list object would result in a error condition,
it is made inaccessible by making it a private operator.

.do end
.el .do begin

The left hand side of the assignment is first cleared with the
.MONO clear
member function.  All elements in the right hand side list are then
copied, as well as the exception
trap states, and any registered :HP1.allocator:eHP1. and :HP1.deallocator:eHP1.
functions.

.if &lpref. eq Val .do begin
.MONO Type's
copy constructor is invoked to copy the values contained by the list
elements.
.do end
.* end if Val

:P.
If all of the elements cannot be copied and the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
is enabled in the right hand side list, the exception is thrown.
The new list is created in a valid state, even if all of the list elements
could not be copied.
:RSLTS.
The &fn. assigns the right hand side to the left hand side and returns
a reference to the left hand side.
:SALSO.
:SAL typ='fun'.WC&lpref.SList
:SAL typ='fun'.WC&lpref.DList
:SAL typ='fun'.clear
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.

.do end
.* if not Isv
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator ==
:SNPL.
:SNPFLF          .#include <wclist.h>
:SNPFLF          .public:
:SNPF index='operator =='.int WC&lpref.SList::operator ==( const WC&lpref.SList & ) const;
:SNPF index='operator =='.int WC&lpref.DList::operator ==( const WC&lpref.DList & ) const;
:eSNPL.
:SMTICS.
The &fn. is the equivalence operator for the &cls.:PERIOD.
Two list objects are equivalent if they are the same object and share the
same address.
:RSLTS.
A TRUE (non-zero) value is returned if the left hand side object and the right
hand side objects are the same object.  A FALSE (zero) value is returned
otherwise.
:eLIBF.
