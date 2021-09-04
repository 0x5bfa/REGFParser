//
//

#include "RegistryLoader.h"

typedef struct _BASE_BLOCK {

    CHAR szSigneture[5];
    int PrimarySequenceNumber;
    int SecondarySequenceNumber;
    FILETIME LastWrittenTimeStamp;


}BASE_BLOCK, *PBASE_BLOCK;

int main(void) {

    BASE_BLOCK BaseBlock = { 0 };

    HANDLE hFile = CreateFileW(L"C:\\Users\\T31068068\\Desktop\\bcd", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ParseBaseBlock(&BaseBlock, hFile);
}





BOOL ParseBaseBlock(PBASE_BLOCK pBaseBlock, HANDLE hFile) {


    // Base block size is 4096
    BYTE byData[4096] = { 0 };
    PBYTE pData = NULL;
    DWORD nBaseBlockSize = 4096;

    // read file until 4096 bytes
    DWORD nReadedSize = 0;
    if (ReadFile(hFile, &byData, nBaseBlockSize, &nReadedSize, NULL) == FAILURE) {
        wprintf(L"ReadFile failed with %d", GetLastError());
        return FAILURE;
    }

    pData = byData;

    wprintf(L"BaseBlock:\n");

    // Copy signenure named "regf"
    for (int i = 0; i < 4; i++) pBaseBlock->szSigneture[i] = pData[i];

    printf("    Signeture: \"%s\"\n", pBaseBlock->szSigneture);

    pData += 4;
    BYTE byTemp[4] = { 0 };

    for (int i = 0; i < 4; i++) byTemp[i] = pData[i];

    printf("    seq#1:     %d\n", *(DWORD*)byTemp);

    pData += 4;

    for (int i = 0; i < 4; i++) byTemp[i] = pData[i];

    printf("    seq#2:     %d\n", *(DWORD*)byTemp);

    pData += 4;

    for (int i = 0; i < 4; i++) byTemp[i] = pData[i];

    printf("    seq#2:     %d\n", *(DWORD*)byTemp);




    return 0;
}

