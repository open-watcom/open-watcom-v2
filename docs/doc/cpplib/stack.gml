:P.
Stack containers maintain an ordered collection of data which
is retrieved in the reverse order to which the data was entered into the stack.
The stack class is implemented as a templated class, allowing the
stacking of any data type.
:P.
A second template parameter specifies the storage class used to implement the
stack.  The
.MONO WCValSList,
.MONO WCIsvSList
and
.MONO WCPtrSList
classes are appropriate storage classes.
.*
:CLFNM.WCStack<Type,FType>
:CMT.========================================================================
:LIBF fmt='hdr'.WCStack<Type,FType>
:HFILE.wcstack.h
:CLSS.
The &cls. is a templated class used to create objects which
maintain data in a stack.
:P.
In the description of each member function, the text
.MONO Type
is used to indicate the template parameter defining the type of the
elements stored in the stack.
The text
.MONO FType
is used to indicate the template parameter defining the storage class
used to maintain the stack.
:P.
For example, to create a stack of integers, the
.MONO WCStack<int,WCValSList<int> >
class can be used.
The
.MONO WCStack<int *,WCPtrSList<int> >
class will create a stack of pointers to integers.
To create an intrusive stack of objects of type
:HP1.isv_link:eHP1.
(derived from the
.MONO WCSLink
class), the
.MONO WCStack<
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
:MFCD cd_idx='c' .WCStack();
:MFCD cd_idx='c' .WCStack( void *(*)( size_t ), void (*)( void *, size_t ) );
:MFCD cd_idx='d' .~~WCStack();
:MFN index='clear'          .void clear();
:MFN index='entries'        .int entries() const;
:MFN index='isEmpty'        .int isEmpty() const;
:MFN index='pop'            .Type pop();
:MFN index='push'           .int push( const Type & );
:MFN index='top'            .Type top() const;
:eMFNL.
:HDG.Sample Program Using a Stack
:XMPL.
:INCLUDE file='wcstack.cpp'
:eXMPL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WCStack
:SNPL.
:SNPFLF                    .#include <wcstack.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCStack();
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
.SE lobjtype='WCStack<Type,FType>'
:LIBF fmt='ctor' prot='public'.WCStack
:SNPL.
:SNPFLF                    .#include <wcstack.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCStack( void *(*allocator)( size_t ),
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
function is registered to perform all memory allocations of the stack
elements, and the :HP1.deallocator:eHP1.
function to perform all freeing of the stack elements' memory.
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
.MONO WCValSListItemSize(Type)
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
storage class constructor performs the initialization of the stack.
:RSLTS.
The &fn. creates an initialized &obj. and registers the :HP1.allocator:eHP1.
and :HP1.deallocator:eHP1. functions.
:SALSO.
:SAL typ='ctor'.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~WCStack
:SNPL.
:SNPFLF                   .#include <wcstack.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual ~~WCStack();
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
the exception is thrown if the stack is not empty of stack elements.
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
:SNPFLF               .#include <wcstack.h>
:SNPFLF               .public:
:SNPF index='clear'.void clear();
:eSNPL.
:SMTICS.
The &fn. is used to clear the stack object and set it to the state of the object
just after the initial construction.
The stack object is not destroyed and re-created by this operator, so
the object destructor is not invoked.
The stack elements are not cleared by the stack class.
However, the class used to maintain the stack,
.MONO FType,
may clear the items as part of the
.MONO clear
member function for that class.
If it does not clear the items,
any stack items still in the list are lost unless pointed to by some
pointer object in the program code.
:RSLTS.
The &fn. resets the stack object to the state of the object immediately
after the initial construction.
:SALSO.
:SAL typ='dtor'.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.entries
:SNPL.
:SNPFLF               .#include <wcstack.h>
:SNPFLF               .public:
:MFN index='entries'  .int entries() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine the number of stack elements contained
in the list object.
:RSLTS.
The number of elements on the stack is returned.
Zero(0) is returned if there are no stack elements.
:SALSO.
:SAL typ='fun'.isEmpty
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.isEmpty
:SNPL.
:SNPFLF               .#include <wcstack.h>
:SNPFLF               .public:
:MFN index='isEmpty'  .int isEmpty() const;
:eSNPL.
:SMTICS.
The &fn. is used to determine if a stack object has any stack elements
contained in it.
:RSLTS.
A TRUE value (non-zero) is returned
if the stack object does not have any stack elements contained within it.
A FALSE (zero) result is returned if the stack contains at least one element.
:SALSO.
:SAL typ='fun'.entries
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.pop
:SNPL.
:SNPFLF               .#include <wcstack.h>
:SNPFLF               .public:
:MFN index='pop'      .Type pop();
:eSNPL.
:SMTICS.
The &fn. returns the top stack element from the stack object.
The top stack element is the last element pushed onto the stack.
The stack element is also removed from the stack.
:P.
If the stack is empty, one of two exceptions can be thrown.
If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, then it will be thrown.
Otherwise, the
.MONO index_range
.ix 'index_range' 'exception'
exception will be thrown, if enabled.
:RSLTS.
The top stack element is removed and returned.
The return value is determined by the
.MONO get
member function of the
.MONO FType
class if there are no elements on the stack.
:SALSO.
:SAL typ='fun'.isEmpty
:SAL typ='fun'.push
:SAL typ='fun'.top
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='FType'.get
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.push
:SNPL.
:SNPFLF               .#include <wcstack.h>
:SNPFLF               .public:
:SNPF index='push'    .int push( const Type & );
:eSNPL.
:SMTICS.
The &fn. is used to push the data onto the top of the stack.
It will be the first element on the stack to be popped.
:P.
If the push fails, the
.MONO out_of_memory
.ix 'out_of_memory' 'exception'
exception will be thrown, if enabled, and the stack will remain unchanged.
:RSLTS.
The stack element is pushed onto the top of the stack.
A TRUE value (non-zero) is returned if the push is successful.
A FALSE (zero) result is returned if the push fails.
:SALSO.
:SAL typ='fun'.pop
:SAL typ='omtyp' ocls='WCExcept'.out_of_memory
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.top
:SNPL.
:SNPFLF               .#include <wcstack.h>
:SNPFLF               .public:
:MFN index='top'      .Type top() const;
:eSNPL.
:SMTICS.
The &fn. returns the top stack element from the stack object.
The top stack element is the last element pushed onto the stack.
The stack element is not removed from the stack.
:P.
If the stack is empty, one of two exceptions can be thrown.
If the
.MONO empty_container
.ix 'empty_container' 'exception'
exception is enabled, then it will be thrown.
Otherwise, the
.MONO index_range
.ix 'index_range' 'exception'
exception will be thrown, if enabled.
:RSLTS.
The top stack element is returned.
The return value is determined by the
.MONO find
member function of the
.MONO FType
class if there are no elements on the stack.
:SALSO.
:SAL typ='fun'.isEmpty
:SAL typ='fun'.pop
:SAL typ='omtyp' ocls='WCExcept'.empty_container
:SAL typ='omtyp' ocls='WCExcept'.index_range
:SAL typ='omtyp' ocls='FType'.find
:eSALSO.
:eLIBF.
