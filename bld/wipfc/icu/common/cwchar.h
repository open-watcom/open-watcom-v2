/*****************************************************************************
*
*                          Open Watcom Project
*
* Copyright (c) 2018-2020 The Open Watcom Contributors. All Rights Reserved.
*
*****************************************************************************/
// (c) 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  cwchar.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001may25
*   created by: Markus W. Scherer
*
*   This file contains ICU-internal definitions of wchar_t operations.
*   These definitions were moved here from cstring.h so that fewer
*   ICU implementation files include wchar.h.
*/

#ifndef __CWCHAR_H__
#define __CWCHAR_H__

#include <stdlib.h>
#include "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#if U_HAVE_WCHAR_H
#   include <wchar.h>
#endif

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are part of the ANSI C standard, defined in wchar.h . */
#define uprv_wcscpy(wcstr1, wcstr2) U_STANDARD_CPP_NAMESPACE wcscpy(wcstr1, wcstr2)
#define uprv_wcscat(wcstr1, wcstr2) U_STANDARD_CPP_NAMESPACE wcscat(wcstr1, wcstr2)
#define uprv_wcslen(wcstr) U_STANDARD_CPP_NAMESPACE wcslen(wcstr)

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#define uprv_wcstombs(mbstr, wcstr, count) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, count)
#define uprv_mbstowcs(wcstr, mbstr, count) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, count)


#endif
