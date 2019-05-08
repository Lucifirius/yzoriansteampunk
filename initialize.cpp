#include "definitions.h"
#include "Plugin.h"
#include "hook_tools.h"

//Hook header files
#include "hooks/game_hooks.h"
#include "graphics/draw_hook.h"

#include "hooks/apply_upgrade_flags.h"
#include "hooks/attack_priority.h"
#include "hooks/bunker_hooks.h"
#include "hooks/cloak_nearby_units.h"
#include "hooks/cloak_tech.h"
#include "hooks/detector.h"
#include "hooks/harvest.h"
#include "hooks/rally_point.h"
#include "hooks/recharge_shields.h"
#include "hooks/spider_mine.h"
#include "hooks/stim_packs.h"
#include "hooks/tech_target_check.h"
#include "hooks/transfer_tech_upgrades.h"
#include "hooks/unit_speed.h"
#include "hooks/update_status_effects.h"
#include "hooks/update_unit_state.h"
#include "hooks/weapons/weapon_cooldown.h"
#include "hooks/weapons/weapon_damage.h"
#include "hooks/weapons/weapon_fire.h"

#include "hooks/unit_destructor_special.h"
#include "hooks/psi_field.h"

#include "hooks/unit_stats/armor_bonus.h"
#include "hooks/unit_stats/sight_range.h"
#include "hooks/unit_stats/max_energy.h"
#include "hooks/unit_stats/weapon_range.h"
#include "hooks/interface/weapon_armor_tooltip.h"
#include "hooks/weapon_effect.h"

//in alphabetical order
#include "hooks/orders/base_orders/attack_orders.h"
#include "hooks/interface/btns_cond.h"
#include "hooks/orders/building_making/building_morph.h"
#include "hooks/interface/buttonsets.h"
#include "hooks/orders/building_making/building_protoss.h"
#include "hooks/orders/building_making/building_terran.h"
#include "hooks/recv_commands/burrow_tech.h"
#include "hooks/orders/spells/cast_order.h"
#include "hooks/recv_commands/CMDRECV_Build.h"
#include "hooks/recv_commands/CMDRECV_Cancel.h"
#include "hooks/recv_commands/CMDRECV_MergeArchon.h"
#include "hooks/recv_commands/CMDRECV_Morph.h"
#include "hooks/recv_commands/CMDRECV_SiegeTank.h"
#include "hooks/recv_commands/CMDRECV_Stop.h"
#include "hooks/create_init_units.h"
#include "hooks/orders/base_orders/die_order.h"
#include "hooks/orders/enter_nydus.h"
#include "hooks/orders/spells/feedback_spell.h"
#include "hooks/give_unit.h"
#include "hooks/orders/spells/hallucination_spell.h"
#include "hooks/orders/infestation.h"
#include "hooks/orders/larva_creep_spawn.h"
#include "hooks/orders/liftland.h"
#include "hooks/orders/load_unload_orders.h"
#include "hooks/load_unload_proc.h"
#include "hooks/orders/building_making/make_nydus_exit.h"
#include "hooks/orders/medic_orders.h"
#include "hooks/orders/merge_units.h"
#include "hooks/orders/spells/mindcontrol_spell.h"
#include "hooks/orders/spells/nuke_orders.h"
#include "hooks/orders/spells/recall_spell.h"
#include "hooks/recv_commands/receive_command.h"
#include "hooks/orders/repair_order.h"
#include "hooks/orders/research_upgrade_orders.h"
#include "hooks/interface/select_larva.h"
#include "hooks/interface/selection.h"
#include "hooks/orders/siege_transform.h"
#include "hooks/interface/stats_panel_display.h"
#include "hooks/orders/base_orders/stopholdpos_orders.h"
#include "hooks/recv_commands/train_cmd_receive.h"
#include "hooks/orders/unit_making/unit_morph.h"
#include "hooks/orders/unit_making/unit_train.h"
#include "hooks/utils/utils.h"
#include "hooks/use_tech.h"
#include "hooks/interface/wireframe.h"
#include "hooks/weapons/wpnspellhit.h"
#include "hooks/weapon_effect.h"

//#include "AI/spellcasting.h"

/// This function is called when the plugin is loaded into StarCraft.
/// You can enable/disable each group of hooks by commenting them.
/// You can also add custom modifications to StarCraft.exe by using:
///		memoryPatch(address_to_patch, value_to_patch_with);

 void runWMode()
 {
   //KYSXD W-Mode start
   /// PLUGIN_NAME is defined in definitions.h
   int msgboxID = MessageBox(NULL, "Do you want to play in windowed mode?", PLUGIN_NAME , MB_YESNOCANCEL);
   switch (msgboxID) {
     case IDYES:
       HINSTANCE hDll;
       hDll =LoadLibrary("./WMODE.dll");
       hDll =LoadLibrary("./WMODE_FIX.dll");
       break;
	 case IDNO:
		 break;
     case IDCANCEL:
       exit(0);
	   break;
	 default:
	   exit(0);
   } //KYSXD W-Mode end
   return;
}

BOOL WINAPI Plugin::InitializePlugin(IMPQDraftServer *lpMPQDraftServer) {

	//StarCraft.exe version check
	runWMode();
	char exePath[300];
	const DWORD pathLen = GetModuleFileName(NULL, exePath, sizeof(exePath));

	if (pathLen == sizeof(exePath)) {
		MessageBox(NULL, "Error: Cannot check version of StarCraft.exe. The file path is too long.", NULL, MB_OK);
		return FALSE;
	}

	if (!checkStarCraftExeVersion(exePath))
		return FALSE;

	const u8 nop3[3] = {0x8d,0x76,0x00};
	const u8 b74 = 0x74;
	u8 swtab[193];
	int i = 0;
	for( ; i <=  64; i++) swtab[i] = 0;
	for( ; i <= 128; i++) swtab[i] = 1;
	for( ; i <= 192; i++) swtab[i] = 2;
	memoryPatch((void*)(0x004A4424 +2), &b74, sizeof(u8));
	memoryPatch((void*)0x004A4584, swtab, sizeof(swtab));
	memoryPatch((void*)0x004A441B, nop3, sizeof(nop3));

	const u8 nop6[6] = {0x8d,0xb6,0x00,0x00,0x00,0x00};
	const u8 bFF = 0xFF;
	const u8 b0F = 0x0F;
	const u8 b00 = 0x00; // ezworkaround
	memoryPatch((void*)(0x0047E347 +3), &b0F, sizeof(u8));
	memoryPatch((void*)(0x0047E38F +2), &b0F, sizeof(u8));
	memoryPatch((void*)(0x0049C563 +3), &b0F, sizeof(u8));
	memoryPatch((void*)(0x0049C6AC +3), &b0F, sizeof(u8));
	memoryPatch((void*)(0x004BCEE0 +2), &b0F, sizeof(u8));
	memoryPatch((void*)(0x004BD13F +3), &b0F, sizeof(u8));
	memoryPatch((void*)(0x0049C595 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0049C6DA +3), &bFF, sizeof(u8));
	memoryPatch((void*)0x0047E3BA, nop6, sizeof(nop6));
	memoryPatch((void*)0x0049C580, nop6, sizeof(nop6));
	memoryPatch((void*)0x0049C6C9, nop6, sizeof(nop6));
	memoryPatch((void*)0x004BCF1E, nop6, sizeof(nop6));
	memoryPatch((void*)(0x004BCF34 +4), &b00, sizeof(u8));
	memoryPatch((void*)(0x00413F38 +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00413F8D +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414210 +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414252 +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x004142C5 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x004142EF +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414326 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0041435A +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414390 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x004143C6 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x004143F2 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0041441A +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414467 +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x004145D5 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414720 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x00414882 +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0047D75A +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0047DC99 +2), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0047DED5 +3), &bFF, sizeof(u8));
	memoryPatch((void*)(0x0047E045 +3), &bFF, sizeof(u8));
	//disable creep spawn
	//u8 creep_nops[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	//memoryPatch(0x0045CF5D, creep_nops, sizeof creep_nops);
	//

	hooks::injectGameHooks();
	hooks::injectDrawHook();

	//in order of creation
	/*hooks::injectInfestationHooks();
	hooks::injectSiegeTransformHooks();
	hooks::injectButtonSetHooks();
	hooks::injectSelectMod();
	hooks::injectMergeUnitsHooks();
	hooks::injectLarvaCreepSpawnHooks();
	hooks::injectLiftLandHooks();
	hooks::injectAttackOrdersHooks();
	hooks::injectStopHoldPosOrdersHooks();
	hooks::injectRecallSpellHooks();
	hooks::injectEnterNydusHooks();
	hooks::injectCastOrderHooks();
	hooks::injectWpnSpellHitHooks();
	hooks::injectBuildingMorphHooks();
	hooks::injectMakeNydusExitHook();
	hooks::injectUnitMorphHooks();
	hooks::injectWireframeHook();
	hooks::injectDieOrderHook();
	hooks::injectBuildingTerranHook();
	hooks::injectBuildingProtossHooks();
	hooks::injectUnitTrainHooks();
	hooks::injectLoadUnloadProcHooks();
	hooks::injectLoadUnloadOrdersHooks();
	hooks::injectNukeOrdersHooks();
	hooks::injectBurrowTechHooks();
	hooks::injectResearchUpgradeOrdersHooks();
	hooks::injectMedicOrdersHooks();
	hooks::injectHallucinationSpellHook();
	hooks::injectFeedbackSpellHook();	
	hooks::injectBtnsCondHook();
	hooks::injectRecvCmdHook();
	hooks::injectCreateInitUnitsHooks();
	hooks::injectGiveUnitHook();
	hooks::injectTrainCmdRecvHooks();
	hooks::injectCMDRECV_SiegeTankHooks();
	hooks::injectCMDRECV_MergeArchonHooks();
	hooks::injectCMDRECV_MorphHooks();
	hooks::injectCMDRECV_StopHooks();
	hooks::injectCMDRECV_CancelHooks();
	hooks::injectSelectLarvaHooks();
	hooks::injectRepairOrderHook();
	hooks::injectStatsPanelDisplayHook();
	hooks::injectUtilsHooks();
	hooks::injectMindControlSpellHook();
	hooks::injectCMDRECV_BuildHooks();

	hooks::injectApplyUpgradeFlags();
	hooks::injectAttackPriorityHooks();
	hooks::injectBunkerHooks();
	hooks::injectCloakNearbyUnits();
	hooks::injectCloakingTechHooks();
	hooks::injectDetectorHooks();
	hooks::injectHarvestResource();
	hooks::injectRallyHooks();*/
	/*hooks::injectRechargeShieldsHooks();
	hooks::injectSpiderMineHooks();
	hooks::injectStimPacksHooks();
	hooks::injectTechTargetCheckHooks();
	hooks::injectTransferTechAndUpgradesHooks();
	hooks::injectUnitSpeedHooks();
	hooks::injectUpdateStatusEffects();
	hooks::injectUpdateUnitState();
	hooks::injectWeaponCooldownHook();
	hooks::injectWeaponDamageHook();
	hooks::injectWeaponFireHooks();
	
	hooks::injectUnitDestructorSpecial();
	hooks::injectPsiFieldHooks();
	
	hooks::injectArmorBonusHook();
	hooks::injectSightRangeHook();
	hooks::injectUnitMaxEnergyHook();
	hooks::injectWeaponRangeHooks();*/
	
	hooks::injectUnitTooltipHook();/*
	hooks::injectSpellcasterAI();
	hooks::injectUseTechHooks();*/
	hooks::injectWeaponEffectHook();

	return TRUE;
}
