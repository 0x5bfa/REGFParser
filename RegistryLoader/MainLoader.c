// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"

int wmain(void) {

    BASE_BLOCK BaseBlock = { 0 };
    HBIN HBin = { 0 };

    HANDLE hFile = CreateFileW(L"C:\\Users\\T31068068\\Desktop\\BCD_Backup", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ParseFileHeader(&BaseBlock, hFile);

    ParseHiveBinHeader(&HBin, hFile);

    return SUCCESS;
}


BOOL ParseFileHeader(PBASE_BLOCK pBaseBlock, HANDLE hFile) {

    // Base block size is 4096
    BYTE byReadData[4096] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 4096;
    DWORD nReadedSize = 0;

    BYTE byTemp[8] = { 0 };
    FILETIME ftBaseBlockTimeStamp;
    SYSTEMTIME stBaseBlockTimeStamp;
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

    wprintf(L"    Signeture:  %s\n", pBaseBlock->szRegfSigneture);

    // Primary sequence number
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwPrimarySequenceNumber = *(DWORD*)byTemp;
    pReadedData += 4;

    // Secondary sequence number
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwSecondarySequenceNumber = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    SequenceNo. { %d, %d }\n", pBaseBlock->dwPrimarySequenceNumber, pBaseBlock->dwSecondarySequenceNumber);

    // Last written timestamp
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->ftLastWrittenTimeStamp.dwLowDateTime = *(DWORD*)byTemp;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->ftLastWrittenTimeStamp.dwHighDateTime = *(DWORD*)byTemp;
    pReadedData += 4;

    SYSTEMTIME stLastWrittenTimeStamp = { 0 };
    FileTimeToSystemTime(&pBaseBlock->ftLastWrittenTimeStamp, &stLastWrittenTimeStamp);
    wprintf(L"    Last written time: %04d/%02d/%02d  %02d:%02d\n"
        , stLastWrittenTimeStamp.wYear, stLastWrittenTimeStamp.wMonth, stLastWrittenTimeStamp.wDay
        , stLastWrittenTimeStamp.wHour, stLastWrittenTimeStamp.wMinute);

    // Major version
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwWriterMajorVersion = *(DWORD*)byTemp;
    pReadedData += 4;

    // Minor version
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwWriterMinorVersion = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    Maj.Min  %d.%d\n", pBaseBlock->dwWriterMajorVersion, pBaseBlock->dwWriterMinorVersion);

    // File type
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwFileType = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    File type:  %d\n", pBaseBlock->dwFileType);

    // File format
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwFileFormat = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    File format:  %d\n", pBaseBlock->dwFileFormat);

    // Root cell offset
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwRootCellOffset = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    Root cell offset:  %d\n", pBaseBlock->dwRootCellOffset);

    // Hive bins data size
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwHiveBinsDataSize = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    Hive bins data:  %d\n", pBaseBlock->dwHiveBinsDataSize);

    // Clustering factor
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwClusteringFactor = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    Clustering factor:  %d\n", pBaseBlock->dwClusteringFactor);

    // File name
    CHAR szTempFileName[64];
    for (int i = 0; i < 64; i++) szTempFileName[i] = (CHAR)pReadedData[i];
    pReadedData += 64;

    ByteToWchar(pBaseBlock->szFileName, szTempFileName, 64);
    wprintf(L"    File name:  %s\n", pBaseBlock->szFileName);

    // Resource manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidRmId);
    WCHAR szRmGuidString[40] = L"";
    GuidToWchar(szRmGuidString, &pBaseBlock->guidRmId);
    pReadedData += 16;

    wprintf(L"    RmId:  %s\n", szRmGuidString);


    // Log file guid
    ByteToGuid(pReadedData, &pBaseBlock->guidLogId);
    WCHAR szLogGuidString[40] = L"";
    GuidToWchar(szLogGuidString, &pBaseBlock->guidLogId);
    pReadedData += 16;

    wprintf(L"    LogId:  %s\n", szLogGuidString);

    // Guid flags
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->dwGuidFlags = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    Guid flags:  %x\n", pBaseBlock->dwGuidFlags);

    // Transaction manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidTmId);
    WCHAR szTmGuidString[40] = L"";
    GuidToWchar(szTmGuidString, &pBaseBlock->guidTmId);
    pReadedData += 16;

    wprintf(L"    TmId:  %s\n", szTmGuidString);

    // Guid signeture
    for (int i = 0; i < 4; i++) szTempSigneture[i] = pReadedData[i];
    CharToWchar(pBaseBlock->szGuidSigneture, szTempSigneture, 5);
    pReadedData += 4;

    wprintf(L"    Guid signeture:  %s\n", pBaseBlock->szGuidSigneture);

    // Last reorganized time

    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->ftLastReorganizedTime.dwLowDateTime = *(DWORD*)byTemp;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pBaseBlock->ftLastReorganizedTime.dwHighDateTime = *(DWORD*)byTemp;
    pReadedData += 4;

    SYSTEMTIME stLastReorganizedTime = { 0 };
    FileTimeToSystemTime(&pBaseBlock->ftLastReorganizedTime, &stLastReorganizedTime);
    wprintf(L"    Last reorganized time: %04d/%02d/%02d  %02d:%02d\n"
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

    return SUCCESS;
}


BOOL ParseHiveBinHeader(PHBIN pHBin, HANDLE hFile) {

    // Move 4096 bytes
    SetFilePointer(hFile, 4096, NULL, FILE_BEGIN);

    // Hive bin header size is 32
    BYTE byReadData[4096] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 4096;
    DWORD nReadedSize = 0;

    BYTE byTemp[8] = { 0 };
    FILETIME ftBaseBlockTimeStamp;
    SYSTEMTIME stBaseBlockTimeStamp;
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
    CharToWchar(pHBin->szHBinSigneture, szTempSigneture, 5);
    pReadedData += 4;

    wprintf(L"    Signeture:  %s\n", pHBin->szHBinSigneture);

    // Relative offset
    for (int i = 0; i < 4; i++) byTemp[i] = pReadedData[i];
    pHBin->dwRelativeOffset = *(DWORD*)byTemp;
    pReadedData += 4;

    wprintf(L"    Relative offset:  %d", pHBin->dwRelativeOffset);


    return SUCCESS;
}


BOOL ByteToGuid(PBYTE pData, GUID* guidResultGuid) {

    if (pData == NULL || guidResultGuid == NULL) return FAILURE;

    BYTE byTemp[8] = { 0 };

    // The first 8 hex digits of the GUID
    for (int i = 0; i < 8; i++) byTemp[i] = pData[i];
    guidResultGuid->Data1 = *(DWORD*)byTemp;
    pData += 4;

    // The first group of 4 hex digits
    for (int i = 0; i < 4; i++) byTemp[i] = pData[i];
    guidResultGuid->Data2 = *(DWORD*)byTemp;
    pData += 2;

    // The second group of 4 hex digits
    for (int i = 0; i < 4; i++) byTemp[i] = pData[i];
    guidResultGuid->Data3 = *(DWORD*)byTemp;
    pData += 2;

    // The third group of 4 hex digits and the final 12 hex digits
    for (int i = 0; i < 8; i++) guidResultGuid->Data4[i] = pData[i];
    pData += 8;

    return SUCCESS;
}


BOOL CharToWchar(WCHAR* szWideString, CHAR* szSingleString, DWORD dwSizeToCopy) {

    if (szWideString == NULL || szSingleString == NULL || dwSizeToCopy == 0) return FAILURE;

    for (int i = 0; i < dwSizeToCopy; i++) {

        szWideString[i] = (WCHAR)szSingleString[i];
    }

    return SUCCESS;
}


BOOL ByteToWchar(WCHAR* szWideString, BYTE* pData, DWORD dwSizeToCopy) {

    if (szWideString == NULL || pData == NULL || dwSizeToCopy == 0) return FAILURE;

    for (int i = 0, j = 0; i < dwSizeToCopy * 2; i++, j++) {

        if (i % 2 == 1) {
            j--;
            continue;
        }

        szWideString[j] = pData[i];
    }

    return SUCCESS;
}


BOOL GuidToWchar(WCHAR* szWideString, GUID* Guid) {

    if (szWideString == NULL || Guid == NULL) return FAILURE;

    swprintf(szWideString, 40, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
        , Guid->Data1, Guid->Data2, Guid->Data3, Guid->Data4[0], Guid->Data4[1], Guid->Data4[2]
        , Guid->Data4[3], Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7]);

    return SUCCESS;
}