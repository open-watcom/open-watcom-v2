/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2013 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  RDOS type definition file
*
****************************************************************************/


#ifndef _RDOSHDR_H
#define _RDOSHDR_H

#pragma pack( push, 1 )

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

#define RDOS_SIGN    0x5A1E75D4

typedef struct TRdosObjectHeader
{
    long sign;
    long len;
    short int type;
    unsigned short int crc;    
} TRdosObjectHeader;

typedef struct TRdosSimpleDeviceHeader
{
    short int StartIp;
} TRdosSimpleDeviceHeader;

typedef struct TRdosDosDeviceHeader
{
    short int Size;
    short int Sel;
    short int StartIp;
    char NameParam;
} TRdosDosDeviceHeader;

typedef struct TRdosDevice16Header
{
    long Size;
    short int CodeSize;
    short int CodeSel;
    short int DataSize;
    short int DataSel;
    short int StartIp;
    char NameParam;
} TRdosDevice16Header;

typedef struct TRdosDevice32Header
{
    long Size;
    long CodeSize;
    short int CodeSel;
    long DataSize;
    short int DataSel;
    long StartIp;
    char NameParam;
} TRdosDevice32Header;

typedef struct TRdosOldFileHeader
{
    char Base[8];
    char Ext[3];
    char Attrib;
    char Resv[10];
    short int Time;
    short int Date;
    short int Cluster;
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

#pragma pack( pop )

#endif

