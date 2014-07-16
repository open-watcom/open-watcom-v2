#include "fail.h"
#include <stdio.h>
#include <string.h>

/* C99 __VA_ARGS__ example */
char buff[1000];

#define debug(...) sprintf(buff, __VA_ARGS__)
#define showlist(...) strcpy(buff, #__VA_ARGS__)
#define report(test, ...) ((test)?(int)strcpy(buff,#test):\
                       sprintf(buff, __VA_ARGS__))

#define test(name,...) #__VA_ARGS__ | name##__VA_ARGS__##two | name##__VA_ARGS__ | __VA_ARGS__##name
#define test1(name,...) #__VA_ARGS__ |  name,##__VA_ARGS__##,two | name,##__VA_ARGS__,two | name,__VA_ARGS__##,two | __VA_ARGS__##,name
//
#define test3(name,name2,...) #__VA_ARGS__ | name##__VA_ARGS__##name2 | name##__VA_ARGS__ | __VA_ARGS__##name2
#define test4(name,name2,...) #__VA_ARGS__ |  name,##__VA_ARGS__##,name2 | name,##__VA_ARGS__,name2 | name,__VA_ARGS__##,name2 | __VA_ARGS__##,name2

#define TOSTR2(...) #__VA_ARGS__
#define TOSTR(s) TOSTR2(s)

int main() {
  int i;
    int x = 2, y = 3;
    debug( "Flag" );
    if ( strcmp( buff, "Flag" ))
        fail(__LINE__);
    debug("X = %d\n", x);
    if ( strcmp( buff, "X = 2\n" ))
        fail(__LINE__);
    showlist(The first, second, and third items.);
    if ( strcmp( buff, "The first, second, and third items." ))
        fail(__LINE__);
    report(x>y, "x is %d but y is %d\n", x, y);
    if ( strcmp( buff, "x is 2 but y is 3\n" ))
        fail(__LINE__);
   if( !(((i=__LINE__),
            strcmp( TOSTR(test(one,x,y,z))            
                    , "\"x,y,z\" | onex,y,ztwo | onex,y,z | x,y,zone" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test(one))                  
                    , "\"\" | onetwo | one | one" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test1(one,x,y,z))           
                    , "\"x,y,z\" | one,x,y,z,two | one,x,y,z,two | one,x,y,z,two | x,y,z,one" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test1(one))                 
                    , "\"\" | one,two | one,two | one,two | one" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test3(red,blue,r,g,b,i,v))  
                    , "\"r,g,b,i,v\" | redr,g,b,i,vblue | redr,g,b,i,v | r,g,b,i,vblue" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test3(red,blue))            
                    , "\"\" | redblue | red | blue" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test4(red,blue,r,g,b,i,v))  
                    , "\"r,g,b,i,v\" | red,r,g,b,i,v,blue | red,r,g,b,i,v,blue | red,r,g,b,i,v,blue | r,g,b,i,v,blue" ) == 0 )
        && ((i=__LINE__),
            strcmp( TOSTR(test4(red,blue))            
                    , "\"\" | red,blue | red,blue | red,blue | blue" ) == 0 )))
        fail(i);
    _PASS;
}
