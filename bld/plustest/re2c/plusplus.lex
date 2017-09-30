#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned char bool;

#define true    (0 == 0)
#define false   (!true)

#define BSIZE   2048

#define CURSOR   ch
#define LOADCURSOR  ch = *cursor;
#define ADVANCE     cursor++;
#define BACK(n)     cursor -= (n);
#define CHECK(n)    if((s->lim - cursor) < (n)){cursor = fill(s, cursor);}
#define MARK(n)     s->ptr = cursor; sel = (n);
#define REVERT   cursor = s->ptr;
#define MARKER   sel

#define RETURN(i)   {s->cur = cursor; return i;}

typedef struct Scanner Scanner;

struct Scanner {
    int        fd;
    uchar     *bot, *tok, *ptr, *cur, *lim, *top;
    uint       lineNum;
    ulong     linePos;
    ulong     pos;
};

uchar *fill(s, cursor)
    Scanner *s;
    uchar *cursor;
{
    uint cnt = s->tok - s->bot;
    if(cnt){
    memcpy(s->bot, s->tok, s->lim - s->tok);
    s->tok = s->bot;
    s->ptr -= cnt;
    cursor -= cnt;
    s->lim -= cnt;
    }
    if((s->top - s->lim) < BSIZE){
    uchar *buf = (uchar*) malloc(((s->lim - s->bot) + BSIZE)*sizeof(uchar));
    memcpy(buf, s->tok, s->lim - s->tok);
    s->tok = buf;
    s->ptr = &buf[s->ptr - s->bot];
    cursor = &buf[cursor - s->bot];
    s->lim = &buf[s->lim - s->bot];
    s->top = &s->lim[BSIZE];
    free(s->bot);
    s->bot = buf;
    }
    if((cnt = read(s->fd, (char*) s->lim, BSIZE)) != BSIZE)
    s->lim[cnt] = '\0';
    s->lim += cnt;
    return cursor;
}

int scan(s)
    Scanner *s;
{
    uchar *cursor = s->cur;
    uint depth;
    uint sel;
    uchar ch;
    s->pos += (cursor - s->tok); s->tok = cursor;
/*!scan

all = [\000-\377];
eof = [\000];
any = all\eof;
digit   = [0-9];
letter  = [a-zA-Z];

std :  "(*"
        {
       s->pos += 2; s->tok = cursor; depth = 1;
       goto comment;
        }
    ! digit +   {RETURN(1);}
    ! digit + / ".."       {RETURN(1);}
    ! [0-7] + "B"          {RETURN(2);}
    ! [0-7] + "C"          {RETURN(3);}
    ! digit [0-9A-F] * "H" {RETURN(4);}
    ! digit + "." digit * ("E" ([+-]) ? digit +) ? {RETURN(5);}
    ! ['] (any\[\n']) * [']    |
      ["] (any\[\n"]) * ["]
              {RETURN(6);}

    ! "#"                  {RETURN(7);}
    ! "&"                  {RETURN(8);}
    ! "("                  {RETURN(9);}
    ! ")"                  {RETURN(10);}
    ! "*"                  {RETURN(11);}
    ! "+"                  {RETURN(12);}
    ! ","                  {RETURN(13);}
    ! "-"                  {RETURN(14);}
    ! "."                  {RETURN(15);}
    ! ".."                 {RETURN(16);}
    ! "/"                  {RETURN(17);}
    ! ":"                  {RETURN(18);}
    ! ":="                 {RETURN(19);}
    ! ";"                  {RETURN(20);}
    ! "<"                  {RETURN(21);}
    ! "<="                 {RETURN(22);}
    ! "<>"                 {RETURN(23);}
    ! "="                  {RETURN(24);}
    ! ">"                  {RETURN(25);}
    ! ">="                 {RETURN(26);}
    ! "["                  {RETURN(27);}
    ! "]"                  {RETURN(28);}
    ! "^"                  {RETURN(29);}
    ! "{"                  {RETURN(30);}
    ! "|"                  {RETURN(31);}
    ! "}"                  {RETURN(32);}
    ! "~"                  {RETURN(33);}

    ! "AND"                {RETURN(34);}
    ! "ARRAY"              {RETURN(35);}
    ! "BEGIN"              {RETURN(36);}
    ! "BY"                 {RETURN(37);}
    ! "CASE"               {RETURN(38);}
    ! "CONST"              {RETURN(39);}
    ! "DEFINITION"         {RETURN(40);}
    ! "DIV"                {RETURN(41);}
    ! "DO"                 {RETURN(42);}
    ! "ELSE"               {RETURN(43);}
    ! "ELSIF"              {RETURN(44);}
    ! "END"                {RETURN(45);}
    ! "EXIT"               {RETURN(46);}
    ! "EXPORT"             {RETURN(47);}
    ! "FOR"                {RETURN(48);}
    ! "FROM"               {RETURN(49);}
    ! "IF"                 {RETURN(50);}
    ! "IMPLEMENTATION"     {RETURN(51);}
    ! "IMPORT"             {RETURN(52);}
    ! "IN"                 {RETURN(53);}
    ! "LOOP"               {RETURN(54);}
    ! "MOD"                {RETURN(55);}
    ! "MODULE"             {RETURN(56);}
    ! "NOT"                {RETURN(57);}
    ! "OF"                 {RETURN(58);}
    ! "OR"                 {RETURN(59);}
    ! "POINTER"            {RETURN(60);}
    ! "PROCEDURE"          {RETURN(61);}
    ! "QUALIFIED"          {RETURN(62);}
    ! "RECORD"             {RETURN(63);}
    ! "REPEAT"             {RETURN(64);}
    ! "RETURN"             {RETURN(65);}
    ! "SET"                {RETURN(66);}
    ! "THEN"               {RETURN(67);}
    ! "TO"                 {RETURN(68);}
    ! "TYPE"               {RETURN(69);}
    ! "UNTIL"              {RETURN(70);}
    ! "VAR"                {RETURN(71);}
    ! "WHILE"              {RETURN(72);}
    ! "WITH"               {RETURN(73);}

    ! letter (letter | digit) *    {RETURN(74);}

    ! [ \t]
        {
       s->pos += 1; s->tok = cursor;
       goto std;
        }

    ! "\n"
        {
       s->linePos = s->pos++; s->lineNum++; s->tok = cursor;
       goto std;
        }

    ! eof
        { exit(0); }

    ! any
        {
       printf("illegal character: %c\n", *s->tok);
       s->pos += 1; s->tok = cursor;
       goto std;
        }
    ;

comment : "*)"
        {
       s->pos += 2; s->tok = cursor;
       if(--depth == 0)
           goto std;
       else
           goto comment;
        }
    ! "(*"
        {
       s->pos += 2; s->tok = cursor; ++depth;
       goto comment;
        }
    ! "\n"
        {
       s->linePos = s->pos++; s->lineNum++; s->tok = cursor;
       goto comment;
        }
    ! eof
        { exit(0); }
        ! any
        {
       s->pos += 1; s->tok = cursor;
       goto comment;
        }
    ;
*/
}

void putStr(o, s, l)
    FILE *o;
    char *s;
    uint l;
{
    while(l-- > 0)
    putc(*s++, o);
}

main(){
    Scanner in;
    int token;
    
    memset((char*) &in, 0, sizeof(in));
    in.fd = 0;
    for(;;){
	token = scan(&in);
	putchar( '<' );
	putStr(stdout, (char*) in.tok, in.cur - in.tok);
	printf( "> (%d) ", token );
    }
}
