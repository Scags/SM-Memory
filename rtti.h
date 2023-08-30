#ifndef RTTI
#define RTTI

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
#elif defined PLATFORM_POSIX
#include <cxxabi.h>
#include <unordered_map>
#endif

#include <string>
#include <memory>

#ifdef PLATFORM_WINDOWS
using allocation_function = void *(__cdecl *)(std::size_t);
using free_function = void(__cdecl *)(void *);
extern "C" PCHAR __unDName(PCHAR, LPCSTR, INT, allocation_function, free_function, INT);
#endif

std::string DemangleTypeName(const char *mangledName)
{
#ifdef PLATFORM_WINDOWS
	// Yoinked from boost
	uint8_t startidx = *mangledName == '.' ? 1 : 0;
	allocation_function alloc = [](std::size_t size)
	{ return static_cast<void *>(new char[size]); };
	free_function free_f = [](void *p)
	{ delete[] static_cast<char *>(p); };
	std::unique_ptr<char> name{__unDName(
		nullptr,
		&mangledName[startidx],
		0,
		alloc,
		free_f,
		static_cast<unsigned short>(0xA800))};

	return name.get();
#elif defined PLATFORM_POSIX
	char *demangled = abi::__cxa_demangle(mangledName, NULL, NULL, NULL);
	if (demangled == NULL)
	{
		return mangledName;
	}
	else
	{
		std::string ret = demangled;
		free(demangled);
		return ret;
	}
#endif
	return "";
}

#ifdef PLATFORM_WINDOWS
struct RTTITypeDescriptor
{
	DWORD typeInfo;
	DWORD internalReference;
	CHAR name[1];
};

struct RTTIClassHierarchyDescriptor;

struct RTTICompleteObjectLocator
{
	DWORD signature;
	DWORD offset;
	DWORD cdOffset;
	RTTITypeDescriptor *typeDescriptor;
	RTTIClassHierarchyDescriptor *hierarchyDescriptor;
#ifdef PLATFORM_X64
	PVOID self;
#endif
};

struct RTTIBaseClassDescriptor;
struct RTTIClassHierarchyDescriptor
{
	DWORD signature;
	DWORD attributes;
	DWORD numBaseClasses;
	RTTIBaseClassDescriptor **baseClassArray;
};

struct RTTIBaseClassDescriptor
{
	RTTITypeDescriptor *typeDescriptor;
	DWORD numContainedBases;
	DWORD mdisp;
	DWORD pdisp;
	DWORD vdisp;
	DWORD attributes;
	RTTIClassHierarchyDescriptor *hierarchyDescriptor;
};

RTTICompleteObjectLocator *GetObjectCOL(PVOID classptr)
{
	PVOID vtable = *(PVOID *)classptr;
	return *(RTTICompleteObjectLocator **)((PBYTE)vtable - sizeof(PVOID));
}

PVOID __dynamic_cast(CONST PVOID src, LPCSTR dstTypeName)
{
	RTTICompleteObjectLocator *srcLocator = GetObjectCOL(src);
	RTTIClassHierarchyDescriptor *srcDescriptor = srcLocator->hierarchyDescriptor;

	DWORD numitems = srcDescriptor->numBaseClasses;
	RTTIBaseClassDescriptor **srcClassArray = srcDescriptor->baseClassArray;

	// Check each TI name for a match
	for (DWORD i = 0; i < numitems; i++)
	{
		std::string demangled = DemangleTypeName(srcClassArray[i]->typeDescriptor->name);
		if (!strcmp(demangled.c_str(), dstTypeName))
		{
			return (PVOID)((PBYTE)src + srcClassArray[i]->mdisp);
		}
	}

	return NULL;
}

PVOID __dynamic_cast2(CONST PVOID src, PVOID dstType)
{
	RTTICompleteObjectLocator *srcLocator = GetObjectCOL(src);
	RTTIClassHierarchyDescriptor *srcDescriptor = srcLocator->hierarchyDescriptor;

	DWORD numitems = srcDescriptor->numBaseClasses;
	RTTIBaseClassDescriptor **srcClassArray = srcDescriptor->baseClassArray;

	// Check each TI for a match
	for (DWORD i = 0; i < numitems; i++)
	{
		if ((PVOID)srcClassArray[i]->typeDescriptor == dstType)
		{
			return (PVOID)((PBYTE)src + srcClassArray[i]->mdisp);
		}
	}

	return NULL;
}

PVOID GetClassTypeInfo(PVOID classptr)
{
	PVOID vtable = *(PVOID *)classptr;
	RTTICompleteObjectLocator *srcLocator = *(RTTICompleteObjectLocator **)((PBYTE)vtable - sizeof(PVOID));
	return srcLocator->typeDescriptor;
}

PVOID GetClassTypeInfoByName(PVOID classptr, LPCSTR classname)
{
	RTTICompleteObjectLocator *srcLocator = GetObjectCOL(classptr);
	RTTIClassHierarchyDescriptor *srcDescriptor = srcLocator->hierarchyDescriptor;

	DWORD numitems = srcDescriptor->numBaseClasses;
	RTTIBaseClassDescriptor **srcClassArray = srcDescriptor->baseClassArray;

	for (DWORD i = 0; i < numitems; i++)
	{
		RTTITypeDescriptor *baseType = srcClassArray[i]->typeDescriptor;
		std::string demangled = DemangleTypeName(baseType->name);
		if (!strcmp(demangled.c_str(), classname))
		{
			return (PVOID)baseType;
		}
	}
	return NULL;
}

std::string GetTypeInfoName(CONST PVOID typeInf)
{
	RTTITypeDescriptor *srcType = (RTTITypeDescriptor *)typeInf;
	return DemangleTypeName(srcType->name);
}

#elif defined PLATFORM_POSIX
struct base_class_type_info
{
	const void *type_info;
	long offset_flags; // >> 8 = thisoffset
};

struct class_type_info
{
	const void *type_info;
	const char *name;
};

struct pointer_type_info : public class_type_info
{
	long flags;
	const void *pointee;
};

struct si_class_type_info : public class_type_info
{
	const void *base_class;
};

struct vmi_class_type_info : public class_type_info
{
	long vmi_flags;
	long vmi_base_count;
	base_class_type_info base_classes[1];
};

void *GetClassTypeInfo(const void *classptr)
{
	void *vtable = *(void **)classptr;
	class_type_info *srcType = *(class_type_info **)((uint8_t *)vtable - sizeof(void *));
	return (void *)srcType;
}

std::string GetTypeInfoName(const void *typeInf)
{
	class_type_info *srcType = (class_type_info *)typeInf;
	return DemangleTypeName(srcType->name);
}

// <typeinfo, thisoffs>
// Thisoffs is only acquired from VMI types
template <typename SearchType>
std::tuple<const void *, long> ParseSI(const void *, SearchType);
template <typename SearchType>
std::tuple<const void *, long> ParseVMI(const void *, SearchType);
template <typename SearchType>
std::tuple<const void *, long> ParseStop(const void *, SearchType);

// The concept is unnecessary but hey it's pretty cool
template <typename SearchType>
using ParseFunc_t = std::tuple<const void *, long>(*)(const void *, SearchType);

// Now that I've written this I've realized that I probably should have made it
// not all in a header file
template <typename SearchType>
static ParseFunc_t<SearchType> GetParseFunc(const std::string &entry)
{
	static std::unordered_map<std::string, ParseFunc_t<SearchType>> searchmap{
		{"_ZTVN10__cxxabiv117__class_type_infoE", ParseStop},
		{"_ZTVN10__cxxabiv120__si_class_type_infoE", ParseSI},
		{"_ZTVN10__cxxabiv121__vmi_class_type_infoE", ParseVMI},
		{"_ZTVN10__cxxabiv119__pointer_type_infoE", ParseStop}};

	return searchmap[entry];
}

static std::unordered_map<const void *, std::string> g_SearchCache;

// Capable to search via type name or by raw type info ptr
template <typename SearchType>
static ParseFunc_t<SearchType> ParseFuncFromVtable(const void *type_info)
{
	class_type_info *base = (class_type_info *)type_info;
	const void *vtable = base->type_info;

	if (g_SearchCache.find(vtable) != g_SearchCache.end())
	{
		return GetParseFunc<SearchType>(g_SearchCache[vtable]);
	}

	// To discover what type of vtable we're looking at (and thus the type_info type)
	// we need to grab the symbol that the type_info points to, which is held in libsteam_api.so
	Dl_info dlinfo;
	if (dladdr(vtable, &dlinfo))
	{
		// And we cache it because idk how expensive dladdr is
		g_SearchCache[vtable] = dlinfo.dli_sname;
		ParseFunc_t<SearchType> fff = GetParseFunc<SearchType>(dlinfo.dli_sname);
		return fff;
	}
	return nullptr;
}

template <typename SearchType>
std::tuple<const void *, long> ParseSI(const void *type_info, SearchType target)
{
	if constexpr (std::is_same_v<SearchType, const char *>)
	{
		const char *base_name = GetTypeInfoName(type_info).c_str();
		if (!strcmp(base_name, target))
			return std::make_tuple(type_info, 0);
	}
	else
	{
		if (type_info == target)
			return std::make_tuple(type_info, 0);
	}

	si_class_type_info *si = (si_class_type_info *)type_info;
	auto func = ParseFuncFromVtable<SearchType>(si->base_class);
	if (func == nullptr)
		return std::make_tuple(nullptr, 0);

	return func(si->base_class, target);
}

template <typename SearchType>
std::tuple<const void *, long> ParseVMI(const void *type_info, SearchType target)
{
	if constexpr (std::is_same_v<SearchType, const char *>)
	{
		const char *base_name = GetTypeInfoName(type_info).c_str();
		if (!strcmp(base_name, target))
			return std::make_tuple(type_info, 0);
	}
	else
	{
		if (type_info == target)
			return std::make_tuple(type_info, 0);
	}

	vmi_class_type_info *vmi = (vmi_class_type_info *)type_info;
	for (int i = 0; i < vmi->vmi_base_count; i++)
	{
		auto func = ParseFuncFromVtable<SearchType>(vmi->base_classes[i].type_info);
		if (func == nullptr)
			continue;

		std::tuple<const void *, long> got = func(vmi->base_classes[i].type_info, target);
		const void *targettinfo = std::get<0>(got);
		// I really am not sure calculating the thisoffset works properly with multiple multiple inheritance
		// But maybe no one will reach that point
		// I *think* it's right, though
		long curroffset = std::get<1>(got);
		// All that tuple work only for it to only be utilized here
		if (targettinfo != nullptr)
			return std::make_tuple(targettinfo, curroffset == 0 ? vmi->base_classes[i].offset_flags >> 8 : curroffset);
	}

	return std::make_tuple(nullptr, 0);
}

template <typename SearchType>
std::tuple<const void *, long> ParseStop(const void *type_info, SearchType target)
{
	const void *pp;
	if constexpr (std::is_same_v<SearchType, const char *>)
	{
		const char *base_name = GetTypeInfoName(type_info).c_str();
		pp = !strcmp(base_name, target) ? type_info : nullptr;
	}
	else
	{
		pp = type_info == target ? type_info : nullptr;
	}
	return std::make_tuple(pp, 0);
}

void *__dynamic_cast(const void *src, const char *typeName)
{
	class_type_info *srcType = (class_type_info *)GetClassTypeInfo(src);
	auto func = ParseFuncFromVtable<const char *>(srcType);
	if (func == nullptr)
		return nullptr;

	auto got = func(srcType, typeName);
	const void *targettinfo = std::get<0>(got);
	if (targettinfo == nullptr)
		return nullptr;

	// Now add the thisoffset
	long offset = std::get<1>(got);

	return (void *)((uint8_t *)src + offset);
}

void *__dynamic_cast2(const void *src, const void *dstType)
{
	class_type_info *srcType = (class_type_info *)GetClassTypeInfo(src);
	auto func = ParseFuncFromVtable<const void *>(srcType);
	if (func == nullptr)
		return nullptr;

	auto got = func(srcType, dstType);
	const void *targettinfo = std::get<0>(got);
	if (targettinfo == nullptr)
		return nullptr;

	// Now add the thisoffset
	long offset = std::get<1>(got);

	return (void *)((uint8_t *)src + offset);
}

void *GetClassTypeInfoByName(const void *src, const char *typeName)
{
	class_type_info *srcType = (class_type_info *)GetClassTypeInfo(src);
	auto func = ParseFuncFromVtable<const char *>(srcType);
	if (func == nullptr)
		return nullptr;

	auto got = func(srcType, typeName);
	const void *targettinfo = std::get<0>(got);
	return (void *)targettinfo;
}

#endif

std::string GetClassTypeInfoName(void *classptr)
{
	void *pInfo = GetClassTypeInfo(classptr);
	return GetTypeInfoName(pInfo);
}

#endif // RTTI