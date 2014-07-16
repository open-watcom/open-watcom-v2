//
//
// String class: strtest1.c                           version: 0.9
//
// author: Uwe Steinmueller, SIEMENS NIXDORF Informationssysteme AG Munich
//         email: uwe.steinmueller@sniap.mchp.sni.de
//
// start: 28.08.92
//
// this source code is fully copyrighted but it is free in use for
// standardization purposes (X3J16 and ISO WG 21)
//

//
// these are not exhaustive tests and should be improved in the future
//

#include <gstring.h>
#include <iostream.h>
#include <strstream.h>
#include <stdlib.h>

/*   #include <monitor.h>    */
/*   #include <x_mem.h>    */
/*   #include <memstat.h>    */

void constuctors()
{
/*       MemStat memStat("constructors");    */
    String s1, s2((Size_T)(55)), s3("12345", 4), s4("12345"), s5('x');
    String s6('x', 27);

    cout << "constructors" << endl;
    s2 = "123";
    assert(s1.length() == 0);
    assert(s2.length() == 3);
    assert(s2.reserve() > s2.length());
    assert(s3.length() == 4);
    assert(s4.length() == 5);
    assert(s5.length() == 1  && s5.getAt(0) == 'x');
    assert(s6.length() == 27 && s6.getAt(26) == 'x');
}

void assign()
{
/*       MemStat memStat("assign");    */
    String a("123");
    String b;

    cout << "assign" << endl;
    b = a;
    assert(a == b);
    b = "123";
    assert(a == b);
    a.assign("1234", 3);
    assert(a == b);
    b.assign('x', 14);
    assert(a != b && b.length() == 14);
    b = 'x';
    assert(b.getAt(0) == 'x' && b.length() == 1);
}

void append()
{
/*       MemStat memStat;    */
    String s1("123"), s2("12345", 4);
    String s3 = s1, s4;

    cout << "append" << endl;
    s3 += s2;
    assert(s3.length() == 7);
    s3 += "abcd";
    assert(s3.length() == 11);
    s3.append("xqz", 4);
    assert(s3.length() == 15);
    s2.append('x', 100);
    assert(s2.length() == 104);
    assert(s2.getAt(103) == 'x');
    s2 += "";
    assert(s2.length() == 104);
    s2 += s4;
    assert(s2.length() == 104);
    s2.append('x', 0);
    assert(s2.length() == 104);
    s2.append("1", 0);
    assert(s2.length() == 104);
    s2 += 'y';
    assert(s2.length() == 105 && s2.getAt(104) == 'y');
    s2 = "1";
    s2.append('x', 1);
    assert(s2.length() == 2 && s2.getAt(1) == 'x');
}

void plus()
{
/*       MemStat memStat("plus");    */
    String s1("123");
    String s2("xyz");
    String s3, s4;

    cout << "plus" << endl;
    s3 = s1 + s2;
    assert(s3 == "123xyz");
    s3 = s1 + "xyz";
    assert(s3 == "123xyz");
    s3 = "123" + s2;
    assert(s3 == "123xyz");
    s3 = s1 + 'x';
    assert(s3 == "123x");
    s3 = '1' + String("23x");
    assert(s3 == "123x");
    s3 = s3 + s4;
    assert(s3 == "123x");
    s3 = s3 + "";
    assert(s3 == "123x");
}



void test1()
{
/*       MemStat memStat("test1");    */
    String  a = "12345";
    String  b;

    cout << "mixed tests" << endl;
    assert(a.length() == 5);

    a += a;

    assert(a.length() == 10);

    assert(strlen(a.cStr()) == 10);

    a += a.cStr();

    assert(a.length() == 20);

    assert(a.getAt(4) == '5');
    a.putAt(4, 'x');
    assert(a.getAt(4) == 'x');

    b = 'x';
    assert(String("x") == b);
    b += 'y';
    assert(String("xy") == b);
    b.putAt(b.length(), 'z');
    assert("xyz" == b);

}

void compare()
{
/*       MemStat memStat("compare");    */
    String s1("1234", 5), s2("1234");
    String s3("1235", 5), s4("1232");
    String s5;

    cout << "compare" << endl;
    assert(s1.compare(s2) != 0);
    assert(compare(s1,s2) != 0);
    assert(s2.compare(s2) == 0);
    assert(s2.compare("12345", 4) == 0);
    assert(s1.compare(s3) < 0);
    assert(compare(s2, s3) < 0);
    assert(compare(s2, "1235") < 0);
    assert(compare("1235", s2) > 0);
    assert(s1.compare(s4) > 0);
    assert(s2.compare(s5) > 0);
    assert(s5.length() == 0);
    assert(s5.compare("") == 0);
    assert(compare(s5,"") == 0);
    assert(compare("",s5) == 0);
    assert(s1 != "1234");
    assert(s2 == "1234");
    assert("1234" == s2);
    assert("1234" != s1);
}

const char* foo(String& s)
{
    // very much a hack, but such is the world of real programming
    // foo is just for hiding the real hack
    const char* p = s.cStr();

    if(s.length() > 2)
        p += 2;
    return p;
}

void insert2()
{
    String s1("1234");

    cout << "insert2" << endl;
    s1.insert(1, foo(s1));
    assert(s1 == "134234");
}

void insert()
{
/*       MemStat memStat("insert");    */
    String s1("1234", 5), s2("1234");
    String s3("1235", 4), s4("1232");
    String s5;

    insert2();
    cout << "insert" << endl;
    s1.insert(1, s1.cStr());
    assert(s1.length() ==  9);
    s2.insert(1, s2);
    assert(s2.length() ==  8 && s2 == "11234234");
    s5.insert(0, s3);
    assert(s3 == s5);
    s5.insert(2, "abcdef", 4);
    assert(s5 == "12abcd35");
    s3.insert(s3.length(), "abcd");
    assert(s3 == ((String)("1235") + "abcd"));
    s3 = "1234";
    s3.insert(0, 'x', 5);
    assert(s3 == "xxxxx1234");
    s3 = "1234";
    s3.insert(2, 'x', 5);
    assert(s3 == "12xxxxx34");
    s3 = "1234";
    s3.insert(s3.length(), 'x', 5);
    assert(s3 == "1234xxxxx");
}

void remove()
{
/*       MemStat memStat("remove");    */
    String s1("1234", 5), s2("1234");
    String s3("1235", 4), s4("1232");
    String s5, s6;
    char   c1;

    cout << "remove" << endl;
    assert(s1.length() == 5);
    s6 = s1;
    assert(s1.length() == 5);
    assert(s6.length() == 5);

    s1.remove(1);
    assert(s1 == "1");
    s1 = s6;
    assert(s1.length() == 5);
    assert(s6.length() == 5);
    s1.remove(1, 2);
    assert(s1 == String("14", 3));
    s1 = s6;
    s1.remove(s1.length());
    assert(s1 == s6);
    s1.getRemove(s5, 1);
    assert(s1 == "1");
    assert(s5 == String("234", 4));
    s1 = s6;
    s1.getRemove(s5, 1, 2);
    assert(s1 == String("14", 3));
    assert(s5 == "23");
    s1 = s6;
    s1.getRemove(c1, 2);
    assert(s1 == String("124", 4));
    assert(c1 == '3');

}

void replace()
{
/*       MemStat memStat("replace");    */
    String s1("1234", 5), s2("abcd");
    String s3("1235", 4), s4("1232");
    String s5, s6;

    cout << "replace" << endl;
    assert(s1.length() == 5);
    s6 = s1;
    assert(s1.length() == 5);
    assert(s6.length() == 5);

    s1.replace(1, 2, s2);
    assert(s1 == String("1abcd4", 7));
    s2.replace(1,2, s2);
    assert(s2 == String("aabcdd", 6));
}

void find()
{
/*       MemStat memStat("find");    */
    String s1("1234", 5), s2("abcd");
    String s3("1235", 3), s4("1232");
    String s5("34", 3), s6("34");
    size_t  fpos;

    cout << "find" << endl;
    assert(s1.find(s6, fpos) != 0);
    assert(fpos == 2);

    assert(s1.find(s6, fpos, 2) != 0);
    assert(fpos == 2);

    assert(s1.find(s6, fpos, 3) == 0);
    assert(fpos == NPOS);

    assert(s1.rfind(s6, fpos) != 0);
    assert(fpos == 2);

    assert(s1.rfind(s5, fpos) != 0);
    assert(fpos == 2);

    assert(s1.rfind(s6, fpos, 2) != 0);
    assert(fpos == 2);

    assert(s1.rfind(s6, fpos, 1) == 0);
    assert(fpos == NPOS);

    assert(s1.find("34", fpos, 0, 3) != 0);
    assert(fpos == 2);

    assert(s1.find("34", fpos, 0) != 0);
    assert(fpos == 2);

    assert(s1.find("34", fpos, 2) != 0);
    assert(fpos == 2);

    assert(s1.find("34", fpos, 3) == 0);
    assert(fpos == NPOS);

    assert(s1.rfind("34", fpos) != 0);
    assert(fpos == 2);

    assert(s1.rfind("34", fpos, NPOS, 3) != 0);
    assert(fpos == 2);

    assert(s1.rfind("34", fpos, NPOS, 4) == 0);
    assert(fpos == NPOS);

    assert(s1.find('\0', fpos, 0) != 0);
    assert(fpos == 4);

    assert(s1.find('\0', fpos) != 0);
    assert(fpos == 4);

    assert(s1.find('x', fpos, 0) == 0);
    assert(fpos == NPOS);

    assert(s4.find('2', fpos, 2) != 0);
    assert(fpos == 3);

    assert(s4.rfind('2', fpos, 2) != 0);
    assert(fpos == 1);

    assert(s4.rfind('2', fpos) != 0);
    assert(fpos == 3);

    assert(s4.rfind('2', fpos, fpos - 1) != 0);
    assert(fpos == 1);
}

void substring()
{
/*       MemStat memStat("substring");    */

    String s1 = "1234567890";
    String s4 = "abcdefghij";
    String s2, s3;

    cout << "substring" << endl;
    assert(s1.substr(2,4)  == "3456");
    assert(s1.substr(2,99) == "34567890");

    s2 = s1;
    s2.insert(2, s4.substr(5, NPOS));
    assert(s2 == "12fghij34567890");

    s2 = s1;
    s2.replace(2, 4, s4.substr(2, 4));
    assert(s2 == "12cdef7890");
}


void find2()
{
/*       MemStat memStat("find2");    */
    String s1("1234", 5), s2("34cb");
    String s3("xy", 3), s4("24"), s5("1234");
    size_t  fpos;


    cout << "find2" << endl;
    assert(s1.findFirstOf(s2, fpos) != 0);
    assert(fpos == 2);

    assert(s1.findFirstOf(s4, fpos) != 0);
    assert(fpos == 1);

    assert(s1.findFirstOf(s3, fpos) != 0);
    assert(fpos == 4);

    assert(s1.findFirstNotOf(s5, fpos) != 0);
    assert(fpos == 4);
}

void copy()
{
/*       MemStat memStat("copy");    */
    String s1("1234", 5), s2;
    char *p = 0;
    size_t len, len2;

    cout << "copy" << endl;
    s2 = s1;
    s2 += s2;

    assert(s2.length() == 10);
    len = s2.length() - 2;
    assert(len == 8);
    p = new char[len + 1];
    assert(p != 0);
    len2 = s2.copy(p, len, 2);
    assert(len2 == 8);
    s2 = s2.substr(2);
    assert(s2.length() == 8);
    assert(s2.compare(p, len) == 0);

    delete [] p;
}

void input()
{
    char buffer[100];
    strcpy(buffer, "xy  1234567890 \nabc");
    istrstream is(buffer, strlen(buffer));
    istrstream i2(buffer, strlen(buffer));
    String s1("1234");

    cout << "input" << endl;
    is >> s1;
    assert(s1 == "xy" && is != 0);
    is >> s1;
    assert(s1 == "1234567890" && is != 0);
    is >> s1;
    assert(s1 == "abc" && is != 0);
    is >> s1;
    assert(s1 == "" && is == 0 && is.eof() != 0);
    getline(i2, s1);
    assert(s1 == "xy  1234567890 ");
    assert(i2 != 0);
    getline(i2, s1);
    assert(s1 == "abc");
    assert(i2 != 0);
}


void reserve()
{
/*       MemStat memStat("reserve");    */
    String s1((Size_T)(200));

    cout << "reserve" << endl;
    s1 = "4711";
    assert(s1 == "4711");
    s1.reserve(0);
    assert(s1 == "4711" && s1.length() < s1.reserve());
    s1.reserve(1000);
    assert(s1 == "4711");
}

void error()
{
    cout << "error" << endl;

    String s1((Size_T)(NPOS - 1));
}


void overflow()
{
    cout << "overflow" << endl;

    String s1('x', 40000);
    String s2('y', 40000);
    String s3 = s1 + s2;
}


int   main()
{
/*       MemStat memStat("main");    */
    for(int i = 0; i < 1; i++) {
        input();
        copy();
        find2();
        substring();
        find();
        replace();
        remove();
        insert();
        compare();
        plus();
        append();
        assign();
        reserve();
        constuctors();
        test1();
        // error();
        // overflow();
    }
    return 0;

}

