#include <wclist.h>
#include <iostream.h>
#include <strdata.h>


static void test1();
static void test2();


int main() {
    test1();
    test2();

    cout << "done test\n";
    cout.flush();

    return( 0 );
}


void sdata_prt( str_sdata * data, void * str ) {
    cout << ">" << (char *)str << "< [" << *data << "]\n";
    cout.flush();
}


void sdata_val_prt( str_sdata data, void * str ) {
    cout << ">" << (char *)str << "< [" << data << "]\n";
    cout.flush();
}



void test1() {
    WCIsvSList<str_sdata>   ilist;

    ilist.append( new str_sdata("1:item 1") );
    ilist.append( new str_sdata("1:data 2") );
    ilist.append( new str_sdata("1:data 3") );
    ilist.append( new str_sdata("1:end data") );

    str_sdata * data;
    while( !ilist.isEmpty() ) {
        cout << "\nNumber of entries: " << ilist.entries() << "\n";
        ilist.forAll( sdata_prt, "isvslist" );
        data = ilist.get( 2 );
        delete data;
    };
}



void test2() {
    WCValSList<str_sdata>   ilist;

    ilist.append( str_sdata("1:item 1") );
    ilist.append( str_sdata("1:data 2") );
    ilist.append( str_sdata("1:data 3") );
    ilist.append( str_sdata("1:end data") );

    str_sdata data;
    while( !ilist.isEmpty() ) {
        cout << "\nNumber of entries: " << ilist.entries() << "\n";
        ilist.forAll( sdata_val_prt, "valslist" );
        data = ilist.get( 2 );
    };
}
