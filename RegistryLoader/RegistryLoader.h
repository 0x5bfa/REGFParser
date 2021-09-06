#pragma once

#include <stdio.h>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")
#pragma warning (disable: 4996)

#define SUCCESS 1
#define FAILURE 0
#define CHECK_RETURN ((x) == 0)

// Offset size

#define BB_REGF_SIGNETURE_SIZE     4
#define BB_PRIMARY_SEQ_NUM_SIZE    4
#define BB_SECONDARY_SEQ_NUM_SIZE  4
#define BB_LAST_TIME_STAMP_SIZE    8
#define BB_MAJOR_VER_SIZE          4
#define BB_MINOR_VER_SIZE          4
#define BB_FILE_TYPE_SIZE          4
#define BB_FILE_FORMAT_SIZE        4
#define BB_ROOT_CELL_OFFSET_SIZE   4
#define BB_HIVE_BINS_DATA_SIZE     4
#define BB_CLUSTERING_FACTOR_SIZE  4
#define BB_FILE_NAME_SIZE          64
#define BB_RESERVED_SIZE1          396
#define BB_CHECK_SUM_SIZE          4
#define BB_RESERVED_SIZE2          3576
#define BB_BOOT_TYPE_SIZE          4
#define BB_BOOT_RECOVER_SIZE       4
#define BB_RMID_SIZE               16
#define BB_LOGID_SIZE              16
#define BB_FLAGS_SIZE              4
#define BB_TMID_SIZE               16
#define BB_GUID_SIGNATURE          4
#define BB_RECOGNIZED_TIME_SIZE    1

typedef struct _BASE_BLOCK {

    WCHAR szRegfSigneture[5];
    DWORD dwPrimarySequenceNumber;
    DWORD dwSecondarySequenceNumber;
    FILETIME ftLastWrittenTimeStamp;
    DWORD dwWriterMajorVersion;
    DWORD dwWriterMinorVersion;
    DWORD dwFileType;
    DWORD dwFileFormat;
    DWORD dwRootCellOffset;
    DWORD dwHiveBinsDataSize;
    DWORD dwClusteringFactor;
    WCHAR szFileName[64];

    GUID guidRmId;
    GUID guidLogId;
    DWORD dwGuidFlags;
    GUID guidTmId;
    CHAR szGuidSigneture;
    FILETIME ftLastReorganizedTime;

    CHAR szOffRegSigneture[5];
    DWORD dwOffRegFlags;
    FILETIME ftSerializationTime;

}BASE_BLOCK, * PBASE_BLOCK;

