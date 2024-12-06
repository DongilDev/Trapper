// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/BaseMonsterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBaseMonsterAnimInstance::UBaseMonsterAnimInstance()
	:MovingThreshold(3.0f)
{
}

void UBaseMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UBaseMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size();
		bIsIdle = Velocity.IsNearlyZero();
		bIsFalling = Movement->IsFalling();	
	}
}
