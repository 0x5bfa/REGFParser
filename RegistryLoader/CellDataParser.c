// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"
#include <sddl.h>


BOOL ParseKeyNodeCell(HANDLE hFile, PKEY_NODE pKeyNode, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nReadedSize = 0;

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pKeyNode->dwSize, sizeof(BYTE))) == NULL) return FAILURE;

    if (ReadFile(hFile, pReadedData, pKeyNode->dwSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };

    // Flags
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwFlags = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Flags:                             0x%X\n", pKeyNode->dwFlags);

    // Time stamp
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->ftLastWrittenTime.dwLowDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->ftLastWrittenTime.dwHighDateTime = *(DWORD*)byDwordArray;
    pReadedData += 4;

    SYSTEMTIME stTimeStamp;
    FileTimeToSystemTime(&pKeyNode->ftLastWrittenTime, &stTimeStamp);
    wprintf(L"    Time stamp:                        %04d/%02d/%02d %02d:%02d\n"
        , stTimeStamp.wYear, stTimeStamp.wMonth, stTimeStamp.wDay, stTimeStamp.wHour, stTimeStamp.wMinute);

    // Access bits
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwAccessBits = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Access bits:                       0x%X\n", pKeyNode->dwAccessBits);

    // Parent offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwParent = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Parent offset:                     0x%X\n", pKeyNode->dwParent);

    // Number of subkeys
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->nSubKeys = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Number of subkeys:                 0x%X\n", pKeyNode->nSubKeys);

    // Number of volatile subkeys
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->nVolatileSubKeys = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Number of volatile subkeys:        0x%X\n", pKeyNode->nVolatileSubKeys);

    // Subkeys list offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwSubKeysListOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Subkeys list offset:               0x%X\n", pKeyNode->dwSubKeysListOffset);

    // Volatile subkeys list offset
    pReadedData += 4;

    // Number of key values
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->nKeyValues = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Number of key values:              0x%X\n", pKeyNode->nKeyValues);

    // Key values list offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwKeyValuesListOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Key values list offset:            0x%X\n", pKeyNode->dwKeyValuesListOffset);

    // Key security offset	
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwKeySecurityOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Key security offset:               0x%X\n", pKeyNode->dwKeySecurityOffset);

    // Class name offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwClassNameOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Class name offset:                 0x%X\n", pKeyNode->dwClassNameOffset);

    // Largest subkey name length
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwLargestSubKeyNameLength = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Largest subkey name length:        0x%X\n", pKeyNode->dwLargestSubKeyNameLength);

    // Largest subkey class name length
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwLargestSubKeyClassLength = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Largest subkey class name length:  0x%X\n", pKeyNode->dwLargestSubKeyClassLength);

    // Largest value name length
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwLargestValueNameLength = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Largest value name length:         0x%X\n", pKeyNode->dwLargestValueNameLength);

    // Largest value data size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwLargestValueDataSize = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Largest value data size:           0x%X\n", pKeyNode->dwLargestValueDataSize);

    // WorkVar
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwWorkVar = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    WorkVar:                           0x%X\n", pKeyNode->dwWorkVar);

    // Key name length
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwKeyNameLength = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Key name length:                   0x%X\n", pKeyNode->dwKeyNameLength);

    // Class name length	
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pKeyNode->dwClassNameLength = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Class name length:                 0x%X\n", pKeyNode->dwClassNameLength);

    // Key name
    wprintf(L"    Key name:                          ");
    for (int i = 0; i < pKeyNode->dwKeyNameLength; i++) wprintf(L"%c", (WCHAR)pReadedData[i]);
    wprintf(L"\n");

    pReadedData += (pKeyNode->dwSize - (76 + 4));

    dwAbsoluteOffsetCurrentPointer += (pKeyNode->dwSize - 6);

    return SUCCESS;
}


BOOL ParseSecurityKey(HANDLE hFile, PSECURITY_KEY pSecurityKey, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffsetCurrentPointer, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pSecurityKey->dwSize, sizeof(BYTE))) == NULL) return 0;
    nBaseBlockSize = pSecurityKey->dwSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

    // Reserved
    pReadedData += 2;

    // Flink
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pSecurityKey->dwFlink = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Flink:                             0x%X\n", pSecurityKey->dwFlink);

    // Blink
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pSecurityKey->dwBlink = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Blink:                             0x%X\n", pSecurityKey->dwBlink);

    // Reference count
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pSecurityKey->dwReferenceCount = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Reference count:                   0x%X\n", pSecurityKey->dwReferenceCount);

    // Security descriptor size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pSecurityKey->dwSecurityDescriptorSize = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Security descriptor size:          0x%X\n", pSecurityKey->dwSecurityDescriptorSize);

    SECURITY_DESCRIPTOR* pSD = pReadedData;
    LPWSTR lpSDString = NULL;

    ConvertSecurityDescriptorToStringSecurityDescriptorW(pSD, SDDL_REVISION_1, DACL_SECURITY_INFORMATION, &lpSDString, NULL);

    wprintf(L"    Security descriptor:               %s\n", lpSDString);

    pReadedData += (pSecurityKey->dwSize - 6);

    dwAbsoluteOffsetCurrentPointer += (pSecurityKey->dwSize - 6);

    return SUCCESS;
}


BOOL ParseFastLeaf(HANDLE hFile, PFAST_LEAF pFastLeaf, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffsetCurrentPointer, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pFastLeaf->dwSize, sizeof(BYTE))) == NULL) return FAILURE;
    nBaseBlockSize = pFastLeaf->dwSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %x\n", GetLastError());
        return FAILURE;
    }

    // Number of elements
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pFastLeaf->nElements = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Number of elements:                0x%X\n", pFastLeaf->nElements);

    DWORD dwElementSize = 0;
    dwElementSize = pFastLeaf->dwSize - (6 + 2);
    DWORD nElements = 0;
    nElements = (dwElementSize / 4);
    if (dwElementSize % 4 != 0) nElements++;

    PELEMENT pElement;
    if ((pElement = (PELEMENT)calloc(nElements, sizeof(ELEMENT))) == NULL) return FAILURE;

    for (int i = 0, j = 0; i < dwElementSize || j < nElements; i++) {

        if (i == 0 || i % 8 == 0) { // offset

            for (int k = 0; k < 4; k++) byDwordArray[k] = pReadedData[k];
            pElement[j].dwKeyNodeOffset = *(DWORD*)byDwordArray;
            pReadedData += 4;
            wprintf(L"    Key node offset:                   0x%X\n", pElement[j].dwKeyNodeOffset);
            j++;
        }
        else if (i % 4 == 0) { // name hint

            for (int k = 0; k < 4; k++) pElement[j].szNameHint[k] = (CHAR)pReadedData[k];
            wprintf(L"    Name hint:                         %hs\n", pElement[j].szNameHint);
            pReadedData += 4;
            j++;
        }
    }

    dwAbsoluteOffsetCurrentPointer += (pFastLeaf->dwSize - 6);

    return SUCCESS;
}


BOOL ParseValueKey(HANDLE hFile, PVALUE_KEY pValueKey, DWORD dwAbsoluteOffset) {

    wprintf(L"(Absolute offset: %X)\n", dwAbsoluteOffsetCurrentPointer);

    SetFilePointer(hFile, dwAbsoluteOffsetCurrentPointer, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pValueKey->dwSize, sizeof(BYTE))) == NULL) return FAILURE;
    nBaseBlockSize = pValueKey->dwSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %x\n", GetLastError());
        return FAILURE;
    }

    // Value name size
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pValueKey->dwValueNameSize = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Value name size:                   0x%X\n", pValueKey->dwValueNameSize);

    // Data size
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pValueKey->dwDataSize = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Data size:                         0x%X\n", pValueKey->dwDataSize);

    // Data offset
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pValueKey->dwDataOffset = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Data offset:                       0x%X\n", pValueKey->dwDataOffset);

    // Data type
    for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
    pValueKey->dwDataType = *(DWORD*)byDwordArray;
    pReadedData += 4;

    wprintf(L"    Data type:                         0x%X\n", pValueKey->dwDataType);

    // Flags
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pValueKey->dwFlags = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Flags:                             0x%X\n", pValueKey->dwFlags);

    // Spare
    pReadedData += 2;

    // Key name
    wprintf(L"    Key name:                          ");
    for (int i = 0; i < pValueKey->dwValueNameSize; i++) wprintf(L"%c", (WCHAR)pReadedData[i]);
    wprintf(L"\n");

    pReadedData += (pValueKey->dwSize - (20 + 6));

    // Move to data offset
    DWORD dwAbsoluteDataOffset = pValueKey->dwAbsoluteHiveBinOffset + pValueKey->dwDataOffset;

    SetFilePointer(hFile, dwAbsoluteDataOffset, NULL, FILE_BEGIN);

    if (pValueKey->dwDataType == REG_DWORD) {

        wprintf(L"    Data:                              0x%X\n", pValueKey->dwDataOffset);
        return SUCCESS;
    }
    // Read data size from data
    BYTE DataSize[4];
    PBYTE pDataSize = NULL;
    DWORD dwDataSizeSize = 4;
    DWORD dwDataSizeReaded = 0;

    if (ReadFile(hFile, &DataSize, dwDataSizeSize, &dwDataSizeReaded, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %x\n", GetLastError());
        return FAILURE;
    }

    pDataSize = DataSize;

    for (int i = 0; i < 4; i++) byDwordArray[i] = pDataSize[i];
    DWORD dwDataSize = abs(*(DWORD*)byDwordArray);
    pDataSize += 4;

    // Data size
    wprintf(L"    Data size from data offset:        0x%X\n", dwDataSize);

    PBYTE pData = NULL;
    DWORD dwReadedDataSize;
    if ((pData = (PBYTE)calloc(dwDataSize, sizeof(BYTE))) == NULL) return FAILURE;

    if (ReadFile(hFile, pData, dwDataSize, &dwReadedDataSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %x\n", GetLastError());
        return FAILURE;
    }

    // Data
    wprintf(L"    Data:                              ");

    for (int i = 0; i < (dwDataSize - 4); i++) {

        wprintf(L"%X", pData[i]);
        if (i != ((dwDataSize - 4) - 1)) wprintf(L",");
        else wprintf(L"\n");
    }

    dwAbsoluteOffsetCurrentPointer += (pValueKey->dwSize + dwDataSize - 6);

    return SUCCESS;
}
