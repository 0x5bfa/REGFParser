// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryParser.h"

DWORD dwAbsoluteCurrentHiveBinOffset = 0;


int wmain(int argc, LPWSTR argv[]) {

    if (argc == 1 || CHECK_RETURN(wcscmp(argv[1], L"/?"))) {

        wprintf(L"\nRegistryParser [filename]\n    filename    Specify registry hive file.\n\n");
        return FAILURE;
    }

    HANDLE hFile = NULL;

    FILE_HEADER FileHeader = { 0 };
    HIVE_BIN_HEADER HiveBinHeader = { 0 };

    if ((hFile = CreateFileW(argv[1], GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE) {

        wprintf(L"ReadFile failed with 0x%X.\n", GetLastError());
        return FAILURE;
    }

    // Parse 0x1000 bytes
    if (ParseFileHeader(hFile, &FileHeader) == FAILURE) return FAILURE;

    // Parse 0x20 bytes
    if (ParseHiveBinHeader(hFile, &HiveBinHeader) == FAILURE) return FAILURE;

    dwAbsoluteCurrentHiveBinOffset = FILE_HEADER_SIZE;

    // Parse root key of a hive bin
    if (ParseCell(hFile, dwAbsoluteCurrentHiveBinOffset + FileHeader.dwRootCellOffset) == FAILURE) return FAILURE;

    return SUCCESS;
}


BOOL ParseFileHeader(HANDLE hFile, PFILE_HEADER pBaseBlock) {

    BYTE byReadData[4096] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 4096;
    DWORD nReadedSize = 0;

    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0X%x in ParseFileHeader().\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };
    CHAR szTempSigneture[5] = "";

    pReadedData = byReadData;

    // Regf signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    pBaseBlock->lpRegfSigneture = szTempSigneture;
    pReadedData += 4;

    // Primary sequence number
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwPrimarySequenceNumber = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Secondary sequence number
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwSecondarySequenceNumber = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Last written timestamp
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastWrittenTimeStamp.dwLowDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastWrittenTimeStamp.dwHighDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Major version
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwWriterMajorVersion = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Minor version
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwWriterMinorVersion = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // File type
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwFileType = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // File format
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwFileFormat = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Root cell offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwRootCellOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Hive bins data size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwHiveBinsDataSize = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Clustering factor
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwClusteringFactor = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // File name
    CHAR szTempFileName[64];
    for (int i = 0; i < 64; i++) szTempFileName[i] = (CHAR)pReadedData[i];
    pReadedData += 64;

    // Resource manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidRmId);
    WCHAR szRmGuidString[40] = L"";
    GuidToWchar(szRmGuidString, &pBaseBlock->guidRmId);
    pReadedData += 16;


    // Log file guid
    ByteToGuid(pReadedData, &pBaseBlock->guidLogId);
    WCHAR szLogGuidString[40] = L"";
    GuidToWchar(szLogGuidString, &pBaseBlock->guidLogId);
    pReadedData += 16;

    // Guid flags
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwGuidFlags = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Transaction manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidTmId);
    WCHAR szTmGuidString[40] = L"";
    GuidToWchar(szTmGuidString, &pBaseBlock->guidTmId);
    pReadedData += 16;

    // Guid signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    pReadedData += 4;

    // Last reorganized time
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastReorganizedTime.dwLowDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastReorganizedTime.dwHighDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Offline registry signeture
    pReadedData += 4;

    // Offline registry flags
    pReadedData += 4;

    // Reserved from (offset)184 to 184 + (size)324
    pReadedData += 324;

    // Check sum
    pReadedData += 4;

    // Serialization time stamp
    pReadedData += 8;

    // Reserved size
    pReadedData += 3520;

    // ThawTmId
    pReadedData += 16;

    // ThawRmId
    pReadedData += 16;

    // ThawLogId
    pReadedData += 16;

    // Boot type
    pReadedData += 4;

    // Boot recover
    pReadedData += 4;

    return SUCCESS;
}


BOOL ParseHiveBinHeader(HANDLE hFile, PHIVE_BIN_HEADER pHBin) {

    SetFilePointer(hFile, FILE_HEADER_SIZE, NULL, FILE_BEGIN);

    BYTE byReadData[32] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 32;
    DWORD nReadedSize = 0;

    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0X%x in ParseHiveBinHeader().\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };
    CHAR szTempSigneture[5] = "";

    pReadedData = byReadData;

    wprintf(L"Hive bin Header:\n");

    // Regf signeture
    for (int i = 0; i < 4; i++) pHBin->szHiveBinSigneture[i] = pReadedData[i];
    pReadedData += 4;

    wprintf(L"    Signeture:                         %hs\n", pHBin->szHiveBinSigneture);

    // Relative offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pHBin->dwRelativeOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Relative offset:                   0x%X\n", pHBin->dwRelativeOffset);

    // Size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pHBin->dwSize = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Size:                              0x%X\n", pHBin->dwSize);

    // Reserved
    pReadedData += 8;

    // Time stamp
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pHBin->TimeStamp.dwLowDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pHBin->TimeStamp.dwHighDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;

    SYSTEMTIME stTimeStamp;
    FileTimeToSystemTime(&pHBin->TimeStamp, &stTimeStamp);
    wprintf(L"    Time stamp:                        %04d/%02d/%02d %02d:%02d\n"
        , stTimeStamp.wYear, stTimeStamp.wMonth, stTimeStamp.wDay
        , stTimeStamp.wHour, stTimeStamp.wMinute);

    // Spare (or MemAlloc field)
    pReadedData += 4;

    wprintf(L"\n");

    return SUCCESS;
}


BOOL ParseCell(HANDLE hFile, DWORD dwAbsoluteCellOffset) {

    SetFilePointer(hFile, dwAbsoluteCellOffset, NULL, FILE_BEGIN);

    BYTE byReadData[6] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 6;
    DWORD nReadedSize = 0;

    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseCell().\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };
    CHAR szCellSigneture[3] = "";

    pReadedData = byReadData;

    wprintf(L"\nCell:(Abs:0x%04X)[%02X][%02X][%02X][%02X][%02X][%02X]\n", dwAbsoluteCellOffset, pReadedData[0], pReadedData[1], pReadedData[2], pReadedData[3], pReadedData[4], pReadedData[5]);

    // Cell size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    DWORD dwSize = abs(*(DWORD*)byDwordArray);
    pReadedData += 4;

    wprintf(L"    Size:                              0x%X\n", dwSize);

    // Cell signeture
    for (int i = 0; i < 2; i++) szCellSigneture[i] = pReadedData[i];
    pReadedData += 2;

    wprintf(L"    Signeture:                         %hs\n", szCellSigneture);

    dwAbsoluteCellOffset += (HBIN_HEADER_BIN_SIZE_LEN + HBIN_HEADER_BIN_SIGNETURE_LEN);

    if (CHECK_RETURN(StrCmpA(szCellSigneture, "li"))) {

    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "lf"))) {

        PFAST_LEAF pFastLeaf = NULL;
        if ((pFastLeaf = (PFAST_LEAF)calloc(1, sizeof(FAST_LEAF))) == NULL) return FAILURE;
        pFastLeaf->dwSize = dwSize;
        if (ParseFastLeaf(hFile, pFastLeaf, dwAbsoluteCellOffset) == FAILURE) return FAILURE;
    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "lh"))) {

        PHASH_LEAF pHashLeaf = NULL;
        if ((pHashLeaf = (PHASH_LEAF)calloc(1, sizeof(HASH_LEAF))) == NULL) return FAILURE;
        pHashLeaf->dwSize = dwSize;
        if (ParseHashLeaf(hFile, pHashLeaf, dwAbsoluteCellOffset) == FAILURE) return FAILURE;
    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "ri"))) {

    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "nk"))) {

        PKEY_NODE pKeyNode = NULL;
        if ((pKeyNode = (PKEY_NODE)calloc(1, sizeof(KEY_NODE))) == NULL) return FAILURE;
        pKeyNode->dwSize = dwSize;
        if (ParseKeyNode(hFile, pKeyNode, dwAbsoluteCellOffset) == FAILURE) return FAILURE;
        free(pKeyNode);
    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "vk"))) {

        PKEY_VALUE pKeyValue = NULL;
        if ((pKeyValue = (PKEY_VALUE)calloc(1, sizeof(KEY_VALUE))) == NULL) return FAILURE;
        pKeyValue->dwSize = dwSize;
        pKeyValue->dwAbsoluteHiveBinOffset = dwAbsoluteCurrentHiveBinOffset;
        if (ParseKeyValue(hFile, pKeyValue, dwAbsoluteCellOffset) == FAILURE) return FAILURE;
        free(pKeyValue);
    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "sk"))) {

        PKEY_SECURITY pKeySecurity = NULL;
        if ((pKeySecurity = (PKEY_SECURITY)calloc(1, sizeof(KEY_SECURITY))) == NULL) return FAILURE;
        pKeySecurity->dwSize = dwSize;
        if (ParseKeySecurity(hFile, pKeySecurity, dwAbsoluteCellOffset) == FAILURE) return FAILURE;
        free(pKeySecurity);
    }
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "db"))) {

    }
    else return FAILURE;

    return SUCCESS;
}


BOOL ParseKeyValueList(HANDLE hFile, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadData = NULL;
    DWORD nReadedSize = 0;

    if ((pReadData = (PBYTE)calloc(4, sizeof(BYTE))) == NULL) return FAILURE;

    if (ReadFile(hFile, pReadData, 4, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyValueList().\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };

    wprintf(L"\nValueList:(Abs:0x%04X)\n", dwAbsoluteOffset);

    // offset list size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadData[i];
    DWORD dwSize = abs(*(DWORD*)byDwordArray);
    pReadData += 4;

    wprintf(L"    Size:                              0x%X\n", dwSize);

    LPDWORD pOffsets = NULL;

    // Offset list
    if ((pReadData = (PBYTE)calloc(dwSize - 4, sizeof(BYTE))) == NULL) return FAILURE;

    if ((pOffsets = (LPDWORD)calloc((dwSize - 4) / 4, sizeof(DWORD))) == NULL) return FAILURE;

    SetFilePointer(hFile, dwAbsoluteOffset + 4, NULL, FILE_BEGIN);

    if (ReadFile(hFile, pReadData, dwSize - 4, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyValueList().\n", GetLastError());
        return FAILURE;
    }

    // Parse
    for (int j = 0; j < ((dwSize - 4) / 4); j++) {

        // Offset
        for (int i = 0; i < 4; i++) byDwordArray[i] = pReadData[i];
        pOffsets[j] = abs(*(DWORD*)byDwordArray);
        pReadData += 4;
        wprintf(L"    Offset #%02d:                        0x%X\n", j, pOffsets[j]);
    }

    // Move
    for (int i = 0; i < ((dwSize - 4) / 4); i++) {

        if (pOffsets[i] == 0x0) break;

        if (ParseCell(hFile, dwAbsoluteCurrentHiveBinOffset + pOffsets[i]) == FAILURE) {

            wprintf(L"ParseCell failed in ParseKeyValueList().\n");
            return FAILURE;
        }
    }

    return SUCCESS;
}