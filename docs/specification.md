# Registry file format specification

```
+------------+-----------------------------------------+
| Base Block |              Hive bins data             |
+------------+-----------------------------------------+
               |
      +--------+
      |
+----------+----------+----------+     +----------+
| Hive bin | Hive bin | Hive bin | ... | Hive bin |
+----------+----------+----------+     +----------+
      |
  +---+
  |
+-----------------+------+------+------+     +------+
| Hive bin header | Cell | Cell | Cell | ... | Cell |
+-----------------+------+------+------+     +------+
```

## 2 Base block
The base block is 4096 bytes in length, it contains the following:

Offset|Size|Desc
---|---|---
0|4|**ASCII string**<br>Registry file signeture<br>Always "regf".
4|4|**DWORD**<br>Primary sequence number<br>This number is incremented by 1 in the beginning of a write operation on the primary file.
8|4|**DWORD**<br>Secondary sequence number<br>This number is incremented by 1 at the end of a write operation on the primary file, a primary sequence number and a secondary sequence number should be equal after a successful write operation.
12|8|**FILETIME**<br>Last written timestamp (UTC)
20|4|**DWORD**<br>[Major version](#21-format-versions)<br>Major version of a hive writer.
24|4|**DWORD**<br>[Minor version](#21-format-versions)<br>Minor version of a hive writer.
28|4|**DWORD**<br>[File type](#22-file-types)
32|4|**DWORD**<br>[File format](#23-file-format)
36|4|**DWORD**<br>[Root cell offset](#24-root-cell-offset)<br>Offset of a root cell in bytes, relative from the start of the hive bins data.
40|4|**DWORD**<br>Hive bins data size<br>Size of the hive bins data in bytes.
44|4|**DWORD**<br>Clustering factor<br>Logical sector size of the underlying disk in bytes divided by 512.
48|64|**ASCII String**<br>File name<br>UTF-16LE string (contains a partial file path to the primary file, or a file name of the primary file), used for debugging purposes.
112|16|**GUID**<br>RmId<br>A GUID of the Resource Manager (RM).<br>(Windows 10 only)
128|16|**GUID**<br>LogId<br>A GUID used to generate a file name of a log file stream for the Transaction Manager (TM).<br>(Windows 10 only)
144|4|**DWORD**<br>[Flags](#25-bit-mask-for-flags)<br>(Windows 10 only)
148|16|**GUID**<br>TmId<br>A GUID used to generate a file name of a physical log file.<br>(Windows 10 only)
164|4|**ASCII String**<br>GUID signature<br>Always "rmtm".<br>(Windows 10 only)
168|8|**FILETIME**<br>Last reorganized timestamp<br>(Windows 10 only)
176|4|**ASCII String**<br>Offline registry signeture<br>Always "OfRg".<br>(written by offline registry library)
180|4|**DWORD**<br>Flags<br>Always 1.<br>written by offline registry library)
184|324|Reserved
508|4|**DWORD**<br>Checksum<br>XOR-32 checksum of the previous 508 bytes.
512|8|**FILETIME**<br>Serialization timestamp (UTC).<br>(written by offline registry library)
520|3520|Reserved
4040|16|**GUID**<br>ThawTmId<br>This field has no meaning on a disk.
4056|16|**GUID**<br>ThawRmId<br>This field has no meaning on a disk.
4072|16|**GUID**<br>ThawLogId<br>This field has no meaning on a disk.
4088|4|**DWORD**<br>Boot type<br>This field has no meaning on a disk.
4092|4|**DWORD**<br>Boot recover<br>This field has no meaning on a disk.

### 2.1 Format versions

Version<br>(Major.Minor)|Description
---|---
1.0|Unknown (mentioned by other sources that this has been seen on pre-release of Windows NT 3.1)
1.1|Seen on Windows NT 3.1 and NT 3.5, format is different from later versions
1.2|Seen on Windows NT 3.51
1.3|Seen on Windows NT 4.0 and later, typically used for NTUSER.DAT<br>This is the REG_STANDARD_FORMAT as used by RegSaveKeyEx on Windows 10 2004
1.4|Unknown (mentioned by some sources, but not yet observed)
1.5|Seen on Windows XP SP2 and later, typically used for SOFTWARE, SYSTEM<br>This is the REG_LATEST_FORMAT as used by RegSaveKeyEx on Windows 10 2004
1.6|Seen on Windows 10 2004, used by Windows Docker delta Registry files

### 2.2 File types

Value|Desc
---|---
0|Primary registry hive file
1|Transaction log variant 1, seen on Windows XP (SP2, SP3), Vista, Windows 7 and 8.0
2|Transaction log variant 2, seen on Windows NT 3.51, NT 4.0 and 2000
6|Transaction log variant 6, seen on Windows 8.1, Server 2012 R2 and Windows 10

### 2.3 File format

Value|Desc
---|---
1|Direct memory load

### 2.4 Root cell offset

`File offset of a root cell = 4096 + Root cell offset.`

This formula also applies to any other offset relative from the start of the hive bins data (however, if such a relative offset is equal to `0xFFFFFFFF`, it doesn't point anywhere).

### 2.5 Bit mask for Flags

Mask|Description
---|---
0x1|KTM locked the hive (there are pending or anticipated transactions)
0x2|The hive has been defragmented (all its pages are dirty therefore) and it is being written to a disk (Windows 8 and Windows Server 2012 only, this flag is used to speed up hive recovery by reading a transaction log file instead of a primary file); this hive supports the layered keys feature (starting from Insider Preview builds of Windows 10 "Redstone 1")

