// There is no atomic representation for a member-ptr constant.
// Consequently, member-pointer constants for aggregates are currently
// initialized with code.
// - discuss with Anthony and Greg


struct A { int a; };

#pragma on ( dump_ptree )

int A::*pa[1] = { &A::a };
