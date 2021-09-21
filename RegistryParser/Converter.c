// Copyright (c) 2021 onein528
// Licensed under the MIT License.

#include "RegistryParser.h"


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


BOOL GuidToWchar(WCHAR* szWideString, GUID* Guid) {

    if (szWideString == NULL || Guid == NULL) return FAILURE;

    swprintf(szWideString, 40, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
        , Guid->Data1, Guid->Data2, Guid->Data3, Guid->Data4[0], Guid->Data4[1], Guid->Data4[2]
        , Guid->Data4[3], Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7]);

    return SUCCESS;
}


DWORD BytesArrayToDword(PBYTE pData) {

    DWORD dwData = 0;
    BYTE byDwordArray[4] = { 0 };

    if (pData == NULL) return FAILURE;

    for (int i = 0; i < 4; i++)byDwordArray[i] = pData[i];
    dwData = *(DWORD*)byDwordArray;

    return dwData;
}