/* $Log:        SCANNER.C $
Revision 1.1  92/08/20  15:50:28  Anthony_Scian
.

// Revision 1.1  1992/08/20  17:14:07  peter
// Initial revision
//
 */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "globals.h"
#include "scanner.h"

Scanner::Scanner(streambuf &i) : in(i) {
    curLine = 1;
    buffer = tokBuf = cursor = limit = NULL;
    bufLen = 0;
}

void Scanner::fatal(char *msg){
    cerr << "line " << curLine << ", near \"" << tokStr() << "\": "
        << msg << endl;
    exit(1);
}

void memlcpy(char *dst, char *src, uint len){
    while(len-- > 0)
        *dst++ = *src++;
}

bool Scanner::fill(){
    uint limLen = limit - tokBuf;
    if((bufLen - limLen) < cChunkSize){
        char *buf = new char[bufLen = limLen + cChunkSize];
        cursor = &buf[cursor - tokBuf];
        if(buffer){
            memcpy(buf, tokBuf, limLen);
            delete buffer;
        }
        limit = &(tokBuf = buffer = buf)[limLen];
    } else if((bufLen - (limit - buffer)) < cChunkSize){
        cursor = &buffer[cursor - tokBuf];
        memlcpy(buffer, tokBuf, limLen);
        limit = &(tokBuf = buffer)[limLen];
    }
    limit = &limit[in.sgetn(limit, cChunkSize)];
    return (bool) (limit > cursor);
}

const char sentinel[7] = {'/', '*', '!', 's', 'c', 'a', 'n'};

void Scanner::copy(ostream &o){
    if(fileName)
        o << "\n#line " << curLine << "\n";
    for(;; ++cursor){
        if((limit - cursor) < sizeof(sentinel)){
            o.write(tokBuf, cursor - tokBuf);
            tokBuf = cursor;
            fill();
            if((limit - cursor) < sizeof(sentinel)){
                o.write(cursor, limit - cursor);
                tokBuf = cursor = limit;
                return;
            }
        }
        if(memcmp(cursor, sentinel, sizeof(sentinel)) == 0){
            o.write(tokBuf, cursor - tokBuf);
            cursor += sizeof(sentinel);
            return;
        }
        if(*cursor == '\n')
            ++curLine;
    }
}

int Scanner::scan(){
    for(;; ++cursor){
        if(!need(1))
            return tEOF;
        if(!isspace(*cursor))
            break;
        if(*cursor == '\n')
            ++curLine;
    }
    tokLine = curLine;
    tokBuf = cursor;
    if(!isascii(*cursor)){
        ++cursor;
        return tOther;
    } else if(isalpha(*cursor)){
        do {
            ++cursor;
        } while(need(1) && isalnum(*cursor));
        return tIdent;
    } else if(*cursor == '\"'){
        for(;;){
            ++cursor;
            if(!need(1) || *cursor == '\n'){
                cursor = &tokBuf[1];
                return tOther;
            } else if(*cursor == '\"')
                break;
        }
        ++cursor;
        return tString;
    } else if(*cursor == '['){
        for(;;){
            ++cursor;
            if(!need(1) || *cursor == '\n'){
                cursor = &tokBuf[1];
                return tOther;
            } else if(*cursor == ']')
                break;
        }
        ++cursor;
        return tCClass;
    } else if(*cursor == '{'){
        for(int depth = 0;; ++cursor){
            if(!need(1)){
                cursor = &tokBuf[1];
                return tOther;
            } else if(*cursor == '{'){
                ++depth;
            } else if(*cursor == '}'){
                if(--depth == 0)
                    break;
            }
            if(*cursor == '\n')
                ++curLine;
        }
        ++cursor;
        return tCode;
    } else {
        switch(*cursor++){
        case '(': return tLParen;
        case ')': return tRParen;
        case '*':
            if(need(1) && *cursor == '/'){
                ++cursor;
                return tEnd;
            } else {
                return t0orMore;
            }
        case '+': return t1orMore;
        case '?': return tOptional;
        case '|': return tChoice;
        case '=': return tEqual;
        case ';': return tSemi;
        case ':': return tColon;
        case '!': return tBang;
        case '/': return tContext;
        case '\\': return tDiff;
        default:  return tOther;
        }
    }
}
