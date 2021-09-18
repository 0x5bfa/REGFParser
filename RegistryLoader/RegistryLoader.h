// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#pragma once

#include <stdio.h>
#include <Windows.h>
#include "HiveData.h"
#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")
#pragma warning (disable: 4996)


// Defines

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

#define FILE_HEADER_SIZE     0x1000
#define HIVE_BIN_HEADER_SIZE 0x20

#define OFFSET_TYPE_IS_CELL       0x001
#define OFFSET_TYPE_IS_VALUE_LIST 0x002

// global variables

extern DWORD dwAbsoluteCurrentHiveBinOffset;



// regf
typedef struct _FILE_HEADER {

    LPSTR lpRegfSigneture;
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

}FILE_HEADER, *PFILE_HEADER;

// hbin
typedef struct _HIVE_BIN_HEADER {

    CHAR szHiveBinSigneture[5];
    DWORD dwRelativeOffset;
    DWORD dwSize;
    FILETIME TimeStamp;

}HIVE_BIN_HEADER, *PHIVE_BIN_HEADER;

// li
typedef struct _INDEX_LEAF {

    DWORD dwSize;
    WCHAR szIndexLeafSigneture[3];

}INDEX_LEAF, *PINDEX_LEAF;

// lf
typedef struct _FAST_LEAF {

    DWORD dwSize;
    WCHAR szFastLeafSigneture[3];
    DWORD nElements;

}FAST_LEAF, *PFAST_LEAF;

// lh
typedef struct _HASH_LEAF {

    DWORD dwSize;
    WCHAR szHashLeafSigneture[3];

}HASH_LEAF, *PHASH_LEAF;

// ri
typedef struct _INDEX_ROOT {

    DWORD dwSize;
    WCHAR szIndexRootSigneture[3];

}INDEX_ROOT, *PINDEX_ROOT;

// nk
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

// vk
typedef struct _KEY_VALUE {

    DWORD dwAbsoluteHiveBinOffset;

    DWORD dwSize;
    WCHAR szValueKeySigneture[3];
    DWORD dwValueNameSize;
    DWORD dwDataSize;
    DWORD dwDataOffset;
    DWORD dwDataType;
    DWORD dwFlags;
    DWORD dwValueNameString;

}KEY_VALUE, *PKEY_VALUE;

// sk
typedef struct _KEY_SECURITY {

    DWORD dwSize;
    WCHAR szSecurityKeySigneture[3];
    DWORD dwFlink;
    DWORD dwBlink;
    DWORD dwReferenceCount;
    DWORD dwSecurityDescriptorSize;


}KEY_SECURITY, *PKEY_SECURITY;

// bd
typedef struct _BIG_DATA {

    DWORD dwSize;
    WCHAR szBigDataSigneture[3];

}BIG_DATA, *PBIG_DATA;

// elements list
typedef struct _ELEMENT {

    DWORD dwKeyNodeOffset;
    CHAR szNameHint[5];

}ELEMENT, * PELEMENT;



// Functions

BOOL ParseCell(HANDLE hFile, DWORD dwAbsoluteCellOffset);
BOOL ParseKeyValueList(HANDLE hFile, DWORD dwAbsoluteOffset);

BOOL ParseKeyNode(HANDLE hFile, PKEY_NODE pKeyNode, DWORD dwAbsoluteOffset);
BOOL ParseKeySecurity(HANDLE hFile, PKEY_SECURITY pSecurityKey, DWORD dwAbsoluteOffset);
BOOL ParseFastLeaf(HANDLE hFile, PFAST_LEAF pFastLeaf, DWORD dwAbsoluteOffset);
BOOL ParseKeyValue(HANDLE hFile, PKEY_VALUE pValueKey, DWORD dwAbsoluteOffset);

BOOL ByteToGuid(PBYTE pData, GUID* guidResultGuid);
BOOL CharToWchar(WCHAR* szWideString, CHAR* szSingleString, DWORD dwSizeToCopy);
BOOL ByteToWchar(WCHAR* szWideString, BYTE* pData, DWORD dwSizeToCopy);
BOOL GuidToWchar(WCHAR* szWideString, GUID* Guid);
