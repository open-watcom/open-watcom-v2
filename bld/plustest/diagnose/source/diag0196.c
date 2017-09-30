// copying return value from foo()(?)
struct S;

S foo();

int z = foo().a;
