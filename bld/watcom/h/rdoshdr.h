/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2021 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  RDOS type definition file
*
****************************************************************************/


#ifndef _RDOSHDR_H
#define _RDOSHDR_H

#pragma pack( __push, 1 )

#define RDOS_OBJECT_KERNEL          0
#define RDOS_OBJECT_FONT            1
#define RDOS_OBJECT_SIMPLE_DEVICE   2
#define RDOS_OBJECT_SHUTDOWN        3
#define RDOS_OBJECT_OLD_FILE        6
#define RDOS_OBJECT_COMMAND         7
#define RDOS_OBJECT_SET             8
#define RDOS_OBJECT_PATH            9

#define RDOS_OBJECT_DOS_DEVICE      16
#define RDOS_OBJECT_FILE            17

#define RDOS_OBJECT_DEVICE16        18
#define RDOS_OBJECT_DEVICE32        19
#define RDOS_OBJECT_LONG_MODE       20

#define RDOS_SIGN    0x5A1E75D4

typedef struct TRdosObjectHeader
{
    long sign;
    long len;
    short type;
    unsigned short crc;
} TRdosObjectHeader;

typedef struct TRdosSimpleDeviceHeader
{
    short StartIp;
} TRdosSimpleDeviceHeader;

typedef struct TRdosDosDeviceHeader
{
    short Size;
    short Sel;
    short StartIp;
    char NameParam;
} TRdosDosDeviceHeader;

typedef struct TRdosDevice16Header
{
    long Size;
    short CodeSize;
    short CodeSel;
    short DataSize;
    short DataSel;
    short StartIp;
    char NameParam;
} TRdosDevice16Header;

typedef struct TRdosDevice32Header
{
    long Size;
    long CodeSize;
    short CodeSel;
    long DataSize;
    short DataSel;
    long StartIp;
    char NameParam;
} TRdosDevice32Header;

typedef struct TRdosLongModeHeader
{
    long Size;
    long StartIp;
    long ImageBase;
    long ImageSize;
    long IdtBase;
    char NameParam;
} TRdosLongModeHeader;

typedef struct TRdosOldFileHeader
{
    char Base[8];
    char Ext[3];
    char Attrib;
    char Resv[10];
    short Time;
    short Date;
    short Cluster;
    int Size;
} TRdosOldFileHeader;

typedef struct TRdosFileHeader
{
    int Size;
    unsigned long LsbTime;
    unsigned long MsbTime;
    int FileSize;
    char Attrib;
    char FileName;
} TRdosFileHeader;

#pragma pack( __pop )

#endif

