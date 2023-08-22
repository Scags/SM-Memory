# SM-Memory
Exposes more memory utility functions for SourceMod plugins.
 
# Installing
Grab a [Release](https://github.com/Scags/SM-Memory/releases) and extract to your server directory.

# Building
Edit build.bat or build.sh to point to your SM and MM folders and run.

# Use Cases

## Make Actual, Real Classes

<details>
<summary>Click</summary>

```cpp
// For sanity
enum PlayerData (+= 0x04)
{
	_pclient = 0,
	_phealth,
	_pkills,
	_ppoints,
	_pdeaths,
	_pdamage,
	_p_LENGTH
}

#define wf(%1,%2) 		StoreToAddressFast((%1), (%2))
#define qget(%1)		public get() { return Deref(this + (%1)); }
#define qset(%1)		public set(int i) { wf(this + (%1), i); }

methodmap PlayerData
{
	property int m_iClient
	{
		qget(_pclient)
		qset(_pclient)
	}
	property int m_iHealth
	{
		qget(_phealth)
		qset(_phealth)
	}
	property int m_iKills
	{
		qget(_pkills)
		qset(_pkills)
	}
	property int m_iPoints
	{
		qget(_ppoints)
		qset(_ppoints)
	}
	property int m_iDeaths
	{
		qget(_pdeaths)
		qset(_pdeaths)
	}
	property int m_iDamage
	{
		qget(_pdamage)
		qset(_pdamage)
	}
	public PlayerData(int client)
	{
		ptr p = malloc(_p_LENGTH);
		p.m_iClient = client;
		memset(this + 4, 0, _p_LENGTH - 4);
	}
}

#undef wf
#undef qget
#undef qset
```
</details>

## Recreate Valve Classes in SourcePawn
<details>
<summary>Click</summary>

```cpp
enum struct _Vector
{
	int x;
	int y;
	int z;
}
enum Vector {nullvec = 0}

#define qget(%1)		public get() { return Deref(this + view_as< any >((%1) * cellbytes)); }
#define qset(%1,%2)		public set(%1 i) { WriteVal(this + view_as< any >((%2) * cellbytes), i); }
methodmap Vector
{
	property float x
	{
		qget(_Vector::x)
		qset(float, _Vector::x)
	}
	property float y
	{
		qget(_Vector::y)
		qset(float, _Vector::y)
	}
	property float z
	{
		qget(_Vector::z)
		qset(float, _Vector::z)
	}
	public Vector(float x = 0.0, float y = 0.0, float z = 0.0)
	{
		Vector v = malloc(sizeof(_Vector) * cellbytes);
		v.x = x;
		v.y = y;
		v.z = z;
		return v;
	}

	public static Vector From(ptr address)
	{
		return view_as< Vector >(address);
	}
};

enum EHANDLE
{
	NULL_EHANDLE = -1
};

enum ECritType
{
	CRIT_NONE = 0,
	CRIT_MINI,
	CRIT_FULL,
};

enum struct _CTakeDamageInfo
{
	_Vector			m_vecDamageForce;
	_Vector			m_vecDamagePosition;
	_Vector			m_vecReportedPosition;	// Position players are told damage is coming from
	EHANDLE			m_hInflictor;
	EHANDLE			m_hAttacker;
	EHANDLE			m_hWeapon;
	float			m_flDamage;
	float			m_flMaxDamage;
	float			m_flBaseDamage;			// The damage amount before skill leve adjustments are made. Used to get uniform damage forces.
	int				m_bitsDamageType;
	int				m_iDamageCustom;
	int				m_iDamageStats;
	int				m_iAmmoType;			// AmmoType of the weapon used to cause this damage, if any
	int				m_iDamagedOtherPlayers;
	int				m_iPlayerPenetrationCount;
	float			m_flDamageBonus;		// Anything that increases damage (crit) - store the delta
	EHANDLE			m_hDamageBonusProvider;	// Who gave us the ability to do extra damage?
	bool			m_bForceFriendlyFire;	// Ideally this would be a dmg type, but we can't add more

	float			m_flDamageForForce;

	ECritType		m_eCritType;
}

methodmap CTakeDamageInfo
{
	property Vector			m_vecDamageForce
	{
		public get()
		{
			return view_as< Vector >(this);
		}
		public set(Vector v)
		{
			StoreToAddressFast(this, v.x);
			StoreToAddressFast(this + view_as< any >(4), v.y);
			StoreToAddressFast(this + view_as< any >(8), v.z);
		}
	}
	property Vector			m_vecDamagePosition
	{
		public get()
		{
			return view_as< Vector >(this + view_as< any >(12))
		}
		public set(Vector v)
		{
			StoreToAddressFast(this + view_as< any >(12), v.x);
			StoreToAddressFast(this + view_as< any >(16), v.y);
			StoreToAddressFast(this + view_as< any >(20), v.z);
		}
	}
	property Vector			m_vecReportedPosition	// Position players are told damage is coming from
	{
		public get()
		{
			return view_as< Vector >(this + view_as< any >(24))
		}
		public set(Vector v)
		{
			StoreToAddressFast(this + view_as< any >(24), v.x);
			StoreToAddressFast(this + view_as< any >(28), v.y);
			StoreToAddressFast(this + view_as< any >(32), v.z);
		}
	}
	property EHANDLE			m_hInflictor
	{
		qget(_CTakeDamageInfo::m_hInflictor)
		qset(EHANDLE, _CTakeDamageInfo::m_hInflictor)
	}
	property EHANDLE			m_hAttacker
	{
		qget(_CTakeDamageInfo::m_hAttacker)
		qset(EHANDLE, _CTakeDamageInfo::m_hAttacker)
	}
	property EHANDLE			m_hWeapon
	{
		qget(_CTakeDamageInfo::m_hWeapon)
		qset(EHANDLE, _CTakeDamageInfo::m_hWeapon)
	}
	property float				m_flDamage
	{
		qget(_CTakeDamageInfo::m_flDamage)
		qset(float, _CTakeDamageInfo::m_flDamage)
	}
	property float				m_flMaxDamage
	{
		qget(_CTakeDamageInfo::m_flMaxDamage)
		qset(float, _CTakeDamageInfo::m_flMaxDamage)
	}
	property float				m_flBaseDamage			// The damage amount before skill leve adjustments are made. Used to get uniform damage forces.
	{
		qget(_CTakeDamageInfo::m_flBaseDamage)
		qset(float, _CTakeDamageInfo::m_flBaseDamage)
	}
	property int				m_bitsDamageType
	{
		qget(_CTakeDamageInfo::m_bitsDamageType)
		qset(int, _CTakeDamageInfo::m_bitsDamageType)
	}
	property int				m_iDamageCustom
	{
		qget(_CTakeDamageInfo::m_iDamageCustom)
		qset(int, _CTakeDamageInfo::m_iDamageCustom)
	}
	property int				m_iDamageStats
	{
		qget(_CTakeDamageInfo::m_iDamageStats)
		qset(int, _CTakeDamageInfo::m_iDamageStats)
	}
	property int				m_iAmmoType			// AmmoType of the weapon used to cause this damage, if any
	{
		qget(_CTakeDamageInfo::m_iAmmoType)
		qset(int, _CTakeDamageInfo::m_iAmmoType)
	}
	property int				m_iDamagedOtherPlayers
	{
		qget(_CTakeDamageInfo::m_iDamagedOtherPlayers)
		qset(int, _CTakeDamageInfo::m_iDamagedOtherPlayers)
	}
	property int				m_iPlayerPenetrationCount
	{
		qget(_CTakeDamageInfo::m_iPlayerPenetrationCount)
		qset(int, _CTakeDamageInfo::m_iPlayerPenetrationCount)
	}
	property float				m_flDamageBonus		// Anything that increases damage (crit) - store the delta
	{
		qget(_CTakeDamageInfo::m_flDamageBonus)
		qset(float, _CTakeDamageInfo::m_flDamageBonus)
	}
	property EHANDLE			m_hDamageBonusProvider	// Who gave us the ability to do extra damage?
	{
		qget(_CTakeDamageInfo::m_hDamageBonusProvider)
		qset(EHANDLE, _CTakeDamageInfo::m_hDamageBonusProvider)
	}
	property bool				m_bForceFriendlyFire	// Ideally this would be a dmg type, but we can't add more
	{
		qget(_CTakeDamageInfo::m_bForceFriendlyFire)
		qset(bool, _CTakeDamageInfo::m_bForceFriendlyFire)
	}
	property float				m_flDamageForForce
	{
		qget(_CTakeDamageInfo::m_flDamageForForce)
		qset(float, _CTakeDamageInfo::m_flDamageForForce)
	}
	property ECritType			m_eCritType
	{
		qget(_CTakeDamageInfo::m_eCritType)
		qset(ECritType, _CTakeDamageInfo::m_eCritType)
	}

	public void Init(int pInflictor, int pAttacker, int pWeapon, Vector damageForce, 
		Vector damagePosition, Vector reportedPosition, float flDamage, int bitsDamageType, int iCustomDamage)
	{
		this.m_hInflictor = GetEntityHandle(pInflictor);
		if ( pAttacker != -1 )
		{
			this.m_hAttacker = GetEntityHandle(pAttacker);
		}
		else
		{
			this.m_hAttacker = GetEntityHandle(pInflictor);
		}

		this.m_hWeapon = GetEntityHandle(pWeapon);

		this.m_flDamage = flDamage;

		// This is actually FLT_MAX but I'm lazy
		this.m_flBaseDamage = 0.0;

		this.m_bitsDamageType = bitsDamageType;
		this.m_iDamageCustom = iCustomDamage;

		this.m_flMaxDamage = flDamage;
		this.m_vecDamageForce = damageForce;
		this.m_vecDamagePosition = damagePosition;
		this.m_vecReportedPosition = reportedPosition;
		this.m_iAmmoType = -1;
		this.m_iDamagedOtherPlayers = 0;
		this.m_iPlayerPenetrationCount = 0;
		this.m_flDamageBonus = 0.0;
		this.m_bForceFriendlyFire = false;
		this.m_flDamageForForce = 0.0;
		this.m_eCritType = CRIT_NONE;
	}

	public void Set(int pInflictor, int pAttacker, int pWeapon,
		const Vector damageForce, const Vector damagePosition, float flDamage, int bitsDamageType, int iKillType, Vector reportedPosition)
	{
		this.Init(pInflictor, pAttacker, pWeapon, damageForce, damagePosition, reportedPosition, flDamage, bitsDamageType, iKillType)
	}

	public CTakeDamageInfo(int pInflictor = -1, int pAttacker = -1, int pWeapon = -1, 
		const Vector damageForce = nullvec, const Vector damagePosition = nullvec, float flDamage = 0.0, int bitsDamageType = 0, int iKillType = 0, Vector reportedPosition = nullvec)
	{
		CTakeDamageInfo info = malloc(sizeof(_CTakeDamageInfo) * cellbytes);
		info.Set( pInflictor, pAttacker, pWeapon, damageForce, damagePosition, flDamage, bitsDamageType, iKillType, reportedPosition );
	}

	public static CTakeDamageInfo From(ptr p)
	{
		return view_as< CTakeDamageInfo >(p);
	}

	public void Free()
	{
		free(this);
	}

	public int GetInflictor()
	{
		return GetEntityFromHandle(this.m_hInflictor);
	}


	public void SetInflictor( int pInflictor )
	{
		this.m_hInflictor = GetEntityHandle(pInflictor);
	}


	public int GetAttacker()
	{
		return GetEntityFromHandle(this.m_hAttacker);
	}


	public void SetAttacker( int pAttacker )
	{
		this.m_hAttacker = GetEntityHandle(pAttacker);
	}

	public int GetWeapon()
	{
		return GetEntityFromHandle(this.m_hWeapon);
	}


	public void SetWeapon( int pWeapon )
	{
		this.m_hWeapon = GetEntityHandle(pWeapon);
	}


	public float GetDamage()
	{
		return this.m_flDamage;
	}

	public void SetDamage( float flDamage )
	{
		this.m_flDamage = flDamage;
	}

	public float GetMaxDamage()
	{
		return this.m_flMaxDamage;
	}

	public void SetMaxDamage( float flMaxDamage )
	{
		this.m_flMaxDamage = flMaxDamage;
	}

	public void ScaleDamage( float flScaleAmount )
	{
		this.m_flDamage *= flScaleAmount;
	}

	public void AddDamage( float flAddAmount )
	{
		this.m_flDamage += flAddAmount;
	}

	public void SubtractDamage( float flSubtractAmount )
	{
		this.m_flDamage -= flSubtractAmount;
	}

	public float GetDamageBonus()
	{
		return this.m_flDamageBonus;
	}

	public int GetDamageBonusProvider()
	{
		return GetEntityFromHandle(this.m_hDamageBonusProvider);
	}

	public void SetDamageBonus( float flBonus, int pProvider /*= NULL*/ )
	{
		this.m_flDamageBonus = flBonus;
		this.m_hDamageBonusProvider = GetEntityHandle(pProvider);
	}

	public bool BaseDamageIsValid()
	{
		return (this.m_flBaseDamage != 0.0);
	}

	public float GetBaseDamage()
	{
		if( this.BaseDamageIsValid() )
			return this.m_flBaseDamage;

		// No one ever specified a base damage, so just return damage.
		return this.m_flDamage;
	}

	public Vector GetDamageForce()
	{
		return this.m_vecDamageForce;
	}

	public void SetDamageForce( const Vector &damageForce )
	{
		this.m_vecDamageForce = damageForce;
	}

	public void	ScaleDamageForce( float flScaleAmount )
	{
		StoreToAddressFast(this.m_vecDamageForce, view_as< float >(Deref(this.m_vecDamageForce) * flScaleAmount));
		StoreToAddressFast(this.m_vecDamageForce + view_as< any >(4), view_as< float >(Deref(this.m_vecDamageForce + view_as< any >(4)) * flScaleAmount));
		StoreToAddressFast(this.m_vecDamageForce + view_as< any >(8), view_as< float >(Deref(this.m_vecDamageForce + view_as< any >(8)) * flScaleAmount));
	}

	public float GetDamageForForceCalc()
	{
		return this.m_flDamageForForce;
	}

	public void SetDamageForForceCalc( float flDamage )
	{
		this.m_flDamageForForce = flDamage;
	}

	public Vector GetDamagePosition()
	{
		return this.m_vecDamagePosition;
	}


	public void SetDamagePosition( Vector damagePosition )
	{
		this.m_vecDamagePosition = damagePosition;
	}

	public Vector GetReportedPosition()
	{
		return this.m_vecReportedPosition;
	}


	public void SetReportedPosition( Vector reportedPosition )
	{
		this.m_vecReportedPosition = reportedPosition;
	}


	public void SetDamageType( int bitsDamageType )
	{
		this.m_bitsDamageType = bitsDamageType;
	}

	public int GetDamageType()
	{
		return this.m_bitsDamageType;
	}

	public void	AddDamageType( int bitsDamageType )
	{
		this.m_bitsDamageType |= bitsDamageType;
	}

	public int GetDamageCustom()
	{
		return this.m_iDamageCustom;
	}

	public void SetDamageCustom( int iDamageCustom )
	{
		this.m_iDamageCustom = iDamageCustom;
	}

	public int GetDamageStats()
	{
		return this.m_iDamageCustom;
	}

	public void SetDamageStats( int iDamageCustom )
	{
		this.m_iDamageCustom = iDamageCustom;
	}

	public int GetAmmoType()
	{
		return this.m_iAmmoType;
	}

	public void SetAmmoType( int iAmmoType )
	{
		this.m_iAmmoType = iAmmoType;
	}

	public void CopyDamageToBaseDamage()
	{ 
		this.m_flBaseDamage = this.m_flDamage;
	}
};
```
</details>

## Hook a Function From Any Library

<details>
<summary>Click</summary>

```cpp
#include <smmem>
#include <dhooks>

public void OnPluginStart()
{
	DynLib lib = new DynLib("./tf/addons/sourcemod/bin/sourcemod.logic");
	Address func = lib.ResolveSymbol("_ZN6Logger8LogErrorEPKcz");

	if (func == Address_Null)
		SetFailState("Could not find Logger::LogErrorEx");

	DynamicDetour detour = new DynamicDetour(func, CallConv_CDECL, ReturnType_Void);
	detour.AddParam(HookParamType_Int);
	detour.AddParam(HookParamType_CharPtr);
 	detour.AddParam(HookParamType_Int);
	detour.Enable(Hook_Pre, Logger_LogErrorEx);
	detour.Enable(Hook_Post, Logger_LogErrorExPost);

	func = lib.ResolveSymbol("_ZN6Logger15LogToOpenFileExEP8_IO_FILEPKcPc");
	detour = new DynamicDetour(func, CallConv_CDECL, ReturnType_Void);
	detour.AddParam(HookParamType_Int);
	detour.AddParam(HookParamType_Int);
	detour.AddParam(HookParamType_CharPtr);
 	detour.AddParam(HookParamType_Int);
	detour.Enable(Hook_Pre, Logger_LogToOpenFile);


	delete lib;

	RegServerCmd("sm_logerror", MakeLogError);
}

public Action MakeLogError(int args)
{
	LogError("hi %s %d", "asd", 1);
	return Plugin_Handled;
}

static bool g_Start;
public MRESReturn Logger_LogErrorEx(DHookParam hParams)
{
	g_Start = true;
}

public MRESReturn Logger_LogErrorExPost(DHookParam hParams)
{
	g_Start = false;
}

static bool g_InCall;
public MRESReturn Logger_LogToOpenFile(DHookParam hParams)
{
	if (!g_Start)
		return MRES_Ignored;

	if (g_InCall)
	{
		PrintToServer("LogHook: Breaking recursion.")
		return MRES_Ignored;
	}

	g_InCall = true;

	char buffer[1024];
	hParams.GetString(3, buffer, sizeof(buffer));
	PrintToServer("%s", buffer);

	char buffer2[1024];
	VAFormat(buffer2, sizeof(buffer2), buffer, hParams.Get(4));
	PrintToServer("%s", buffer2);

	g_InCall = false;
	return MRES_Ignored;
}
```
</details>

## Call Library API From a Plugin

<details>
<summary>Click</summary>

```cpp
#include <smmem>
#include <sdktools>

public void OnPluginStart()
{
	DynLib lib = new DynLib("user32.dll");
	Address export = lib.ResolveSymbol("MessageBoxA");
	PrintToServer("MessageBoxA -> 0x%X", export);

	StartPrepSDKCall(SDKCall_Static);
	PrepSDKCall_SetAddress(export);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);

	Handle call = EndPrepSDKCall();
	// Opens a message box that prints "Hello World!"
	SDKCall(call, 0, "Hello World!", "Hi", 0);

	delete lib;
	delete call;
}
```
</details>