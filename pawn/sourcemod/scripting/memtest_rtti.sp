#include <smmem>
#include <sdktools>

public void OnMapStart()
{
	int ent = CreateEntityByName("tf_weapon_sword");
	any addr = GetEntityAddress(ent);
	PrintToServer("GetEntityAddress -> 0x%x", addr);

	TypeInfo classTi = GetClassTypeInfo(addr);
	PrintToServer("GetClassTypeInfo -> 0x%x", classTi);

	// CTFSword
	char buffer[64];
	GetClassTypeInfoName(addr, buffer, sizeof(buffer));
	PrintToServer("GetClassTypeInfoName -> %s", buffer);

	TypeInfo attrTi = GetClassTypeInfoByName(addr, "IHasAttributes");
	PrintToServer("GetClassTypeInfoByName(IHasAttributes) -> 0x%x", attrTi);

	// IHasAttributes
	GetTypeInfoName(attrTi, buffer, sizeof(buffer));
	PrintToServer("GetTypeInfoName(attrTi) -> %s", buffer);

	any genMeter = DynamicCast(addr, "IHasGenericMeter");
	PrintToServer("DynamicCast(IHasGenericMeter) -> 0x%x", genMeter);

	// Same as classTi
	TypeInfo vmeter = GetClassTypeInfo(genMeter);
	PrintToServer("GetClassTypeInfo(vmeter) -> 0x%x", vmeter);

	// CTFSword
	GetTypeInfoName(vmeter, buffer, sizeof(buffer));
	PrintToServer("GetTypeInfoName(vmeter) -> %s", buffer);

	any hasAttr = DynamicCast2(addr, attrTi);
	PrintToServer("DynamicCast2(IHasAttributes) -> 0x%x", hasAttr);

	// Same as classTi
	TypeInfo vattr = GetClassTypeInfo(hasAttr);
	PrintToServer("GetClassTypeInfo(hasAttr) -> 0x%x", vattr);

	// CTFSword
	GetTypeInfoName(vattr, buffer, sizeof(buffer));
	PrintToServer("GetTypeInfoName(vattr) -> %s", buffer);

	RemoveEntity(ent);
}