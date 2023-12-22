#include "extension.h"
#include "dynlib.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <sstream>
#include <fstream>

// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
static bool endswith(const std::string &str, const char *suffix, unsigned suffixLen)
{
	return str.size() >= suffixLen && str.compare(str.size() - suffixLen, suffixLen, suffix, suffixLen) == 0;
}

DynLib::DynLib(const std::string &name)
	: m_Name(name)
{
#ifdef PLATFORM_WINDOWS
	m_Handle = LoadLibraryA(m_Name.c_str());
	if (m_Handle == nullptr)
	{
		// Try and fix up the name
		if (!endswith(m_Name, ".dll", 4))
		{
			m_Name += ".dll";
			m_Handle = LoadLibraryA(m_Name.c_str());
		}
	}

	m_BaseAddress = (void *)m_Handle;
#elif defined PLATFORM_POSIX
	m_Handle = dlopen(m_Name.c_str(), RTLD_LAZY);
	if (m_Handle == nullptr)
	{
		// Try and fix up the name
		if (!endswith(m_Name, ".so", 3))
		{
			m_Name += ".so";
			m_Handle = dlopen(m_Name.c_str(), RTLD_LAZY);
		}
	}

	if (m_Handle != nullptr)
	{
		m_BaseAddress = LoadBaseAddress(m_Name);
		// Name is not full path
		if (m_BaseAddress == nullptr)
		{
			char abspath[260];
			char *rpath = realpath(m_Name.c_str(), abspath);
			if (rpath != nullptr)
			{
				m_Name = rpath;
				m_BaseAddress = LoadBaseAddress(m_Name);
			}
		}
	}
#endif
}

DynLib::~DynLib()
{
	if (m_Handle != nullptr)
	{
#ifdef PLATFORM_WINDOWS
		FreeLibrary(m_Handle);
#elif defined PLATFORM_POSIX
		dlclose(m_Handle);
#endif
	}
}

void *DynLib::ResolveSymbol(const char *sym)
{
	return memutils->ResolveSymbol((void *)m_Handle, sym);
}

void *DynLib::FindPattern(const char *pattern, size_t len)
{
	return memutils->FindPattern(m_BaseAddress, pattern, len);
}

std::string DynLib::ErrorMessage()
{
#ifdef PLATFORM_WINDOWS
	std::stringstream ss;
	ss << std::hex << GetLastError();
	return "0x" + ss.str();
#elif defined PLATFORM_POSIX
	const char *err = dlerror();
	return err ? err : "";
#endif
}

void *DynLib::LoadBaseAddress(const std::string &name)
{
#ifdef PLATFORM_WINDOWS
	return nullptr;
#elif defined PLATFORM_POSIX
	std::ifstream maps("/proc/self/maps");
	std::string line;
	void *startAddr = nullptr;
	while (std::getline(maps, line))
	{
		if (line.find(name) != std::string::npos)
		{
			std::stringstream ss(line);
			char dash;
			ss >> std::hex >> startAddr >> dash;
			break;
		}
	}
	return startAddr;
#endif
}