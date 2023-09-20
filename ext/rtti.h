#pragma once

#include "extension.h"

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

namespace rtti
{
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
#endif

#ifdef PLATFORM_WINDOWS
	static RTTICompleteObjectLocator *GetObjectCOL(CONST void *);

#elif defined PLATFORM_POSIX

	// <typeinfo, thisoffs>
	// Thisoffs is only acquired from VMI types
	template <typename SearchType>
	std::tuple<const void *, long> ParseSI(const void *, SearchType);
	template <typename SearchType>
	std::tuple<const void *, long> ParseVMI(const void *, SearchType);
	template <typename SearchType>
	std::tuple<const void *, long> ParseStop(const void *, SearchType);

	template <typename SearchType>
	using ParseFunc_t = std::tuple<const void *, long> (*)(const void *, SearchType);

	template <typename SearchType>
	static ParseFunc_t<SearchType> GetParseFunc(const std::string &);

	// Capable to search via type name or by raw type info ptr
	template <typename SearchType>
	static ParseFunc_t<SearchType> ParseFuncFromVtable(const void *);

	template <typename SearchType>
	static std::tuple<const void *, long> ParseSI(const void *, SearchType);
	template <typename SearchType>
	static std::tuple<const void *, long> ParseVMI(const void *, SearchType);
	template <typename SearchType>
	static std::tuple<const void *, long> ParseStop(const void *, SearchType);

#endif

	const void *DynamicCast(const void *, const char *);
	const void *DynamicCast2(const void *, const void *);

	const void *GetClassTypeInfo(const void *);
	const void *GetClassTypeInfoByName(const void *, const char *);
	std::string GetClassTypeInfoName(const void *);
	std::string GetTypeInfoName(const void *);

	std::string DemangleTypeName(const char *);
}