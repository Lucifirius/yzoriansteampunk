#include "weapon_armor_tooltip.h"
#include <SCBW/api.h>
#include <SCBW/enumerations/WeaponId.h>
#include <cstdio>


char buffer[128];

//Returns the special damage multiplier factor for units that don't use the
//"Damage Factor" property in weapons.dat.
u8 getDamageFactorForTooltip(u8 weaponId, CUnit* unit) {

	//Default StarCraft behavior
	if (
		unit->id == UnitId::firebat || 
		unit->id == UnitId::gui_montag ||
		unit->id == UnitId::zealot || 
		unit->id == UnitId::fenix_zealot
	)
		return 2;

	if (unit->id == UnitId::valkyrie)
		return 1;

	return weapons_dat::DamageFactor[weaponId];
}

//Returns the C-string for the tooltip text of the unit's weapon icon.
//This function is used for weapon icons and special icons.
//Precondition: @p entryStrIndex is a stat_txt.tbl string index.
const char* getDamageTooltipString(u8 weaponId, CUnit* unit, u16 entryStrIndex) {
	//Default StarCraft behavior

	//get strings
	const char* entryName = statTxtTbl->getString(entryStrIndex);
	const char* damageStr = statTxtTbl->getString(777);					 //"Damage:"
	const char* perRocketStr = statTxtTbl->getString(1301);	 //"per rocket"
	const char* type = "Type:";
	const char* typeName = "";
	const char* rangeText = "Range:";
	const char* meleeText = "Melee";

	//grab weapon details from weapon dat
	const u8 damageFactor = getDamageFactorForTooltip(weaponId, unit);
	const u8 upgradeLevel = scbw::getUpgradeLevel(unit->playerId, weapons_dat::DamageUpgrade[weaponId]);
	const u16 baseDamage = weapons_dat::DamageAmount[weaponId] * damageFactor;
	const u16 bonusDamage = weapons_dat::DamageBonus[weaponId] * damageFactor * upgradeLevel;
	const u8 damageType = weapons_dat::DamageType[weaponId];
	//calculate max range for weapon
	const u32 range = weapons_dat::MaxRange[weaponId]/32;

	u32 bonusRange = 0;

	//find range bonus for weapon
	switch (weaponId) {

		case WeaponId::GaussRifle:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::U_238Shells))
				bonusRange = 1;
			break;
		case WeaponId::NeedleSpines:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::GroovedSpines))
				bonusRange = 1;
			break;
		case WeaponId::PhaseDisruptor:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::SingularityCharge))
				bonusRange = 2;
			break;
		case WeaponId::PhaseDisruptor_Fenix_Dragoon:
			if (scbw::isBroodWarMode())
				bonusRange = 2;
			break;
		case WeaponId::HellfireMissilePack:
			if (scbw::getUpgradeLevel(unit->playerId, UpgradeId::CharonBooster))
				bonusRange = 3;
			break;
		case WeaponId::HellfireMissilePack_AlanSchezar:
			if (scbw::isBroodWarMode())
				bonusRange = 3;
			break;
		default:
			bonusRange = 0;

	}

	//grab the string for the damage type of the weapon
	switch(damageType) {
	case DamageType::Normal:
		//Normal type damage uses green text
		typeName = "\017Normal\002";
		break;
	case DamageType::Explosive:
		//Explosive type damage uses orange text
		typeName = "\021Explosive\002";
		break;
	case DamageType::Concussive:
		//Concussive type damage uses yellow text
		typeName = "\003Concussive\002";
		break;
	case DamageType::Independent:
		//Independent type damage uses white text
		typeName = "\004Independent\002";
		break;
	case DamageType::IgnoreArmor:
		//Ignore Armor type damage uses white text
		typeName = "\004Magic\002";
		break;
	default:
		typeName = "None";
		break;
	};

	//check if not melee
	if(range > 0) {
		//check for valk weapon
		if (weaponId == WeaponId::HaloRockets) {
			//check for upgrade damage
			if (bonusDamage > 0)
				if(bonusRange > 0) {
					sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d %s\n%s %d+%d\n%s %s",
									entryName, damageStr, baseDamage, bonusDamage, perRocketStr, rangeText, range, bonusRange, type, typeName);
				} else {
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d %s\n%s %d\n%s %s",
									entryName, damageStr, baseDamage, bonusDamage, perRocketStr, rangeText, range, type, typeName);
				}
			else
				if(bonusRange > 0) {
					sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d %s\n%s %d+%d\n%s %s",
									entryName, damageStr, baseDamage, perRocketStr, rangeText, range, bonusRange, type, typeName);
				} else {
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d %s\n%s %d\n%s %s",
									entryName, damageStr, baseDamage, perRocketStr, rangeText, range, type, typeName);
				}
		}
		else {
			//check for upgrade damage
			if (bonusDamage > 0)
				if(bonusRange > 0) {
					sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d\n%s %d+%d\n%s %s",
									entryName, damageStr, baseDamage, bonusDamage, rangeText, range, bonusRange, type, typeName);
				} else {
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d\n%s %d\n%s %s",
									entryName, damageStr, baseDamage, bonusDamage, rangeText, range, type, typeName);
				}
			else
				if(bonusRange >0) {
					sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d\n%s %d+%d\n%s %s",
						entryName, damageStr, baseDamage, rangeText, range, bonusRange, type, typeName);
				} else {
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d\n%s %d\n%s %s",
						entryName, damageStr, baseDamage, rangeText, range, type, typeName);
				}
		}
	//melee attacks
	} else {
		//check for valk weapon
		if (weaponId == WeaponId::HaloRockets) {
			//check for upgrade damage
			if (bonusDamage > 0)
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d %s\n%s %s\n%s %s",
									entryName, damageStr, baseDamage, bonusDamage, perRocketStr, rangeText, meleeText, type, typeName);
			else
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d %s\n%s %s\n%s %s",
									entryName, damageStr, baseDamage, perRocketStr, rangeText, meleeText, type, typeName);
		}
		else {
			//check for upgrade damage
			if (bonusDamage > 0)
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d\n%s %s\n%s %s",
									entryName, damageStr, baseDamage, bonusDamage, rangeText, meleeText, type, typeName);
			else
				sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d\n%s %s\n%s %s",
									entryName, damageStr, baseDamage, rangeText, meleeText, type, typeName);
		}
	}

	return buffer;
}

namespace hooks {

//Returns the C-string for the tooltip text of the unit's weapon icon.
const char* getWeaponTooltipString(u8 weaponId, CUnit* unit) {
	return getDamageTooltipString(weaponId, unit, weapons_dat::Label[weaponId]);
}

//Returns the C-string for the tooltip text of the unit's armor icon.
const char* getArmorTooltipString(CUnit* unit) {
	//Default StarCraft behavior
	
	const u16 labelId = upgrades_dat::Label[units_dat::ArmorUpgrade[unit->id]];
	const char *armorUpgradeName = statTxtTbl->getString(labelId);
	const char *armorStr = statTxtTbl->getString(778);						//"Armor:"

	const u8 baseArmor = units_dat::ArmorAmount[unit->id];
	const u8 bonusArmor = unit->getArmorBonus();
	const u8 unitSize = units_dat::SizeType[unit->id];
	const char* sizeLabel = "Size:";
	const char* sizeText = "";

	switch(unitSize) {
	case 1:
		sizeText = "Small";
		break;
	case 2:
		sizeText = "Medium";
		break;
	case 3:
		sizeText = "Large";
		break;
	default:
		sizeText = "Unknown";
		break;
	}

	if (bonusArmor > 0)
		sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d\nSize: %s",
							armorUpgradeName, armorStr, baseArmor, bonusArmor, sizeText);
	else
		sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d\nSize: %s",
							armorUpgradeName, armorStr, baseArmor, sizeText);

	return buffer;
}


//Returns the C-string for the tooltip text of the plasma shield icon.
const char* getShieldTooltipString(CUnit* unit) {
	//Default StarCraft behavior

	const u16 labelId = upgrades_dat::Label[UpgradeId::ProtossPlasmaShields];
	const char *shieldUpgradeName = statTxtTbl->getString(labelId);
	const char *shieldStr = statTxtTbl->getString(779);					 //"Shields:"

	const u8 shieldUpgradeLevel = scbw::getUpgradeLevel(unit->playerId, UpgradeId::ProtossPlasmaShields);

	if (shieldUpgradeLevel > 0)
		sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d+%d",
							shieldUpgradeName, shieldStr, 0, shieldUpgradeLevel);
	else
		sprintf_s(buffer, sizeof(buffer), "\004%s\002\n%s %d",
							shieldUpgradeName, shieldStr, 0);

	return buffer;
}

//Returns the C-string for the tooltip text of the Interceptor icon (Carriers),
//Scarab icon (Reavers), Nuclear Missile icon (Nuclear Silos), and Spider Mine
//icon (Vultures).
const char* getSpecialTooltipString(u16 iconUnitId, CUnit* unit) {
	//Default StarCraft behavior

	if (iconUnitId == UnitId::interceptor)
		return getDamageTooltipString(WeaponId::PulseCannon, unit, 791);		//"Interceptors"

	if (iconUnitId == UnitId::scarab)
		return getDamageTooltipString(WeaponId::Scarab, unit, 792);				 //"Scarabs"

	if (iconUnitId == UnitId::nuclear_missile)
		return statTxtTbl->getString(793);																	//"Nukes"

	if (iconUnitId == UnitId::spider_mine)
		return getDamageTooltipString(WeaponId::SpiderMines, unit, 794);		//"Spider Mines"

	//Should never reach here
	return "";

}

} //hooks
