.pp
Operations at a higher level in the table will occur before those
below.
All operators involving more than one
operand associate from left to right, except
for the conditional and assignment operators, which associate from
right to left.
Operations at the same level, except where discussed in the relevant
section, may be executed in any order that the compiler chooses
(subject to the usual algebraic rules).
In particular, the compiler may regroup sub-expressions that are
both associative and commutative in order to improve the efficiency of
the code, provided the meaning (i.e. types and results) of the
operands and result are not affected by the regrouping.
.pp
The order of any side-effects (for example, assignment, or action
taken by a function call) is also subject to alteration by the
compiler.
