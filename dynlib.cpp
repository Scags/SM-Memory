#include "extension.h"
#include "dynlib.h"

#ifdef PLATFORM_WINDOWS
#include <libloaderapi.h>
#endif

DynLib::DynLib(const char *name)
	: m_Name(name)
	, m_Handle(nullptr)
{
#ifdef PLATFORM_WINDOWS
	m_Handle = GetModuleHandleA(name);
	if (m_Handle == nullptr)
		return;

	m_BaseAddress = (void *)m_Handle;
#elif defined PLATFORM_POSIX
	m_Handle = dlopen(name, RTLD_LAZY);
	if (m_Handle == nullptr)
		return;

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