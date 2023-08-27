#if defined _smmem_rtti_included
    #endinput
#endif
#define _smmem_rtti_included

#include <smmem>

// NOTE NOTE NOTE None of this is supported on 64-bit

// Could have used this in a methodmap, but it's so small I don't think it really matters
enum TypeInfo:{INVALID_TINFO = 0}

/**
 *  Perform a dynamic_cast on an address.
 *
 *  @param classptr         Class pointer to cast.
 *                          If the class pointer does not have a virtual table, this will
 *                          most likely segfault.
 *  @param typeInf          Type info string such as "IHasGenericMeter or CSimpleBot::CSimpleBotIntention".
 *                          Templated types are supported too.
 *
 *  @return                 On success, the class pointer + the displacement of the
 *                          inherited class' member position in the classptr.
 *                          In most cases this is zero, and you will get the same address
 *                          that you passed as classptr, but in cases of multiple inheritance
 *                          the pointer will be displaced.
 * 
 *                          For example, take CTFSword:
 *                          public class CTFSword - mdisp:0 :
 *                            public class CTFDecapitationMeleeWeaponBase - mdisp:0 :
 *                              public class CTFWeaponBaseMelee - mdisp:0 :
 *                                public class CTFWeaponBase - mdisp:0 :
 *                                  public class CBaseCombatWeapon - mdisp:0 :
 *                                    public class CEconEntity - mdisp:0 :
 *                                      public class CBaseAnimating - mdisp:0 :
 *                                        public class CBaseEntity - mdisp:0 :
 *                                          public class IServerEntity - mdisp:0 :
 *                                            public class IServerUnknown - mdisp:0 :
 *                                              public class IHandleEntity - mdisp:0,
 *                                      public class IHasAttributes - mdisp:1196,
 *                                  public class IHasOwner - mdisp:1728,
 *                                  public class IHasGenericMeter - mdisp:1732,
 *                              public class CGameEventListener - mdisp:2080 :
 *                                public class IGameEventListener2 - mdisp:2080
 *
 *                          If performing DynamicCast(GetEntityAddress(entity), "IHasGenericMeter"),
 *                          the address that will be returned will be GetEntityAddress + 1732 since 
 *                          that is the displacement of the inherited class and thus its virtual table
 *                          and supported members.
 *
 *                          This might be convenient for virtual table function hooking/calling when 
 *                          dealing with multiple inheritance due to being able to retrieve  offsets
 *                          that are cross-platform compliant.
 *                          This might also be an alternative to HasEntProp or more convenient when 
 *                          a sub/super-class type does not have networked variables.
 * 
 *                          On failure, 0 is returned.
*/
native any DynamicCast(any classptr, const char[] typeName);

/**
 *  Perform a dynamic_cast on an address.
 *  Functionally equivalent to @DynamicCast but uses type info objects rather than type names.
 *
 *  @param classptr         Class pointer to cast.
 *                          If the class pointer does not have a virtual table, this will
 *                          most likely segfault.
 *  @param typeInf          TypeInfo object to cast, retrieved by @GetClassTypeInfo.
 *  
 *  @return                 On success, the class pointer + the displacement of the
 *                          inherited class' member position in the classptr.
 *                          On failure, 0.
*/
native any DynamicCast2(any classptr, TypeInfo typeInf);

/**
 * Retrieve the type info pointer of a class object.
 * 
 *  @param classptr         Class pointer.
 *                          If the class pointer does not have a virtual table, this will
 *                          most likely segfault.
 * 
 * @return                  Pointer to a type info object on success, 0/INVALID_TINFO otherwise.
 *                          Note that this returns the *lowest* type info of a class e.g.
 *                          int ent = CreateEntityByName("tf_weapon_sword");
 *                          g_CTFSword = GetClassTypeInfo(GetEntityAddress(ent));
 *                          // g_CTFSword -> _ZTI8CTFSword / ??_R0?AVCTFSword@@@8
 * 
 *                          If using for @DynamicCast2, it may be wise to create an entity and
 *                          cache its type info for later use OnMapStart().
*/
native TypeInfo GetClassTypeInfo(any classptr);

/**
 * Retrieve a class' inherited type info via its name.
 * 
 *  @param classptr         Class pointer.
 *                          If the class pointer does not have a virtual table, this will
 *                          most likely segfault.
 *  @param typeName         String that contains the type name of the class whose type info
 *                          you'd like to retrieve.
 * 
 *  @return                 On success, the type info object of the desired class type.
 *                          On failure, 0/INVALID_TINFO.
 *                          This is similar to @DynamicCast but instead of returning the
 *                          class pointer, this instead returns the type info object.
*/
native TypeInfo GetClassTypeInfoByName(any classptr, const char[] typeName);

/**
 * Retrieve the type name of the class via it's type info,
 * similar to that of typeid(class).name().
 * 
 *  @param classptr         Class pointer.
 *                          If the class pointer does not have a virtual table, this will
 *                          most likely segfault.
 *  @param buffer           Buffer to fill.
 *                          In most cases, this will be equivalent to GetEntityNetClass,
 *                          however this supports classes that are not inherently entities.
 *  @param maxlen           Maximum length of the buffer.
 * 
 *  @noreturn
*/
native void GetClassTypeInfoName(any classptr, char[] buffer, int maxlen);

/**
 * Retrieve the type name of a type info object,
 * similar to that of typeid(class).name().
 * 
 *  @param tInfo            Type info object.
 *  @param buffer           Buffer to fill.
 *  @param maxlen           Maximum length of the buffer.
 * 
 *  @noreturn
*/
native void GetTypeInfoName(TypeInfo tInfo, char[] buffer, int maxlen);