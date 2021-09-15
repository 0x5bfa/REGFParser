// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"

DWORD dwAbsoluteOffsetCurrentPointer = 0;

BOOL ParseFileHeader(HANDLE hFile, PFILE_HEADER pBaseBlock);
BOOL ParseHiveBinHeader(HANDLE hFile, PHIVE_BIN_HEADER pHBin);
BOOL ParseCell(HANDLE hFile, LPDWORD pCellSize, DWORD dwAbsoluteCellOffset);


int wmain(void) {

    HANDLE hFile = NULL;
    DWORD dwAbsoluteOffset = 0;

    FILE_HEADER FileHeader = { 0 };
    HIVE_BIN_HEADER HiveBinHeader = { 0 };
    KEY_NODE KeyNode = { 0 };
    SECURITY_KEY SecurityKey = { 0 };
    FAST_LEAF FastLeaf = { 0 };
    VALUE_KEY ValueKey = { 0 };

    if ((hFile = CreateFileW(L"C:\\Users\\T31068068\\Desktop\\BCD", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE) {

        wprintf(L"ReadFile failed with 0x%X.\n", GetLastError());
        return FAILURE;
    }

    // Parse 0x1000 bytes
    if (ParseFileHeader(hFile, &FileHeader) == FAILURE) {

        wprintf(L"ParseFileHeader failed.\n");
        return FAILURE;
    }

    // Parse 0x20 bytes
    if (ParseHiveBinHeader(hFile, &HiveBinHeader) == FAILURE) {

        wprintf(L"ParseHiveBinHeader failed.\n");
        return FAILURE;
    }

    // Parse root key of a hive bin and recurse
    DWORD dwCellSize = 0;
    DWORD dwCellType = REG_NONE;
    dwAbsoluteOffset = (FILE_HEADER_SIZE + HiveBinHeader.dwRelativeOffset + HIVE_BIN_HEADER_SIZE);
    if ((dwCellType = ParseCell(hFile, &dwCellSize, dwAbsoluteOffset)) == FAILURE) {

        wprintf(L"ParseCell failed.\n");
        return FAILURE;
    }

    dwAbsoluteOffset += (4 + 2); // cell size + cell signeture size

    do {
    
        switch (dwCellType) {

        case CELL_INDEX_LEAF:

            break;

        case CELL_FAST_LEAF:

            FastLeaf.dwSize = dwCellSize;
            if (ParseFastLeaf(&FastLeaf, hFile, dwAbsoluteOffset) == FAILURE) {

                wprintf(L"ParseFastLeaf failed.\n");
                return FAILURE;
            }
            break;

        case CELL_HASH_LEAF:

            break;

        case CELL_INDEX_ROOT:

            break;

        case CELL_KEY_NODE:

            KeyNode.dwSize = dwCellSize;
            if (ParseKeyNodeCell(hFile, &KeyNode, dwAbsoluteOffset) == FAILURE) {

                wprintf(L"ParseKeyNodeCell failed.\n");
                return FAILURE;
            }
            break;

        case CELL_KEY_VALUE:

            ValueKey.dwSize = dwCellSize;
            ValueKey.dwAbsoluteHiveBinOffset = (HiveBinHeader.dwRelativeOffset + FILE_HEADER_SIZE);
            if (ParseValueKey(hFile, &ValueKey, dwAbsoluteOffset) == FAILURE) {

                wprintf(L"ParseValueKey failed.\n");
                return FAILURE;
            }
            break;

        case CELL_KEY_SECURITY:

            SecurityKey.dwSize = dwCellSize;
            if(ParseSecurityKey(hFile, &SecurityKey, dwAbsoluteOffset) == FAILURE){

                wprintf(L"ParseSecurityKey failed.\n");
                return FAILURE;
            }
            break;

        case CELL_BIG_DATA:

            break;

        default: break;
        }

    } while (ParseCell(hFile, &dwCellSize, dwAbsoluteOffset) != FAILURE);

    return SUCCESS;
}


BOOL ParseFileHeader(HANDLE hFile, PFILE_HEADER pBaseBlock) {

    BYTE byReadData[4096] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 4096;
    DWORD nReadedSize = 0;

    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0X%x\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };
    CHAR szTempSigneture[5] = "";

    pReadedData = byReadData;

    wprintf(L"File Header:\n");

    // Regf signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    pBaseBlock->lpRegfSigneture = szTempSigneture;
    pReadedData += 4;

    wprintf(L"    Signeture:                         %hs\n", pBaseBlock->lpRegfSigneture);

    // Primary sequence number
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwPrimarySequenceNumber = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Secondary sequence number
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwSecondarySequenceNumber = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    SequenceNumber:                    {0x%X, 0x%X}\n", pBaseBlock->dwPrimarySequenceNumber, pBaseBlock->dwSecondarySequenceNumber);

    // Last written timestamp
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastWrittenTimeStamp.dwLowDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastWrittenTimeStamp.dwHighDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;

    SYSTEMTIME stLastWrittenTimeStamp = { 0 };
    FileTimeToSystemTime(&pBaseBlock->ftLastWrittenTimeStamp, &stLastWrittenTimeStamp);
    wprintf(L"    Last written time:                 %04d/%02d/%02d %02d:%02d\n"
        , stLastWrittenTimeStamp.wYear, stLastWrittenTimeStamp.wMonth, stLastWrittenTimeStamp.wDay
        , stLastWrittenTimeStamp.wHour, stLastWrittenTimeStamp.wMinute);

    // Major version
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwWriterMajorVersion = *(DWORD*)byDwordArray;
    pReadedData += 4;

    // Minor version
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwWriterMinorVersion = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Maj.Min                            %d.%d\n", pBaseBlock->dwWriterMajorVersion, pBaseBlock->dwWriterMinorVersion);

    // File type
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwFileType = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    File type:                         0x%X\n", pBaseBlock->dwFileType);

    // File format
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwFileFormat = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    File format:                       0x%X\n", pBaseBlock->dwFileFormat);

    // Root cell offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwRootCellOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Root cell offset:                  0x%X\n", pBaseBlock->dwRootCellOffset);

    // Hive bins data size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwHiveBinsDataSize = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Hive bins data:                    0x%X\n", pBaseBlock->dwHiveBinsDataSize);

    // Clustering factor
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwClusteringFactor = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Clustering factor:                 0x%X\n", pBaseBlock->dwClusteringFactor);

    // File name
    CHAR szTempFileName[64];
    for (int i = 0; i < 64; i++) szTempFileName[i] = (CHAR)pReadedData[i];
    pReadedData += 64;

    ByteToWchar(pBaseBlock->szFileName, szTempFileName, 64);
    wprintf(L"    File name:                         %s\n", pBaseBlock->szFileName);

    // Resource manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidRmId);
    WCHAR szRmGuidString[40] = L"";
    GuidToWchar(szRmGuidString, &pBaseBlock->guidRmId);
    pReadedData += 16;

    wprintf(L"    RmId:                              %s\n", szRmGuidString);

    // Log file guid
    ByteToGuid(pReadedData, &pBaseBlock->guidLogId);
    WCHAR szLogGuidString[40] = L"";
    GuidToWchar(szLogGuidString, &pBaseBlock->guidLogId);
    pReadedData += 16;

    wprintf(L"    LogId:                             %s\n", szLogGuidString);

    // Guid flags
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->dwGuidFlags = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Guid flags:                        0x%X\n", pBaseBlock->dwGuidFlags);

    // Transaction manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidTmId);
    WCHAR szTmGuidString[40] = L"";
    GuidToWchar(szTmGuidString, &pBaseBlock->guidTmId);
    pReadedData += 16;

    wprintf(L"    TmId:                              %s\n", szTmGuidString);

    // Guid signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    CharToWchar(pBaseBlock->szGuidSigneture, szTempSigneture, 5);
    pReadedData += 4;

    wprintf(L"    Guid signeture:                    %s\n", pBaseBlock->szGuidSigneture);

    // Last reorganized time
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastReorganizedTime.dwLowDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pBaseBlock->ftLastReorganizedTime.dwHighDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;

    SYSTEMTIME stLastReorganizedTime = { 0 };
    FileTimeToSystemTime(&pBaseBlock->ftLastReorganizedTime, &stLastReorganizedTime);
    wprintf(L"    Last reorganized time:             %04d/%02d/%02d %02d:%02d\n"
        , stLastReorganizedTime.wYear, stLastReorganizedTime.wMonth, stLastReorganizedTime.wDay
        , stLastReorganizedTime.wHour, stLastReorganizedTime.wMinute);

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

    wprintf(L"\n");

    // Constant
    dwAbsoluteOffsetCurrentPointer = FILE_HEADER_SIZE;

    return SUCCESS;
}


BOOL ParseHiveBinHeader(HANDLE hFile, PHIVE_BIN_HEADER pHBin) {

    SetFilePointer(hFile, FILE_HEADER_SIZE, NULL, FILE_BEGIN);

    BYTE byReadData[32] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 32;
    DWORD nReadedSize = 0;

    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0X%x\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };
    CHAR szTempSigneture[5] = "";

    pReadedData = byReadData;

    wprintf(L"Hive bin Header:\n");

    // Regf signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    CharToWchar(pHBin->szHiveBinSigneture, szTempSigneture, 5);
    pReadedData += 4;

    wprintf(L"    Signeture:                         %s\n", pHBin->szHiveBinSigneture);

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

    // Move 32 bytes
    dwAbsoluteOffsetCurrentPointer += 32;

    return SUCCESS;
}


BOOL ParseCell(HANDLE hFile, LPDWORD pCellSize, DWORD dwAbsoluteCellOffset) {

    SetFilePointer(hFile, dwAbsoluteCellOffset, NULL, FILE_BEGIN);

    BYTE byReadData[6] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 6;
    DWORD nReadedSize = 0;

    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X.\n", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };
    CHAR szCellSigneture[3] = "";

    pReadedData = byReadData;

    wprintf(L"\nCell:\n");

    // Cell size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    DWORD dwSize = abs(*(DWORD*)byDwordArray);
    *pCellSize = dwSize;
    pReadedData += 4;

    wprintf(L"    Size:                              0x%X\n", dwSize);

    // Cell signeture
    for (int i = 0; i < 2; i++) szCellSigneture[i] = pReadedData[i];
    pReadedData += 2;

    wprintf(L"    Signeture:                         %hs\n", szCellSigneture);

    DWORD dwCellType = 0;

    if (CHECK_RETURN(StrCmpA(szCellSigneture, "li")))      dwCellType = CELL_INDEX_LEAF;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "lf"))) dwCellType = CELL_FAST_LEAF;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "lh"))) dwCellType = CELL_HASH_LEAF;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "ri"))) dwCellType = CELL_INDEX_ROOT;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "nk"))) dwCellType = CELL_KEY_NODE;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "vk"))) dwCellType = CELL_KEY_VALUE;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "sk"))) dwCellType = CELL_KEY_SECURITY;
    else if (CHECK_RETURN(StrCmpA(szCellSigneture, "db"))) dwCellType = CELL_BIG_DATA;
    else return FAILURE;

    return dwCellType;
}
