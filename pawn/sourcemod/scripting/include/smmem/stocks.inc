#if defined _smmem_stocks_included
	#endinput
#endif
#define _smmem_stocks_included

#include <smmem>

stock any Deref(any addr, NumberType numt = NumberType_Int32)
{
	return LoadFromAddress(view_as< Address >(addr), numt);
}

stock void WriteVal(any addr, any val, NumberType numt = NumberType_Int32)
{
	StoreToAddress(view_as< Address >(addr), view_as< int >(val), numt);
}
stock void WriteValF(any addr, any val, NumberType numt = NumberType_Int32)
{
	StoreToAddressFast(view_as< Address >(addr), view_as< int >(val), numt);
}

stock any GetEntityHandle(int ent)
{
	if (!IsValidEntity(ent))
		return -1;
	return Deref(GetEntityAddress(ent) + view_as< Address >(FindDataMapInfo(0, "m_angRotation") + 12));
}

stock int GetEntityFromHandle(any handle)
{
	int ent = handle & 0xFFF;
	if (ent == 0xFFF)
		ent = -1;
	return ent;
}

// Props to nosoop
stock int GetEntityFromAddress(Address pEntity)
{
	if (pEntity == Address_Null)
		return -1;

	return GetEntityFromHandle(Deref(pEntity + view_as< Address >(FindDataMapInfo(0, "m_angRotation") + 12)));
}

stock void TransmuteCell(any cell, any array[4])
{
	for (int i = 0, byte = 0xFF; i < 4; array[i++] = cell & byte, byte <<= 8) {}
}

stock any ArrayToPtr(any[] array, int size, NumberType numt = NumberType_Int32)
{
	int p = Malloc(size * 4);
	for (int i = 0, mult = 0; i < size; ++i, mult += (1 << view_as< int >(numt)))
		WriteVal(p + mult, array[i], numt);
	return p;
}

stock void PtrToArray(any p, any[] array, int size, NumberType numt = NumberType_Int32)
{
	for (int i = 0, mult = 0; i < size; ++i, mult += (1 << view_as< int >(numt)))
		array[i] = Deref(p + mult, numt);
}

stock any StringToPtr(const char[] str, int size)
{
	int p = Malloc(size);
	for (int i = 0; i < size; ++i)
		WriteVal(p + i, str[i], NumberType_Int8);
	return p;
}

stock void PtrToString(any p, char[] str, int size)
{
	for (int i = 0; i < size; ++i)
		str[i] = view_as< int >(Deref(p + i, NumberType_Int8));
}