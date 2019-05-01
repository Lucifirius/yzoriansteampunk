#include "use_tech.h"
#include "SCBW/scbwdata.h"
#include "SCBW/api.h"
#include "hook_tools.h"



//getStatTxt from stimpack.cpp
const u32 Func_get_statTxt_Str_0 = 0x0048EF30;
u32 get_statTxt_Str_0(CUnit* unit, u32 playerId, char* message) {

	static u32 return_value;
	__asm {
		PUSHAD
		MOV EAX, message
		MOV ECX, playerId
		MOV EDX, unit
		CALL Func_get_statTxt_Str_0
		MOV EAX, return_value
		POPAD
	}
	return return_value;
}
;
//
//Func_CreateUnit from wpnspell
const u32 Func_CreateUnit = 0x004A09D0;
CUnit* CreateUnit(u32 unitId, int x, int y, u32 playerId) {
	static CUnit* unit_created;
	__asm {
		PUSHAD
		PUSH playerId
		PUSH y
		MOV ECX, unitId
		MOV EAX, x
		CALL Func_CreateUnit
		MOV unit_created, EAX
		POPAD
	}
	return unit_created;
}
;


extern const u32 Func_TriTargetOrder = 0x0048CAF0;
const void IssueTriTargetOrder(u8 order1, u8 order2, u8 order3)
{
	if (order2 == 0)
	{
		_asm
		{
			PUSHAD
			MOV DL, order1
			MOV CL, DL
			MOV AL, DL
			CALL Func_TriTargetOrder
			POPAD
		}
	}
	else
	{
		if (order3 == 0)
		{
			_asm
			{
				PUSHAD
				MOV CL, order1
				MOV DL, order2
				MOV AL, CL
				CALL Func_TriTargetOrder
				POPAD
			}
		}
		else
		{
			_asm
			{
				PUSHAD
				MOV DL, order1
				MOV CL, order2
				MOV AL, order3
				CALL Func_TriTargetOrder
				POPAD
			}
		}
	}
}

namespace hooks 
{
	void BTNSACT_UseTech(u16 tech)
	{
		//replacement for proc 0x00423660
		bool TechCapableUnit = false;
		CUnit* currentClientSelectionUnit;
		for (int i = 0; i < SELECTION_ARRAY_LENGTH && !TechCapableUnit; i++)
		{
			currentClientSelectionUnit = clientSelectionGroup->unit[i];
			if (currentClientSelectionUnit != NULL)
			{
				if (techdata_dat::EnergyCost[tech] * 256 <= currentClientSelectionUnit->energy)
				{
					TechCapableUnit = true;
				}
			}
		}
		if (!TechCapableUnit)
		{
			char* message = (char*)statTxtTbl->getString(864);
			get_statTxt_Str_0(*activePortraitUnit, *LOCAL_NATION_ID, message);
			return;
		}
		switch (tech)
		{
		case TechId::ScannerSweep:
			IssueTriTargetOrder(OrderId::PlaceScanner, 0, 0);
			break;
		case TechId::SpiderMines:
			IssueTriTargetOrder(OrderId::PlaceMine, 0, 0);
			break;
		case TechId::YamatoGun:
			IssueTriTargetOrder(OrderId::FireYamatoGun1, OrderId::FireYamatoGun2, 0);
			break;
		case TechId::Lockdown:
			IssueTriTargetOrder(OrderId::MagnaPulse, 0, 0);
			break;
		case TechId::Recall:
			IssueTriTargetOrder(OrderId::Teleport, 0, 0);
			break;
		case TechId::DefensiveMatrix:
			IssueTriTargetOrder(OrderId::DefensiveMatrix, 0, 0);
			break;
		case TechId::SpawnBroodlings:
			IssueTriTargetOrder(OrderId::SummonBroodlings, 0, 0);
			break;
		case TechId::Parasite:
			IssueTriTargetOrder(OrderId::CastParasite, 0, 0);
			break;
		case TechId::Infestation:
			IssueTriTargetOrder(OrderId::InfestMine3, OrderId::Move, OrderId::InfestMine2);
			break;
		case TechId::EMPShockwave:
			IssueTriTargetOrder(OrderId::EmpShockwave, 0, 0);
			break;
		case TechId::Irradiate:
			IssueTriTargetOrder(OrderId::Irradiate, 0, 0);
			break;
		case TechId::Ensnare:
			IssueTriTargetOrder(OrderId::Ensnare, 0, 0);
			break;
		case TechId::StasisField:
			IssueTriTargetOrder(OrderId::StasisField, 0, 0);
			break;
		case TechId::DarkSwarm:
			IssueTriTargetOrder(OrderId::DarkSwarm, 0, 0);
			break;
		case TechId::Plague:
			IssueTriTargetOrder(OrderId::Plague, 0, 0);
			break;
		case TechId::Consume:
			IssueTriTargetOrder(OrderId::Consume, 0, 0);
			break;
		case TechId::PsionicStorm:
			IssueTriTargetOrder(OrderId::PsiStorm, 0, 0);
			break;
		case TechId::Hallucination:
			IssueTriTargetOrder(OrderId::Hallucianation1, 0, 0);
			break;
		case TechId::Restoration:
			IssueTriTargetOrder(OrderId::Restoration, 0, 0);
			break;
		case TechId::DisruptionWeb:
			IssueTriTargetOrder(OrderId::CastDisruptionWeb, 0, 0);
			break;
		case TechId::MindControl:
			IssueTriTargetOrder(OrderId::CastMindControl, 0, 0);
			break;
		case TechId::Feedback:
			IssueTriTargetOrder(OrderId::CastFeedback, 0, 0);
			break;
		case TechId::OpticalFlare:
			IssueTriTargetOrder(OrderId::CastOpticalFlare, 0, 0);
			break;
		case TechId::Maelstrom:
			IssueTriTargetOrder(OrderId::CastMaelstrom, 0, 0);
			break;
		default:
			break;
		};
	}
}