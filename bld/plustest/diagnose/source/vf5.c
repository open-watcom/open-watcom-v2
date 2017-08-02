struct D { int d1, d2; };
struct F { int f1, f2; virtual long foo( int a, int b[] ); virtual ~F(); F(); };
struct V : D, virtual F { int v1, v2; };
struct X : V { int d1, d2; virtual long foo( int a, int b[] ); virtual ~X(); };

X::~X()
{}

X x;
