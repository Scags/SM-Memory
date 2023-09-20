#include "natives.h"
#include "dynlib.h"
#include "rtti.h"
#include <sourcehook.h>
#include <sh_memory.h>
#include <cstdlib>

#define GET_HNDL(h, htype)                                                                     \
	do                                                                                         \
	{                                                                                          \
		Handle_t hndl = (Handle_t)params[1];                                                   \
		HandleError err;                                                                       \
		HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());                    \
		if ((err = handlesys->ReadHandle(hndl, htype, &sec, (void **)&h)) != HandleError_None) \
		{                                                                                      \
			return pContext->ThrowNativeError("Invalid Handle %x (error %d)", hndl, err);      \
		}                                                                                      \
	} while (0)

static cell_t Native_Calloc(IPluginContext *pContext, const cell_t *params)
{
	size_t num = (size_t)params[1];
	size_t size = (size_t)params[2];

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)calloc(num, size);
#else
	returnval = smutils->ToPseudoAddress(calloc(num, size));
#endif
	return returnval;
}

static cell_t Native_Malloc(IPluginContext *pContext, const cell_t *params)
{
	size_t size = (size_t)params[1];
	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)malloc(size);
#else
	returnval = smutils->ToPseudoAddress(malloc(size));
#endif
	return returnval;
}

#define VALID_MINIMUM_MEMORY_ADDRESS 0x10000 	// Yoinked from smn_core.cpp
static cell_t Native_Realloc(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr = (void *)params[1];
#else
	void *ptr = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	size_t size = (size_t)params[2];
	SourceHook::SetMemAccess(ptr, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);
	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)realloc(ptr, size);
#else
	returnval = smutils->ToPseudoAddress(realloc(ptr, size));
#endif
	return returnval;
}

static cell_t Native_Free(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr = (void *)params[1];
#else
	void *ptr = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	SourceHook::SetMemAccess(ptr, sizeof(void *), SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);
	free(ptr);
	return 0;
}

static cell_t Native_MemMove(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *dest = (void *)params[1];
#else
	void *dest = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)dest < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", dest);
	}

#ifdef PLATFORM_X86
	void *src = (void *)params[2];
#else
	void *src = smutils->FromPseudoAddress(params[2]);
#endif
	if ((uintptr_t)src < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", src);
	}

	size_t size = (size_t)params[3];
	SourceHook::SetMemAccess(dest, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);
	SourceHook::SetMemAccess(src, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)memmove(dest, src, size);
#else
	returnval = smutils->ToPseudoAddress(memmove(dest, src, size));
#endif
	return returnval;
}

static cell_t Native_MemCopy(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *dest = (void *)params[1];
#else
	void *dest = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)dest < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", dest);
	}

#ifdef PLATFORM_X86
	void *src = (void *)params[2];
#else
	void *src = smutils->FromPseudoAddress(params[2]);
#endif
	if ((uintptr_t)src < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", src);
	}

	size_t size = (size_t)params[3];
	SourceHook::SetMemAccess(dest, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);
	SourceHook::SetMemAccess(src, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)memcpy(dest, src, size);
#else
	returnval = smutils->ToPseudoAddress(memcpy(dest, src, size));
#endif
	return returnval;
}

static cell_t Native_MemCmp(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr1 = (void *)params[1];
#else
	void *ptr1 = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr1 < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", ptr1);
	}

#ifdef PLATFORM_X86
	void *ptr2 = (void *)params[2];
#else
	void *ptr2 = smutils->FromPseudoAddress(params[2]);
#endif
	if ((uintptr_t)ptr2 < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", ptr2);
	}

	size_t size = (size_t)params[3];
	SourceHook::SetMemAccess(ptr1, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);
	SourceHook::SetMemAccess(ptr2, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);

	return (cell_t)memcmp(ptr1, ptr2, size);
}

static cell_t Native_MemSet(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr = (void *)params[1];
#else
	void *ptr = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	int val = (int)params[2];
	size_t size = (size_t)params[3];

	SourceHook::SetMemAccess(ptr, size, SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)memset(ptr, val, size);
#else
	returnval = smutils->ToPseudoAddress(memset(ptr, val, size));
#endif
	return returnval;
}

static unsigned char* g_CurrJump = NULL;

// This will only work on 32-bit SourceMod
// No, I will not try for 64-bit compatibility
static cell_t Native_Emit(IPluginContext *pContext, const cell_t *params)
{
#ifdef WIN32
	static const unsigned char epilogue[] = {
		0x8B, 0xE5,		// mov esp, ebp
		0x5D,			// pop ebp
		0xC3			// retn
	};
#else
	static const unsigned char epilogue[] = {
		0x8B, 0x5D, 0xFC,	// move ebx, [ebp-4]
		0xC9,				// leave
		0xC3				// retn
	};
#endif

	cell_t *array;
	pContext->LocalToPhysAddr(params[1], &array);

	cell_t length = params[2];
	if (length < 0)
	{
		pContext->ThrowNativeError("Invalid length %d specified.", length);
	}

	if (g_CurrJump != NULL)
	{
		delete[] g_CurrJump;
	}

	g_CurrJump = new unsigned char[length + sizeof(epilogue)/sizeof(*epilogue)];
	for (int i = 0; i < length; ++i)
	{
		g_CurrJump[i] = (unsigned char)array[i];
	}

	if (!params[3])	// Custom cleanup
	{
		if (params[0] - 3 > 0)
		{
			delete[] g_CurrJump;
			return pContext->ThrowNativeError("Custom cleanup (param 3) is required for argumented assembly functions.");
		}
		for (unsigned i = length; i < length + sizeof(epilogue)/sizeof(*epilogue); ++i)
		{
			g_CurrJump[i] = epilogue[i-length];
		}
	}

	SourceHook::SetMemAccess(g_CurrJump, length + sizeof(epilogue)/sizeof(*epilogue), SH_MEM_READ|SH_MEM_WRITE|SH_MEM_EXEC);
	// This is fucking awful but let's just roll with it
	switch (params[0] - 3)
	{
		case 0:
		{
			cell_t (*ptr)(void) = (cell_t (*)(void))g_CurrJump;
			return ptr();
			break;
		}
		case 1:
		{
			cell_t (*ptr)(cell_t) = (cell_t (*)(cell_t))g_CurrJump;
			return ptr(params[4]);
			break;
		}
		case 2:
		{
			cell_t (*ptr)(cell_t, cell_t) = (cell_t (*)(cell_t, cell_t))g_CurrJump;
			return ptr(params[4], params[5]);
			break;
		}
		case 3:
		{
			cell_t (*ptr)(cell_t, cell_t, cell_t) = (cell_t (*)(cell_t, cell_t, cell_t))g_CurrJump;
			return ptr(params[4], params[5], params[6]);
			break;
		}
		case 4:
		{
			cell_t (*ptr)(cell_t, cell_t, cell_t, cell_t) = (cell_t (*)(cell_t, cell_t, cell_t, cell_t))g_CurrJump;
			return ptr(params[4], params[5], params[6], params[7]);
			break;
		}
		case 5:
		{
			cell_t (*ptr)(cell_t, cell_t, cell_t, cell_t, cell_t) = (cell_t (*)(cell_t, cell_t, cell_t, cell_t, cell_t))g_CurrJump;
			return ptr(params[4], params[5], params[6], params[7], params[8]);
			break;
		}
		case 6:
		{
			cell_t (*ptr)(cell_t, cell_t, cell_t, cell_t, cell_t, cell_t) = (cell_t (*)(cell_t, cell_t, cell_t, cell_t, cell_t, cell_t))g_CurrJump;
			return ptr(params[4], params[5], params[6], params[7], params[8], params[9]);
			break;
		}
		// No
		default:
			pContext->ThrowNativeError("Too many assembly parameters! Max 6 (%d given)", params[0] - 3);
			break;
	}

	return 0;

//#ifdef WIN32
//	__asm
//	{
//		jmp g_CurrJump;
//	}
//#else
//	asm volatile("jmp *%0" : : "r" (g_CurrJump));
//#endif
}

static cell_t Native_AddressOf(IPluginContext *pContext, const cell_t *params)
{
	cell_t *var;
	if (pContext->LocalToPhysAddr(params[1], &var) != SP_ERROR_NONE)
	{
		return pContext->ThrowNativeError("Variable must be declared and contained within a plugin's scope.");
	}
	return (cell_t)var;
}

enum NumberType
{
	NumberType_Int8,
	NumberType_Int16,
	NumberType_Int32
};

// This is literally the SM native but there's no SetMemAccess so we go faaaast
static cell_t Native_StoreToAddressFast(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *addr = reinterpret_cast<void*>(params[1]);
#else
	void *addr = pseudoAddr.FromPseudoAddress(params[1]);
#endif

	if (addr == NULL)
	{
		return pContext->ThrowNativeError("Address cannot be null");
	}
	else if (reinterpret_cast<uintptr_t>(addr) < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", addr);
	}
	cell_t data = params[2];

	NumberType size = static_cast<NumberType>(params[3]);

	switch(size)
	{
	case NumberType_Int8:
		*reinterpret_cast<uint8_t*>(addr) = data;
		break;
	case NumberType_Int16:
		*reinterpret_cast<uint16_t*>(addr) = data;
		break;
	case NumberType_Int32:
		*reinterpret_cast<uint32_t*>(addr) = data;
		break;
	default:
		return pContext->ThrowNativeError("Invalid number types %d", size);
	}

	return 0;
}

static cell_t Native_SetMemAccess(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *addr = (void *)params[1];
#else
	void *addr = smutils->FromPseudoAddress(params[1]);
#endif

	if ((uintptr_t)addr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", addr);
	}

	size_t length = (size_t)params[2];
	int flags = params[3];
	return SourceHook::SetMemAccess(addr, length, flags) ? 1 : 0;
}

static cell_t Native_VAFormat(IPluginContext *pContext, const cell_t *params)
{
#ifndef PLATFORM_POSIX
	return pContext->ThrowNativeError("This function is not available on this platform.");
#else
	char *buffer;
	pContext->LocalToString(params[1], &buffer);

	size_t size = (size_t)params[2];

	char *format;
	pContext->LocalToString(params[3], &format);

#ifdef PLATFORM_X86
	va_list va = (va_list)params[4];
#else
	va_list va = (va_list)smutils->FromPseudoAddress(params[4]);
#endif

	va_list cpy;
	va_copy(cpy, va);
	cell_t length = (cell_t)ke::SafeVsprintf(buffer, size, format, cpy);
	va_end(cpy);

	return length;
#endif
}

static cell_t Native_ReallocF(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr = (void *)params[1];
#else
	void *ptr = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	size_t size = (size_t)params[2];
	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)realloc(ptr, size);
#else
	returnval = smutils->ToPseudoAddress(realloc(ptr, size));
#endif
	return returnval;
}

static cell_t Native_FreeF(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr = (void *)params[1];
#else
	void *ptr = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	free(ptr);
	return 0;
}

static cell_t Native_MemMoveF(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *dest = (void *)params[1];
#else
	void *dest = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)dest < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", dest);
	}

#ifdef PLATFORM_X86
	void *src = (void *)params[2];
#else
	void *src = smutils->FromPseudoAddress(params[2]);
#endif
	if ((uintptr_t)src < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", src);
	}

	size_t size = (size_t)params[3];

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)memmove(dest, src, size);
#else
	returnval = smutils->ToPseudoAddress(memmove(dest, src, size));
#endif
	return returnval;
}

static cell_t Native_MemCopyF(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *dest = (void *)params[1];
#else
	void *dest = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)dest < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", dest);
	}

#ifdef PLATFORM_X86
	void *src = (void *)params[2];
#else
	void *src = smutils->FromPseudoAddress(params[2]);
#endif
	if ((uintptr_t)src < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", src);
	}

	size_t size = (size_t)params[3];

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)memcpy(dest, src, size);
#else
	returnval = smutils->ToPseudoAddress(memcpy(dest, src, size));
#endif
	return returnval;
}

static cell_t Native_MemCmpF(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr1 = (void *)params[1];
#else
	void *ptr1 = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr1 < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", ptr1);
	}

#ifdef PLATFORM_X86
	void *ptr2 = (void *)params[2];
#else
	void *ptr2 = smutils->FromPseudoAddress(params[2]);
#endif
	if ((uintptr_t)ptr2 < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", ptr2);
	}

	size_t size = (size_t)params[3];

	return (cell_t)memcmp(ptr1, ptr2, size);
}

static cell_t Native_MemSetF(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X86
	void *ptr = (void *)params[1];
#else
	void *ptr = smutils->FromPseudoAddress(params[1]);
#endif
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	int val = (int)params[2];
	size_t size = (size_t)params[3];

	cell_t returnval;

#ifdef PLATFORM_X86
	returnval = (cell_t)memset(ptr, val, size);
#else
	returnval = smutils->ToPseudoAddress(memset(ptr, val, size));
#endif
	return returnval;
}

static cell_t Native_DynLib(IPluginContext *pContext, const cell_t *params)
{
	char *name;
	pContext->LocalToString(params[1], &name);

	DynLib *pLib = new DynLib(name);
	if (!pLib->IsLoaded())
	{
		delete pLib;
		if (params[0] >= 2)
		{
			if (!params[2])
				pContext->ThrowNativeError("Failed to load library %s: (Error: %s)", name, DynLib::ErrorMessage().c_str());
		}
		else
		{
			// Replicate legacy functionality if an older include/build used called this
			smutils->LogError(myself, "Failed to load library %s: (Error: %s)", name, DynLib::ErrorMessage().c_str());
		}

		return 0;
	}

	Handle_t hndl = handlesys->CreateHandle(g_DynLib, pLib, pContext->GetIdentity(), myself->GetIdentity(), NULL);
	if (!hndl)
	{
		delete pLib;
		return pContext->ThrowNativeError("Failed to create DynLib handle");
	}

	return hndl;
}

static cell_t Native_DynLib_BaseAddr_Get(IPluginContext *pContext, const cell_t *params)
{
	DynLib *handle;
	GET_HNDL(handle, g_DynLib);
	void *ptr = handle->GetBaseAddress();

	cell_t returnval;
#ifdef PLATFORM_X86
	returnval = (cell_t)ptr;
#else
	returnval = smutils->ToPseudoAddress(ptr);
#endif
	return returnval;
}

static cell_t Native_DynLib_GetName(IPluginContext *pContext, const cell_t *params)
{
	DynLib *handle;
	GET_HNDL(handle, g_DynLib);
	pContext->StringToLocal(params[2], params[3], handle->GetName().c_str());
	return 0;
}

static cell_t Native_DynLib_FindPattern(IPluginContext *pContext, const cell_t *params)
{
	DynLib *handle;
	GET_HNDL(handle, g_DynLib);
	char *pattern;
	pContext->LocalToString(params[2], &pattern);
	size_t len = (size_t)params[3];
	void *addr = handle->FindPattern(pattern, len);
	if (addr == nullptr)
		return 0;

	cell_t returnval;
#ifdef PLATFORM_X86
	returnval = (cell_t)addr;
#else
	returnval = smutils->ToPseudoAddress(addr);
#endif
	return returnval;
}

static cell_t Native_DynLib_ResolveSymbol(IPluginContext *pContext, const cell_t *params)
{
	DynLib *handle;
	GET_HNDL(handle, g_DynLib);
	char *sym;
	pContext->LocalToString(params[2], &sym);
	uint8_t startidx = sym[0] == '@' ? 1 : 0;

	void *addr = handle->ResolveSymbol(&sym[startidx]);
	if (addr == nullptr)
		return 0;

	cell_t returnval;
#ifdef PLATFORM_X86
	returnval = (cell_t)addr;
#else
	returnval = smutils->ToPseudoAddress(addr);
#endif
	return returnval;
}

static cell_t Native_DynamicCast(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X64
	return pContext->ThrowNativeError("This function is not available on this platform.");
#endif

	void *ptr = (void *)params[1];
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	char *typeName;
	pContext->LocalToString(params[2], &typeName);

	return (cell_t)rtti::DynamicCast(ptr, typeName);
}

static cell_t Native_DynamicCast2(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X64
	return pContext->ThrowNativeError("This function is not available on this platform.");
#endif

	void *ptr = (void *)params[1];
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", ptr);
	}

	void *pType = (void *)params[2];
	if ((uintptr_t)pType < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", pType);
	}

	return (cell_t)rtti::DynamicCast2(ptr, pType);
}

static cell_t Native_GetClassTypeInfo(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X64
	return pContext->ThrowNativeError("This function is not available on this platform.");
#endif

	void *ptr = (void *)params[1];

	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	return (cell_t)rtti::GetClassTypeInfo(ptr);
}

static cell_t Native_GetClassTypeInfoByName(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X64
	return pContext->ThrowNativeError("This function is not available on this platform.");
#endif

	void *ptr = (void *)params[1];
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	char *name;
	pContext->LocalToString(params[2], &name);

	return (cell_t)rtti::GetClassTypeInfoByName(ptr, name);
}

static cell_t Native_GetClassTypeInfoName(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X64
	return pContext->ThrowNativeError("This function is not available on this platform.");
#endif

	void *ptr = (void *)params[1];
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	std::string name = rtti::GetClassTypeInfoName(ptr);
	pContext->StringToLocal(params[2], params[3], name.c_str());
	return 0;
}

static cell_t Native_GetTypeInfoName(IPluginContext *pContext, const cell_t *params)
{
#ifdef PLATFORM_X64
	return pContext->ThrowNativeError("This function is not available on this platform.");
#endif

	void *ptr = (void *)params[1];
	if ((uintptr_t)ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	std::string name = rtti::GetTypeInfoName(ptr);

	pContext->StringToLocal(params[2], params[3], name.c_str());
	return 0;
}

sp_nativeinfo_t g_Natives[] = {
	{"Calloc", 					Native_Calloc},
	{"Free", 					Native_Free},
	{"Malloc", 					Native_Malloc},
	{"Realloc", 				Native_Realloc},
	{"MemMove", 				Native_MemMove},
	{"MemCopy", 				Native_MemCopy},
	{"MemCmp", 					Native_MemCmp},
	{"MemSet", 					Native_MemSet},
	{"Emit", 					Native_Emit},
	{"AddressOf", 				Native_AddressOf},
	{"AddressOfString", 		Native_AddressOf},
	{"StoreToAddressFast", 		Native_StoreToAddressFast},
	{"SetMemAccess", 			Native_SetMemAccess},
	{"VAFormat", 				Native_VAFormat},

	// Faster calls as they don't call SetMemAccess
	{"ReallocF", 				Native_ReallocF},
	{"FreeF", 					Native_FreeF},
	{"MemMoveF", 				Native_MemMoveF},
	{"MemCopyF", 				Native_MemCopyF},
	{"MemCmpF", 				Native_MemCmpF},
	{"MemSetF", 				Native_MemSetF},

	// MemUtils
	{"DynLib.DynLib", 			Native_DynLib},
	{"DynLib.BaseAddr.get", 	Native_DynLib_BaseAddr_Get},
	{"DynLib.GetName", 			Native_DynLib_GetName},
	{"DynLib.FindPattern", 		Native_DynLib_FindPattern},
	{"DynLib.ResolveSymbol", 	Native_DynLib_ResolveSymbol},

	// RTTI
	{"DynamicCast", 			Native_DynamicCast},
	{"DynamicCast2", 			Native_DynamicCast2},
	{"GetClassTypeInfo", 		Native_GetClassTypeInfo},
	{"GetClassTypeInfoByName", 	Native_GetClassTypeInfoByName},
	{"GetClassTypeInfoName", 	Native_GetClassTypeInfoName},
	{"GetTypeInfoName", 		Native_GetTypeInfoName},
	{NULL, NULL}
};