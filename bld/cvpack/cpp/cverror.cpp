/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "common.hpp"
#include "cverror.hpp"

void CVpackError::ErrorPrint() const
/**********************************/
{
    cerr << "CVPACK : ";
}

void RetrieveError::ErrorPrint() const
/************************************/
{
    CVpackError::ErrorPrint();
    cerr << "cannot retrieve debugging information : " << endl;
}

void WriteError::ErrorPrint() const
/*********************************/
{
    CVpackError::ErrorPrint();
    cerr << "error writing to file " << _fileName << endl;
}

FileError::FileError(const char* fName)
        : _fileName(fName)
/*************************************/
{
}

void FileError::ErrorPrint() const
/********************************/
{
    RetrieveError::ErrorPrint();
    cerr << "error reading from " << _fileName << endl;
}

void DebugInfoError::ErrorPrint() const
/*************************************/
{
    RetrieveError::ErrorPrint();
    cerr << "fail to retrieve debugging information." << endl;
}

void CVSigError::ErrorPrint() const
/*********************************/
{
    RetrieveError::ErrorPrint();
    cerr << "cannot detect CV signature / invalid CV signature." << endl;
}

OutOfMemory::OutOfMemory( const size_t reqSize )
        : _requestSize(reqSize)
/**********************************************/
{
}

void OutOfMemory::ErrorPrint() const
/**********************************/
{
    CVpackError::ErrorPrint();
    cerr << "out of memory : requested size = " << _requestSize << endl;
}

InternalError::InternalError( const char* msg )
        : _debugMsg(msg)
/*********************************************/
{
}

void InternalError::ErrorPrint() const
/************************************/
{
    cerr << "Internal Error : " << _debugMsg << endl;
}

MiscError::MiscError( const char* msg )
        : _errorMsg(msg)
/*********************************************/
{
}

void MiscError::ErrorPrint() const
/**********************************/
{
    CVpackError::ErrorPrint();
    cerr << _errorMsg << endl;
}
