struct X {
    static int i;
    static char a[];
};

extern __segment i_seg;
extern __segment a_seg;
extern void __based(void) *i_off;
extern void __based(void) *a_off;
