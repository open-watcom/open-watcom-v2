:P.
List containers are single or double linked lists.
The choice of which type of list to use is determined
by the direction in which the list is traversed and by what is
stored in the list.
A list to which items are just added and removed may be
most efficiently implemented as a single linked list.
If frequent retrievals of items at given indexes within the
list are made, double linked lists can offer some improved
search performance.
:P.
There are three sets of list container classes: value, pointer and intrusive.
:P.
Value lists are the simplest to use but have the most requirements on the
.ix 'value' 'lists'
type stored in the lists.
Copies are made of the values stored in the list, which could be undesirable
if the stored objects are complicated and copying is expensive.
Value lists should not be used to store objects of a base class if any
derived types of different sizes would be stored in the list, or if the
destructor for the derived class must be called.
The
.MONO WCValSList<Type>
container class implements single linked value lists, and the
.MONO WCValDList<Type>
class double linked value lists.
:P.
Pointer list elements store pointers to objects.  No creating, copying or
.ix 'pointer' 'lists'
destroying of objects stored in the list occurs.
The only requirement of the type pointed to is that an
equivalence operator is provided so that lookups can be performed.  The
.MONO WCPtrSList<Type>
class implements single linked pointer lists, and the
.MONO WCPtrDList<Type>
class double linked pointer lists.
:P.
Intrusive lists require that the list elements are objects derived from
the
.MONO WCSLink
or
.MONO WCDLink
class, depending on whether a single or double linked list is used.
The list classes require nothing else from the list elements.
No creating, destroying or copying of any object is performed by the
intrusive list classes, and must be done
by the user of the class.  One advantage of an intrusive list is a
list element can be removed from one list and inserted into another list
without creating new list element objects or deleting old objects.
The
.MONO WCIsvSList<Type>
class implements single linked intrusive lists, and the
.MONO WCIsvDList<Type>
class double linked intrusive lists.
:P.
A list may be traversed using the corresponding list iterator class.
Iterators allow lists to be stepped through one or more elements at a time.
The iterator classes which correspond to single linked list containers
have some functionality inhibited.
If backward traversal is required, the double linked containers and iterators
must be used.
:P.
The classes are presented in alphabetical order.
The
.MONO WCSLink
and
.MONO WCDLink
class provide a common control interface for the list elements for the
intrusive classes.
:P.
Since the container classes are all template classes, deriving most
of the functionality from common base classes was used.
In the listing of class member functions, those public member functions
which appear to be in the container class but are actually defined in
the common base class are identified as if they were explicitly specified
in the container class.
