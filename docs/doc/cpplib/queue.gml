:P.
Queue containers maintain an ordered collection of data which
is retrieved in the order in which the data was entered into the queue.
The queue class is implemented as a templated class, allowing the
use of any data type as the queue data.
:P.
A second template parameter specifies the storage class used to implement the
queue.  The
.MONO WCValSList,
.MONO WCIsvSList
and
.MONO WCPtrSList
classes are appropriate storage classes.
.*
:CLFNM.WCQueue<Type,FType>
:CMT.========================================================================
:LIBF fmt='hdr'.WCQueue<Type,FType>
:HFILE.wcqueue.h
:CLSS.
The &cls. is a templated class used to create objects which
maintain data in a queue.
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the template parameter defining the type of the
elements stored in the queue.
The text
.MONO FType
is used to indicate the template parameter defining the storage class
used to maintain the queue.
:P.
For example, to create a queue of integers, the
.MONO WCQueue<int,WCValSList<int> >
class can be used.
The
.MONO WCQueue<int *,WCPtrSList<int> >
class will create a queue of pointers to integers.
To create an intrusive queue of objects of type
:HP1.isv_link:eHP1.
(derived from the
.MONO WCSLink
class), the
.MONO WCQueue<
:HP1.isv_link:eHP1.
.MONO *,WCIsvSList<
:HP1.isv_link:eHP1.
.MONO > >
class can be used.
:P.
:CMT. state WCExcept is base class
:INCLUDE file='_EXPT_BC'.
:HDG.Requirements of Type
.MONO Type
must provide any constructors and/or operators required by the
.MONO FType
class.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c' .WCQueue();
:MFCD cd_idx='c' .WCQueue( void *(*)( size_t ), void (*)( void *, size_t ) );
:MFCD cd_idx='d' .~~WCQueue();
:MFN index='clear'          .void clear();
:MFN index='entries'        .int entries() const;
:MFN index='first'          .Type first() const;
:MFN index='get'            .Type get();
:MFN index='insert'         .int insert( const Type & );
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='last'           .Type last() const;
:eMFNL.
:HDG.Sample Program Using a Queue
:XMPL.
:INCLUDE file='wcqueue.cpp'
:eXMPL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WCQueue
:SNPL.
:SNPFLF                    .#include <wcqueue.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCQueue();
:eSNPL.
:SMTICS.
The &fn. creates an empty &obj.:PERIOD.
The
.MONO FType
storage class constructor performs the initialization.
:RSLTS.
The &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:CMT. symbol for _ALOCFNS include file
.SE lobjtype='WCQueue<Type,FType>'
:LIBF fmt='ctor' prot='public'.WCQueue
:SNPL.
:SNPFLF                    .#include <wcqueue.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCQueue( void *(*allocator)( size_t ),
:SNPFLF          .         void (*deallocator)( void *, size_t ) );
:eSNPL.
:SMTICS.
The &fn. creates an empty &obj.:PERIOD.
If
.MONO FType
is either the
.MONO WCValSList
or
.MONO WCPtrSList
class, then the :HP1.allocator:eHP1.
function is registered to perform all memory allocations of the queue
elements, and the :HP1.deallocator:eHP1.
function to perform all freeing of the queue elements' memory.
The :HP1.allocator:eHP1. and :HP1.deallocator:eHP1. functions are ignored if
.MONO FType
is the
.MONO WCIsvSList
class.
:INCLUDE file='_ALOCFNS'.
:P.
The :HP1.allocator:eHP1. and :HP1.deallocator:eHP1. functions may assume that
for a list object instance, the
:HP1.allocator:eHP1. is always called with the same first argument
(the size of the memory to be allocated).  If
.MONO FType
is the
.MONO WCValSList<Type>
class, then the
.MONO WCValSListItemSize( Type )
.ix 'WCValSListItemSize' 'macro'
macro returns the size of the elements which are allocated by the
:HP1.allocator:eHP1. function.  Similarly, the
.MONO WCPtrSListItemSize( Type )
.ix 'WCPtrSListItemSize' 'macro'
macro returns the size of
.MONO WCPtrSList<Type>
elements.
:P.The
.MONO FType
storage class constructor performs the initialization of the queue.
:RSLTS.
The &fn. creates an initialized &obj. and registers the :HP1.allocator:eHP1.
and :HP1.deallocator:eHP1. functions.
:SALSO.
:SAL typ='ctor'.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~WCQueue
:SNPL.
:SNPFLF                   .#include <wcqueue.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual ~~WCQueue();
:eSNPL.
:SMTICS.
The &fn. destroys the &obj.:PERIOD
The
.MONO FType
storage class destructor performs the destruction.
:INCLUDE file='_DTOR'.
:P.
If the
.MONO not_empty
.ix 'not_empty' 'exception'
exception is enabled,
the exception is thrown if the queue is not empty of queue elements.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:SAL typ='fun'.clear
:SAL typ='omtyp' ocls='WCExcept'.not_empty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:SNPF index='clear'.void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the queue object and set it to the state of the object
just after the initial construction.
The queue object is not destroyed and re-created by this operator, so
the object destructor is not invoked.
The queue elements are not cleared by the queue class.
However, the class used to maintain the queue,
.MONO FType,
may clear the items as part of the clear function for that class.
If it does not clear the items,
any queue items still in the list are lost unless pointed to by some
pointer object in the program code.
:RSLTS.
The &fn. resets the queue object to the state of the object immediately
after the initial construction.
:SALSO.
:SAL typ='dtor'.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.entries
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:MFN index='entries'  .int entries() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine the number of queue elements contained
in the list object.
:RSLTS.
The number of elements in the queue is returned.
Zero(0) is returned if there are no queue elements.
:SALSO.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.first
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:MFN index='first'    .Type first() const;
:eSNPL.
:SMTICS.
The &fn. returns a queue element from the beginning of the queue object.
The queue element is not removed from the queue.
:P.
If the queue is empty, one of two exceptions can be thrown.
If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, then it will be thrown.
Otherwise, the
.MONO index_range
.ix 'index_range' 'exception'
exception will be thrown, if enabled.
:RSLTS.
The first queue element is returned.
If there are no elements in the queue, the return value is determined by the
.MONO find
member function of the
.MONO FType
class.
:SALSO.
:SAL typ='fun'.get
:SAL typ='fun'.isEmpty
:SAL typ='fun'.last
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='FType'.find
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:MFN index='get'      .Type get();
:eSNPL.
:SMTICS.
The &fn. returns the queue element which was first inserted into
the queue object.
The queue element is also removed from the queue.
:P.
If the queue is empty, one of two exceptions can be thrown.
If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, then it will be thrown.
Otherwise, the
.MONO index_range
.ix 'index_range' 'exception'
exception will be thrown, if enabled.
:RSLTS.
The first element in the queue is removed and returned.
If there are no elements in the queue, the return value is determined by the
.MONO get
member function of the
.MONO FType
class.
:SALSO.
:SAL typ='fun'.first
:SAL typ='fun'.insert
:SAL typ='fun'.isEmpty
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='FType'.get
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.insert
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:SNPF index='insert'  .int insert( const Type & );
:eSNPL.
:SMTICS.
The &fn. is used to insert the data into the queue.
It will be the last element in the queue, and the last to be retrieved.
:P.
If the insert fails, the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception will be thrown, if enabled.
The queue will remain unchanged.
:RSLTS.
The queue element is inserted at the end of the queue.
A TRUE value (non-zero) is returned if the insert is successful.
A FALSE (zero) result is returned if the insert fails.
:SALSO.
:SAL typ='fun'.get
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.isEmpty
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:MFN index='isEmpty'  .int isEmpty() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if a queue object has any queue elements
contained in it.
:RSLTS.
A TRUE value (non-zero) is returned
if the queue object does not have any queue elements contained within it.
A FALSE (zero) result is returned if the queue contains at least one element.
:SALSO.
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.last
:SNPL.
:SNPFLF               .#include <wcqueue.h>
:SNPFLF               .public:
:MFN index='last'     .Type last() const;
:eSNPL.
:SMTICS.
The &fn. returns a queue element from the end of the queue object.
The queue element is not removed from the queue.
:P.
If the queue is empty, one of two exceptions can be thrown.
If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, then it will be thrown.
Otherwise, the
.MONO index_range
.ix 'index_range' 'exception'
exception will be thrown, if enabled.
:RSLTS.
The last queue element is returned.
If there are no elements in the queue, the return value is determined by the
.MONO find
member function of the
.MONO FType
class.
:SALSO.
:SAL typ='fun'.first
:SAL typ='fun'.get
:SAL typ='fun'.isEmpty
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='FType'.find
:eSALSO.
:eLIBF.
