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