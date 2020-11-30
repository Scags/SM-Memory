#include <smmem>
#include <smmem_stocks>
#include <sdktools>

#include <profiler>

// Stolen from https://github.com/GAMMACASE/MomSurfFix/blob/3ba7c368c7942fb7c4511aeeb5d3bf326468ba99/addons/sourcemod/scripting/momsurffix2.sp#L195
int g_Store[] = {
	0x55, 
	0x89, 0xE5, 
	0x8B, 0x45, 0x0C, 
	0x8B, 0x4D, 0x08, 
	0x89, 0x01, 
	0x89, 0xEC, 
	0x5D, 
	0xC3
};

int g_Load[] = {
	0x55,
	0x89, 0xe5,
	0x8B, 0x4D, 0x08,
	0x8B, 0x01,
	0x89, 0xEC,
	0x5D,
	0xC3
};

Handle g_Storef, g_Loadf;

Profiler storeprof1, storeprof2, loadprof1, loadprof2;
float storetime1[(1 << 13)], storetime2[(1 << 13)], loadtime1[(1 << 13)], loadtime2[(1 << 13)];

ptr storeaddr, loadaddr;

public void OnPluginStart()
{
	storeaddr = ArrayToPtr(g_Store, sizeof(g_Store), NumberType_Int8);
	StartPrepSDKCall(SDKCall_Static);
	PrepSDKCall_SetAddress(storeaddr);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);

	g_Storef = EndPrepSDKCall();
	if (!g_Storef)
		SetFailState("wtf");

	loadaddr = ArrayToPtr(g_Load, sizeof(g_Load), NumberType_Int8);
	StartPrepSDKCall(SDKCall_Static);
	PrepSDKCall_SetAddress(loadaddr);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);

	g_Loadf = EndPrepSDKCall();
	if (!g_Loadf)
		SetFailState("wtf2");

	storeprof1 = new Profiler();
	storeprof2 = new Profiler();
	loadprof1 = new Profiler();
	loadprof2 = new Profiler();

	any val;
	any addr = AddressOf(val);

	for (int i = 0; i < (1 << 13); ++i)
	{
		storeprof1.Start();
		StoreToAddress(addr, 10, NumberType_Int32);
		storeprof1.Stop();
		storetime1[i] = storeprof1.Time;

		storeprof2.Start();
		SDKCall(g_Storef, addr, 10);
		storeprof2.Stop();
		storetime2[i] = storeprof2.Time;

		loadprof1.Start();
		LoadFromAddress(addr, NumberType_Int32);
		loadprof1.Stop();
		loadtime1[i] = loadprof1.Time;

		loadprof2.Start();
		SDKCall(g_Loadf, addr);
		loadprof2.Stop();
		loadtime2[i] = loadprof2.Time;
	}

	PrintToServer("StoreToAddress %f", 1000.0 * GetAverageTime(storetime1, 1 << 13));
	PrintToServer("g_Store %f", 1000.0 * GetAverageTime(storetime2, 1 << 13));
	PrintToServer("LoadFromAddress %f", 1000.0 * GetAverageTime(loadtime1, 1 << 13));
	PrintToServer("g_Load %f", 1000.0 * GetAverageTime(loadtime2, 1 << 13));
}

public void OnPluginEnd()
{
	free(storeaddr);
	free(loadaddr);
}

stock float GetAverageTime(float[] array, int size)
{
	float fmax;
	for (int i = 0; i < size; ++i)
		fmax += array[i];

	return fmax / size;
}