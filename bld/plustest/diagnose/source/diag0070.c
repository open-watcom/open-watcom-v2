struct S {
    S();
};

struct R : S {
    int r;
};

R::R()		// compiler should complain because R::R doesn't exist (easy)
{
    r = 1;
}

R q;

struct T : S {
    int t;
};

T x;

T::T()		// compiler should complain because T::T doesn't exist (hard)
{
    t = 1;
}

struct OK : S {
    int o;
};

OK z;
