// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TrapperPlayerAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTakeArrowDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDrawBowDelegate);
DECLARE_MULTICAST_DELEGATE(FOnReloadEndDelegate);
/**
 * 
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UTrapperPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UTrapperPlayerAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float Angle; /*Direction*/

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
	float Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsFalling : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	uint8 bIsMagneticMoving : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	uint8 bIsMagneticCasting : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsDead : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsInstall : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Controller")
	float ControllerYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow")
	bool bIsHandlingBow = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow")
	bool bIsDrawingBow = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow")
	bool bIsCombatMode = false;

	TObjectPtr<class ACharacter> Owner;
	TObjectPtr<class APlayerController> PlayerController;
	TObjectPtr<class UBowMachanics> BowMechanics;

	///  --------------- Animation Notify Delegate ---------------
	UFUNCTION() void AnimNotify_TakeArrow();
	UFUNCTION() void AnimNotify_DrawBow();
	UFUNCTION() void AnimNotify_EndReload();

	FOnTakeArrowDelegate OnTakeArrow;
	FOnDrawBowDelegate OnDrawBow;
	FOnReloadEndDelegate OnReloadEnd;
};
