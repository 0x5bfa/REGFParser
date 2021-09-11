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
#define CHECK_RETURN(x) ((x) == 0)

#define CELL_INDEX_LEAF   0x002
#define CELL_FAST_LEAF    0x004
#define CELL_HASH_LEAF    0x008
#define CELL_INDEX_ROOT   0x010
#define CELL_KEY_NODE     0x020
#define CELL_KEY_VALUE    0x040
#define CELL_KEY_SECURITY 0x080
#define CELL_BIG_DATA     0x100

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


typedef struct _KEY_NODE {

    DWORD dwSize;
    WCHAR szKeyNodeSigneture[3];
    DWORD dwFlags;
    FILETIME ftLastWrittenTime;
    DWORD dwAccessBits;
    DWORD dwParent;
    DWORD nSubKeys;
    DWORD nVolatileSubKeys;
    DWORD dwSubKeysListOffset;
    DWORD nKeyValues;
    DWORD dwKeyValuesListOffset;
    DWORD dwKeySecurityOffset;
    DWORD dwClassNameOffset;
    DWORD dwLargestSubKeyNameLength;
    DWORD dwLargestSubKeyClassLength;
    DWORD dwLargestValueNameLength;
    DWORD dwLargestValueDataSize;
    DWORD dwWorkVar;
    DWORD dwKeyNameLength;
    DWORD dwClassNameLength;
    DWORD dwKeyNameString;






}KEY_NODE, *PKEY_NODE;



// functions

BOOL ByteToGuid(PBYTE pData, GUID* guidResultGuid);
BOOL CharToWchar(WCHAR* szWideString, CHAR* szSingleString, DWORD dwSizeToCopy);
BOOL ByteToWchar(WCHAR* szWideString, BYTE* pData, DWORD dwSizeToCopy);
BOOL GuidToWchar(WCHAR* szWideString, GUID* Guid);

BOOL ParseKeyNodeCell(PKEY_NODE pKeyNode, HANDLE hFile, DWORD dwCellSize);
