// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"

DWORD dwAbsoluteOffsetCurrentPointer = 0;

BOOL ParseFileHeader(PFILE_HEADER pBaseBlock, HANDLE hFile);
BOOL ParseHiveBinHeader(PHIVE_BIN pHBin, HANDLE hFile);
DWORD ParseCell(HANDLE hFile, LPDWORD pCellSize);


int wmain(void) {

    HANDLE hFile = NULL;

    FILE_HEADER FileHeader = { 0 };
    HIVE_BIN HiveBin = { 0 };
    KEY_NODE KeyNode = { 0 };
    SECURITY_KEY SecurityKey = { 0 };
    FAST_LEAF FastLeaf = { 0 };
    VALUE_KEY ValueKey = { 0 };

    if ((hFile = CreateFileW(L"C:\\Users\\T31068068\\Desktop\\BCD", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE) {

        wprintf(L"ReadFile failed with 0x%X.\n", GetLastError());
        return FAILURE;
    }

    // Parse 4096 bytes
    if (ParseFileHeader(&FileHeader, hFile) == FAILURE) {

        wprintf(L"ParseFileHeader failed.\n");
        return FAILURE;
    }

    // Parse 32 bytes
    if (ParseHiveBinHeader(&HiveBin, hFile) == FAILURE) {

        wprintf(L"ParseFileHeader failed.\n");
        return FAILURE;
    }

    DWORD dwCellType = REG_NONE;
    DWORD dwCellSize = 0;

    /*
    while ((dwCellType = ParseCell(hFile, &dwCellSize)) != FAILURE) {

        switch (dwCellType) {

        case CELL_INDEX_LEAF:

            break;

        case CELL_FAST_LEAF:

            ParseFastLeaf(&FastLeaf, hFile, dwCellSize);
            break;

        case CELL_HASH_LEAF:

            break;

        case CELL_INDEX_ROOT:

            break;

        case CELL_KEY_NODE:

            ParseKeyNodeCell(&KeyNode, hFile, dwCellSize);
            break;

        case CELL_KEY_VALUE:

            ValueKey.dwAbsoluteHiveBinOffset = (HiveBin.dwRelativeOffset + 0x1000);
            ParseValueKey(&ValueKey, hFile, dwCellSize);
            break;

        case CELL_KEY_SECURITY:

            ParseSecurityKey(&SecurityKey, hFile, dwCellSize);
            break;

        case CELL_BIG_DATA:

            break;

        default: break;
        }
    }
    */

    return SUCCESS;
}


BOOL ParseFileHeader(PFILE_HEADER pBaseBlock, HANDLE hFile) {

    // Base block size is 4096
    BYTE byReadData[4096] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 4096;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };
    FILETIME ftBaseBlockTimeStamp = { 0 };
    SYSTEMTIME stBaseBlockTimeStamp = { 0 };
    CHAR szTempSigneture[5] = "";

    // read file until 4096 bytes
    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

    pReadedData = byReadData;

    wprintf(L"File Header:\n");

    // Regf signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    CharToWchar(pBaseBlock->szRegfSigneture, szTempSigneture, 5);
    pReadedData += 4;

    wprintf(L"    Signeture:                         %s\n", pBaseBlock->szRegfSigneture);

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

    // Rewind
    // pReadedData -= 4096;

    wprintf(L"\n");

    // Move 4096 bytes
    SetFilePointer(hFile, 4096, NULL, FILE_BEGIN);
    dwAbsoluteOffsetCurrentPointer += 4096;

    return SUCCESS;
}


BOOL ParseHiveBinHeader(PHIVE_BIN pHBin, HANDLE hFile) {

    // Hive bin header size is 32
    BYTE byReadData[32] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 32;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };
    CHAR szTempSigneture[5] = "";

    // read file until 4096 bytes
    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

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

    // Spara (or MemAlloc)
    pReadedData += 4;

    wprintf(L"\n");

    // Move 32 bytes
    SetFilePointer(hFile, 4096 + 32, NULL, FILE_BEGIN);
    dwAbsoluteOffsetCurrentPointer += 32;

    return SUCCESS;
}


DWORD ParseCell(HANDLE hFile, LPDWORD pCellSize) {

    SetFilePointer(hFile, dwAbsoluteOffsetCurrentPointer, NULL, FILE_BEGIN);

    BYTE byReadData[32] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 32;
    DWORD nReadedSize = 0;

    BYTE dwDwordArray[4] = { 0 };
    CHAR szTempSigneture[3] = "";

    // read 6 bytes
    if (ReadFile(hFile, &byReadData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

    pReadedData = byReadData;
    DWORD dwCellType = 0;
    DWORD dwCellOffset = 0;

    // calculate Padding
    for (int i = 0; i < 32; i++, dwCellOffset++) {

        if (tolower(pReadedData[i]) == L'l' && tolower(pReadedData[i + 1]) == L'i') {

            dwCellType = CELL_INDEX_LEAF;
            break;
        }
        else if (tolower(pReadedData[i]) == L'l' && tolower(pReadedData[i + 1]) == L'f') {

            dwCellType = CELL_FAST_LEAF;
            break;
        }
        else if (tolower(pReadedData[i]) == L'l' && tolower(pReadedData[i + 1]) == L'h') {

            dwCellType = CELL_HASH_LEAF;
            break;
        }
        else if (tolower(pReadedData[i]) == L'r' && tolower(pReadedData[i + 1]) == L'i') {

            dwCellType = CELL_INDEX_ROOT;
            break;
        }
        else if (tolower(pReadedData[i]) == L'n' && tolower(pReadedData[i + 1]) == L'k') {

            dwCellType = CELL_KEY_NODE;
            break;
        }
        else if (tolower(pReadedData[i]) == L'v' && tolower(pReadedData[i + 1]) == L'k') {

            dwCellType = CELL_KEY_VALUE;
            break;
        }
        else if (tolower(pReadedData[i]) == L's' && tolower(pReadedData[i + 1]) == L'k') {

            dwCellType = CELL_KEY_SECURITY;
            break;
        }
        else if (tolower(pReadedData[i]) == L'd' && tolower(pReadedData[i + 1]) == L'b') {

            dwCellType = CELL_BIG_DATA;
            break;
        }
    }

    dwCellOffset -= 4;
    pReadedData += dwCellOffset;
    dwAbsoluteOffsetCurrentPointer += dwCellOffset;
    if (dwCellType == 0) return FAILURE;

    wprintf(L"\nCell: [%X][%X][%X][%X]\n", pReadedData[0], pReadedData[1], pReadedData[2], pReadedData[3]);

    WCHAR szCellSigneture[3] = L"";
    DWORD dwSize = 0;

    // Cell size
    for (int i = 0; i < 4; i++) dwDwordArray[i] = pReadedData[i];
    dwSize = abs(*(DWORD*)dwDwordArray);
    *pCellSize = dwSize;
    pReadedData += 4;

    wprintf(L"    Size:                              0x%X\n", dwSize);

    // Cell signeture
    for (int i = 0; i < 2; i++) szTempSigneture[i] = pReadedData[i];
    CharToWchar(szCellSigneture, szTempSigneture, 3);
    pReadedData += 2;

    wprintf(L"    Signeture:                         %s\n", szCellSigneture);

    dwAbsoluteOffsetCurrentPointer += 6;

    return dwCellType;
}
