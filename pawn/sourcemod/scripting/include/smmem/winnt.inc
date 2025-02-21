#if defined _winnt_included
    #endinput
#endif

#define _winnt_included

#include <smmem>

/**
 * Note:
 * To properly parse a PE header, either acquire a DynLib object or 
 * at least the base address of a library, file stream, or memory block.
 * 
 * Intialize it:
 * PIMAGE_DOS_HEADER pe = PIMAGE_DOS_HEADER(address);
 * 
 * And, of course, check its magic number.
 * 
 * The representation of the methodmap "object" in each case is
 * actually a syntactically wrapped over pointer. There is no
 * copying done whatsoever, therefore you have a pointer to each
 * structure and any reading (or writing) will be dealing directly
 * with the memory block you passed. This is also why all of the
 * methodmaps start with "P".
**/

methodmap PIMAGE_FILE_HEADER
{
    public PIMAGE_FILE_HEADER(any p)
    {
        return view_as< PIMAGE_FILE_HEADER >(p);
    }
    property int Machine
    {
        public get() { return ReadWord(this); }
    }
    property int NumberOfSections
    {
        public get() { return ReadWord(view_as< int >(this) + 2); }
    }
    property int TimeDateStamp
    {
        public get() { return ReadDWord(view_as< int >(this) + 4); }
    }
    property int PointerToSymbolTable
    {
        public get() { return ReadDWord(view_as< int >(this) + 8); }
    }
    property int NumberOfSymbols
    {
        public get() { return ReadDWord(view_as< int >(this) + 12); }
    }
    property int SizeOfOptionalHeader
    {
        public get() { return ReadWord(view_as< int >(this) + 16); }
    }
    property int Characteristics
    {
        public get() { return ReadWord(view_as< int >(this) + 18); }
    }
}

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved external references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Aggressively trim working set
#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define IMAGE_FILE_SYSTEM                    0x1000  // System File.
#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_TARGET_HOST       0x0001  // Useful for indicating we want to interact with the host and not a WoW guest.
#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define IMAGE_FILE_MACHINE_SH3DSP            0x01a3
#define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define IMAGE_FILE_MACHINE_SH5               0x01a8  // SH5
#define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define IMAGE_FILE_MACHINE_THUMB             0x01c2  // ARM Thumb/Thumb-2 Little-Endian
#define IMAGE_FILE_MACHINE_ARMNT             0x01c4  // ARM Thumb-2 Little-Endian
#define IMAGE_FILE_MACHINE_AM33              0x01d3
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define IMAGE_FILE_MACHINE_POWERPCFP         0x01f1
#define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
#define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS
#define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_TRICORE           0x0520  // Infineon
#define IMAGE_FILE_MACHINE_CEF               0x0CEF
#define IMAGE_FILE_MACHINE_EBC               0x0EBC  // EFI Byte Code
#define IMAGE_FILE_MACHINE_AMD64             0x8664  // AMD64 (K8)
#define IMAGE_FILE_MACHINE_M32R              0x9041  // M32R little-endian
#define IMAGE_FILE_MACHINE_ARM64             0xAA64  // ARM64 Little-Endian
#define IMAGE_FILE_MACHINE_CEE               0xC0EE

#define IMAGE_SIZEOF_FILE_HEADER             20

#if 0
typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#endif

methodmap PIMAGE_DATA_DIRECTORY
{
    public PIMAGE_DATA_DIRECTORY(any p)
    {
        return view_as< PIMAGE_DATA_DIRECTORY >(p);
    }
    property int VirtualSize
    {
        public get() { return ReadDWord(this); }
    }
    property int Size
    {
        public get() { return ReadDWord(view_as< int >(this) + 4); }
    }
}

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

#if 0
typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;

    //
    // NT additional fields.
    //

    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;
#endif

methodmap PIMAGE_OPTIONAL_HEADER32
{
    public PIMAGE_OPTIONAL_HEADER32(any p)
    {
        return view_as< PIMAGE_OPTIONAL_HEADER32 >(p);
    }
    property int Magic
    {
        public get() { return ReadWord(this); }
    }
    property int MajorLinkerVersion
    {
        public get() { return ReadByte(view_as< int >(this) + 2); }
    }
    property int MinorLinkerVersion
    {
        public get() { return ReadByte(view_as< int >(this) + 3); }
    }
    property int SizeOfCode
    {
        public get() { return ReadDWord(view_as< int >(this) + 4); }
    }
    property int SizeOfInitializedData
    {
        public get() { return ReadDWord(view_as< int >(this) + 8); }
    }
    property int SizeOfUninitializedData
    {
        public get() { return ReadDWord(view_as< int >(this) + 12); }
    }
    property int AddressOfEntryPoint
    {
        public get() { return ReadDWord(view_as< int >(this) + 16); }
    }
    property int BaseOfCode
    {
        public get() { return ReadDWord(view_as< int >(this) + 20); }
    }
    property int BaseOfData
    {
        public get() { return ReadDWord(view_as< int >(this) + 24); }
    }
    property int ImageBase
    {
        public get() { return ReadDWord(view_as< int >(this) + 28); }
    }
    property int SectionAlignment
    {
        public get() { return ReadDWord(view_as< int >(this) + 32); }
    }
    property int FileAlignment
    {
        public get() { return ReadDWord(view_as< int >(this) + 36); }
    }
    property int MajorOperatingSystemVersion
    {
        public get() { return ReadWord(view_as< int >(this) + 40); }
    }
    property int MinorOperatingSystemVersion
    {
        public get() { return ReadWord(view_as< int >(this) + 42); }
    }
    property int MajorImageVersion
    {
        public get() { return ReadWord(view_as< int >(this) + 44); }
    }
    property int MinorImageVersion
    {
        public get() { return ReadWord(view_as< int >(this) + 46); }
    }
    property int MajorSubsystemVersion
    {
        public get() { return ReadWord(view_as< int >(this) + 48); }
    }
    property int MinorSubsystemVersion
    {
        public get() { return ReadWord(view_as< int >(this) + 50); }
    }
    property int Win32VersionValue
    {
        public get() { return ReadDWord(view_as< int >(this) + 52); }
    }
    property int SizeOfImage
    {
        public get() { return ReadDWord(view_as< int >(this) + 56); }
    }
    property int SizeOfHeaders
    {
        public get() { return ReadDWord(view_as< int >(this) + 60); }
    }
    property int CheckSum
    {
        public get() { return ReadDWord(view_as< int >(this) + 64); }
    }
    property int Subsystem
    {
        public get() { return ReadWord(view_as< int >(this) + 68); }
    }
    property int DllCharacteristics
    {
        public get() { return ReadWord(view_as< int >(this) + 70); }
    }
    property int SizeOfStackReserve
    {
        public get() { return ReadDWord(view_as< int >(this) + 72); }
    }
    property int SizeOfStackCommit
    {
        public get() { return ReadDWord(view_as< int >(this) + 76); }
    }
    property int SizeOfHeapReserve
    {
        public get() { return ReadDWord(view_as< int >(this) + 80); }
    }
    property int SizeOfHeapCommit
    {
        public get() { return ReadDWord(view_as< int >(this) + 84); }
    }
    property int LoaderFlags
    {
        public get() { return ReadDWord(view_as< int >(this) + 88); }
    }
    property int NumberOfRvaAndSizes
    {
        public get() { return ReadDWord(view_as< int >(this) + 92); }
    }

    public PIMAGE_DATA_DIRECTORY DataDirectory(int dir)
    {
        return PIMAGE_DATA_DIRECTORY(view_as< int >(this) + 96 + dir * 8);
    }
}

#define PIMAGE_OPTIONAL_HEADER PIMAGE_OPTIONAL_HEADER32

#if 0
typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;
#endif

methodmap PIMAGE_NT_HEADERS32
{
    public PIMAGE_NT_HEADERS32(any p)
    {
        return view_as< PIMAGE_NT_HEADERS32 >(p);
    }
    property int Signature
    {
        public get() { return ReadDWord(this); }
    }
    property PIMAGE_FILE_HEADER FileHeader
    {
        public get() { return PIMAGE_FILE_HEADER(view_as< int >(this) + 4); }
    }
    property PIMAGE_OPTIONAL_HEADER OptionalHeader
    {
        public get() { return PIMAGE_OPTIONAL_HEADER(view_as< int >(this) + 4 + IMAGE_SIZEOF_FILE_HEADER); }
    }
}

// I will be angry with my past self when TF2 goes 64-bit
#define PIMAGE_NT_HEADERS PIMAGE_NT_HEADERS32

// Subsystem Values

#define IMAGE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem.
#define IMAGE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.
#define IMAGE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem.
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   // image runs in the Posix character subsystem.
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS       8   // image is a native Win9x driver.
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI       9   // Image runs in the Windows CE subsystem.
#define IMAGE_SUBSYSTEM_EFI_APPLICATION      10  //
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11   //
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER   12  //
#define IMAGE_SUBSYSTEM_EFI_ROM              13
#define IMAGE_SUBSYSTEM_XBOX                 14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION 16
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG    17

// DllCharacteristics Entries

//      IMAGE_LIBRARY_PROCESS_INIT            0x0001     // Reserved.
//      IMAGE_LIBRARY_PROCESS_TERM            0x0002     // Reserved.
//      IMAGE_LIBRARY_THREAD_INIT             0x0004     // Reserved.
//      IMAGE_LIBRARY_THREAD_TERM             0x0008     // Reserved.
#define IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA    0x0020  // Image can handle a high entropy 64-bit virtual address space.
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE 0x0040     // DLL can move.
#define IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY    0x0080     // Code Integrity Image
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT    0x0100     // Image is NX compatible
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION 0x0200     // Image understands isolation and doesn't want it
#define IMAGE_DLLCHARACTERISTICS_NO_SEH       0x0400     // Image does not use SEH.  No SE handler may reside in this image
#define IMAGE_DLLCHARACTERISTICS_NO_BIND      0x0800     // Do not bind this image.
#define IMAGE_DLLCHARACTERISTICS_APPCONTAINER 0x1000     // Image should execute in an AppContainer
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER   0x2000     // Driver uses WDM model
#define IMAGE_DLLCHARACTERISTICS_GUARD_CF     0x4000     // Image supports Control Flow Guard.
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE     0x8000

#if 0
typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[4];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;
#endif

#define IMAGE_DOS_SIGNATURE                 0x5A4D      // MZ
#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00

methodmap PIMAGE_DOS_HEADER
{
    public PIMAGE_DOS_HEADER(any p)
    {
        return view_as<PIMAGE_DOS_HEADER>(p);
    }
    property int e_magic
    {
        public get() { return ReadWord(this); }
    }
    property int e_cblp
    {
        public get() { return ReadWord(view_as< int >(this) + 2); }
    }
    property int e_cp
    {
        public get() { return ReadWord(view_as< int >(this) + 4); }
    }
    property int e_crlc
    {
        public get() { return ReadWord(view_as< int >(this) + 6); }
    }
    property int e_cparhdr
    {
        public get() { return ReadWord(view_as< int >(this) + 8); }
    }
    property int e_minalloc
    {
        public get() { return ReadWord(view_as< int >(this) + 10); }
    }
    property int e_maxalloc
    {
        public get() { return ReadWord(view_as< int >(this) + 12); }
    }
    property int e_ss
    {
        public get() { return ReadWord(view_as< int >(this) + 14); }
    }
    property int e_sp
    {
        public get() { return ReadWord(view_as< int >(this) + 16); }
    }
    property int e_csum
    {
        public get() { return ReadWord(view_as< int >(this) + 18); }
    }
    property int e_ip
    {
        public get() { return ReadWord(view_as< int >(this) + 20); }
    }
    property int e_cs
    {
        public get() { return ReadWord(view_as< int >(this) + 22); }
    }
    property int e_lfarlc
    {
        public get() { return ReadWord(view_as< int >(this) + 24); }
    }
    property int e_ovno
    {
        public get() { return ReadWord(view_as< int >(this) + 26); }
    }
    property any e_res
    {
        public get() { return view_as< int >(this) + 28; }
    }
    property int e_oemid
    {
        public get() { return ReadWord(view_as< int >(this) + 36); }
    }
    property int e_oeminfo
    {
        public get() { return ReadWord(view_as< int >(this) + 38); }
    }
    property any e_res2
    {
        public get() { return view_as< int >(this) + 40; }
    }
    property int e_lfanew
    {
        public get() { return ReadWord(view_as< int >(this) + 60); }
    }
}
