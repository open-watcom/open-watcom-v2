// REWRITEs need independent cursors for every RewriteRewind call
template <class T>
    class S {
	S *p;
	S *n;
	int S;
	::S<double> *zz;
	::S<T> *t;
    };

S<int> x;
