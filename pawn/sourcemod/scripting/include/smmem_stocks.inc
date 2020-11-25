#if defined _smmem_stocks_included
	#endinput
#endif
#define _smmem_stocks_included

#include <smmem>

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

stock any ArrayToPtr(any[] array, int size)
{
	int p = Malloc(size * 4);
	for (int i = 0; i < size; ++i)
		WriteVal(p + i * 4, array[i]);
	return p;
}

stock void PtrToArray(any p, any[] array, int size)
{
	for (int i = 0; i < size; ++i)
		array[i] = Deref(p + i * 4);
}

stock any StringToPtr(const char[] str, int size)
{
	int p = Malloc(size);
	for (int i = 0; i < size; ++i)
		WriteVal(p + i, array[i], NumberType_Int8);
	return p;
}

stock void PtrToString(any p, char[] str, int size)
{
	for (int i = 0; i < size; ++i)
		str[i] = view_as< int >(Deref(p + i, NumberType_Int8));
}