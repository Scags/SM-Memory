#ifndef DYNLIB
#define DYNLIB

#include "sm_platform.h"
#include <string>

#ifdef PLATFORM_WINDOWS
#include <libloaderapi.h>
#include <Psapi.h>
#endif

// Would use LibrarySys but meneeds handle ptr
struct DynLib
{
	std::string m_Name;
#ifdef PLATFORM_WINDOWS
	HMODULE m_Handle;
	void *m_BaseAddress;
#elif defined PLATFORM_POSIX
	void *m_Handle;
	Dl_info m_Info;
#endif

	DynLib(std::string name);
	~DynLib();
	void *ResolveSymbol(const char *sym);
	void *FindPattern(const char *pattern, size_t len);
};
#endif 	// DYNLIB