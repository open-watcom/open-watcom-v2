
This directory contains the functional regression tests for OWSTL, the Open
Watcom STL implementation. Most of the test programs have a similar form.
Each test program contains a separate function for each facility being
tested. The file template.cpp is a template test program that can be used
as a starting point when a new test program is introduced. Some of the test
programs are more generic and demonstrate a collection of related features
in a semi-realistic way. Test programs that exercise "unusual" but legal
usages are encouraged.

The file sanity.cpp is #included into the other test programs. It contains
some helper functions and macros to do sanity checking (for example on the
heap). Such checks should be performed aggressively by the test programs.
Currently the test programs attempt to detect memory leaks using functions
from sanity.cpp.

The file itcat.hpp provides iterator wrapper templates. The purpose of
these templates is to "dumb down" ordinary pointers so that they only
provide the operations of the indicated iterator category. For example,
FwdIt<int> takes a pointer to int and makes it behave like a forward
iterator. Using these wrappers allows the test programs to pass (wrapped)
pointers into algorithms that are intended to work with more limited
iterators. If the algorithm accidentally uses an operation that is not
allowed for its intended iterator category a compile time error results.

While consistency is a good thing in normal programs, these test programs
should make use of varying and inconsistent styles. Doing so increases the
number of combinations of features exercised and improves the scope of the
testing.

Ideally test programs should produce no output when all tests pass. This
makes it easy to verify correct output when a test is run manually (as is
sometimes useful to do). Some tests currently don't follow this rule (see
the file test.chk for the expected output of the test programs), but those
tests probably should be updated to fix this.

In theory it would be nice if the tests were portable enough to run on
other compilers as well. That may be impractical to achieve in some cases.
In any event, some tests take into account implementation details specific
to Open Watcom; on a different system the tests might not be particularly
interesting or they might fail spuriously.

Ideal this test suite should exercise exception conditions (including low
memory conditions) as well as the non-exceptional cases. In addition, this
test suite should exercise the OWSTL components in a multithreaded
environment. Tests specific to 16 bit targets would also be useful. Some of
these tests have yet to be added. As with all software, this test suite is
a constantly evolving body of code.
