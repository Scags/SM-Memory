#pragma once

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
	std::string m_Name = "";
#ifdef PLATFORM_WINDOWS
	HMODULE m_Handle = {};
#elif defined PLATFORM_POSIX
	void *m_Handle = {};
#endif
	void *m_BaseAddress = {};

	static void *LoadBaseAddress(const std::string &name);

public:
	DynLib(const std::string &name);
	~DynLib();
	void *ResolveSymbol(const char *sym);
	void *FindPattern(const char *pattern, size_t len);

	std::string &GetName() { return m_Name; }
	void *GetBaseAddress() { return m_BaseAddress; }

	bool IsLoaded() { return GetBaseAddress() != nullptr; }

	static std::string ErrorMessage();
};
