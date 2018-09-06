.chap Stack
.*
.sect Introduction
.*
This chapter describes the std::~stack adaptor.
.*
.section Status
.np
:AUTHOR date='24 Nov 2005'.D Cletheroe
.np
Reviewer: Not reviewed
.np
The default container is currently a vector as deque has yet to be written
.np
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
.*
.section Design Details
.np
:CLASS.template < class Type, class Container = std::dequeue > class std::~stack
.np
This template is called an adaptor because it uses a real container (the
Container template parameter) to do the storing of the objects and just
provides a different interface to it.
It lacks begin() and end() so you can't use iterators with it and therefore
you can't use the standard algorithms.
.*
.sect Description of a stack
.*
A stack is a bit like a pile of books, where each book is the contained object.
You can add books to the top of the pile (push), look at the top book (top),
and discard the top book (pop).
.*
.sect Overview of the class
.*
This is a very simple wrapper.
I suggest you just look at the source code.
