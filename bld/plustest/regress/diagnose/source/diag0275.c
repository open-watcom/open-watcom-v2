// C compiler diagnoses return of address of auto
int *main() {
    int a;
    return( &a );
}
#error last line
