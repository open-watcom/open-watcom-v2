#include "stdafx.h"

int main()
{
    CString str1 = _T("abcd");
    CString str2 = _T("ABCD");

    CString strWork;
    strWork = str1 + str2;
    if( strWork != _T("abcdABCD") ) _fail;
    strWork = str1 + _T("ABCD");
    if( strWork != _T("abcdABCD") ) _fail;
    strWork = _T("abcd") + str2;
    if( strWork != _T("abcdABCD") ) _fail;
    strWork = 'a' + str2;
    if( strWork != _T("aABCD") ) _fail;
    strWork = str1 + 'A';
    if( strWork != _T("abcdA") ) _fail;
    strWork = L'a' + str2;
    if( strWork != _T("aABCD") ) _fail;
    strWork = str1 + L'A';
    if( strWork != _T("abcdA") ) _fail;

    strWork = str1;
    strWork += "abcd";
    strWork += L"ABCD";
    strWork += str2;
    strWork += 'a';
    strWork += L'A';
    if( strWork != _T("abcdabcdABCDABCDaA") ) _fail;

    strWork = str1;
    if( !(strWork == str1) ) _fail;
    if( strWork == str2 ) _fail;
    if( !(strWork == _T("abcd")) ) _fail;
    if( strWork == _T("ABCD") ) _fail;
    if( !(_T("abcd") == strWork) ) _fail;
    if( _T("ABCD") == strWork ) _fail;
    if( strWork == _T('a') ) _fail;
    if( _T('b') == strWork ) _fail;

    if( strWork != str1 ) _fail;
    if( !(strWork != str2) ) _fail;
    if( strWork != _T("abcd") ) _fail;
    if( !(strWork != _T("ABCD")) ) _fail;
    if( _T("abcd") != strWork ) _fail;
    if( !(_T("ABCD") != strWork) ) _fail;
    if( !(strWork != _T('a')) ) _fail;
    if( !(_T('b') != strWork) ) _fail;

    _PASS;
}
