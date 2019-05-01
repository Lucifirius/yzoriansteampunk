#include "weapon_effect.h"

#include <SCBW/UnitFinder.h>
#include <SCBW/enumerations.h>
#include <SCBW/api.h>

namespace {
  bool isUnitDistanceWithin(CUnit* target, CBullet* bullet, u32 radius);
  void weaponBulletShot(CUnit* target, CBullet* bullet, u32 dmgDivisor);
  void doWeaponHit(CUnit* target, CUnit* attacker, u8 dmgDivisor);

  // SC internal data for lurker spines
  struct SpineHitTable {
    struct {
      CUnit* source;
      CUnit* target;
    } hits[32][16];
  };
  SpineHitTable* const spineHitTable = (SpineHitTable*)(0x0064DEC8);
  u32* const spineHitRow = (u32*)(0x64EEC8);
  u32* const spineHitCol = (u32*)(0x64DEA8);
}//unnamed namespace

namespace hooks {

  void friendlySplash(CBullet* bullet, u32 weaponId){
    s16 x,y,r;
    x = bullet->sprite->position.x;
    y = bullet->sprite->position.y;
    r = weapons_dat::OuterSplashRadius[weaponId];
    
    // Get units within the outer splash radius
    scbw::UnitFinder unitsInSplash(x-r, y-r, x+r, y+r);
    
    for(int i = 0; i < unitsInSplash.getUnitCount(); i++){
      CUnit* target = unitsInSplash.getUnit(i);
      
      // Units can't splash themselves -- except psi storm
      if(target == bullet->sourceUnit && bullet->weaponType != WeaponId::PsiStorm) continue;
      
      // Don't hit allied units -- normally just checks target->playerId == bullet->srcPlayer (also checks if type is 'splash enemy' or 'nuclear missile')
      if(scbw::isUnitEnemy(bullet->srcPlayer, target) == false && target != bullet->attackTarget.unit) continue;
      
      // Can the unit be targetted?
      if(!scbw::canWeaponTargetUnit(bullet->weaponType, target, target)) continue;
      
      // We found units in a square around the target, but is the target within the splash radius?
      if(!isUnitDistanceWithin(target, bullet, weapons_dat::OuterSplashRadius[bullet->weaponType])) continue;
      
      // Don't storm a unit that's already being stormed
      if(bullet->weaponType == WeaponId::PsiStorm){
        if(target->isUnderStorm) continue;
        target->isUnderStorm = 1;
      }
      
      // Inner radius
      if(isUnitDistanceWithin(target, bullet, weapons_dat::InnerSplashRadius[bullet->weaponType])){
        weaponBulletShot(target, bullet, 1); // 100% damage
        continue;
      }
      
      // Burrowed units can only be hit by inner radius
      if(target->status & UnitStatus::Burrowed) continue;
      
      // Medium radius
      if(isUnitDistanceWithin(target, bullet, weapons_dat::MediumSplashRadius[bullet->weaponType])){
        weaponBulletShot(target, bullet, 2); // 50% damage
        continue;
      }
      
      // Outer radius
      weaponBulletShot(target, bullet, 4); // 25% damage
    }
  }

  void friendlyLurkerSplash(CBullet* bullet, u32 weaponId) {
    s16 x, y, r;
    x = bullet->sprite->position.x;
    y = bullet->sprite->position.y;
    r = weapons_dat::OuterSplashRadius[weaponId];

    // Get units within the outer splash radius
    scbw::UnitFinder unitsInSplash(x - r, y - r, x + r, y + r);

    for (int i = 0; i < unitsInSplash.getUnitCount(); i++) {
      CUnit* target = unitsInSplash.getUnit(i);

      // Units can't splash themselves
      if (target == bullet->sourceUnit) continue;

      // Don't hit allied units -- normally just checks target->playerId == bullet->srcPlayer
      if (scbw::isUnitEnemy(bullet->srcPlayer, target) == false && target != bullet->attackTarget.unit) continue;

      // Can the unit be targetted?
      if (!scbw::canWeaponTargetUnit(bullet->weaponType, target, target)) continue;

      // We found units in a square around the target, but is the target within the splash radius?
      if (!isUnitDistanceWithin(target, bullet, weapons_dat::InnerSplashRadius[bullet->weaponType])) continue;

      if (bullet->sourceUnit != NULL) {
        bool cont = false; // supposed to be a return within the loops, but since we can't it's a bool
        for (int r = 0; r < 32 && !cont; r++) {
          for (int c = 0; c < 16; c++) {
            if (spineHitTable->hits[r][c].source == bullet->sourceUnit && spineHitTable->hits[r][c].target == target) {
              cont = true; // unit has already been hit
              break;
            }
          }
        }
        if (cont) continue;

        if (*spineHitCol != 16) {
          spineHitTable->hits[*spineHitRow][*spineHitCol].source = bullet->sourceUnit;
          spineHitTable->hits[*spineHitRow][*spineHitCol].target = target;
          (*spineHitCol)++;
        }
      }
      weaponBulletShot(target, bullet, 1);
    }
  }
  
  /*
  // clone of 'Normal' damage for reference
  void normalDamage(CBullet* bullet, u32 weaponId) {
    CUnit* target = bullet->attackTarget.unit;
    if (target == NULL) return;
    
    u8 divisor = 1;
    if (bullet->hitFlags & 1) { // Miss ?
      if (target->pAI == NULL) return;
      if (bullet->sourceUnit == NULL) return;
      doWeaponHit(target, bullet->sourceUnit, divisor);
      return;
    } else {
      if (weapons_dat::Behavior[weaponId] == WeaponBehavior::Bounce) {
        for (int i = 2 - bullet->remainingBounces; i != 0; i--) divisor *= 3;
      }
	  weaponBulletShot(target, bullet, divisor);
    }
  }

  // clone of 'Yamato' damage for reference
  void yamatoDamage(CBullet* bullet, u32 weaponId) {
    if (bullet->attackTarget.unit == NULL) return;
	weaponBulletShot(bullet->attackTarget.unit, bullet, 1);
  }
  */

}//hooks

namespace {
   /*** helper functions that I couldn't find in gptp ***/

  // checks if the distance between target and bullet's positions is < radius
  const u32 Helper_IsUnitDistanceWithin = 0x0048ADB0;
  bool isUnitDistanceWithin(CUnit* target, CBullet* bullet, u32 radius){
    u32 result;
    __asm {
      PUSHAD
      PUSH radius;
      PUSH bullet;
      MOV ECX, target
      CALL Helper_IsUnitDistanceWithin
      MOV result, EAX
      POPAD
    }
    return result;
  }
  
  // creates attack overlay & deals damage from the bullet (just calls overlayAndNotify if the weapon deals no damage, or calls target->damageWith if it does
  const u32 Helper_WeaponBulletShot = 0x00479AE0;
  void weaponBulletShot(CUnit* target, CBullet* bullet, u32 dmgDivisor){
    __asm {
      PUSHAD
      PUSH dmgDivisor
      MOV EAX, target
      MOV EDX, bullet
      CALL Helper_WeaponBulletShot
      POPAD
    }
  }

  const u32 Helper_DoWeaponHit = 0x004795D0;
  void doWeaponHit(CUnit* target, CUnit* attacker, u8 dmgDivisor) {
    u32 divisor = dmgDivisor;
    __asm {
      PUSHAD
      PUSH divisor
      PUSH attacker
      MOV EBX, target
      CALL Helper_DoWeaponHit
      POPAD
    }
  }

}//unnamed namespace

