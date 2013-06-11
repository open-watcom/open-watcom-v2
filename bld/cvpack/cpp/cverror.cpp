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
    std::cerr << "CVPACK : ";
}

void RetrieveError::ErrorPrint() const
/************************************/
{
    CVpackError::ErrorPrint();
    std::cerr << "cannot retrieve debugging information : " << std::endl;
}

void WriteError::ErrorPrint() const
/*********************************/
{
    CVpackError::ErrorPrint();
    std::cerr << "error writing to file " << _fileName << std::endl;
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
    std::cerr << "error reading from " << _fileName << std::endl;
}

void DebugInfoError::ErrorPrint() const
/*************************************/
{
    RetrieveError::ErrorPrint();
    std::cerr << "fail to retrieve debugging information." << std::endl;
}

void CVSigError::ErrorPrint() const
/*********************************/
{
    RetrieveError::ErrorPrint();
    std::cerr << "cannot detect CV signature / invalid CV signature." << std::endl;
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
    std::cerr << "out of memory : requested size = " << _requestSize << std::endl;
}

InternalError::InternalError( const char* msg )
        : _debugMsg(msg)
/*********************************************/
{
}

void InternalError::ErrorPrint() const
/************************************/
{
    std::cerr << "Internal Error : " << _debugMsg << std::endl;
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
    std::cerr << _errorMsg << std::endl;
}
