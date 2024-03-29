#include <smmem>
#include <sourcemod>

public void OnPluginStart()
{
	int p = calloc(8, 4);
	PrintToServer("Calloc 0x%X", p);
	for (int i = 0; i < 8; ++i)
	{
		WriteVal(p + i*4, 10);
	}
	for (int i = 0; i < 8; ++i)
	{
		PrintToServer("WriteVal %d: %d", i, Deref(p + i*4));
	}

	memset(p, 0, 8*4);
	for (int i = 0; i < 8; ++i)
	{
		PrintToServer("memset %d: %d", i, Deref(p + i*4));
	}

	free(p);

	p = malloc(4);
	PrintToServer("Malloc 0x%X", p);

	Address l = malloc(4);
	WriteVal(l, 100);
	WriteVal(p, 10);

	PrintToServer("Before memmove: %d", Deref(l));
	memmove(l, p, 4);
	PrintToServer("After memmove: %d", Deref(l));

	p = malloc(4);

	WriteVal(p, 12);
	WriteVal(l, 20);
	PrintToServer("Before memcpy: p = %d; l = %d", Deref(p), Deref(l));
	PrintToServer("memcmp(p, l): %d", memcmp(p, l, 4))

	memcpy(l, p, 4);
	PrintToServer("After memcpy: p = %d; l = %d", Deref(p), Deref(l));
	PrintToServer("memcmp(p, l): %d", memcmp(p, l, 4))

	free(p);
	free(l);

	DynLib lib = new DynLib("server");
	PrintToServer("BaseAddr: %X", lib.BaseAddr);
	char name[64]; lib.GetName(name, sizeof(name));
	PrintToServer("Name: %s", name);
	PrintToServer("Pattern: %X", lib.FindPattern("\x55", 1));
	PrintToServer("Pattern-BaseAddr: %X", lib.FindPattern("\x55", 1) - lib.BaseAddr);
	PrintToServer("ResolveSymbol: %X", lib.ResolveSymbol("?DropSoundThink@CTFAmmoPack@@QAEXXZ"));
	PrintToServer("ResolveSymbol-BaseAddr: %X", lib.ResolveSymbol("?DropSoundThink@CTFAmmoPack@@QAEXXZ") - lib.BaseAddr);
	delete lib;
}