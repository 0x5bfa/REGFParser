// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#pragma once

#include <stdio.h>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")
#pragma warning (disable: 4996)

#define SUCCESS 1
#define FAILURE 0
#define CHECK_RETURN ((x) == 0)

typedef struct _BASE_BLOCK {

    WCHAR szRegfSigneture[5];
    DWORD dwPrimarySequenceNumber;
    DWORD dwSecondarySequenceNumber;
    FILETIME ftLastWrittenTimeStamp;
    DWORD dwWriterMajorVersion;
    DWORD dwWriterMinorVersion;
    DWORD dwFileType;
    DWORD dwFileFormat;
    DWORD dwRootCellOffset;
    DWORD dwHiveBinsDataSize;
    DWORD dwClusteringFactor;
    WCHAR szFileName[64];

    GUID guidRmId;
    GUID guidLogId;
    DWORD dwGuidFlags;
    GUID guidTmId;
    WCHAR szGuidSigneture[5];
    FILETIME ftLastReorganizedTime;

    WCHAR szOffRegSigneture[5];
    DWORD dwOffRegFlags;
    FILETIME ftSerializationTime;

}BASE_BLOCK, * PBASE_BLOCK;

typedef struct _HBIN {

    WCHAR szHBinSigneture[5];
    DWORD dwRelativeOffset;
    DWORD dwSize;
    FILETIME TimeStamp;

}HBIN, *PHBIN;


typedef struct _CELL {

    DWORD dwSize; // (Nebative: Assigned, Positive: Not assigned)
    WCHAR szHBinSigneture[3];


}CELL, *PCELL;