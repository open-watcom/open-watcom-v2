:H1.Introduction
:P.
This chapter describes the std::~stack adaptor.

:H2.Status
:P.
:AUTHOR date='24 Nov 2005'.D Cletheroe
:P.
Reviewer: Not reviewed
:P.

The default container is currently a vector as deque has yet to be written
:P.

All members complete:
:UL.
:LI.explicit stack( Container const & x = Container() )
:LI.empty() const
:LI.size() const
:LI.top() and top() const
:LI.push( value_type const & )
:LI.pop()
:LI._Sane()
:LI.operators == != < > <= >=
:eUL.

:H1.Design Details
:P.
:CLASS.template < class Type, class Container = std::dequeue > class std::~stack

:P.
This template is called an adaptor because it uses a real container (the
Container template parameter) to do the storing of the objects and just
provides a different interface to it.
It lacks begin() and end() so you can't use iterators with it and therefore
you can't use the standard algorithms.


:H2.Description of a stack
:P.
A stack is a bit like a pile of books, where each book is the contained object.
You can add books to the top of the pile (push), look at the top book (top),
and discard the top book (pop).

:H2.Overview of the class
:P.
This is a very simple wrapper.
I suggest you just look at the source code.

