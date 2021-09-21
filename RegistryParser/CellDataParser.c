// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryParser.h"
#include <sddl.h>

LPWSTR GetStringDataType(DWORD dwDataType);


BOOL ParseIndexLeaf(HANDLE hFile, PINDEX_LEAF pIndexLeaf, DWORD dwAbsoluteOffset) {

}


BOOL ParseFastLeaf(HANDLE hFile, PFAST_LEAF pFastLeaf, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pFastLeaf->dwSize, sizeof(BYTE))) == NULL) return FAILURE;
    nBaseBlockSize = pFastLeaf->dwSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseFastLeaf().\n", GetLastError());
        return FAILURE;
    }

    // Number of elements
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pFastLeaf->nElements = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Number of elements:                0x%X\n", pFastLeaf->nElements);

    PFAST_ELEMENT pElement = NULL;
    if ((pElement = (PFAST_ELEMENT)calloc((pFastLeaf->nElements * 2), sizeof(FAST_ELEMENT))) == NULL) return FAILURE;

    for (int j = 0; j < pFastLeaf->nElements; j++) {

        // Read 8 bytes

        // Offset
        for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
        pElement[j].dwKeyNodeOffset = *(DWORD*)byDwordArray;
        pReadedData += 4;

        wprintf(L"    Key node offset:                   0x%X\n", pElement[j].dwKeyNodeOffset);

        // Name hint
        for (int i = 0; i < 4; i++) pElement[j].szNameHint[i] = (CHAR)pReadedData[i];
        wprintf(L"    Name hint:                         %hs\n", pElement[j].szNameHint);
        pReadedData += 4;
    }

    for (int i = 0; i < pFastLeaf->nElements; i++) {

        if (ParseCell(hFile, dwAbsoluteCurrentHiveBinOffset + pElement[i].dwKeyNodeOffset) == FAILURE) {

            wprintf(L"ParseCell failed in ParseFastLeaf().\n");
            return FAILURE;
        }
    }

    return SUCCESS;
}


BOOL ParseHashLeaf(HANDLE hFile, PHASH_LEAF pHashLeaf, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nReadedSize = 0;

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pHashLeaf->dwSize, sizeof(BYTE))) == NULL) return FAILURE;

    if (ReadFile(hFile, pReadedData, pHashLeaf->dwSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyNode().", GetLastError());
        return FAILURE;
    }

    BYTE byDwordArray[4] = { 0 };

    // Number of elements
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pHashLeaf->nElements = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Number of elements:                0x%X\n", pHashLeaf->nElements);

    PHASH_ELEMENT pElement = NULL;
    if ((pElement = (PHASH_ELEMENT)calloc((pHashLeaf->nElements * 2), sizeof(HASH_ELEMENT))) == NULL) return FAILURE;

    for (int j = 0; j < pHashLeaf->nElements; j++) {

        // Read 8 bytes

        // Offset
        for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
        pElement[j].dwKeyNodeOffset = *(DWORD*)byDwordArray;
        pReadedData += 4;

        wprintf(L"    #%02d Key node offset:               0x%X\n", j, pElement[j].dwKeyNodeOffset);

        // Hash value
        for (int i = 0; i < 4; i++) byDwordArray[i] = pReadedData[i];
        pElement[j].dwNameHash = *(DWORD*)byDwordArray; // temp
        wprintf(L"        Hash value:                    0x%X\n", pElement[j].dwNameHash);
        pReadedData += 4;
    }

    for (int i = 0; i < pHashLeaf->nElements; i++) {

        if (ParseCell(hFile, dwAbsoluteCurrentHiveBinOffset + pElement[i].dwKeyNodeOffset) == FAILURE) {

            wprintf(L"ParseCell failed in ParseFastLeaf().\n");
            return FAILURE;
        }
    }

    return SUCCESS;
}


BOOL ParseIndexRoot(HANDLE hFile, PINDEX_ROOT pIndexRoot, DWORD dwAbsoluteOffset) {

}


BOOL ParseKeyNode(HANDLE hFile, PKEY_NODE pKeyNode, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nReadedSize = 0;

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pKeyNode->dwSize, sizeof(BYTE))) == NULL) return FAILURE;

    if (ReadFile(hFile, pReadedData, pKeyNode->dwSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyNode().", GetLastError());
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
    pReadedData += (pKeyNode->dwSize - (76 + 4));
    wprintf(L"\n");

    if (pKeyNode->dwSubKeysListOffset != 0xFFFFFFFF) {

        if (ParseCell(hFile, dwAbsoluteCurrentHiveBinOffset + pKeyNode->dwSubKeysListOffset) == FAILURE) {

            wprintf(L"ParseCell failed in ParseKeyNode().\n");
            return FAILURE;
        }
    }
    if (pKeyNode->dwKeyValuesListOffset != 0xFFFFFFFF) {

        if (ParseKeyValueList(hFile, dwAbsoluteCurrentHiveBinOffset + pKeyNode->dwKeyValuesListOffset) == FAILURE) {

            wprintf(L"ParseCell failed in ParseKeyNode().\n");
            return FAILURE;
        }
    }
    if (pKeyNode->dwClassNameOffset != 0xFFFFFFFF) {

        if (ParseCell(hFile, dwAbsoluteCurrentHiveBinOffset + pKeyNode->dwClassNameOffset) == FAILURE) {

            wprintf(L"ParseCell failed in ParseKeyNode().\n");
            return FAILURE;
        }
    }

    return SUCCESS;
}


BOOL ParseKeyValue(HANDLE hFile, PKEY_VALUE pValueKey, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pValueKey->dwSize, sizeof(BYTE))) == NULL) return FAILURE;
    nBaseBlockSize = pValueKey->dwSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0X%x in ParseKeyValue().\n", GetLastError());
        return FAILURE;
    }

    // Value name size
    for (int i = 0; i < 2; i++) byDwordArray[i] = pReadedData[i];
    pValueKey->dwValueNameSize = *(DWORD*)byDwordArray;
    pReadedData += 2;

    wprintf(L"    Value name size:                   0x%X\n", pValueKey->dwValueNameSize);

    // Data size
    pValueKey->dwDataSize = BytesArrayToDword(pReadedData);
    pReadedData += 4;

    wprintf(L"    Data size:                         0x%X\n", pValueKey->dwDataSize);

    // Data offset
    pValueKey->dwDataOffset = BytesArrayToDword(pReadedData);
    pReadedData += 4;

    wprintf(L"    Data offset:                       0x%X\n", pValueKey->dwDataOffset);

    // Data type
    pValueKey->dwDataType = BytesArrayToDword(pReadedData);
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
    if(pValueKey->dwValueNameSize == 0) wprintf(L"(Default)\n");
    else {

        for (int i = 0; i < pValueKey->dwValueNameSize; i++) wprintf(L"%c", (WCHAR)pReadedData[i]);
        wprintf(L"\n");
    }

    pReadedData += (pValueKey->dwSize - (20 + 6));

    // Move to data offset
    DWORD dwAbsoluteDataOffset = pValueKey->dwAbsoluteHiveBinOffset + pValueKey->dwDataOffset;

    SetFilePointer(hFile, dwAbsoluteDataOffset, NULL, FILE_BEGIN);

    if (pValueKey->dwDataSize >= 0x80000000) {

        wprintf(L"    Data:                              0x%X\n", pValueKey->dwDataOffset);
        return SUCCESS;
    }

    // Read data size from data
    BYTE byDataSize[4] = { 0 };
    PBYTE pDataSize = NULL;
    DWORD dwDataSizeSize = 4;
    DWORD dwDataSizeReaded = 0;

    if (ReadFile(hFile, &byDataSize, dwDataSizeSize, &dwDataSizeReaded, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyValue().\n", GetLastError());
        return FAILURE;
    }

    pDataSize = byDataSize;

    DWORD dwDataSize = abs(BytesArrayToDword(pDataSize));
    pDataSize += 4;

    // Data size
    wprintf(L"    Data size from data offset:        0x%X\n", dwDataSize);

    PBYTE pData = NULL;
    DWORD dwReadedDataSize;
    if ((pData = (PBYTE)calloc(dwDataSize, sizeof(BYTE))) == NULL) return FAILURE;

    if (ReadFile(hFile, pData, dwDataSize, &dwReadedDataSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyValue().\n", GetLastError());
        return FAILURE;
    }

    // Data
    wprintf(L"    Data:                              ");

    if (pValueKey->dwDataSize == 0) wprintf(L"(No value set)\n");
    else {

        for (int i = 0; i < pValueKey->dwDataSize; i++) {

            wprintf(L"%X", pData[i]);
            if (i != (pValueKey->dwDataSize - 1)) wprintf(L",");
            else wprintf(L"\n");
        }
    }

    return SUCCESS;
}


BOOL ParseKeySecurity(HANDLE hFile, PKEY_SECURITY pSecurityKey, DWORD dwAbsoluteOffset) {

    SetFilePointer(hFile, dwAbsoluteOffset, NULL, FILE_BEGIN);

    PBYTE pReadedData = NULL;
    DWORD nBaseBlockSize = 0;
    DWORD nReadedSize = 0;

    BYTE byDwordArray[4] = { 0 };

    // Allocate memory for reading
    if ((pReadedData = (BYTE*)calloc(pSecurityKey->dwSize, sizeof(BYTE))) == NULL) return 0;
    nBaseBlockSize = pSecurityKey->dwSize;

    if (ReadFile(hFile, pReadedData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {

        wprintf(L"ReadFile failed with 0x%X in ParseKeyValue().", GetLastError());
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

    return SUCCESS;
}


BOOL ParseBigData(HANDLE hFile, PBIG_DATA pBigData, DWORD dwAbsoluteOffset) {

}


LPWSTR GetStringDataType(DWORD dwDataType) {

    WCHAR szDataType[32] = L"";

    switch (dwDataType) {

    case REG_SZ:
        wcscpy(szDataType, L"REG_SZ");

    case REG_EXPAND_SZ:
        wcscpy(szDataType, L"REG_EXPAND_SZ");

    case REG_BINARY:
        wcscpy(szDataType, L"REG_BINARY");

    case REG_DWORD:
        wcscpy(szDataType, L"REG_DWORD");

    case REG_DWORD_BIG_ENDIAN:
        wcscpy(szDataType, L"REG_DWORD_BIG_ENDIAN");

    case REG_LINK:
        wcscpy(szDataType, L"REG_LINK");

    case REG_MULTI_SZ:
        wcscpy(szDataType, L"REG_MULTI_SZ");

    case REG_RESOURCE_LIST:
        wcscpy(szDataType, L"REG_RESOURCE_LIST");

    case REG_FULL_RESOURCE_DESCRIPTOR:
        wcscpy(szDataType, L"REG_FULL_RESOURCE_DESCRIPTOR");

    case REG_RESOURCE_REQUIREMENTS_LIST:
        wcscpy(szDataType, L"REG_RESOURCE_REQUIREMENTS_LIST");

    case REG_QWORD:
        wcscpy(szDataType, L"REG_QWORD");

    default: return NULL;
    }

    return *szDataType;
}


