// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"

int wmain(void) {

    BASE_BLOCK BaseBlock = { 0 };

    HANDLE hFile = CreateFileW(L"C:\\Users\\T31068068\\Desktop\\BCD_Backup", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ParseFileHeader(&BaseBlock, hFile);

    return SUCCESS;
}





BOOL ParseFileHeader(PBASE_BLOCK pBaseBlock, HANDLE hFile) {

    // Base block size is 4096
    BYTE byReadData[4096] = { 0 };
    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 4096;
    BYTE byTemp[8] = { 0 };
    FILETIME ftBaseBlockTimeStamp;
    SYSTEMTIME stBaseBlockTimeStamp;
    CHAR szTempSigneture[5] = "";

    // read file until 4096 bytes
    DWORD nReadedSize = 0;
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
    for (int i = 0; i < 64; i++) pBaseBlock->szFileName[i] = pReadedData[i];
    pReadedData += 64;

    wprintf(L"    File name:  %s\n", pBaseBlock->szFileName);

    // Resource manager guid
    ByteToGuid(pReadedData, &pBaseBlock->guidRmId);
    wprintf(L"    ResMgr GUID:  {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n"
        , pBaseBlock->guidRmId.Data1, pBaseBlock->guidRmId.Data2, pBaseBlock->guidRmId.Data3
        , pBaseBlock->guidRmId.Data4[0], pBaseBlock->guidRmId.Data4[1], pBaseBlock->guidRmId.Data4[2]
        , pBaseBlock->guidRmId.Data4[3], pBaseBlock->guidRmId.Data4[4], pBaseBlock->guidRmId.Data4[5]
        , pBaseBlock->guidRmId.Data4[6], pBaseBlock->guidRmId.Data4[7]);

    // Log file guid
    pReadedData += 16;

    // Guid flags
    pReadedData += 4;

    // Transaction manager guid
    pReadedData += 16;

    // Guid signeture
    pReadedData += 4;

    // Last reorganized time
    pReadedData += 8;

    // Offline registry signeture
    pReadedData += 4;

    // Offline registry flags
    pReadedData += 4;

    // Serialization time stamp
    pReadedData += 8;

    // Reserved size
    pReadedData += 3576;
    pReadedData -= 4096;

    //wprintf(L"%c", pReadedData[0]);
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