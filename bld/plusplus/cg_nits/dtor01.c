// control flow optimizer doesn't straighten this mess out
// WCCP -ox -d1

extern void *VD( void * );
extern void *FD( void * );

void *XD( void *p, unsigned m )
{
    goto L1;
L6:;
    goto L7;
L5:;
    endif:;
    goto L6;
L4:;
    VD(p);
    goto L5;
L3:;
    if( m & 1 ) goto endif;
    goto L4;
L2:;
    FD(p);
    goto L3;
L1:;
    goto L2;
L7:;
    return p;
}
