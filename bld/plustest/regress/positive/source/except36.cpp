#include "fail.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

struct EXEIntErr : _CD {
    char* m_pErrMsg;
    int m_ObjID;
    EXEIntErr(const char* pErrMsg);
    EXEIntErr(const EXEIntErr& Err);
    ~EXEIntErr();
    const char * GetErrStr();
};

static int ObjID = 0;

EXEIntErr::EXEIntErr(const char* pErrMsg) {
    m_pErrMsg = 0;
    m_ObjID = ++ObjID;
    m_pErrMsg = strdup(pErrMsg);
}

EXEIntErr::EXEIntErr(const EXEIntErr& Err) : _CD(Err) {
    if( this == &Err ) fail(__LINE__);
    m_pErrMsg = 0;
    m_ObjID = ++ObjID;
    if ( Err.m_pErrMsg )
	m_pErrMsg = strdup(Err.m_pErrMsg);
}

EXEIntErr::~EXEIntErr() {
    if ( m_pErrMsg )
	free(m_pErrMsg);
    m_pErrMsg = (char*)-1;
}

const char * EXEIntErr::GetErrStr() {
    return (const char *)m_pErrMsg;
}

void CauseException() {
    char *Msg = "AA";
    throw EXEIntErr(Msg);
}

void CauseThenReThrow() {
    try {
	CauseException();
    } catch (EXEIntErr& Err) {
#if __WATCOM_REVISION__ < 7
	EXEIntErr Err2(Err);
	throw Err2;
#else
	throw Err;
#endif
    }
}

int main() {
    try {
	CauseThenReThrow();
    } catch (EXEIntErr& Err) {
	const char* pMsg = Err.GetErrStr();
	if( !pMsg || strcmp(pMsg, "AA" ) ) fail(__LINE__);
    } 
    _PASS;
}
