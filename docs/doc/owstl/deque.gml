:H1.Introduction
:P.
The class template :CLASS.std::deque
provides a random access sequence with amortized O(1) :FUNC.push_back
and :FUNC.push_front
operations.

:H2. Status
:P.
:AUTHOR date='4 Dec 2005'.Peter C. Chapin
:P.
Reviewer: Not reviewed
:P.
The basic functionality of :CLASS.std::deque
has been implemented. This includes the specialized deque operations and
deque iterators. However, the more "exotic" vector-like operations
(insert and erase in the middle of the sequence) have not yet been
implemented. There has been essentially no user feedback.

:H1. Design Details

:H2. Overall Structure
:P.
This implementation is based on a circular buffer. Like a vector, a deque
object allocates more memory than it actually uses. In other words its
capacity may be greater than its size. However, unlike a vector the
sequence stored in a deque is allowed to wrap around in the buffer
resulting in non-contiguous storage. This means an operation such as
:CODE.&deq[0] + n
may result in a pointer that is invalid even if
:CODE.n
is less than the deque's size. This behavior is allowed by the standard
[reference?].

:P.
A deque object maintains two indicies. The
:CODE.head_index
refers to the location in the buffer where the first item is stored. The
:CODE.tail_index
refers to the location in the buffer just past (after possible
wrap-around) where the last item is stored. When
:CODE.head_index == tail_index
the deque is empty. To avoid the potential ambiguity of this condition,
the buffer is reallocated just before it is full (when
:CODE.deq_length + 1 == buf_length
) so that the condition
:CODE.head_index == tail_index
never occurs due to a full buffer. This makes implementing some of the
deque operations much easier.

:P.
For example, deque iterators are represented using a pointer to the
deque object and an index value that marks the iterator's current
position in the deque's buffer. If the iterator's index value equals
:CODE.head_index
this can only mean the iterator is at the beginning of the sequence. It
never means that the iterator is just past the end of the sequence.
This disambiguation makes implementing
:FUNC.operator<
and the other relational operators on iterator much more straight
forward.

:P.
The general organization and style of deque's implementation follows
that of the other buffered sequences,
:CLASS.std::vector
and
:CLASS.std::~string.
This consistency is intentional. It is intended to make the
:CLASS.std::deque
code easier to understand. It also opens up some possibility that all
the buffered sequences might one day share code.

:H2. Alternative Implementations

:P.
In addition to the circular buffer implementation an alternative
approach was considered that uses contiguous storage. The idea was to
store the deque's contents in the "middle" of the buffer so that some
free space would be available on either end for fast
:FUNC.push_back
and
:FUNC.push_front
operations. If the deque grows to the point where one of the buffer ends
is reached, the active contents of the deque might be recentered (if the
allocated space was not too large) or completely reallocated (if the
allocated space was almost full).

:P.
This contiguous storage approach allows deque to be more vector-like and
might promote code sharing between deque and vector. For example, a
vector would be a special kind of deque in this case. However, at the
time of this writing it is unclear how such an implementation would best
decide between recentering and reallocation. More analysis is necessary
to understand the issues involved.

:H2. Open Watcom Extensions

:P.
Because of this implementation's use of a circular buffer it is not
difficult to provide
:FUNC.capacity
and
:FUNC.reserve
methods for deque even though the standard does not require them. As
with vector, the
:FUNC.reserve
method causes a deque to set aside enough memory so that no additional
allocations or internal copies will be needed until at least the
reserved size is reached.
