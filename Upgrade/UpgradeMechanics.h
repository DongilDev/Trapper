// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UpgradeMechanics.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAPPERPROJECT_API UUpgradeMechanics : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUpgradeMechanics();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:	
	bool IsOnUpgradeMode() { return bIsOnUpgradeMode; }
	void EnterUpgradeMode();
	void ExitUpgradeMode();

	void EnterUpgradeModeTutorial();
	void ExitUpgradeModeTutorial();
	uint8 bTutorialCheck : 1;

	void UpgradeHUDCollapsed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SelectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* DebugAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class USoundBase* SelectOverSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class USoundBase* SelectClickSound;

private:
	void SetupInputComponent();

	FTimerHandle CheckHoverTimer;
	void CheckOnHovered();

	FTimerHandle LimitTimer;
	void DecreaseLastTime();
	
	FTimerHandle CheckHoverTimerTutorial;
	void CheckOnHoveredTutorial();

	FTimerHandle CanSelectableTimer;
	void ChangeSelectable() { bCanSelectable = true; }

	uint8 LastHoveredType = 0;
	int32 LimitTime = 30;
	int32 LastTime = -1;

	bool bIsOnUpgradeMode = false;	
	bool bIsOnUpgradeModeTutorial = false;
	bool bCanSelectable = false;

	class UUpgradeHUD* HudRef = nullptr;
	class ATrapperPlayer* PlayerRef = nullptr;
	class ATrapperPlayerController* PlayerController = nullptr;
	class ATrapperGameState* GameState = nullptr;

private:
	UFUNCTION(Server, Reliable)
	void ServerRPCUpgrade(uint8 HoveredType);
};
