/// This is where the magic happens; program your plug-in's core behavior here.

#include "game_hooks.h"
#include <graphics/graphics.h>
#include <SCBW/api.h>
#include <SCBW/ExtendSightLimit.h>
#include "psi_field.h"
#include <cstdio>
const int hyperTrigger = 0x006509A0;

namespace {
	bool unitHasPathToDestOnGround(const CUnit *unit, int x, int y);
	bool unitHasPathToUnitOnGround(const CUnit *unit, CUnit *target);
	inline void manageUnitStatusFlags(CUnit *unit, u32 flag, bool status);
}

namespace plugins {
	void runZealotCharge(CUnit *unit);
	bool chargeTargetInRange(const CUnit *fenix_zealot);
}
namespace hooks {

/// This hook is called every frame; most of your plugin's logic goes here.
bool nextFrame() {

	if (!scbw::isGamePaused()) { //If the game is not paused

		scbw::setInGameLoopState(true); //Needed for scbw::random() to work
		graphics::resetAllGraphics();
		hooks::updatePsiFieldProviders();
		*(int*)hyperTrigger = 0; // Single frame hypertriggers
    
		//This block is executed once every game.
		if (*elapsedTimeFrames == 0) {
			//Write your code here
			//scbw::printText(PLUGIN_NAME ": Hello, world!");
		}

		//Loop through all visible units in the game.
		for (CUnit *unit = *firstVisibleUnit; unit; unit = unit->link.next) {
			plugins::runZealotCharge(unit);
		}

		scbw::setInGameLoopState(false);
		
	}
  
	return true;
  
}

bool gameOn() {
	return true;
}

bool gameEnd() {
	return true;
}

} //hooks


namespace {
	 bool unitHasPathToUnitOnGround(const CUnit *unit, CUnit *target) {
		if(target != NULL)
			return unitHasPathToDestOnGround(unit, target->getX(), target->getY());
		return false;
	}
	
	const u32 Func_unitHasPathToDestOnGround = 0x0042FA00;
	bool unitHasPathToDestOnGround(const CUnit *unit, int x, int y) {
		static Bool32 bPreResult;

		__asm {
			PUSHAD
			PUSH y
			MOV EDX, x
			MOV EAX, unit
			CALL Func_unitHasPathToDestOnGround
			MOV bPreResult, EAX
			POPAD
		}

		return (bPreResult != 0);

	}
}

namespace plugins {
	//KYSXD helper code

	//Same as chargeTarget_ in unit_speed.cpp
	bool chargeTargetInRange(const CUnit *zealot) {
		if(!zealot->orderTarget.unit)
			return false;
		CUnit *chargeTarget = zealot->orderTarget.unit;
		int maxChargeRange = 3 << 5;
		int minChargeRange = 16;
		int chargeRange = zealot->getDistanceToTarget(zealot->orderTarget.unit);
		if(zealot->mainOrderId != OrderId::AttackUnit)
			return false;
		else if(minChargeRange > chargeRange
			|| chargeRange > maxChargeRange)
			return false;
		else if(unitHasPathToUnitOnGround(zealot, chargeTarget))
			return true;
		return false;
	}

	//KYSXD zealot's charge
	void runZealotCharge(CUnit *unit) {
		//Check max_energy.cpp and unit_speed.cpp for more info
		if(unit->id == UnitId::fenix_zealot) {
			//Unit isn't in charge state
			if(!unit->stimTimer) {
				if(chargeTargetInRange(unit)) {
					//Must be: orderTo(CastStimPack)
					if(unit->energy == unit->getMaxEnergy()) {
						unit->stimTimer = 5;
						unit->energy = 0;
					}
				}
			}
		}
	}
}