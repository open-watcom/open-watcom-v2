// MSC accepts this code (AFS) (no positive test entered)
void foo(char _based(void) *p,char _based(void) *q,_segment sel)
        {
        p = q + 1;
        *(sel:>p) = 0;
        }

