// accepted by Borland C++!
int ok();
template <class T, int = 2, int (*foo)() = &ok > class X { };

X<int> q;
