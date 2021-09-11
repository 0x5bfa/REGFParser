// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"


BOOL ParseKeyNodeCell(PKEY_NODE pKeyNode, HANDLE hFile, DWORD dwCellSize) {

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(dwCellSize, sizeof(BYTE))) == NULL) return 0;
    nBaseBlockSize = dwCellSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

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
    for (int i = 0; pReadedData[i] != NULL; i++) wprintf(L"%c", (WCHAR)pReadedData[i]);
    wprintf(L"\n");

    return SUCCESS;
}