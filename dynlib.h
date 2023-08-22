#ifndef DYNLIB
#define DYNLIB

#include "sm_platform.h"
#include <string>

#ifdef PLATFORM_WINDOWS
#include <libloaderapi.h>
#include <Psapi.h>
#endif

// Would use LibrarySys but meneeds handle ptr
class DynLib
{
private:
	std::string m_Name;
#ifdef PLATFORM_WINDOWS
	HMODULE m_Handle;
	void *m_BaseAddress;
#elif defined PLATFORM_POSIX
	void *m_Handle;
	Dl_info m_Info;
#endif

public:
	DynLib(const std::string &name);
	~DynLib();
	void *ResolveSymbol(const char *sym);
	void *FindPattern(const char *pattern, size_t len);
	void *GetExport(const char *name);

	std::string &GetName() { return m_Name; }
	void *GetBaseAddress()
	{
#ifdef PLATFORM_WINDOWS
		return m_BaseAddress;
#elif defined PLATFORM_POSIX
		return m_Info.dli_fbase;
#endif
	}

#ifdef PLATFORM_POSIX
	Dl_info *GetDlInfo() { return &m_Info; }
#endif

};
#endif 	// DYNLIB