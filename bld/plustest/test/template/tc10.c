template <class G> struct Q {
    static G foo;
    static G bar;
};

Q<int> *x;

template <class _G>
_G Q<_G>::foo;
template <class _G>
_G Q<_G>::bar;

int Q<int>::foo = 3;
int Q<int>::bar;
