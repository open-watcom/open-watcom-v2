.DM ITERATOR_UNDEFINED BEGIN
If the iterator is not associated with a list, or the iterator
position is either before the first element or past the last element in
the list, the current iterator position is undefined.
.DM ITERATOR_UNDEFINED END
.*
:CLFNM cl2='WC&lpref.&lconst.DListIter<Type>'.WC&lpref.&lconst.SListIter<Type>
:CMT.========================================================================
:LIBF fmt='hdr'.WC&lpref.&lconst.SListIter<Type>, WC&lpref.&lconst.DListIter<Type>
:HFILE.wclistit.h
:CLSS.
.if '&lconst.' eq Const .th .do begin
The &cls. are the templated classes used to create iterator objects
for constant single and double linked list objects.  These classes may be
used to iterate over non-constant lists, but the
.MONO WC&lpref.DListIter<Type>
and
.MONO WC&lpref.SListIter<Type>
classes provide additional functionality for only non-constant lists.
.do end
.el .do begin
The &cls. are the templated classes used to create iterator objects
for single and double linked list objects.  These classes can be used only
for non-constant lists.  The
.MONO WC&lpref.DConstListIter<Type>
and
.MONO WC&lpref.SConstListIter<Type>
classes are provided to iterate over constant lists.
.do end
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the list element type specified as the template parameter.
:P.
:CMT. state WCIterExcept as base class
:INCLUDE file='_IEXT_BC'.
:HDG.Private Member Functions
.if '&lconst.' eq Const .do begin
Some functionality supported by base classes of the iterator are
not appropriate for the constant list iterator classes.
Setting those functions as private members in the derived class
is the standard mechanism
to prevent them from being invoked.
:MFNL.
:MFN index='append'         .int append( &lparm. );
:MFN index='insert'         .int insert( &lparm. );
:eMFNL.
.do end
.el .do begin
Some functionality supported by base classes of the iterator are
not appropriate in the single linked list iterator classes.
Setting those functions as private members in the derived class
is the standard mechanism
to prevent them from being invoked.
The following member functions are declared in the single
linked list iterator private interface:
:MFNL.
:MFN index='operator --'     .&lret. operator --();
:MFN index='operator -='     .&lret. operator -=( int );
:MFN index='insert'         .int insert( &lparm. );
:eMFNL.
.do end
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c' .WC&lpref.&lconst.SListIter();
:MFCD cd_idx='c' .WC&lpref.&lconst.SListIter( &lcparam.WC&lpref.SList<Type> & );
:MFCD cd_idx='d' .~~WC&lpref.&lconst.SListIter();
:MFCD cd_idx='c' .WC&lpref.&lconst.DListIter();
:MFCD cd_idx='c' .WC&lpref.&lconst.DListIter( &lcparam.WC&lpref.DList<Type> & );
:MFCD cd_idx='d' .~~WC&lpref.&lconst.DListIter();
.if '&lconst.' ne Const .do begin
:MFN index='append'         .int append( &lparm. );
.do end
:MFN index='container'      .&lcparam.WC&lpref.SList<Type> *WC&lpref.&lconst.SListIter<Type>::container() const;
:MFN index='container'      .&lcparam.WC&lpref.DList<Type> *WC&lpref.&lconst.DListIter<Type>::container() const;
.if &lpref. eq Val .do begin
:MFN index='current'        .Type current() const;
.do end
.el .do begin
:MFN index='current'        .Type * current() const;
.do end
:MFN index='reset'          .void reset();
:MFN index='reset'          .void WC&lpref.&lconst.SListIter<Type>::reset( &lcparam.WC&lpref.SList<Type> & );
:MFN index='reset'          .void WC&lpref.&lconst.DListIter<Type>::reset( &lcparam.WC&lpref.DList<Type> & );
.if '&lconst.' ne Const .do begin
.sk 1
In the iterators for double linked lists only:
.sk
:MFN index='insert'         .int insert( &lparm. );
.do end
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
:MFN index='operator ()'     .&lret. operator ()();
:MFN index='operator ++'     .&lret. operator ++();
:MFN index='operator +='     .&lret. operator +=( int );
.sk 1
In the iterators for double linked lists only:
.sk
:MFN index='operator --'     .&lret. operator --();
:MFN index='operator -='     .&lret. operator -=( int );
:eMFNL.
:SALSO.
:SAL typ='omtyp' ocls='WC&lpref.SList'.forAll
:SAL typ='omtyp' ocls='WC&lpref.DList'.forAll
:eSALSO.

.if &lpref. eq Val and '&lconst.' ne Const .do begin
:HDG.Sample Program Using Value List Iterators
:XMPL.
:INCLUDE file='wcvdli.cpp'.
:eXMPL.
.do end
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.SListIter<Type>' fmt='mfun' prot='public'.WC&lpref.&lconst.SListIter
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.&lconst.SListIter'.WC&lpref.&lconst.SListIter();
:eSNPL.
:SMTICS.
The &fn. is the default constructor for the class and initializes the
iterator with no list to operate on.  The
.MONO reset
member function must be called to provide the iterator with a list to
iterate over.
:RSLTS.
The &fn. creates an initialized
.MONO WC&lpref.&lconst.SListIter
object.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.SListIter
:SAL typ='fun'.~~WC&lpref.&lconst.SListIter
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.SListIter<Type>' fmt='mfun' prot='public'.WC&lpref.&lconst.SListIter
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.&lconst.SListIter'.WC&lpref.&lconst.SListIter( &lcparam.WC&lpref.SList<Type> & );
:eSNPL.
:SMTICS.
The &fn. is a constructor for the class.
The value passed as a parameter is a
.MONO WC&lpref.SList
list object.
The iterator will be initialized for that list object and positioned before the
first list element.
To position the iterator to a valid element within the list, increment
it using any of the
.MONO operator ++,
.MONO operator (),
or
.MONO operator +=
operators.
:RSLTS.
The &fn. creates an initialized
.MONO WC&lpref.&lconst.SListIter
object positioned before the first element in the list.
:SALSO.
:SAL typ='fun'.~~WC&lpref.&lconst.SListIter
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.SListIter<Type>' fmt='mfun' prot='public'.~~WC&lpref.&lconst.SListIter
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='~~WC&lpref.&lconst.SListIter'.~~WC&lpref.&lconst.SListIter();
:eSNPL.
:SMTICS.
The &fn. is the destructor for the class.
.se lobjtype=WC&lpref.&lconst.SListIter
:INCLUDE file='_DTOR2'.
:RSLTS.
The
.MONO &lobjtype.
object is destroyed.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.SListIter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.DListIter<Type>' fmt='mfun' prot='public'.WC&lpref.&lconst.DListIter
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.&lconst.DListIter'.WC&lpref.&lconst.DListIter();
:eSNPL.
:SMTICS.
The &fn. is the default constructor for the class and initializes the
iterator with no list to operate on.  The
.MONO reset
member function must be called to provide the iterator with a list to
iterate over.
:RSLTS.
The &fn. creates an initialized
.MONO WC&lpref.&lconst.DListIter
object.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.DListIter
:SAL typ='fun'.~~WC&lpref.&lconst.DListIter
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.DListIter<Type>' fmt='mfun' prot='public'.WC&lpref.&lconst.DListIter
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='WC&lpref.&lconst.DListIter'.WC&lpref.&lconst.DListIter( &lcparam.WC&lpref.DList<Type> & );
:eSNPL.
:SMTICS.
The &fn. is a constructor for the class.
The value passed as a parameter is the
.MONO WC&lpref.DList
list object.
The iterator will be initialized for that list object and positioned before the
first list element.
To position the iterator to a valid element within the list, increment
it using any of the
.MONO operator ++,
.MONO operator (),
or
.MONO operator +=
operators.
:RSLTS.
The &fn. creates an initialized
.MONO WC&lpref.&lconst.DListIter
object positioned before the first list element.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.DListIter
:SAL typ='fun'.~~WC&lpref.&lconst.DListIter
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.DListIter<Type>' fmt='mfun' prot='public'.~~WC&lpref.&lconst.DListIter
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='~~WC&lpref.&lconst.DListIter'.~~WC&lpref.&lconst.DListIter();
:eSNPL.
:SMTICS.
The &fn. is the destructor for the class.
.se lobjtype=WC&lpref.&lconst.DListIter
:INCLUDE file='_DTOR2'.
:RSLTS.
The
.MONO &lobjtype.
object is destroyed.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.DListIter
:eSALSO.
:eLIBF.
.if '&lconst.' ne Const .do begin
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.append
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='append'.int append( &lparm. );
:eSNPL.
:SMTICS.
The &fn. inserts a new element into the list container object.
The new element is inserted after the current iterator item.
:P.
.ITERATOR_UNDEFINED
The element is not appended.  If the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is enabled, it is thrown.
.*
.if &lpref. ne Isv .th .do begin
:P.
If the append fails, the
.MONO out_of_memory
.ix 'out_of_memory'
exception is thrown, if enabled in the list being iterated over.
The list remains unchanged.
.do end
.*
:RSLTS.
The new element is inserted after the current iterator item.  A TRUE value
(non-zero) is returned if the append is successful.  A FALSE (zero) result
is returned if the append fails.
:SALSO.
:SAL typ='fun'.insert
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
.do end
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.container
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='container'.&lcparam.WC&lpref.SList<Type> *WC&lpref.&lconst.SListIter<Type>::container() const;
:SNPF index='container'.&lcparam.WC&lpref.DList<Type> *WC&lpref.&lconst.DListIter<Type>::container() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the list container object.
If the iterator has not been initialized with a list object, and the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is enabled, the exception is thrown.
:RSLTS.
A pointer to the list object associated with the iterator is returned,
or NULL(0) if the iterator has not been initialized with a list.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.SListIter
:SAL typ='fun'.WC&lpref.&lconst.DListIter
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.current
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
.if &lpref. eq Val .th .do begin
:SNPF index='current'.Type current();
.do end
.el .do begin
:SNPF index='current'.Type * current();
.do end
:eSNPL.
:SMTICS.
.if &lpref. eq Val .th .do begin
The &fn. returns the value of the list element at the current iterator
position.
.do end
.el .do begin
The &fn. returns a pointer to the list item at the current iterator
position.
.do end
:P.
.ITERATOR_UNDEFINED
In this case the
.MONO undef_item
.ix 'undef_item'
exception is thrown, if enabled.
:RSLTS.
.if &lpref. eq Val .th .do begin
The value at the current iterator element is returned.
If the current element is undefined, a default initialized object is returned.
.do end
.el .do begin
A pointer to the current list element is returned.
If the current element is undefined,
:CMT. error returns for Isv and Ptr list iterators
. .if &lpref. eq Isv .th .do begin
     NULL(0) is returned.
. .do end
. .el .do begin
     an uninitialized pointer is returned.
. .do end
.do end
:SALSO.
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.operator~b--
:SAL typ='fun'.operator~b-=
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_item
:eSALSO.
:eLIBF.
.if '&lconst.' ne Const .do begin
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.DListIter<Type>' fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='insert'.int insert( &lparm. );
:eSNPL.
:SMTICS.
The &fn. inserts a new element into the list container object.
The new element is inserted before the current iterator item.
This process uses the previous link in the double linked list,
so the &fn. is not allowed with single linked lists.
:P.
.ITERATOR_UNDEFINED
The element is not inserted.  If the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is enabled, the exception is thrown.
.*
.if &lpref. ne Isv .th .do begin
:P.
If the insert fails and the
.MONO out_of_memory
.ix 'out_of_memory'
exception is enabled in the list being iterated over, the exception is
thrown.
The list remains unchanged.
.do end
.*
:RSLTS.
The new element is inserted before the current iterator item.  A TRUE value
(non-zero) is returned if the insert is successful.  A FALSE (zero) result
is returned if the insert fails.
:SALSO.
:SAL typ='fun'.append
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
.do end
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator ()
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='operator ()'.&lret. operator ()();
:eSNPL.
:SMTICS.
The &fn. is the call operator for the class.
The list element which follows the current item is set to be the new
current item.
If the previous current item was the last element in the list, the
iterator is positioned after the end of the list.
:P.
The &fn. has the same semantics as the pre-increment operator,
.MONO operator ++.
:P.
If the iterator was positioned before the first element in the list,
the current item will be set to the first element in the list.
If the list is empty, the iterator will be positioned after the end of
the list.
:P.
If the iterator is not associated with a list or the iterator
position before the increment was past the last element the list, the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is thrown, if enabled.
:RSLTS
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to the new current item.
NULL(0) is returned when the iterator is incremented past the end of the list.
.do end
.el .do begin
The &fn. returns a non-zero value if the iterator is positioned on a
list item.
Zero(0) is returned when the iterator is incremented past the end of the list.
.do end
:SALSO.
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.operator~b--
:SAL typ='fun'.operator~b-=
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator ++
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='operator ++'.&lret. operator ++();
:eSNPL.
:SMTICS.
The &fn. is the pre-increment operator for the class.
The list element which follows the current item is set to be the new
current item.
If the previous current item was the last element in the list, the
iterator is positioned after the end of the list.
:P.
The &fn. has the same semantics as the call operator,
.MONO operator ().
:P.
If the iterator was positioned before the first element in the list,
the current item will be set to the first element in the list.
If the list is empty, the iterator will be positioned after the end of
the list.
:P.
If the iterator is not associated with a list or the iterator
position before the increment was past the last element the list, the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is thrown, if enabled.
:RSLTS
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to the new current item.
NULL(0) is returned when the iterator is incremented past the end of the list.
.do end
.el .do begin
The &fn. returns a non-zero value if the iterator is positioned on a
list item.
Zero(0) is returned when the iterator is incremented past the end of the list.
.do end
:SALSO.
:SAL typ='fun'.current
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.operator~b--
:SAL typ='fun'.operator~b-=
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator +=
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='operator +='.&lret. operator +=( int );
:eSNPL.
:SMTICS.
The &fn. accepts an integer value that causes the iterator
to move that many elements after the current item.
If the iterator was positioned before the first element in the list, the
operation will set the current item to be the given element in the list.
:P.
If the current item was after the last element in the list previous to
the iteration, and the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is enabled, the exception will be thrown.
Attempting to increment the iterator
position more than element after the end of the list, or by less than one
element causes the
.MONO iter_range
.ix 'iter_range' 'exception'
exception to be thrown, if enabled.
:RSLTS
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to the new current item.
NULL(0) is returned when the iterator is incremented past the end of the list.
.do end
.el .do begin
The &fn. returns a non-zero value if the iterator is positioned on a
list item.
Zero(0) is returned when the iterator is incremented past the end of the list.
.do end
:SALSO.
:SAL typ='fun'.current
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b--
:SAL typ='fun'.operator~b-=
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.iter_range
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.DListIter<Type>' fmt='mfun' prot='public'.operator --
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='operator --'.&lret. operator --();
:eSNPL.
:SMTICS.
The &fn. is the pre-decrement operator for the class.
The list element previous to the current item is set to be the
new current item.
If the current item was the first element in the list,
the iterator is positioned before the first element in the list.
If the list is empty, the iterator will be positioned before the start
of the list.
:P.
If the iterator was positioned after the last element in the list,
the current item will be set to the last element.
:P.
If the iterator is not associated with a list or the iterator
position previous to the decrement was before the first element the list, the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is thrown, if enabled.
:RSLTS
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to the new current item.
NULL(0) is returned
.do end
.el .do begin
The &fn. returns a non-zero value if the iterator is positioned on a
list item.
Zero(0) is returned
.do end
when the iterator is decremented past the first element of the list.
:SALSO.
:SAL typ='fun'.current
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.operator~b-=
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='WC&lpref.&lconst.DListIter<Type>' fmt='mfun' prot='public'.operator -=
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='operator -='.&lret. operator -=( int );
:eSNPL.
:SMTICS.
The &fn. accepts an integer value that causes the iterator
to move that many elements before the current item.
If the iterator was positioned after the last element in the list, the
operation will set the current item to be the given number of elements from
the end of the list.
:P.
If the current item was before the first element in the list previous to
the iteration, and the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is enabled, the exception will be thrown.
Attempting to decrement the iterator position more than one element before
the beginning of the list, or by less than one element causes the
.MONO iter_range
.ix 'iter_range' 'exception'
exception to be thrown, if enabled.
:RSLTS
.if &lpref. eq Isv .th .do begin
The &fn. returns a pointer to the new current item.
NULL(0) is returned
.do end
.el .do begin
The &fn. returns a non-zero value if the iterator is positioned on a
list item.
Zero(0) is returned
.do end
when the iterator is decremented past the first element in the list.
:SALSO.
:SAL typ='fun'.current
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.operator~b+=
:SAL typ='fun'.operator~b--
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.iter_range
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.reset
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='reset'    .void reset();
:eSNPL.
:SMTICS.
The &fn. resets the iterator to the initial state, positioning the iterator
before the first element in the associated list.
:RSLTS
The iterator is positioned before the first list element.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.SListIter
:SAL typ='fun'.WC&lpref.&lconst.DListIter
:SAL typ='fun'.container
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.reset
:SNPL.
:SNPFLF          .#include <wclistit.h>
:SNPFLF          .public:
:SNPF index='reset'    .void WC&lpref.&lconst.SListIter<Type>::reset( &lcparam.WC&lpref.SList<Type> & );
:SNPF index='reset'    .void WC&lpref.&lconst.DListIter<Type>::reset( &lcparam.WC&lpref.DList<Type> & );
:eSNPL.
:SMTICS.
The &fn. resets the iterator to operate on the specified list.
The iterator is positioned before the first element in the list.
:RSLTS
The iterator is positioned before the first element of the specified list.
:SALSO.
:SAL typ='fun'.WC&lpref.&lconst.SListIter
:SAL typ='fun'.WC&lpref.&lconst.DListIter
:SAL typ='fun'.container
:eSALSO.
:eLIBF.
.*
.DM ITERATOR_UNDEF DELETE
