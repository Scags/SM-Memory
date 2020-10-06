#include <smmem_vec>
#include <smmem_stocks>
#include <sdkhooks>
#include <sdktools>

public void OnPluginStart()
{
	for (int i = MaxClients; i; --i)
		if (IsClientInGame(i))
			OnClientPutInServer(i);

	RegAdminCmd("sm_adddonk", CmdAddDonk, ADMFLAG_ROOT);
}

public void OnClientPutInServer(int client)
{
	SDKHook(client, SDKHook_PreThink, OnThink);
}

stock CUtlVector GetHealerVector(int client)
{
	return CUtlVector(GetEntityAddress(client) + view_as< Address >(FindSendPropInfo("CTFPlayer", "m_nHalloweenBombHeadStage") + 8));
}

#if 0
struct healers_t
{
	EHANDLE	pHealer;
	float	flAmount;
	float   flHealAccum;
	float	flOverhealBonus;
	float	flOverhealDecayMult;
	bool	bDispenserHeal;
	EHANDLE pHealScorer;
	int		iKillsWhileBeingHealed;
	float	flHealedLastSecond;
};
#endif

#define SIZEOF_HEALERS 36

stock CUtlVector GetDonkVector(int wep)
{
	return CUtlVector(GetEntityAddress(wep) + view_as< Address >(FindSendPropInfo("CTFGrenadeLauncher", "m_iGoalTube") + 20));
}
public void OnThink(int client)
{
	CUtlVector vec = GetHealerVector(client);
	int struc;
	for (int i = 0; i < vec.Count(); ++i)
	{
		struc = vec.Element(i, SIZEOF_HEALERS);
		if (struc)
		{
			PrintToChatAll("client = %N", client);
			int pHealer = GetEntityFromHandle(Deref(struc));
			PrintToChatAll("pHealer = %N", pHealer);
			float flAmount = Deref(struc + 4);
			PrintToChatAll("flAmount = %.2f", flAmount);
			float flHealAccum = Deref(struc + 8);
			PrintToChatAll("flHealAccum = %.2f", flHealAccum);
			float flOverhealBonus = Deref(struc + 12);
			PrintToChatAll("flOverhealBonus = %.2f", flOverhealBonus);
			float flOverhealDecayMult = Deref(struc + 16);
			PrintToChatAll("flOverhealDecayMult = %.2f", flOverhealDecayMult);
			bool bDispenserHeal = Deref(struc + 20, NumberType_Int8);
			PrintToChatAll("bDispenserHeal = %d", bDispenserHeal);
			int pHealScorer = GetEntityFromHandle(Deref(struc + 24));
			PrintToChatAll("pHealScorer = %N", pHealScorer);
			int iKillsWhileBeingHealed = Deref(struc + 28);
			PrintToChatAll("iKillsWhileBeingHealed = %d", iKillsWhileBeingHealed);
			float flHealedLastSecond = Deref(struc + 32);
			PrintToChatAll("flHealedLastSecond = %.2f", flHealedLastSecond);
		}
	}

	if (client == 1)
	{
		vec = GetDonkVector(GetPlayerWeaponSlot(client, 0))
		for (int i = 0; i < vec.Count(); ++i)
		{
			struc = vec.Element(i, 8);
			if (struc)
			{
				PrintToChat(client, "m_hVictim = %d", GetEntityFromHandle(Deref(struc)));
				PrintToChat(client, "m_flExpireTime - currtime = %.2f", view_as< float >(Deref(struc + 4)) - GetGameTime());
			}
		}
	}
}

public Action CmdAddDonk(int client, int args)
{
	if (args < 2)
		return Plugin_Handled;

	char arg[32]; GetCmdArg(1, arg, sizeof(arg));
	int target = FindTarget(client, arg);
	if (target == -1)
		return Plugin_Handled;

	int donker = GetPlayerWeaponSlot(client, 0);
	if (!HasEntProp(donker, Prop_Send, "m_iGoalTube"))
		return Plugin_Handled;

	CUtlVector m_vecDonkVictims = GetDonkVector(donker);

	PrintToChat(client, "0x%X", GetEntityHandle(target));
	for (int i = 0; i < m_vecDonkVictims.Count(); ++i)
	{
		PrintToChat(client, "0x%X", m_vecDonkVictims.GetEx(i, 8));
	}
	int index = m_vecDonkVictims.FindEx(GetEntityHandle(target), 8);
	int donk;
	if (index == -1)
		donk = m_vecDonkVictims.Get(m_vecDonkVictims.AddToTail(0, 8), 8);
	else donk = m_vecDonkVictims.Get(index, 8);

	WriteVal(donk, GetEntityHandle(target));
	GetCmdArg(2, arg, sizeof(arg));
	WriteVal(donk + 4, GetGameTime() + StringToFloat(arg));

	ReplyToCommand(client, "Set target %N time to %.2f", target, GetGameTime() + StringToFloat(arg));
	return Plugin_Handled;
}