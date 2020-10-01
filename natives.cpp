#include "natives.h"
#include <cstdlib>

static cell_t Native_Calloc(IPluginContext *pContext, const cell_t *params)
{
	size_t num = (size_t)params[1];
	size_t size = (size_t)params[2];
	return (cell_t)calloc(num, size);
}

static cell_t Native_Malloc(IPluginContext *pContext, const cell_t *params)
{
	size_t size = (size_t)params[1];
	return (cell_t)malloc(size);
}

#define VALID_MINIMUM_MEMORY_ADDRESS 0x10000 	// Yoinked from smn_core.cpp
static cell_t Native_Realloc(IPluginContext *pContext, const cell_t *params)
{
	uintptr_t ptr = (uintptr_t)params[1];
	if (ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	size_t size = (size_t)params[2];
	return (cell_t)realloc((void *)ptr, size);
}

static cell_t Native_Free(IPluginContext *pContext, const cell_t *params)
{
	uintptr_t ptr = (uintptr_t)params[1];
	if (ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	free((void *)ptr);
	return 0;
}

static cell_t Native_MemMove(IPluginContext *pContext, const cell_t *params)
{
	uintptr_t dest = (uintptr_t)params[1];
	if (dest < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", dest);
	}

	uintptr_t src = (uintptr_t)params[2];
	if (src < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", src);
	}

	size_t size = (size_t)params[3];

	return (cell_t)memmove((void *)dest, (void *)src, size);
}

static cell_t Native_MemCopy(IPluginContext *pContext, const cell_t *params)
{
	uintptr_t dest = (uintptr_t)params[1];
	if (dest < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", dest);
	}

	uintptr_t src = (uintptr_t)params[2];
	if (src < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", src);
	}

	size_t size = (size_t)params[3];

	memcpy((void *)dest, (void *)src, size);
	return 0;
}

static cell_t Native_MemCmp(IPluginContext *pContext, const cell_t *params)
{
	uintptr_t ptr1 = (uintptr_t)params[1];
	if (ptr1 < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 1) is pointing to reserved memory.", ptr1);
	}

	uintptr_t ptr2 = (uintptr_t)params[2];
	if (ptr2 < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x (arg 2) is pointing to reserved memory.", ptr2);
	}

	size_t size = (size_t)params[3];

	return (cell_t)memcmp((void *)ptr1, (void *)ptr2, size);
}

static cell_t Native_MemSet(IPluginContext *pContext, const cell_t *params)
{
	uintptr_t ptr = (uintptr_t)params[1];
	if (ptr < VALID_MINIMUM_MEMORY_ADDRESS)
	{
		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
	}

	int val = (int)params[2];
	size_t size = (size_t)params[3];

	return (cell_t)memset((void *)ptr, val, size);
}

//static cell_t Native_AddressOf(IPluginContext *pContext, const cell_t *params)
//{
//	cell_t *var;
//	if (pContext->LocalToPhysAddr(params[1], &var) != SP_ERROR_NONE)
//	{
//		return pContext->ThrowNativeError("Variable must be declared and contained within a plugin's scope.");
//	}
//	return (cell_t)var;
//}

//static cell_t Native_AddrToArray(IPluginContext *pContext, const cell_t *params)
//{
//	uintptr_t ptr = (uintptr_t)params[1];
//	if (ptr < VALID_MINIMUM_MEMORY_ADDRESS)
//	{
//		return pContext->ThrowNativeError("Invalid address 0x%x is pointing to reserved memory.", ptr);
//	}
//
//	cell_t *array;
//	if (pContext->LocalToPhysAddr(params[2], &array) != SP_ERROR_NONE)
//	{
//		return pContext->ThrowNativeError("Arrays must be declared and contained within a plugin's scope.");
//	}
//
//	*array = (cell_t)ptr;
//	return 0;
//}

sp_nativeinfo_t g_Natives[] = {
	{"Calloc", 			Native_Calloc},
	{"Free", 			Native_Free},
	{"Malloc", 			Native_Malloc},
	{"Realloc", 		Native_Realloc},
	{"MemMove", 		Native_MemMove},
	{"MemCopy", 		Native_MemCopy},
	{"MemCmp", 			Native_MemCmp},
	{"MemSet", 			Native_MemSet},
//	{"AddressOf", 		Native_AddressOf},
//	{"AddrToArray", 	Native_AddrToArray},
	{NULL, NULL}
};