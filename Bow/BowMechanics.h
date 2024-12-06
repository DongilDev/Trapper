// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "BowEnums.h"
#include "BowMechanics.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TRAPPERPROJECT_API UBowMachanics : public UActorComponent
{
	GENERATED_BODY()

public:
	UBowMachanics();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

/// 멤버 함수 ---------------------------------
public:
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDrawingBow() const { return bIsDrawingBow; }

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsHandlingBow() const { return bIsHandlingBow; }

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsCombatMode() const { return bIsCombatMode; }

	UFUNCTION(BlueprintCallable, Category = "State")
	float GetDrawTime() const { return DrawTime; }

	UFUNCTION(BlueprintCallable, Category = "State")
	float GetMaxDrawTime() const { return MaxDrawTime; }

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsUsingMagneticArrow() const 
	{ if(ArrowType == EArrowType::Magnetic) return true; return false;}

	void EnterBlocked();			
	void ReleaseBlocked();			

private:
	void SetupInputComponent();		
	void AttachBow();				
	FVector CalculateAimDirection();

private:
	void CombatModeBegin();			
	void CombatModeEnd();			
	void DrawBegin();				
	void DrawEnd();					
	void FireArrowBegin();			
	void FireArrowEnd();			
	void ReleaseBlockReal();

	void SwitchArrowType(const FInputActionValue& Value);
	void SwitchArrowSocket(EArrowSocketType type);		
	void ClientFireArrow(float drawTime, FTransform ArrowTransform);

	// 애니메이션 노티파이
	UFUNCTION() void OnTakeArrowNotify();				
	UFUNCTION() void OnDrawBowNotify();					
	UFUNCTION() void OnReloadEndNotify();				
	UFUNCTION() void OnReloadEndDelegate(UAnimMontage* Montage, bool bInterrupted);


public:
	// 데이터 적용
	void SetDrawTime(float Value) { MaxDrawTime = Value; }
	void ApplyDefaultArrowData();
	void ApplyMagneticArrowData();
	void SetAwaken();

public:
	float ArrowMinSpeed = 10000.f;
	float ArrowMaxSpeed = 20000.f;
	float NormalArrowMinDamage = 55.f;
	float NormalArrowMaxDamage = 55.f;
	float MagneticArrowMinDamage = 55.f;
	float MagneticArrowMaxDamage = 55.f;
	

/// 멤버 변수 ---------------------------------
public:
	// 컴포넌트 & 클래스
	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	FName BowSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	FName ArrowSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	FName NockedArrowSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class ABow> BowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> DefaultArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> MagneticArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> DefaultAwakenArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> MagneticAwakenArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> ClientDefaultArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> ClientMagneticArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> ClientDefaultAwakenArrowClass;

	UPROPERTY(EditDefaultsOnly, Category = "BowMachanics")
	TSubclassOf<class AArrow> ClientMagneticAwakenArrowClass;


	// 인풋 ----------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchArrowAction;


	// 애니메이션 -----------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	class UAnimMontage* DrawMontage;


	// 사운드 -------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	USoundBase* DrawSound;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	USoundBase* DrawAwakenSound;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	USoundBase* FireAwakenSound;


private:
	// 포인터 변수 -------------------------------------------
	UPROPERTY() TObjectPtr<class ATrapperPlayer> CharacterRef;
	UPROPERTY(Replicated) TObjectPtr<class ABow> BowRef;
	UPROPERTY(Replicated) TObjectPtr<class AArrow> ArrowRef;
	UPROPERTY() TObjectPtr<class UTrapperPlayerAnimInstance> AnimInstance;
	UPROPERTY() TObjectPtr<class ATrapperGameState> GameState;


	// 상태값 -------------------------------------------
	float DrawTime = 0.f;
	float MaxDrawTime = 0.f;
	float DrawIncrementTime = 0.033333f;
	float CombatModeDuration = 1.f;

	EArrowType ArrowType = EArrowType::Magnetic;
	EArrowSocketType ArrowSocketType = EArrowSocketType::None;

	bool bIsInputTriggerOnGoing = false;
	bool bIsBlocked = false;
	bool bCanSwitchArrow = true;

	UPROPERTY(Replicated) 
	bool bIsCombatMode = false;		// 줌 인 상태(활을 앞으로 들고 있는가)

	UPROPERTY(Replicated) 
	bool bIsDrawingBow = false;		// 활을 당기고 있는가
	
	UPROPERTY(Replicated) 
	bool bIsHandlingBow = false;	// 활을 당기고 - 발사 - 장전중인 상태인지

	UPROPERTY(Replicated) 
	bool bIsAwaken = false;			// 각성상태

	bool bPleasePleasePlease = false;

/// 타이머 ------------------------------------------------------
#pragma region Timer
private:
	FTimerHandle DrawBowTimer;
	FTimerHandle SwitchArrowTimer;
	FTimerHandle CombatTimer;
	FTimerHandle ArrowPleaseStartTimer;

	void IncrementDrawTime();
	void SwitchArrowCoolDownTimer() { bCanSwitchArrow = true; }
	void ArrowPleaseStart();

#pragma endregion Timer


/// 리플리케이션 -----------------------------------------------
#pragma region Replication
private:
	UFUNCTION(Server, Reliable)
	void ServerRPCSpawnArrow(EArrowType Type);
	void ServerRPCSpawnArrow_Implementation(EArrowType Type);

	UFUNCTION(Server, Reliable)
	void ServerRPCDestroyArrow();
	void ServerRPCDestroyArrow_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerRPCSwitchArrowSocket(EArrowSocketType type);
	void ServerRPCSwitchArrowSocket_Implementation(EArrowSocketType type);

	UFUNCTION(Server, Reliable)
	void ServerRPCFireArrow(float drawTime, FTransform ArrowTransform);
	void ServerRPCFireArrow_Implementation(float drawTime, FTransform ArrowTransform);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayFireMontage();
	void MulticastRPCPlayFireMontage_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeDrawState(EBowDrawState State);

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeCombatMode(bool IsCombatMode);

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeDrawingBow(bool IsDrawing);

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeHandlingBow(bool IsHandling);

#pragma endregion Replication
};
