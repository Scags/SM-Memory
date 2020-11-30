#if defined _smmem_included_
  #endinput
#endif
#define _smmem_included_

#if !defined NO_PTR_MACROS
  #define ptr Address
  #define cellbytes 4
  #define Address(%0) view_as< Address >(%0)
  #define memoffs(%0) ((%0) * cellbytes)
#endif

native any Calloc(int num, int size);
native any Malloc(int size);
native any Realloc(any p, int size);
native void Free(any p);
native void MemMove(any dest, any src, int size);
native any MemCopy(any dest, any src, int size);
native int MemCmp(any p1, any p2, int size);
native void MemSet(any p, int val, int size);

/**
 *	Emit a sequence of asm.
 *	Note: You're better off creating a static SDKCall with the address being your own pointer
 *	to assembly code. 
 *
 *	@param bytes 			Array of assembly to emit.
 *	@param len 				Size of the array.
 *	@param customcleanup	If true, you are responsible for cleaning up the function call.
 *							Otherwise, the native will do it for you. If you're passing params,
 *							you must clean them up yourself (Set this to true).
 *	@param ... 				Optional parameters to add in case this is a function call.
 *							Note that you're most likely better off creating your own SDKCall.
 *							This exists solely for the pedantic and crazy.
 *
 *	@return 				Whatever you tell it to return.
*/
native any Emit(any[] bytes, int len, bool customcleanup = false, any ...);

/**
 *	Get Address of a variable.
 *
 *	@param cell 			Variable to get the address of.
 *
 *	@return 				Address of the variable.
*/
native any AddressOf(any &cell);
stock any AddressOfArray(any[] array)
{
	return AddressOf(array[0]);
}
// For some reason str[0] gets a mismatch. Probably because it's too small to be a cell
native any AddressOfString(const char[] str);

/**
 *	Store memory to an address
 *	NOTE: This does not set memory access. You must use SetMemAccess first.
 *	This is same as StoreToAddress but exists purely for optimization purposes.
 *
 *	@param p 				Address to store to.
 *	@param val 				Value to store.
 *	@param numt 			Size to store.
*/
native void StoreToAddressFast(any p, any val, NumberType numt = NumberType_Int32);

#define SH_MEM_READ 	1
#define SH_MEM_WRITE 	2
#define SH_MEM_EXEC 	4
// See above defines for flags
native bool SetMemAccess(any p, int size, int flags);

#if !defined NO_C_NAMES
stock any calloc(int num, int size)
{
	return Calloc(num, size);
}
stock any malloc(int size)
{
	return Malloc(size);
}
stock any realloc(any p, int size)
{
	return Realloc(p, size);
}
stock void free(any p)
{
	Free(p);
}
stock void memmove(any dest, any src, int size)
{
	MemMove(dest, src, size);
}
stock any memcpy(any dest, any src, int size)
{
	return MemCopy(dest, src, size);
}
stock int memcmp(any p1, any p2, int size)
{
	return MemCmp(p1, p2, size);
}
stock void memset(any p, int val, int size)
{
	MemSet(p, val, size);
}
#endif

stock any Deref(any addr, NumberType numt = NumberType_Int32)
{
	return LoadFromAddress(view_as< Address >(addr), numt);
}

stock void WriteVal(any addr, any val, NumberType numt = NumberType_Int32)
{
	StoreToAddress(view_as< Address >(addr), view_as< int >(val), numt);
}

public Extension __ext_smmem =
{
	name = "SM-Mem",
	file = "sm-mem.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_smmem_SetNTVOptional()
{
	MarkNativeAsOptional("Calloc");
	MarkNativeAsOptional("Malloc");
	MarkNativeAsOptional("Realloc");
	MarkNativeAsOptional("Free");
	MarkNativeAsOptional("MemMove");
	MarkNativeAsOptional("MemCopy");
	MarkNativeAsOptional("MemCmp");
	MarkNativeAsOptional("MemSet");
	MarkNativeAsOptional("Emit");
	MarkNativeAsOptional("AddressOf");
	MarkNativeAsOptional("AddressOfString");
}
#endif