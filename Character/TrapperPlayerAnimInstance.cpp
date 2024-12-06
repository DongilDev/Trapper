// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapperPlayerAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/TrapperPlayerMovementComponent.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "KismetAnimationLibrary.h"

#include "TrapperPlayer.h"
#include "Bow/BowMechanics.h"

UTrapperPlayerAnimInstance::UTrapperPlayerAnimInstance()
{
}

void UTrapperPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (IsValid(Owner))
	{
		PlayerController = Cast<APlayerController>(Owner->GetController());
		ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Owner);

		if (IsValid(TrapperPlayer))
		{
			BowMechanics = TrapperPlayer->BowMechanics;
		}
	}
}

void UTrapperPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner)
	{
		Owner = Cast<ACharacter>(GetOwningActor());
	}
	else
	{
		ATrapperPlayer* Player = CastChecked<ATrapperPlayer>(Owner);
		UTrapperPlayerMovementComponent* Movement = CastChecked<UTrapperPlayerMovementComponent>(Player->GetCharacterMovement());

		Velocity = Player->GetVelocity();
		Speed = Velocity.Size2D();

		if (Player->GetControlState() == ECharacterControlType::Idle)
		{
			Angle = Player->GetLookRotation();
		}
		else
		{
			Angle = UKismetAnimationLibrary::CalculateDirection(Player->GetVelocity(), Player->GetActorRotation());
		}

		bIsDead = Player->GetDeadState();
		bIsFalling = Movement->IsFalling();

		bIsMagneticMoving = Movement->GetMagneticMovingState();
		bIsMagneticCasting = Movement->IsMagneticCast();

		bIsInstall = Player->IsPreviewStartAnim();

		FRotator TargetRotator = Player->GetControlRotation().GetNormalized();
		FRotator Rotator = UKismetMathLibrary::RInterpTo(FRotator(Pitch, Yaw, 0), TargetRotator, GetDeltaSeconds(), 15);
		//Pitch = UKismetMathLibrary::Clamp(Rotator.Pitch, -90, 90);
		Pitch = Player->GetAimOffset().Pitch;
		Yaw = UKismetMathLibrary::Clamp(Rotator.Yaw, -180, 180);
	}

	if (!PlayerController)
	{
		if (IsValid(Owner))
			PlayerController = Cast<APlayerController>(Owner->GetController());
	}
	else
	{
		ControllerYaw = PlayerController->GetControlRotation().Yaw;
	}

	if (!IsValid(BowMechanics))
	{
		if (IsValid(Owner))
		{
			PlayerController = Cast<APlayerController>(Owner->GetController());
			ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Owner);

			if (IsValid(TrapperPlayer))
			{
				BowMechanics = TrapperPlayer->BowMechanics;
			}
		}
	}
	else
	{
		bIsHandlingBow = BowMechanics->IsHandlingBow();
		bIsDrawingBow = BowMechanics->IsDrawingBow();
		bIsCombatMode = BowMechanics->IsCombatMode();
	}
}

void UTrapperPlayerAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTrapperPlayerAnimInstance, Pitch);
}

void UTrapperPlayerAnimInstance::AnimNotify_TakeArrow()
{
	OnTakeArrow.Broadcast();
}

void UTrapperPlayerAnimInstance::AnimNotify_DrawBow()
{
	OnDrawBow.Broadcast();
}

void UTrapperPlayerAnimInstance::AnimNotify_EndReload()
{
	OnReloadEnd.Broadcast();
}

