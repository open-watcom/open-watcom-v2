:P.
List iterators operate on single or double linked lists.
They are used to step through a list one or more elements at
a time.
The choice of which type of iterator to use is determined
by the list you wish to iterate over.
For example, to iterate over a non-constant
.MONO WCIsvDList<Type>
object, use the
.MONO WCIsvDListIter<Type>
class.
A constant
.MONO WCValSList<Type>
object can be iterated using the
.MONO WCValConstSListIter<Type>
class.
The iterators which correspond to the single link list containers
have some functionality inhibited.
If backward traversal is required, the double linked containers
and corresponding iterators must be used.
:P.
Like all WATCOM iterators, newly constructed and reset iterators are
positioned before the first element in the list.
The list may be traversed one element at a time using the pre-increment
or call operator.
An increment operation causing the iterator to be positioned after the end
of the list returns zero.
Further increments will cause the
.MONO undef_iter
.ix 'undex_iter' 'exception'
exception to be thrown, if it is enabled.
This behaviour allows lists to be traversed simply using a while loop, and is
demonstrated in the examples for the iterator classes.
:P.
The classes are presented in alphabetical order.
The
.MONO WCIterExcept
class provides the common exception handling
control interface for all of the iterators.
:P.
Since the iterator classes are all template classes, deriving most
of the functionality from common base classes was used.
In the listing of class member functions, those public member functions
which appear to be in the iterator class but are actually defined in
the common base class are identified as if they were explicitly specified
in the iterator class.
