// use -wx switch


static void B() { }
static void C() { }

void *c = &B;

void another() {
    void (*d) (void) = &B;
    void *e = &C;
}
// should not give any unreferenced symbol errors
