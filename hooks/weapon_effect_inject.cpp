#include "weapon_effect.h"
#include "../hook_tools.h"
#include "SCBW/enumerations.h"

const u32 Addr_WeaponsDatEffectCount = 0x0048B785 +2;
const u32 Addr_WeaponsDatEffectSwitch = 0x0048B790 +3;

namespace {

  void __declspec(naked) friendlySplashWrapper(){
    static CBullet* bullet;
    static u32 weaponId;
    __asm {
      MOV bullet, EBX
      MOV weaponId, ECX
      ;//MOV effect, EAX -- or weapons_dat::ExplosionType[weaponId]
    }
    if (weaponId == WeaponId::SubterraneanSpines) {
      hooks::friendlyLurkerSplash(bullet, weaponId);
    } else {
      hooks::friendlySplash(bullet, weaponId);
    }
    // Everything is conveniently already pushed !
    __asm {
      POP EDI
      POP ESI
      POP EBX
      MOV ESP,EBP
      POP EBP
      RETN 4
    }
  }
  
  const void* WeaponEffects_SwitchTable[] = {
    (void*)0x0048BBF6, //  0 - None
    (void*)0x0048B797, //  1 - Normal
    (void*)0x0048B8B9, //  2 - Splash (Radial)
    (void*)0x0048B8B9, //  3 - Splash (Enemy)
    (void*)0x0048B910, //  4 - Lockdown
    (void*)0x0048B8B9, //  5 - Nuclear Missile
    (void*)0x0048B969, //  6 - Parasite
    (void*)0x0048B9C8, //  7 - Broodlings
    (void*)0x0048BA0F, //  8 - EMP Shockwave
    (void*)0x0048BA67, //  9 - Irradiate
    (void*)0x0048BAAB, // 10 - Ensnare
    (void*)0x0048BAC8, // 11 - Plague
    (void*)0x0048B93D, // 12 - Stasis Field
    (void*)0x0048BAE2, // 13 - Dark Swarm
    (void*)0x0048BB00, // 14 - Consume
    (void*)0x0048B80B, // 15 - Yamato Gun
    (void*)0x0048BA2C, // 16 - Restoration
    (void*)0x0048BB35, // 17 - Disruption Web
    (void*)0x0048BB53, // 18 - Corrosive Acid
    (void*)0x0048BBE9, // 19 - Mind Control -- unused
    (void*)0x0048BBE9, // 20 - Feedback -- unused
    (void*)0x0048BBC2, // 21 - Optical Flare
    (void*)0x0048BB99, // 22 - Maelstrom
    friendlySplashWrapper, // 23 - Unused
    (void*)0x0048B828  // 24 - Splash (Corsair)
    // Add more !
  };
  
} //unnamed namespace

namespace hooks {
  void injectWeaponEffectHook(){
    u8 maxID = ARRAY_SIZE(WeaponEffects_SwitchTable)-1;
	u8 data[4];
	*(u32*)data = (u32)WeaponEffects_SwitchTable;
    memoryPatch(Addr_WeaponsDatEffectCount, maxID);
    memoryPatch(Addr_WeaponsDatEffectSwitch, data, 4); // load pointer to address table
  }
} //hooks
