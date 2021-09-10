// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryLoader.h"


BOOL ParseKeyNodeCell(PNODE_KEY pNodeKey, HANDLE hFile, DWORD dwCellSize) {

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
    pNodeKey->dwFlags = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Flags:  %d\n", pNodeKey->dwFlags);

    return SUCCESS;
}