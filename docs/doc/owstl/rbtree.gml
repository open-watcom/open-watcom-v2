:H1.Introduction
Template class :CLASS.std::~_ow::~RedBlackTree<>
is an implementation of a red-black tree data structure.
It is used as a common base for :CLASS.std::~set
and :CLASS.std::~map.
It can be found in :PATH.hdr/watcom/_rbtree.mh.
The intention was to allow easy replacement and experementation with 
other implementations such as an AVL tree or perhaps some sort of 
relaxed chromatic tree suited to concurrent systems.


:H2.Status
:P.
:AUTHOR date='16 Oct 2005'.D Cletheroe
:P.
Reviewer: Not reviewed
:P.
The majority of the required functionality has been written.
Regression tests have been written in parallel, but little user testing and 
feedback exists.
:P.
The missing members are:
:CMT can list be modified so there isn't a skip or maby a half skip between 
:CMT each item: variable under the UL tag?
:UL.
:LI.reverse_iterator
:LI.const_reverse_iterator
:LI.template<InputIterator> ctor( InputIterator, InputIterator,... )
:LI.rend() and rend() const
:LI.rbegin() and rbegin() const
:LI.max_size()
:LI.erase( iterator first, iterator last )
:LI.swap( RBTree& )
:LI.key_comp()
:LI.value_comp()
:LI.find( key_type ) const
:LI.count()
:LI.equal_range( key_type ) and equal_range( key_type ) const
:LI.non member operators and specialized swap algorithm
:eUL.

Completed member are:
:UL.
:LI.iterator
:LI.const_iterator
:LI.ctor( Compare, Allocator )
:LI.cpyctor
:LI.operator=
:LI.dtor
:LI.begin() and begin() const
:LI.end() and end() const
:LI.empty()
:LI.size()
:LI.{ operator[] is implemented in class map not tree }
:LI.insert( value_type )
:LI.insert( iterator, value_type ) (see N1780)
:LI.erase( iterator )
:LI.erase( key_type cont &)
:LI.clear()
:LI.find( key_type )
:LI.lower_bound( key_type ) and lower_bound( key_type ) const
:LI.upper_bound( key_type ) and upper_bound( key_type ) const
:LI._Sane()
:LI.internal tree balancing functions
:eUL.


:H1.Design Details
:P.
:CLASS.template < class Key, class Compare, class Allocator, class ValueWrapper >
:CLASS.class RedBlackTree

:P.
:CLASS.Key
is the type that is used to index the tree;
:CLASS.Compare 
is a functor (class with operator() defined) that provides ordering to the keys;
:CLASS.Allocator
provides the memory allocation;
:CLASS.ValueWrapper
is a class that defines the type of the objects stored in the tree and provides
an operator() that knows how to extract the key from that type. 
:CLASS.ValueWrapper
allows the same tree code to apply to sets where the key is the only thing 
stored and maps where the object stored has a key and a mapped value.

:H2. Relation to map and set
:P.
:CLASS.std::~set 
and :CLASS.std::~map
take their base class as a template parameter.
They select the appropriate value wrapper and inherit all the functionallity.
The base currently defaults to RedBlackTree and is the only implementation 
avaliable.

:H2. Description of a Red Black Tree
:P.
A Red-Black tree is a ordered binary tree.
An binary tree is made up of nodes, where each node can have up to two children.
An ordered binary tree orders the nodes so that a left child is less than its 
parent and a right child is greater.
It could be the other way around, and this implementation uses a comparison
function and puts the child on the left if compare( child, parent ) evaluates true.
If a node has no children it is a leaf, otherwise it is an internal node.
Some implementations only hold the actual data in the leaves and the internal
nodes are just place holders.
This implemenation has imaginary leaves - null pointers.
If a node's child pointer is null then that non-existant child is a leaf, and we 
hold all the data in the real, exisiting nodes.
Therefore there is no special leaf node type, just a null pointer if there is no
child with data.

:P.
A Red-Black tree adds a colour to ever node, and defines some rules that
mean the tree stays balanced.
A tree is balanced if the difference between the largest and smallest depth of 
a leaf is bounded.
The invarients are:
:UL.
:LI.Every red node has a black parent
:LI.Every route from the root node to a node with 0 or 1 children has the same
number of black nodes
:LI.Every leaf is black (note this is assumed as leaves don't really exist in 
this implementation.)
:LI.The root is black
:eUL.

:P.
This data structure has been well covered in the literature, for a more detailed
information see:
:CMT do proper references with tags
[Prof Lyn Turba, Wellesly College, CS231 Algorithms Handout 21, 2001]
[McGill University, Notes for 308-251B, http://www.cs.mcgill.ca/~cs251/ !check link!, 1997]
[err...]
[google red black trees, binary trees etc]


:CMT -------------------------------------------------------------------------
:H2. Overview of the class
:P.
The tree class defines an internal Node structure that is made up of the object
stored in the tree, Node pointers for the parent and left and right children,
and the Node colour.
There is an allocator member object, mMem, that is rebound to allocate Node 
types.
There are pointers to the root and furthest left and right nodes.
These are used to mark were to start the search, and create the begin and
end iterators respectively.
The iterator and const_iterator are member classes derived from a common
member class.
There is an &company extention method 
:FUNC.bool _Sane()
that checks the integrity of the data structure.
Related to this is a integer mError member that is assigned a value if an 
error is detected when _Sane is run.
:CMT to do: make a 'for review' tag?
[for review] This should perhaps be renamed _Error or made private and 
a _GetError() method provided.

:H2.Inserting Elements and Reballancing
:P.
The :FUNC.insert
method calls :FUNC.unbalancedInsert
and :FUNC.insBalTransform.
The loop in :FUNC.unBallancedInsert 
moves from child to child searching for the leaf of the tree where
the new item can be inserted, 
in a simular way as the find algorithm checks for the item.
A final check is made at the end of the loop to see if the key already
exists.
If it does an iterator to the existing key is returned.
Otherwise a new node is allocated and constructed.
The Node is linked into the tree at the place found.
A try-catch construction is placed around the construction of the node
to deallocate the node again if any exceptions are raised.
This is needed to stop a memory leak that could occur beacuse the memory
has been allocated but the exception has stopped the Node being linked in
to the tree (so it would never get destroyed when the tree is destoyed).

:P.
At this point the tree is a valid binary tree but not necessaraly obeying
the Red-Black balance criteria.
The new Node is painted Red so as not to invalidate the black-height
rule, but this may introduce a violation of the red-red rule.
:FUNC.insBalTransform
is called with a pointer to the newly inserted Node to correct this.
This is where this implementation of a Red-Black tree varies from the 
most common implementations.
Usually the balancing procedure is broken down into a series of
:Q.rotations:eQ. where a sub-tree of the tree would appear to be rotated 
if represented graphically.
These rotations can be left or right and the procedure moves up to the
parent sub-tree and is repeated until the violation is removed.
Instead &product uses the concept of a :Q.transformation:eQ..
:CMT figure out/make reference tag
[Alternatives to Two Classic Data Structures, Chris Okaski, 2005?]
A sub-branch of the tree is analysed to see which case it matches 
and the elements in that branch are then reorganised and recoloured
in one block of code.
Although this isn't wildly different it was hoped that it would allow a
faster algorithm to be created because larger sub-trees and special
cases could be matched and manipulated in one go, and the code
generator may be able to make a better job of optimising the code
because a larger block of manipulating instructions would be together.
Whether this was a good desision will be born out in time.

:P.
Explain why the insert methods are currently inline - compiler bug - 
what exactly was the problem?


:CMT ------------------------------------------------------------------------
:H2.Deleting Elements
:P.
Deletion is a bit more complicated than insertion.
The main method that gets called is 
:FUNC.erase( iterator ).
I did hope it may be possible to rewrite this in a way that is easier to 
understand.
There are two main cases: 
:OL.
:LI.The node to be removed has both children
:LI.The node has one or more children null (i.e. has 0 or 1 real child, 
in other words 1 or 2 leaves) - I've called this an :Q.end node:eQ.
:eOL.
If it is an :Q.end node:eQ. (has 0 or 1 real child) then that child can 
be linked into it's place or the node can just be deleted.
We take note of the deleted node's parent, the child, and its colour.
The other case where it has 2 children is more complicated.
We swap the predecesor (which cannot have a right child by definition) of 
the deleted node into the place of the deleted node, and change its colour
so that part of the tree is still valid.
The node being removed is now effectively the predecesors old possition,
so we take note of its original child, parent and colour.
:P.
Now we have created a situation where we are really removing an 
:Q.end node:eQ. (0 or 1 real child).
We can look at the colour of the node to be removed,
if it is red then there is no violation of the black height rule by removing it.
Also it cannot have a real child so there is nothing to link in its place.
If the removed :Q.end node:eQ. is black there are two cases.
If it has a child that child must be red or there would have been a black
height violation, thus we link the child in the place and paint it black
to maintain the black height.
If there was no child we have created a black height problem - there is
a lack of black on this branch.
The deleted node has left a null leaf node in its place, we usually count 
these as black, but in this case we have to call it double black to resolve 
the black hieght problem.
This isn't valid so we call :FUNC.doubleBlackTransform() to run through
a set of cases to rearrange subtrees and remove the need for double
black.

