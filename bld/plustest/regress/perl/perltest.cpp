#ifdef  TEST

#include <iostream.h>
/*
 * V1.6
 */

#include <string.h>
#include <malloc.h>
#include <stdio.h>

#ifdef  __TURBOC__
#pragma hdrstop
#endif

#include "perlclass.h"

int main()
{
PerlList<int> il, il1, il2, il3;
PerlStringList x, y, z;
int n;

    if(x) cout << "x is not empty" << endl;
    else cout << "x is empty" << endl;

    if(x.isempty()) cout << "x.isempty() is true" << endl;
    else cout << "x.isempty() is false" << endl;

    n= x.split("a b c d e f");

    if(x) cout << "x is not empty" << endl;
    else cout << "x is empty" << endl;

    if(x.isempty()) cout << "x.isempty() is true" << endl;
    else cout << "x.isempty() is false" << endl;

    cout << "x.split(a b c d e f)= " << n << ": " << x << endl;
    cout << "x[0] = " << x[0] << endl;
    z= x; z[0]= "x";
    cout << "z= x; z[0]=\"x\" " << "z: " << z << endl;

    PerlString ss("1.2.3.4.5.6.7.8.9.0");
    y= ss.split("\\.");
    cout << "ss= " << ss << ", y= ss.split(\"\\.\"), y=" << endl << y << endl;
    cout << "y.join(\" \")" << y.join(" ") << endl;       
    {
        PerlString xx= "a b c\nd e\tf   g";
        cout << xx << endl << "xx.split()= " << xx.split() << endl;
        xx= "a b c d e f g";
        cout << xx << endl << "xx.split(\",\")= " << xx.split(",") << endl;
        xx= "  a b c d e f g hi  ";
        cout << xx << endl << "xx.split(\"\")= " << xx.split("") << endl;
        xx= "a,b,c,d,,e,f,g,,,,";
        cout << xx << endl << "xx.split(\",\")= " << xx.split(",") << endl;
        xx= "a,b,c,d,,e,f,g,,";
        cout << xx << endl << "xx.split(\",\", 5)= " << xx.split(",", 5) << endl;
        xx= " a b c d e f g  ";
        cout << xx << endl << "xx.split(\" \")= " << xx.split(" ") << endl;
        xx= "a b c d,e,f g";
        cout << xx << endl << "xx.split(\"([ ,])+\")= " << xx.split("([ ,])+") << endl;
        xx= ",,,,";
        cout << xx << endl << "xx.split(\",\")= " << xx.split(",") << endl;
        xx= "";
        cout << xx << endl << "xx.split(\",\")= " << xx.split(",") << endl;
        xx= "   a b c\td    e\nf  g   ";
        cout << xx << endl << "xx.split(\"' '\")= " << xx.split("' '") << endl;
   }

    cout << "x = " << x << endl;

    cout << "x.pop() : " << x.pop() << ", " ;
    cout << x.pop() << endl << "x= " << x << endl;;
    cout << "x.shift() : " << x.shift() << ", ";
    cout << x.shift() << endl<< "x= " << x << endl;
    
    x.unshift(y);
    cout << "x.unshift(y): " << x << endl;
    
    if(il) cout << "il is not empty" << endl;
    else cout << "il is empty" << endl;
    
    il.push(1); il.push(2); il.push(3); il.push(4);

    if(il) cout << "il is not empty" << endl;
    else cout << "il is empty" << endl;
    
    cout << "il(1, 2, 3, 4) : " << il << endl;
    il3= il; il3[0]= 9999;
    cout << "il3= il; il3[0]= 9999; il3 = " << il3 << endl << "il= " << il << endl;

    il1= il.reverse();
    cout << "il.reverse: " << il1 << endl;

    cout << "il1.sort(): " << il1.sort() << endl;

    y.reset();
    y.push("one"); y.push("two"); y.push("three"); y.push("four");
    cout << "y = " << endl << y;
    cout << "y.reverse() " << y.reverse() << endl;
    cout << "y.sort() " << y.sort() << endl;
    cout << "y.sort().reverse() " << y.sort().reverse() << endl;
        
    il2.push(3); il2.push(4);
    cout << "il2.push(3, 4) : " << il2 << endl;

    il.push(il2);    
    cout << "il.push(il2) : " << il << endl;
    
    cout << "il.pop() : " << il.pop() << ", ";
    cout << il.pop() << endl;
    
    il.unshift(il2);    
    cout << "il.unshift(il2) : " << il << endl;
    
    cout << "il.shift() : " << il.shift() << ", ";
    cout << il.shift() << endl;
     
    cout << "testing splice:" << endl;
    x.reset();
    x.split("a b c d e f g h i");
    cout << "x = " << x << endl;
    z= x.splice(2, 3);
    cout << "z= x.splice(2, 3): z= " << z << endl << "x = " << x << endl;
    cout << "x.splice(2, 0, z): " << x.splice(2, 0, z);
    cout << "x= " << x << endl;
    cout << "z.splice(1, 1, x): " << z.splice(1, 1, x);
    cout << "z= " << z << endl;
    cout << "x= " << x << endl;
    cout << "z.splice(20, 1, x): " << z.splice(20, 1, x);
    cout << "z= " << z << endl;
    
 // test auto expand
    PerlList<int> ile;
    ile[3]= 3;
    cout << ile.scalar() << ", " << ile[3] << endl;
    ile[100]= 1234;
    ile[0]= 5678;
    cout << ile.scalar() << ", " << ile[0] << ", " << ile[100] << endl;
    PerlList<int> ile2;
    int i;
    for(i=0;i<=100;i++) ile2[i]= i;
    for(i=200;i>100;i--) ile2[i]= i;
    for(i=0;i<=200;i++) if(ile2[i] != i) cout << "error at index " << i << endl;
    cout << "Index check done" << endl;
    cout << ile2.scalar() << ", " << ile2[0] << ", " << ile2[200] << endl;

// test Regexp stuff
    cout << endl << "testing regexp stuff:" << endl;
    x.reset();
    cout << "x.m(\".*X((...)...(...))\", \"12345Xabcxyzdef\") returns " <<
         x.m(".*X((...)...(...))", "12345Xabcxyzdef") << endl;
    cout << "subs matched = " << x << endl;

    Regexp rexp("abc");
    PerlString rst("12345Xabcxyzdef");
    cout << "rst.m(rexp) returns " << rst.m(rexp) << endl;
    
    cout << endl << "testing grep:" << endl;
    x.reset();
    x.push("abcd"); x.push("a2345"); x.push("X2345"); x.push("Xaaaaa"); x.push("aaaaa");
    
    y= x.grep("^a.*");
    cout << "x: " << endl << x << endl << "grep(^a.*)" << endl;
    cout << "Expect 3 matches:" << endl << y << endl;
    {
        PerlString s1("abcdef");
        cout << "s1= " << s1 << ", s1.m(\"^cde\") : " << s1.m("^cde") << endl;
        cout << "s1= " << s1 << ", s1.m(\"^..cde\") : " << s1.m("^..cde") << endl;
    }
    {
    PerlStringList sl;
    PerlString str= "ab cd ef";
        sl = m("(..) (..)", str);
        cout << "sl = m(\"(..) (..)\", \"ab cd ef\"); sl = " << endl <<
                 sl << endl;
    }

    {
    Regexp ncr("abc", Regexp::nocase);
    Regexp cr("abc");
    PerlString s= "ABC";
    cout << "s= " << s << ": s.m(ncr)= " << s.m(ncr) << endl;
    cout << "s= " << s << ": s.m(cr)= " << s.m(cr) << endl;
    cout << "s.m(\"abc\", \"i\")= " << s.m("abc", "i") << endl;
    cout << "s.m(\"abc\")= " << s.m("abc") << endl;
    }
    
// Test strings
    cout << "test string stuff:" << endl;

    PerlString s1("string1"), s2, s3;
    const char *s= s1;

    cout << "Empty string: " << s2 << " length= " << s2.length()
         << ",  strlen(s2) = " << strlen(s2) << endl;
    
    cout << "s1:" << s1 << endl;
    cout << "s[0]= " << s[0] << ", s[5]= " << s[5] << endl;
//    s[2]= 'X';
//    cout << "s[2]= 'X', s= " << s << endl;
//    s[2]= 'r';
    
    cout << "const char *s= s1: s= " << s << endl;
    s2= s1;
    cout << "s2=s1,  s2:" << s2 << endl;
    s1.chop();
    cout << "s1.chop()" << s1 << endl;
    s3= s;
    cout << "s3= s: s3 = " << s3 << endl;
    cout << "index(\"ri\") in " << s1 << ": " << s1.index("ri") << endl;
    s3= "1";
    cout << "index(" << s3 << ") in " << s1 << ": " << s1.index(s3) << endl;
    s3= "abcabcabc";
    cout << "rindex(abc) in" << s3 << ": " << s3.rindex("abc") << endl;
    cout << "rindex(abc, 5) in" << s3 << ": " << s3.rindex("abc", 5) << endl;

// test substrings
    cout << "substr(5, 3) in " << s3 << ": " << s3.substr(5, 3) << endl;
    s3.substr(5, 3)= "XXX";
    cout << "s3.substr(5, 3) = \"XXX\"" << s3 << endl;
    s3.substr(5, 3)= s1;
    cout << "s3.substr(5, 3) = s1" << s3 << endl;
    s3.substr(5, 3)= s1.substr(1, 3);
    cout << "s3.substr(5, 3) = s1.substr(1, 3)" << s3 << endl;
    s3.substr(0, 6)= s1.substr(0, 3);
    cout << "s3.substr(0, 6) = s1.substr(0, 3)" << s3 << endl;
    s3.substr(-3, 2)= s1.substr(0, 2);
    cout << "s3.substr(-3, 2) = s1.substr(0, 2)" << s3 << endl;

// test overlapping substrings
    s1= "1234567890";
    cout << "s1 = " << s1 << endl;
    s1.substr(0, 10)= s1.substr(1, 9);
    cout << "s1.substr(0, 10)= s1.substr(1, 9) " << s1 << endl;
    s1= "1234567890";
    cout << "s1 = " << s1 << endl;
    s1.substr(1, 9)= s1.substr(0, 10);
    cout << "s1.substr(1, 9)= s1.substr(0, 10) " << s1 << endl;

    // test over-large substrings
    s1= "1234567890"; s1.substr(7, 10)= "abcdefghij";
    cout << "s1.substr(7, 10)= \"abcdefghij\" " << s1 << endl;
    s1= "1234567890"; s1.substr(10, 5)= "abcdefghij";
    cout << "s1.substr(10, 5)= \"abcdefghij\" " << s1 << endl;
    s1= "1234567890"; s1.substr(20, 1)= "abcdefghij";
    cout << "s1.substr(20, 1)= \"abcdefghij\" " << s1 << endl;

    s1= "abcdef"; s2= "123456";
     
    cout << s1 << " + " << s2 << ": " << s1 + s2 << endl;
    cout << s1 << " + " << "\"hello\"= " << s1 + "hello" << endl;
    cout << "\"hello\"" << " + " << s1 << "= " << "hello" + s1 << endl;
    cout << s1 << " + \'x\' = " << s1 + 'x' << endl;
    
    s1= "abc"; s2= "def"; s3= "abc";
    cout << s1 << " == " << s2 << ": " << (s1 == s2) << endl; 
    cout << s1 << " != " << s2 << ": " << (s1 != s2) << endl;
    cout << s1 << " == " << s3 << ": " << (s1 == s3) << endl; 
    cout << s1 << " != " << s3 << ": " << (s1 != s3) << endl;
    cout << s1 << " < " << s2 << ": " << (s1 < s2) << endl; 
    cout << s1 << " > " << s2 << ": " << (s1 > s2) << endl; 
    cout << s1 << " <= " << s2 << ": " << (s1 <= s2) << endl; 
    cout << s1 << " >= " << s3 << ": " << (s1 >= s3) << endl; 

// Test the tr() functions
    s1= "abcdefghi";
    cout << "s1 = " << s1;
    cout << ", s1.tr(\"ceg\", \"123\") = " << s1.tr("ceg", "123");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghi";
    cout << "s1.tr(\"a-z\", \"A-Z\") = " << s1.tr("a-z", "A-Z");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghi";
    cout << "s1.tr(\"efg\", \"\") = " << s1.tr("efg", "");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghi";
    cout << "s1.tr(\"ac-e\", \"X\") = " << s1.tr("ac-e", "X");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghiiii";
    cout << "s1 = " << s1;
    cout << ", s1.tr(\"ac-e\", \"X\", \"s\") = " << s1.tr("ac-e", "X", "s");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghi";
    cout << "s1.tr(\"ac-e\", \"\", \"d\") = " << s1.tr("ac-e", "", "d");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghi";
    cout << "s1.tr(\"ac-e\", \"d\", \"d\") = " << s1.tr("ac-e", "d", "d");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdefghi";
    cout << "s1.tr(\"ac-e\", \"\", \"cd\") = " << s1.tr("ac-e", "", "cd");
    cout << ", s1 = " << s1 << endl;
    s1= "bookkeeper";
    cout << s1;
    cout << ": s1.tr(\"a-zA-Z\", \"\", \"s\") = " << s1.tr("a-zA-Z", "", "s");
    cout << ", s1 = " << s1 << endl;
    s1= "abc123def456ghi";
    cout << s1;
    cout << ": s1.tr(\"a-zA-Z\", \" \", \"c\") = " << s1.tr("a-zA-Z", " ", "c");
    cout << ", s1 = " << s1 << endl;
    s1= "abc123def456ghi789aaa";
    cout << s1;
    cout << ": s1.tr(\"a-zA-Z\", \" \", \"cs\") = " << s1.tr("a-zA-Z", " ", "cs");
    cout << ", s1 = " << s1 << endl;
    s1= "abcdddaaaxxx";
    cout << s1;
    cout << ": s1.tr(\"a\", \"d\", \"s\") = " << s1.tr("a", "d", "s");
    cout << ", s1 = " << s1 << endl;
    
// Test substitute command
    s1= "abcdefghi";
    cout << s1;
    cout <<" s1.s(\"def\", \"FED\") = " << s1.s("def", "FED");
    cout << ", s1= " << s1 << endl;
    s1= "abcDEFghi";
    cout << s1;
    cout <<" s1.s(\"def\", \"FED\") = " << s1.s("def", "FED");
    cout << ", s1= " << s1 << endl;
    s1= "abcDEFghi";
    cout << s1;
    cout <<" s1.s(\"def\", \"FED\", \"i\") = " << s1.s("def", "FED", "i");
    cout << ", s1= " << s1 << endl;
    s1= "abcdefghi";
    cout << s1;
    cout <<" s1.s(\"(...)(...)\", \"\\$,$&,$2 $1\") = " <<
             s1.s("(...)(...)", "\\$,$&,$2 $1");
    cout << ", s1= " << s1 << endl;
    s1= "abcdefabcghiabc";
    cout << s1;
    cout <<" s1.s(\"abc\", \"XabcX\", \"g\") = " << s1.s("abc", "XabcX", "g");
    cout << ", s1= " << s1 << endl;
    s1= "abcdefabcghiabc";
    cout << s1;
    cout <<" s1.s(\"abc\", \"X\", \"g\") = " << s1.s("abc", "X", "g");
    cout << ", s1= " << s1 << endl;
    s1= "abcdefabcghiabc";
    cout << s1;
    cout <<" s1.s(\"abc(.)\", \"X$1abcX$1\", \"g\") = " <<
             s1.s("abc(.)", "X$1abcX$1", "g");
    cout << ", s1= " << s1 << endl;
    s1= "abcdefabcghiabc";
    cout << s1;
    cout <<" s1.s(\"(.)abc\", \"$1X$1abcX\", \"g\") = " <<
             s1.s("(.)abc", "$1X$1abcX", "g");
    cout << ", s1= " << s1 << endl;
    s1= "1234567890";
    cout << s1;
    cout <<" s1.s(\"(.)(.)\", \"$2$1\", \"g\") = " <<
             s1.s("(.)(.)", "$2$1", "g");
    cout << ", s1= " << s1 << endl;
    return 0;
}
#endif
