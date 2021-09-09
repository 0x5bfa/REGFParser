<!-- omit in toc -->
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

## 1. Base block
The base block is 4096 bytes in length, it contains the following:

Offset|Size|Description
---|---|---
0|4|**ASCII string**<br>Registry file signeture<br>Always "regf".
4|4|**UINT**<br>Primary sequence number<br>This number is incremented by 1 in the beginning of a write operation on the primary file.
8|4|**UINT**<br>Secondary sequence number<br>This number is incremented by 1 at the end of a write operation on the primary file, a primary sequence number and a secondary sequence number should be equal after a successful write operation.
12|8|**FILETIME**<br>Last written timestamp (UTC)
20|4|**UINT**<br>[Major version](#21-format-versions)<br>Major version of a hive writer.
24|4|**UINT**<br>[Minor version](#21-format-versions)<br>Minor version of a hive writer.
28|4|**UINT**<br>[File type](#22-file-types)
32|4|**UINT**<br>[File format](#23-file-format)
36|4|**UINT**<br>[Root cell offset](#24-root-cell-offset)<br>Offset of a root cell in bytes, relative from the start of the hive bins data.
40|4|**UINT**<br>Hive bins data size<br>Size of the hive bins data in bytes.
44|4|**UINT**<br>Clustering factor<br>Logical sector size of the underlying disk in bytes divided by 512.
48|64|**ASCII String**<br>File name<br>UTF-16LE string (contains a partial file path to the primary file, or a file name of the primary file), used for debugging purposes.
112|16|**GUID**<br>RmId<br>A GUID of the Resource Manager (RM).<br>(Windows 10 only)
128|16|**GUID**<br>LogId<br>A GUID used to generate a file name of a log file stream for the Transaction Manager (TM).<br>(Windows 10 only)
144|4|**UINT**<br>[Flags](#25-bit-mask-for-flags)<br>(Windows 10 only)
148|16|**GUID**<br>TmId<br>A GUID used to generate a file name of a physical log file.<br>(Windows 10 only)
164|4|**ASCII String**<br>GUID signature<br>Always "rmtm".<br>(Windows 10 only)
168|8|**FILETIME**<br>Last reorganized timestamp<br>(Windows 10 only)
176|4|**ASCII String**<br>Offline registry signeture<br>Always "OfRg".<br>(written by offline registry library)
180|4|**UINT**<br>Flags<br>Always 1.<br>(written by offline registry library)
184|324|Reserved
508|4|**UINT**<br>Checksum<br>XOR-32 checksum of the previous 508 bytes.
512|8|**FILETIME**<br>Serialization timestamp (UTC).<br>(written by offline registry library)
520|3520|Reserved
4040|16|**GUID**<br>ThawTmId<br>This field has no meaning on a disk.
4056|16|**GUID**<br>ThawRmId<br>This field has no meaning on a disk.
4072|16|**GUID**<br>ThawLogId<br>This field has no meaning on a disk.
4088|4|**UINT**<br>Boot type<br>This field has no meaning on a disk.
4092|4|**UINT**<br>Boot recover<br>This field has no meaning on a disk.

### 1.1. Format versions

Version<br>(Major.Minor)|Description
---|---
1.0|Unknown (mentioned by other sources that this has been seen on pre-release of Windows NT 3.1)
1.1|Seen on Windows NT 3.1 and NT 3.5, format is different from later versions
1.2|Seen on Windows NT 3.51
1.3|Seen on Windows NT 4.0 and later, typically used for NTUSER.DAT<br>This is the REG_STANDARD_FORMAT as used by RegSaveKeyEx on Windows 10 2004
1.4|Unknown (mentioned by some sources, but not yet observed)
1.5|Seen on Windows XP SP2 and later, typically used for SOFTWARE, SYSTEM<br>This is the REG_LATEST_FORMAT as used by RegSaveKeyEx on Windows 10 2004
1.6|Seen on Windows 10 2004, used by Windows Docker delta Registry files

### 1.2. File types

Value|Description
---|---
0|Primary registry hive file
1|Transaction log variant 1, seen on Windows XP (SP2, SP3), Vista, Windows 7 and 8.0
2|Transaction log variant 2, seen on Windows NT 3.51, NT 4.0 and 2000
6|Transaction log variant 6, seen on Windows 8.1, Server 2012 R2 and Windows 10

### 1.3. File format

Value|Description
---|---
1|Direct memory load

### 1.4. Root cell offset

`File offset of a root cell = 4096 + Root cell offset.`

This formula also applies to any other offset relative from the start of the hive bins data (however, if such a relative offset is equal to `0xFFFFFFFF`, it doesn't point anywhere).

### 1.5. Bit mask for Flags

Mask|Description
---|---
0x1|KTM locked the hive (there are pending or anticipated transactions)
0x2|The hive has been defragmented (all its pages are dirty therefore) and it is being written to a disk (Windows 8 and Windows Server 2012 only, this flag is used to speed up hive recovery by reading a transaction log file instead of a primary file); this hive supports the layered keys feature (starting from Insider Preview builds of Windows 10 "Redstone 1")

## 2. Hive Bin

A hive bin size is multiple of 4096 bytes.

### 2.1. Hive Bin Header

The Hive bin is variable in size and consists of a header and cells. A hive bin's header is 32 bytes in length.

Offset|Length|Description
---|---|---
0|4|**ASCII String**<br>Hibe bin signeture<br>Always "hbin".
4|4|**UINT**<br>Offset <br>A current hive bin in bytes, relative from the start of the hive bins data.
8|4|**UINT**<br>Size <br>A current hive bin in bytes.
12|8|Reserved
20|8|**FILETIME**<br>defined for the first hive bin only.
28|4|**UINT**<br>Spare (or MemAlloc)<br>This field has no meaning on a disk

### 2.2. Cells

The cells fill the remaining space in the hive bin (there are no gaps between the cells). The size of each cell is variable.

Offset|Length|Description
---|---|---
0|4|Size of a current cell in bytes, including this field (aligned to 8 bytes): the size is positive if a cell is unallocated or negative if a cell is allocated (use absolute values for calculations)
4|...|Cell data

## 3. Cell Data

Record|Description
---|---
Index leaf (li)|Subkeys list
Fast leaf (lf)|Subkeys list with name hints
Hash leaf (lh)|Subkeys list with name hashes
Index root (ri)|List of subkeys lists (used to subdivide subkeys lists)
Key node (nk)|Registry key node
Key value (vk)|Registry key value
Key security (sk)|Security descriptor
Big data (db)|List of data segments

When a record (entity) contains an offset field pointing to another record (entity), this offset points to a cell containing the latter record (entity). As already mentioned above, an offset relative from the start of the hive bins data doesn't point anywhere when it is equal to `0xFFFFFFFF`.

### 3.1. Index Leaf

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always "li".
2|2|**UINT**<br>Number of elements
4|...|**UINT**<br>List elements

A list element has the following structure (a list element is repeated Number of elements times):

Offset|Length|Description
---|---|---
0|4|**UINT**<br>In bytes, relative from the start of the hive bins data

### 3.2. Fast Leaf

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always "lf".
2|2|**UINT**<br>Number of elements
4|...|**UINT**<br>List elements

A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|**UINT**<br>In bytes, relative from the start of the hive bins data
4|4|**ASCII String**<br>The first 4 ASCII characters of a key name string (used to speed up lookups)

If a key name string is less than 4 characters in length, it is stored in the beginning of the Name hint field (hereinafter, the beginning of the field means the byte at the lowest address or the first few bytes at lower addresses in the field), unused bytes of this field are set to null. UTF-16LE characters are converted to ASCII (extended), if possible (if it isn't, the first byte of the Name hint field is null).

Hereinafter, an extended ASCII string means a string made from UTF-16LE characters with codes less than 256 by removing the null byte (at the highest address) from each character.

All list elements are required to be sorted (as described above).

### 3.3. Hash Leaf
The Hash leaf has the following structure:

Offset|Length|Description
---|---|---
0|2|ASCII string
2|2|Number of elements
4|...|List elements

A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|In bytes, relative from the start of the hive bins data
4|4|Hash of a key name string, see below (used to speed up lookups)

All list elements are required to be sorted (as described above). The Hash leaf is used when the Minor version field of the base block is greater than 4.

The hash is calculated using the following algorithm:

let H be a 32-bit value, initial value is zero;
let N be an uppercase name of a key;
split N into characters, and for each character (exactly two bytes in the case of UTF-16LE, also known as a wide character), treated as a number (character code), C[i], do the following: H = 37 * H + C[i];
H is the hash value.

### 3.4. Index root
The Index root has the following structure:

Offset|Length|Description
---|---|---
0|2|ASCII string
2|2|Number of elements
4|...|List elements

A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|In bytes, relative from the start of the hive bins data

An Index root can't point to another Index root.
A Subkeys list can't point to an Index root.
List elements within subkeys lists referenced by a single Index root must be sorted as a whole (i.e. the first list element of the second subkeys list must be greater than the last list element of the first subkeys list).

### 3.5. Key Node

The Key node has the following structure:

Offset|Length|Description
---|---|---
0|2|ASCII string
2|2|Bit mask, see below
4|8|FILETIME (UTC)
12|4|Bit mask, see below (this field is used as of Windows 8 and Windows Server 2012; in previous versions of Windows, this field is reserved and called Spare)
16|4|Offset of a parent key node in bytes, relative from the start of the hive bins data (this field has no meaning on a disk for a root key node)
20|4|Number of subkeys
24|4|Number of volatile subkeys
28|4|In bytes, relative from the start of the hive bins data (also, this field may point to an Index root)
32|4|This field has no meaning on a disk (volatile keys are not written to a file)
36|4|Number of key values
40|4|In bytes, relative from the start of the hive bins data
44|4|In bytes, relative from the start of the hive bins data
48|4|In bytes, relative from the start of the hive bins data
52|4|In bytes, a subkey name is treated as a UTF-16LE string (see below)
56|4|In bytes
60|4|In bytes, a value name is treated as a UTF-16LE string
64|4|In bytes
68|4|Cached index (see below)
72|2|In bytes
74|2|In bytes
76|...|ASCII (extended) string or UTF-16LE string

Starting from Windows Vista, Windows Server 2003 SP2, and Windows XP SP3, the Largest subkey name length field has been split into 4 bit fields (the offsets below are relative from the beginning of the old Largest subkey name length field, i.e. the first bit field starts within the byte at the lowest address):

Offset (bits)|Length (bits)|Description
---|---|---
0|16|Largest subkey name length
16|4|Bit mask, see below
20|4|Bit mask, see below
24|8|Debug