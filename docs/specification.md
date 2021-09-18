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
0|4|**ASCII string**<br>Registry file signeture<br>Always `regf`.
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
164|4|**ASCII String**<br>GUID signature<br>Always `rmtm`.<br>(Windows 10 only)
168|8|**FILETIME**<br>Last reorganized timestamp<br>(Windows 10 only)
176|4|**ASCII String**<br>Offline registry signeture<br>Always `OfRg`.<br>(written by offline registry library)
180|4|**UINT**<br>Flags<br>Always `1`.<br>(written by offline registry library)
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
0|4|**ASCII String**<br>Hibe bin signeture<br>Always `hbin`.
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
[Index leaf (li)](#31-index-leaf)|Subkeys list
[Fast leaf (lf)](#32-fast-leaf)|Subkeys list with name hints
[Hash leaf (lh)](#33-hash-leaf)|Subkeys list with name hashes
[Index root (ri)](#34-index-root)|List of subkeys lists (used to subdivide subkeys lists)
[Key node (nk)](#35-key-node)|Registry key node
[Key value (vk)]()|Registry key value
[Key security (sk)]()|Security descriptor
[Big data (db)]()|List of data segments

When a record (entity) contains an offset field pointing to another record (entity), this offset points to a cell containing the latter record (entity). As already mentioned above, an offset relative from the start of the hive bins data doesn't point anywhere when it is equal to `0xFFFFFFFF`.

### 3.1. Index Leaf

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always `li`.
2|2|**UINT**<br>Number of elements
4|...|**UINT**<br>List elements

A list element has the following structure (a list element is repeated Number of elements times):

Offset|Length|Description
---|---|---
0|4|**UINT**<br>In bytes, relative from the start of the hive bins data

### 3.2. Fast Leaf

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always `lf`.
2|2|**UINT**<br>Number of elements
4|...|**UINT**<br>List elements

A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|**UINT**<br>In bytes, relative from the start of the hive bins data
4|4|**ASCII String**<br>The first 4 ASCII characters of a key name string (used to speed up lookups)

**Note:**

1. If a key name string is less than 4 characters in length, it is stored in the beginning of the Name hint field (hereinafter, the beginning of the field means the byte at the lowest address or the first few bytes at lower addresses in the field), unused bytes of this field are set to null. UTF-16LE characters are converted to ASCII (extended), if possible (if it isn't, the first byte of the Name hint field is null).
2. Hereinafter, an extended ASCII string means a string made from UTF-16LE characters with codes less than 256 by removing the null byte (at the highest address) from each character.
3. All list elements are required to be sorted (as described above).

### 3.3. Hash Leaf
The Hash leaf has the following structure:

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always `lh`.
2|2|**UINT**<br>Number of elements
4|...|List elements

A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|**UINT**<br>In bytes, relative from the start of the hive bins data
4|4|**UINT**<br>Hash of a key name string, see below (used to speed up lookups)

**Note:**

1. The Hash leaf is used when the Minor version field of the base block is greater than 4.

2. The hash is calculated using the following algorithm:

    1. let `H` be a 32-bit value, initial value is zero;
    2. let `N` be an uppercase name of a key;
    3. split `N` into characters, and for each character (exactly two bytes in the case of UTF-16LE, also known as a wide character), treated as a number (character code), `C[i]`, do the following: `H = 37 * H + C[i]`;
    4. `H` is the hash value.

### 3.4. Index root

The Index root has the following structure:

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always `ri`.
2|2|**UINT**<br>Number of elements
4|...|List elements

A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|**UINT**<br>In bytes, relative from the start of the hive bins data

**Note:**

1. An Index root can't point to another Index root.
2. A Subkeys list can't point to an Index root.
3. List elements within subkeys lists referenced by a single Index root must be sorted as a whole (i.e. the first list element of the second subkeys list must be greater than the last list element of the first subkeys list).

### 3.5. Key Node

The Key node has the following structure:

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Signeture<br>Always `nk`.
2|2|**UINT**<br>[Flags](#354-flags)<br>Bit mask
4|8|**FILETIME**<br>Last written timestamp
12|4|**UINT**<br>[Access bits](#351-access-bits)<br>Bit mask (this field is used as of Windows 8 and Windows Server 2012; in previous versions of Windows, this field is reserved and called Spare)
16|4|**UINT**<br>Offset of a parent key node in bytes, relative from the start of the hive bins data (this field has no meaning on a disk for a root key node)
20|4|**UINT**<br>Number of subkeys
24|4|**UINT**<br>Number of volatile subkeys
28|4|**UINT**<br>Subkeys list offset<br>In bytes, relative from the start of the hive bins data (also, this field may point to an Index root)
32|4|**UINT**<br>Volatile subkeys list offset<br>This field has no meaning on a disk (volatile keys are not written to a file)
36|4|**UINT**<br>Number of key values
40|4|**UINT**<br>[Key values list](#359-key-values-list) offset<br>In bytes, relative from the start of the hive bins data
44|4|**UINT**<br>Key security offset<br>In bytes, relative from the start of the hive bins data
48|4|**UINT**<br>[Class name](#352-class-name) offset<br>In bytes, relative from the start of the hive bins data
52|4|**UINT**<br>Largest subkey name length<br>In bytes, a subkey name is treated as a UTF-16LE string (see below)
56|4|**UINT**<br>Largest subkey class name length<br>In bytes
60|4|**UINT**<br>Largest value name length<br>In bytes, a value name is treated as a UTF-16LE string
64|4|**UINT**<br>Largest value data size<br>In bytes
68|4|**UINT**<br>[WorkVar](#353-workvar)<br>Cached index (see below)
72|2|**UINT**<br>Key name length<br>In bytes
74|2|**UINT**<br>Class name length<br>In bytes
76|...|**ASCII (extended) or UTF-16LE String**<br>Key name string

**Note:**

Starting from Windows Vista, Windows Server 2003 SP2, and Windows XP SP3, the Largest subkey name length field has been split into 4 bit fields (the offsets below are relative from the beginning of the old Largest subkey name length field, i.e. the first bit field starts within the byte at the lowest address):

Offset (bits)|Length (bits)|Description
---|---|---
0|16|Largest subkey name length
16|4|[Virtualization control flags](#356-virtualization-control-flags)<br>Bit mask
20|4|[User flags](#355-user-flags) (Wow64 flags)<br>Bit mask
24|8|[Debug](#357-debug)

**Warning:**

When implementing the structure defined above in a program, keep in mind that a compiler may pack the Virtualization control flags and User flags bit fields in a different way. In C, two or more bit fields inside an integer may be packed right-to-left, so the first bit field defined in an integer may reside in the less significant (right) bits. In debug symbols for Windows, the UserFlags field is defined before the VirtControlFlags field exactly for this reason (however, these fields are written to a file in the order indicated in the table above).

**End of warning:**

Starting from Insider Preview builds of Windows 10 "Redstone 1", the Access bits field has been split into the following fields:

Offset|Length|Description
---|---|---
0|1|Access bits|
1|1|Layered key bit fields
2|2|Spare|Not used

The Layered key bit fields have the following structure (the offsets below are relative from the most significant bit):

Offset (bits)|Length (bits)|Description
---|---|---
0|1|Inherit class<br>0 or 1<br>
1|5|Spare<br>Not used
6|2|Layer semantic<br>0, 1, 2, or 3<br>

#### 3.5.1. Access bits

Access bits are used to track when key nodes are being accessed (e.g. via the RegCreateKeyEx() and RegOpenKeyEx() calls), the field is set according to the following bit masks:

Mask|Description
---|---
0x1|This key was accessed before a Windows registry was initialized with the NtInitializeRegistry() routine during the boot
0x2|This key was accessed after a Windows registry was initialized with the NtInitializeRegistry() routine during the boot

When the Access bits field is equal to 0, the access history of a key is clear.

#### 3.5.2. Class name

Typically, a class name is a UTF-16LE string assigned to a key node; it doesn't have any particular meaning. For example, Microsoft was using class names to store keys for the Syskey encryption. Originally, class names were intended to be used to define key node types (similar to data types of the Key value).

#### 3.5.3. WorkVar
In Windows 2000, the WorkVar field may contain a zero-based index of a subkey in a subkeys list found at the last successful lookup (used to speed up lookups when the same subkey is accessed many times consecutively: a list element with the cached index is always tried first); such a cached index may point to a list element either in a subkeys list or in a volatile subkeys list, the latter is examined last.

This field isn't used as of Windows XP.

#### 3.5.4. Flags
In Windows XP and Windows Server 2003, the first 4 bits, counting from the most significant bit, are reserved for user flags (set via the NtSetInformationKey() call, read via the NtQueryKey() call). Other bits have the following meaning:

Mask|Name|Description
---|---|---
0x0001|KEY_VOLATILE|Is volatile (not used, a key node on a disk isn't expected to have this flag set)
0x0002|KEY_HIVE_EXIT|Is the mount point of another hive (a key node on a disk isn't expected to have this flag set)
0x0004|KEY_HIVE_ENTRY|Is the root key for this hive
0x0008|KEY_NO_DELETE|This key can't be deleted
0x0010|KEY_SYM_LINK|This key is a symlink (a target key is specified as a UTF-16LE string (REG_LINK) in a value named "SymbolicLinkValue", example: \REGISTRY\MACHINE\SOFTWARE\Classes\Wow6432Node)
0x0020|KEY_COMP_NAME|Key name is an ASCII string, possibly an extended ASCII string (otherwise it is a UTF-16LE string)
0x0040|KEY_PREDEF_HANDLE|Is a predefined handle (a handle is stored in the Number of key values field)

The following bits are also used as of Windows Vista:

Mask|Name|Description
---|---|---
0x0080|VirtualSource|This key was virtualized at least once
0x0100|VirtualTarget|Is virtual
0x0200|VirtualStore|Is a part of a virtual store path

It is plausible that both a registry key virtualization (when registry writes to sensitive locations are redirected to per-user locations in order to protect a Windows registry against corruption) and a registry key reflection (when registry changes are synchronized between keys in 32-bit and 64-bit views; this feature was removed in Windows 7 and Windows Server 2008 R2) required more space than this field can provide, that is why the Largest subkey name length field was split and the new fields were introduced.

Starting from Windows Vista, user flags were moved away from the first 4 bits of the Flags field to the new User flags bit field (see above). These user flags in the new location are also called Wow64 flags. In Windows XP and Windows Server 2003, user flags are stored in the old location anyway.

It should be noted that, in Windows Vista and Windows 7, the 4th bit (counting from the most significant bit) of the Flags field is set to 1 in many key nodes belonging to different hives; this bit, however, can't be read through the NtQueryKey() call. Such key nodes are present in initial primary files within an installation image (install.wim), and their number doesn't increase after the installation. A possible explanation for this oddity is that initial primary files were generated on Windows XP or Windows Server 2003 using the Wow64 subsystem (see below).

#### 3.5.5. User flags
The User flags field (in the appropriate location for a version of Windows being used) is set according to the following bit masks:

Mask|Description
---|---
0x1|Is a 32-bit key: this key was created through the Wow64 subsystem or this key shall not be used by a 64-bit program (e.g. by a 64-bit driver during the boot)
0x2|This key was created by the reflection process (when reflecting a key from another view)
0x4|Disable registry reflection for this key
0x8|In the old location of the User flags field: execute the int 3 instruction on an access to this key (both retail and checked Windows kernels), this bit was superseded by the Debug field (see below); in the new location of the User flags field: disable registry reflection for this key if a corresponding key exists in another view and it wasn't created by a caller (see below)

In Windows 7, Windows Server 2008 R2, and more recent versions of Windows, the bit mask 0x1 isn't used to mark 32-bit keys created by userspace programs.

The bit mask 0x8 was supported in the new location of the User flags field only in pre-release versions of Windows Vista, e.g. beta 2.

#### 3.5.6. Virtualization control flags

The Virtualization control flags field is set according to the following bit masks:

Mask|Name|Description
---|---|---
0x2|REG_KEY_DONT_VIRTUALIZE|Disable registry write virtualization for this key
0x4|REG_KEY_DONT_SILENT_FAIL|Disable registry open virtualization for this key
0x8|REG_KEY_RECURSE_FLAG|Propagate virtualization flags to new child keys (subkeys)

#### 3.5.7. Debug

When the CmpRegDebugBreakEnabled kernel variable is set to 1, a checked Windows kernel will execute the int 3 instruction on various events according to the bit mask in the Debug field. A retail Windows kernel has this feature disabled.

The following bit masks are supported:

Mask|Name|Event description
---|---|---
0x01|BREAK_ON_OPEN|This key is opened
0x02|BREAK_ON_DELETE|This key is deleted
0x04|BREAK_ON_SECURITY_CHANGE|A key security item is changed for this key
0x08|BREAK_ON_CREATE_SUBKEY|A subkey of this key is created
0x10|BREAK_ON_DELETE_SUBKEY|A subkey of this key is deleted
0x20|BREAK_ON_SET_VALUE|A value is set to this key
0x40|BREAK_ON_DELETE_VALUE|A value is deleted from this key
0x80|BREAK_ON_KEY_VIRTUALIZE|This key is virtualized

#### 3.5.8. Layered keys

Layered keys were introduced in Insider Preview builds of Windows 10 "Redstone 1". When a hive supports the layered keys feature, a kernel may treat some key nodes in a special way. This feature is used to implement the containerized registry.

When a kernel is accessing a key node treated as a part of a layered key, it builds a key node stack, including the key node being accessed, its "parent" key node and no more than 2 (or 14 as of Windows 10 Insider Preview Build 14986, or 126 as of Windows 10 Insider Preview Build 15025) "parent" key nodes towards the top layer. Then this stack is used to produce cumulative information about the layered key. For example, if you query the last written timestamp for a layered key, the most recent timestamp will be returned from the key node stack; if you enumerate key values for a layered key, key values from key nodes in the stack will be returned (except tombstone values; if there are two or more values with the same name in the key node stack, a value from a lower ("child") key node is used and returned).

When the Inherit class field is set to 0, the layered key will have the same class name as the key node originally accessed by a kernel. Otherwise, the layered key will receive the same class name (possibly an empty class name) as an upper ("parent") key node (from the stack) having the Inherit class field set to 0.

The Layer semantics field is set using the following values:

Value|Name|Description
---|---|---
0||This key node and its "parent" key nodes can be included in the current layered key (based on the stack built)
1|IsTombstone|Is a tombstone key node: this key node and its "parent" key nodes can't be included in the current layered key (also, such a key node has no class name, no subkeys, and no values)
2|IsSupersedeLocal|This key node can be included in the current layered key, but its "parent" key nodes can't
3|IsSupersedeTree|This key node can be included in the current layered key, but its "parent" key nodes can't; child key nodes (subkeys), except tombstone key nodes, are required to have the same value set in the field

Here, a "parent" key node doesn't mean a key node which offset is stored in the Parent field of a given key node. And a "child" key node doesn't mean a subkey of a given key node (unless otherwise mentioned). Instead, these relationships are created between key nodes in different registry hives (a host registry hive, which represents the top layer, and a differencing registry hive, which represents the lower layer; both registry hives are used to provide the merged view of registry data, the top layer provides base data and the lower layer provides modifications to be applied on top of base data).

#### 3.5.9. Key values list

The Key values list has the following structure:

Offset|Length|Field
---|---|---
0|...|List elements
A list element has the following structure:

Offset|Length|Description
---|---|---
0|4|**UINT**<br>Key value offset<br>In bytes, relative from the start of the hive bins data

List elements are not required to be sorted.

### 3.6. Key value
The Key value has the following structure:

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Key value Signature<br>Always `vk`.
2|2|**UINT**<br>Name length<br>In bytes, can be 0 (name isn't set)
4|4|**UINT**<br>[Data size](#361-data-size)<br>In bytes, can be 0 (value isn't set), the most significant bit has a special meaning.
8|4|**UINT**<br>Data offset<br>In bytes, relative from the start of the hive bins data (or data itself, see below)
12|4|**UINT**<br>[Data type](#362-data-types)
16|2|**UINT**<br>[Flags](#363-flags)<br>Bit mask
18|2|**UINT**<br>Spare<br>Not used
20|...|**ASCII (extended) or UTF-16LE String**<br>Value name string

#### 3.6.1. Data size

When the most significant bit is 1, data (4 bytes or less) is stored in the Data offset field directly (when data contains less than 4 bytes, it is being stored as is in the beginning of the Data offset field). The most significant bit (when set to 1) should be ignored when calculating the data size.

When the most significant bit is 0, data is stored in the Cell data field of another cell (pointed by the Data offset field) or in the Cell data fields of multiple cells (referenced in the Big data structure stored in a cell pointed by the Data offset field).

#### 3.6.2. Data types

Value|Name(s)
---|---
0x00000000|REG_NONE
0x00000001|REG_SZ
0x00000002|REG_EXPAND_SZ
0x00000003|REG_BINARY
0x00000004|REG_DWORD, REG_DWORD_LITTLE_ENDIAN
0x00000005|REG_DWORD_BIG_ENDIAN
0x00000006|REG_LINK
0x00000007|REG_MULTI_SZ
0x00000008|REG_RESOURCE_LIST
0x00000009|REG_FULL_RESOURCE_DESCRIPTOR
0x0000000a|REG_RESOURCE_REQUIREMENTS_LIST
0x0000000b|REG_QWORD, REG_QWORD_LITTLE_ENDIAN

Other values are allowed as well (but they are not predefined).

#### 3.6.3. Flags

Mask|Name|Description
---|---|---
0x0001|VALUE_COMP_NAME|Value name is an ASCII string, possibly an extended ASCII string (otherwise it is a UTF-16LE string)
0x0002|IsTombstone|Is a tombstone value (the flag is used starting from Insider Preview builds of Windows 10 "Redstone 1"), a tombstone value also has the Data type field set to REG_NONE, the Data size field set to 0, and the Data offset field set to `0xFFFFFFFF`

### 3.7. Key security
The Key security item has the following structure:

Offset|Length|Description
---|---|---
0|2|**ASCII String**<br>Key security signature<br>Always `sk`.
2|2|Reserved
4|4|**UINT**<br>Flink
8|4|**UINT**<br>Blink
12|4|**UINT**<br>Reference count<br>Number of key nodes pointing to this item
16|4|**UINT**<br>In bytes, security descriptor size<br>
20|...|**PSECURITY_DESCRIPTOR**<br>Security descriptor

**Notes:**

1. Flink and blink are offsets in bytes, relative from the start of the hive bins data.
2. Key security items form a doubly linked list. A key security item may act as a list header or a list entry (the only difference here is the meaning of flink and blink fields).
3. When a key security item acts as a list header, flink and blink point to the first and the last entries of this list respectively. If a list is empty, flink and blink point to a list header (i.e. to a current cell).
4. When a key security item acts as a list entry, flink and blink point to the next and the previous entries of this list respectively. If there is no next entry in a list, flink points to a list header. If there is no previous entry in a list, blink points to a list header.

### 3.8. Big data

The Big data is used to reference data larger than 16344 bytes (when the Minor version field of the base block is greater than 3), it has the following structure:

Offset|Length|Description
---|---|---
0|2|**SCII String**<br>Big data signature<br>Always `db`.
2|2|**UINT**<br>Number of segments
4|4|**UINT**<br>Offset of a list of segments<br>In bytes, relative from the start of the hive bins data

The list of segments has the following structure:

Offset|Length|Field
---|---|---
0|...|List elements
A list element has the following structure:

Offset|Length|Description
---|---|---|---
0|4|**UINT**<br>Data segment offset<br>In bytes, relative from the start of the hive bins data

#### 3.7.1. Data segment

A data segment is stored in the Cell data field of a cell pointed by the Data segment offset field. A data segment has the maximum size of 16344 bytes.

Data segments of a Big data record, except the last one, always have the maximum size.

### 3.9. Unallocated cell

If a cell to be marked as unallocated has an adjacent unallocated cell, these cells are coalesced, this is why a single unallocated cell may contain multiple remnant records (entities).

In Windows 2000, the following record is written to the beginning of the Cell data field of an unallocated cell:

Offset|Length|Description
---|---|---
0|4|Offset of a next unallocated cell in a free list (in bytes, relative from the start of the hive bins data) or `0xFFFFFFFF`, if there is no such a cell in that list (there are many free lists for a single hive in memory)

The record described above isn't used as of Windows XP.

### 3.10. Summary

1. A Base block points to a root cell, which contains a Key node.
2. A Key node points to a parent Key node, to a Subkeys list (a subkey is a Key node too), to a Key values list, to a Key security item.
3. A Subkeys list can be subdivided with the help of the Index root structure.
4. A Key value points to data. Data may be stored in the Data offset field of a Key value structure, in a separate cell, or in a bunch of cells. In the last case, a Key value points to the Big data structure in a cell.