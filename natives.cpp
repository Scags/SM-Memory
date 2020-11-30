#include "natives.h"
#include <sourcehook.h>
#include <sh_memory.h>
#include <cstdlib>

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
	void *addr = pseudoAddr.FromPseudoAddress(params[1]);
#endif

	if ((uintptr_t)addr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", addr);
	}

	size_t length = (size_t)params[2];
	int flags = params[3];
	return SourceHook::SetMemAccess(addr, length, flags) ? 1 : 0;
}

sp_nativeinfo_t g_Natives[] = {
	{"Calloc", 				Native_Calloc},
	{"Free", 				Native_Free},
	{"Malloc", 				Native_Malloc},
	{"Realloc", 			Native_Realloc},
	{"MemMove", 			Native_MemMove},
	{"MemCopy", 			Native_MemCopy},
	{"MemCmp", 				Native_MemCmp},
	{"MemSet", 				Native_MemSet},
	{"Emit", 				Native_Emit},
	{"AddressOf", 			Native_AddressOf},
	{"AddressOfString", 	Native_AddressOf},
	{"StoreToAddressFast", 	Native_StoreToAddressFast},
	{"SetMemAccess", 		Native_SetMemAccess},
	{NULL, NULL}
};