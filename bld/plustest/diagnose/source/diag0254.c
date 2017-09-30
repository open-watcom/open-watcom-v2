// from [class.conv.ctor] 12.3.1

#ifdef __WATCOM_EXPLICIT__
class Z {
public:
    explicit Z(int);
};

Z a1 = 1; 			// error: no implicit conversion
Z a3 = Z(1); 			// OK: direct initialization syntax used
Z a2(1); 			// OK: direct initialization syntax used
Z *p = new Z(1);		// OK: direct initialization syntax used
Z a4 = (Z)1;			// OK: explicit cast used
Z a5 = static_cast<Z>(1);	// OK: explicit cast used
#else
#error Explicit keyword not implemented
#endif
