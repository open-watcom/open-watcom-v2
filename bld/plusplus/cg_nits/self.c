// WCC386P self.c -ox -d1 generates poor quality code for 4[eax] fetch
// ditto for WCCP (Anthony)
typedef struct S S;
struct S {
    S __based(__self) *next;
    S __based(__self) *prev;
    int x;
};

__segment s;
S __based(s) * __based(__segname("CODE")) p;

void zap( int y )
{
    p->next->prev->x = y;
}
