#include "extension.h"
#include "dynlib.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

DynLib::DynLib(const std::string &name)
	: m_Name(name)
	, m_Handle(nullptr)
{
#ifdef PLATFORM_WINDOWS
	m_Handle = GetModuleHandleA(name.c_str());
	if (m_Handle == nullptr)
	{
		// Doesn't exist, try and load it
		m_Handle = LoadLibraryA(name.c_str());
		if (m_Handle == nullptr)
		{
			smutils->LogError(myself, "Failed to load library %s (Error: 0x%X)", name.c_str(), GetLastError());
			return;
		}
	}

	m_BaseAddress = (void *)m_Handle;
#elif defined PLATFORM_POSIX
	m_Handle = dlopen(name.c_str(), RTLD_LAZY);
	if (m_Handle == nullptr)
	{
		smutils->LogError(myself, "Failed to load library %s (Error: %s)", name.c_str(), dlerror());
		return;
	}

	dladdr(m_Handle, &m_Info);
#endif
}

DynLib::~DynLib()
{
#if defined PLATFORM_POSIX
	if (m_Handle != nullptr)
		dlclose(m_Handle);
#endif
}

void *DynLib::ResolveSymbol(const char *sym)
{
	return memutils->ResolveSymbol((void *)m_Handle, sym);
}

void *DynLib::FindPattern(const char *pattern, size_t len)
{
	return memutils->FindPattern((void *)m_Handle, pattern, len);
}