:H1.Introduction
:P.

:H2.Status
:P.
:AUTHOR date='3 Nov 2005'.P Chapin, D Cletheroe
:P.
Reviewer: Not reviewed
:P.
Missing members:
:UL.
:LI.err... need to look throught the standard
:LI.
:eUL.

Completed members:
:UL.
:LI.explicit list( Allocator const & )
:LI.list( list const & )
:LI.~list()
:LI operator=( list const & )
:LI.assign( size_type, value_type const & )
:LI.get_allocator() const
:LI.iterator
:LI.const_iterator
:LI.reverse_iterator
:LI.const_reverse_iterator
:LI.begin() (+const)
:LI.end() (+const)
:LI.rbegin() (+const)
:LI.rend() (+const)
:LI.size()
:LI.empty()
:LI.front()
:LI.back()
:LI.push_front( value_type const & )
:LI.push_back( value_type const & )
:LI.pop_front()
:LI.pop_back()
:LI.insert( iterator, value_type const & )
:LI.erase( iterator )
:LI.erase( iterator, iterator )
:LI.swap( list& )
:LI.clear()
:LI.remove( value_type const & )
:LI.splice( iterator, list & )
:LI.splice( iterator, list &, iterator )
:LI.splice( iterator list &, iterator, literator )
:LI.reverse()
:LI.merge( list const & )
:eUL.

:H1.Design Details
:P.
:CLASS.template < class Type, class Allocator > class std::~list

:H2.Description of a Double Linked List
:P.
This is a data structure that is made up of nodes, where each node
contains the data, a pointer to the next node, and a pointer to the
previous node. The overall structure also knows where the first element
in the list is and usually the last. Obviously it requires two pointers
for every piece of data held in the list, but this allows movement
between adjacent nodes in both directions in constant time.

:H2.Overview of the class
:P.
The class defines a internal DoubleLink structure that only holds
forward and backward pointers to itself. It then defines a Node
structure that inherits the DoubleLink and adds to it the space for the
real data (of type value_type) that is held in the list nodes. This is
done so a special sentinel object can be created that is related to
every other node in the list, but it doesn't require space for a
value_type object. This sentinel is used by the list class to point to
the first and last elements in the list. A sentinel is useful in this
case (the alternative would just be individual first and last pointers)
because it means the insertion and deletion code does not have to check
for the special case of editing the list at the begining and end. The
sentinel is initialised pointing to itself and is used as the reference
point of the end of the list. When the an element is inserted or deleted
before the end or at the begining all the pointer manipulation just
falls out in the wash. [This seems to be a good uses of sentinels, I
can't see the point of using them in a tree structure for example.]

:P.
There are two allocators that need to be rebound for the Node and
DoubleLink types. [For review: DJFC called the first one mMem where the
lower case m was for
:Q.member:eQ. then PCC needed to add another for
DoubleLink and called it dlMem, we could really do with standadising a
naming convention---perhaps mMem should be nMem or dlMem should be
mDLMem???] The two allocators are needed because objects of different
types are being allocated: the node allocation allocates nodes (with
their contained value_type) while the link allocator allocates the
sentinel node of type DoubleLink.



:H2.Inserting nodes
:P.
The work for the functions :FUNC.push_front, push_back
and
:FUNC.insert
is done by the private member :FUNC.push.
:FUNC.Node* push( Node* o, Type const & )
is quite simple.
It allocates a Node and then tries to make a copy of type in the memory
allocated.
The usual try-catch wrappings deallocate the memory if the construction
was unsuccessful.
It then modifies the pointers of the Node o that was passed, the element
before o, and the new Node so that the new node is linked in is now placed
just before o.
The end of the list is signified by the sentinal object, so
if we are trying to insert before the end o is sentinal and everything works.
If we are trying to insert before the first node the old node
before the first is again the sentinal, so the pointers are all valid
and everything works.


:H2.Deleting nodes
:P.

:H2.Clearing all
:P.

