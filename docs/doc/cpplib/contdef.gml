:P.
Container classes define common methods to store information.
Any non-trivial program will have some type of data structure
to maintain information needed by various program components.
These data structures range from the simplicity of a vector to the
complexity of a relational database.
This library supplies some of the more common data structures used
by software developers.
:P.
The purpose of a container class is to 'contain' user data.
The class definition must take care of the implementation details.
It also provides an interface for manipulating and accessing the information.
:P.
The WATCOM container classes are all
.MONO template
classes.
Each class will take as a minimal template parameter a
.MONO type
value.
The following definition
.exam begin
WCValSList<int>     list_one;
.exam end
uses a templated class definition to create a list object.
The object
.MONO list_one
is a value based single linked list of integers.
The
.MONO type
value is integer.
This list stores copies of integers in each list item.
The implementation detail of this list is that it is a single linked list.
:P.
The container you chose can affect what operations are available to you.
For example, an iterator defined for a single linked list can only move
forward through the list.
If you want to be able to move the iterator in both directions, you should
use a double linked list.
:P.
The following definition
.exam begin
WCValDList<String>  list_two;
.exam end
illustrates that the type of the data in the list does not need to be
a scalar value such as integer or character.
The
.MONO String
class is a product (or user) defined class for operating on character strings.
:P.
Some of the container classes have template parameters in addition to
the contained type.
The stack class has two parameters, and can be instantiated as in the
following:
.exam begin
WCStack<int,WCValSList<int> >  int_stack;
.exam end
The class
.MONO int_stack
is a stack of integers, with the implementation of the stack supplied
by the single linked list value class.
Note that there is a space between the two '>' characters.
If you do not leave a space, the character sequence '>>' is interpreted
as the right shift operator.
:SECTION.Intrusive Classes
:P.
Some classes use the
.MONO intrusive
.ix 'intrusive classes'
method of storing data.
These classes require the data being stored in the container to be
defined in terms of a common base class.
With this common base class, the intrusive container class can
operate on the data elements with no information about the data
stored in the element.
This requires a class definition for each data type you may wish to
store in the container.
The following definition
.exam begin
class str_sdata : public WCSLink {
    String information;
}
.exam end
will create a new class
.MONO str_sdata
which can be used in single link intrusive list classes.
The information contained in
.MONO str_sdata
is a
.MONO String
value stored in the member data
.MONO information
:PERIOD
:SECTION.Value Classes
:P.
Some classes use the
.MONO value
.ix 'value classes'
method of storing data.
These classes make a copy of the data when creating a new element.
A new storage class definition is not required
when defining a value class.
Since the data is copied, this method may be very expensive
when the data is a complex definition or contains large elements.
:SECTION.Pointer Classes
:P.
Some classes use the
.MONO pointer
method of storing data.
These classes store a pointer to the data when creating a new element.
A new storage class definition is not required
when defining a pointer class.
Although the
.MONO pointer
.ix 'pointer classes'
container class is really a specialization of the
.MONO value
class,
The storage of pointers to a particular data type
is a common use of container classes.
Providing the class definition is a convenience when specifying the
container objects.
