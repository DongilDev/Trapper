// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventCode.generated.h"

UENUM(BlueprintType)
enum class ETrapperEventCode : uint8
{
	/// ====================
	///		   Event
	/// ====================
	 
	E_Move_Maintenance_Item_Given,
	E_Move_Maintenance_Item_No,
	E_Tutorial_End_Cleanup,
	E_Maintenance_Progress,
	E_Maintenance_Enter,
	E_Tower_Upgrade,
	E_Hide_Wave_HUD,
	E_Start_Bonus_Wave,
	E_Turret_Tutorial_Dialog,
	E_Kill_All_Monster,
	E_Game_Clear,
	E_Start_Awakening,
	E_Stop_BGM,
	E_Change_Trap_Tutorial_Text,
	E_Active_Skip_Button,
	E_Deactive_Skip_Button,
	E_Start_Main_Quest,
	E_Show_Media_HUD,
	E_Hide_Media_HUD,

	/// ====================
	///		   Media
	/// ====================

	S_Wave_Start,				
	S_Boss_Appear,				
	S_BonusWave_Start,				
	S_Ending,				

	/// ====================
	///		   Cheat
	/// ====================

	C_Maintenance_1,				
	C_Maintenance_2,					
	C_Maintenance_3,						
	C_Maintenance_4,					
	C_Maintenance_5,					
	C_Maintenance_6,					
	C_After_Boss,					
	C_Add_Bone,						
	C_Invincibility_Mode,				
	C_All_Monster_Kill,
	C_Game_Restart,

	/// ====================
	///		   Dialog
	/// ====================

	D_Opening,			

	D_Tutorial_Quest_Entry,
	D_Soluna_Quest_Success,		
	D_Soluna_Quest_1P_Fail,				
	D_Soluna_Quest_2P_Fail,			
	D_Chalco_Trap_Quest_Entry,			
	D_Chalco_Trap_1P_Fail,		
	D_Chalco_Trap_2P_Fail,
	D_Soluna_Quest_Entry,				

	D_Altar_Move_Quest_Entry,		
	D_Altar_Interaction_Quest_Entry,
	D_First_Maintenance_Entry,	
	D_First_Wave_Entry,		
	D_Turret_Under_Attack,				
	D_Second_Maintenance_Entry,			

	D_Second_Wave_Entry,			
	D_Third_Maintenance_Entry,		
	D_Third_Wave_Entry,		
	D_Fourth_Maintenance_Entry,
	D_Fourth_Wave_Entry,		
	D_Fifth_Maintenance_Entry,
	D_Fifth_Wave_Entry,		
	D_Sixth_Maintenance_Entry,
	D_Boss_Battle_Entry,	
	D_Boss_Appearance_End,				
	D_Boss_Battle_End,			
	D_Bonus_Wave_Start,					
	D_Seal_Release_End,					

	D_Second_Maintenance_Upgrade,	
	D_Third_Maintenance_Upgrade,		
	D_Fourth_Maintenance_Upgrade,		
	D_Fifth_Maintenance_Upgrade,		
	D_Sixth_Maintenance_Upgrade,

	D_Trap_Install_Quest,
	
	/// ====================
	///		추가 이벤트
	/// ====================
	
	E_Skip_Quest,
	E_Skip_Media,
	E_Initialize_Skip_Media,

	E_Disabled_Input,
	E_Enabled_Input,

	E_Enter_Upgrade,
	E_Start_Wave,

	E_AbleFActive,
	E_DisableFActive,

	E_GoToBossStage,
	E_ContinueGame,
	E_SpawnBoss,
};

UCLASS()
class TRAPPERPROJECT_API AEventCode : public AActor
{
	GENERATED_BODY()
};
