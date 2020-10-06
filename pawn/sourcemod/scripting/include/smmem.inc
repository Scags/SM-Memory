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
}
#endif