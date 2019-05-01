#pragma once
#include <SCBW/scbwdata.h>

namespace hooks {
  
  void friendlySplash(CBullet* bullet, u32 weaponId);
  void friendlyLurkerSplash(CBullet* bullet, u32 weaponId);

  void injectWeaponEffectHook();

} //hooks
