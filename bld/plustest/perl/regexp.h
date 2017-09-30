/*
 * version 1.6
 * Regexp is a class that encapsulates the Regular expression
 * stuff. Hopefully this means I can plug in different regexp
 * libraries without the rest of my code needing to be changed.
 * Written by Jim Morris,  jegm@sgi.com
 */
#ifndef _REGEXP_H
#define _REGEXP_H
#include    <iostream.h>
#include    <stdlib.h>
#include    <malloc.h>
#include    <string.h>
#include    <assert.h>
#include    <ctype.h>

#include    "regex.h"

/*
 * Note this is an inclusive range where it goes
 * from start() to, and including, end()
 */
class Range
{
private:
    int st, en;
    
public:
    Range()
    {
        st=0; en= -1;
    }
    
    Range(int s, int e)
    {
        st= s; en= e;
    }
    
    int start(void) const { return st;}
    int end(void) const { return en;}
    int length(void) const { return (en-st)+1;}
};

class Regexp
{
public:
    enum options {def=0, nocase=1};
    
private:
    regexp *repat;
    const char *target; // only used as a base address to get an offset
    int res;
    int iflg;
#ifndef __TURBOC__
    void strlwr(char *s)
    {
        while(*s){
            *s= (char) tolower(*s);
            s++;
        }
    }
#endif    
public:
    Regexp(const char *rege, int ifl= 0)
    {
        iflg= ifl;
        if(iflg == nocase){ // lowercase fold
            char *r= new char[strlen(rege)+1];
            strcpy(r, rege);
            strlwr(r);
            if((repat=regcomp(r)) == NULL){
                cerr << "regcomp() error" << endl;
                exit(1);
            }
            delete [] r;
        }else{
            if((repat=regcomp (rege)) == NULL){
                cerr << "regcomp() error" << endl;
                exit(1);
            }
        }
    }
    
    ~Regexp()
    {
        free(repat);
    }    

    int match(const char *targ)
    {
        int res;
        if(iflg == nocase){ // fold lowercase
            char *r= new char[strlen(targ)+1];
            strcpy(r, targ);
            strlwr(r);
            res= regexec(repat, r); 
            target= r; // looks bad but is really ok, really
            delete [] r;
        }else{
            res= regexec(repat, targ);
            target= targ;
        }

        return ((res == 0) ? 0 : 1);
    }
    
    int groups(void) const
    {
        int res= 0;
        for (int i=0; i<NSUBEXP; i++) {
            if(repat->startp[i] == NULL) break;
            res++;
        }
        return res;
    }
    
    Range getgroup(int n) const
    {
        assert(n < NSUBEXP);
        return Range((int)(repat->startp[n] - (char *)target),
                     (int)(repat->endp[n] - (char *)target) - 1);
    }
};
#endif
