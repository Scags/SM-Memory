#include "rtti.h"

namespace rtti
{
#ifdef PLATFORM_WINDOWS
	RTTICompleteObjectLocator *GetObjectCOL(CONST VOID *classptr)
	{
		CONST VOID *vtable = *(CONST VOID **)classptr;
		return *(RTTICompleteObjectLocator **)((CONST BYTE *)vtable - sizeof(PVOID));
	}

#elif defined PLATFORM_POSIX
	template <typename SearchType>
	ParseFunc_t<SearchType> GetParseFunc(const std::string &entry)
	{
		static std::unordered_map<std::string, ParseFunc_t<SearchType>> searchmap{
			{"_ZTVN10__cxxabiv117__class_type_infoE", ParseStop},
			{"_ZTVN10__cxxabiv120__si_class_type_infoE", ParseSI},
			{"_ZTVN10__cxxabiv121__vmi_class_type_infoE", ParseVMI},
			{"_ZTVN10__cxxabiv119__pointer_type_infoE", ParseStop}};

		return searchmap[entry];
	}

	static std::unordered_map<const void *, std::string> g_SearchCache;

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
			ParseFunc_t<SearchType> func = GetParseFunc<SearchType>(dlinfo.dli_sname);
			return func;
		}
		return nullptr;
	}

	template <typename SearchType>
	std::tuple<const void *, long> ParseSI(const void *type_info, SearchType target)
	{
		if constexpr (std::is_same_v<SearchType, const char *>)
		{
			std::string base_name = GetTypeInfoName(type_info);
			if (base_name == target)
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
			std::string base_name = GetTypeInfoName(type_info);
			if (base_name == target)
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
			std::string base_name = GetTypeInfoName(type_info);
			pp = base_name == target ? type_info : nullptr;
		}
		else
		{
			pp = type_info == target ? type_info : nullptr;
		}
		return std::make_tuple(pp, 0);
	}
#endif

	const void *DynamicCast(const void *src, const char *typeName)
	{
#ifdef PLATFORM_WINDOWS
		RTTICompleteObjectLocator *srcLocator = GetObjectCOL(src);
		RTTIClassHierarchyDescriptor *srcDescriptor = srcLocator->hierarchyDescriptor;

		DWORD numitems = srcDescriptor->numBaseClasses;
		RTTIBaseClassDescriptor **srcClassArray = srcDescriptor->baseClassArray;

		// Check each TI name for a match
		for (DWORD i = 0; i < numitems; i++)
		{
			std::string demangled = DemangleTypeName(srcClassArray[i]->typeDescriptor->name);
			if (demangled == typeName)
			{
				return (PVOID)((PBYTE)src + srcClassArray[i]->mdisp);
			}
		}
#elif defined PLATFORM_POSIX
		class_type_info *srcType = (class_type_info *)GetClassTypeInfo(src);
		auto func = ParseFuncFromVtable<const char *>(srcType);
		if (func == nullptr)
			return nullptr;

		auto got = func(srcType, typeName);
		const void *targettinfo = std::get<0>(got);
		if (targettinfo != nullptr)
		{
			// Now add the thisoffset
			long offset = std::get<1>(got);
			return (const void *)((const uint8_t *)src + offset);
		}
#endif
		return nullptr;
	}

	const void *DynamicCast2(const void *src, const void *dstType)
	{
#ifdef PLATFORM_WINDOWS
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
#elif defined PLATFORM_POSIX
		class_type_info *srcType = (class_type_info *)GetClassTypeInfo(src);
		auto func = ParseFuncFromVtable<const void *>(srcType);
		if (func == nullptr)
			return nullptr;

		auto got = func(srcType, dstType);
		const void *targettinfo = std::get<0>(got);
		if (targettinfo != nullptr)
		{
			// Now add the thisoffset
			long offset = std::get<1>(got);
			return (void *)((uint8_t *)src + offset);
		}
#endif

		return nullptr;
	}

	const void *GetClassTypeInfo(const void *classptr)
	{
		const void *vtable = *(const void **)classptr;
#ifdef PLATFORM_WINDOWS
		RTTICompleteObjectLocator *srcLocator = *(RTTICompleteObjectLocator **)((PBYTE)vtable - sizeof(PVOID));
		return srcLocator->typeDescriptor;
#elif defined PLATFORM_POSIX
		class_type_info *srcType = *(class_type_info **)((uint8_t *)vtable - sizeof(void *));
		return (void *)srcType;
#endif
		return nullptr;
	}

	const void *GetClassTypeInfoByName(const void *classptr, const char *typeName)
	{
#ifdef PLATFORM_WINDOWS
		RTTICompleteObjectLocator *srcLocator = GetObjectCOL(classptr);
		RTTIClassHierarchyDescriptor *srcDescriptor = srcLocator->hierarchyDescriptor;

		DWORD numitems = srcDescriptor->numBaseClasses;
		RTTIBaseClassDescriptor **srcClassArray = srcDescriptor->baseClassArray;

		for (DWORD i = 0; i < numitems; i++)
		{
			RTTITypeDescriptor *baseType = srcClassArray[i]->typeDescriptor;
			std::string demangled = DemangleTypeName(baseType->name);
			if (demangled == typeName)
			{
				return (PVOID)baseType;
			}
		}
#elif defined PLATFORM_POSIX
		class_type_info *srcType = (class_type_info *)GetClassTypeInfo(classptr);
		auto func = ParseFuncFromVtable<const char *>(srcType);
		if (func != nullptr)
		{
			auto got = func(srcType, typeName);
			const void *targettinfo = std::get<0>(got);
			return (void *)targettinfo;
		}
#endif
		return nullptr;
	}

	std::string GetClassTypeInfoName(const void *classptr)
	{
		const void *pInfo = GetClassTypeInfo(classptr);
		return GetTypeInfoName(pInfo);
	}

	std::string GetTypeInfoName(const void *typeInf)
	{
#ifdef PLATFORM_WINDOWS
		RTTITypeDescriptor *srcType = (RTTITypeDescriptor *)typeInf;
		return DemangleTypeName(srcType->name);
#elif defined PLATFORM_POSIX
		class_type_info *srcType = (class_type_info *)typeInf;
		return DemangleTypeName(srcType->name);
#endif
		return "";
	}

	std::string DemangleTypeName(const char *mangledName)
	{
#ifdef PLATFORM_WINDOWS
		// Yoinked from boost
		uint8_t startidx = *mangledName == '.' ? 1 : 0;
		allocation_function alloc = [](std::size_t size)
		{ return static_cast<void *>(new char[size]); };
		free_function free_f = [](void *p)
		{ delete[] static_cast<char *>(p); };
		std::unique_ptr<char> name(__unDName(nullptr, &mangledName[startidx], 0, alloc, free_f, static_cast<unsigned short>(0xA800)));

		return name.get();
#elif defined PLATFORM_POSIX
		char *demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, nullptr);
		if (demangled == nullptr)
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
}