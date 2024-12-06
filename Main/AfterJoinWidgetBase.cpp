// Fill out your copyright notice in the Description page of Project Settings.


#include "Main/AfterJoinWidgetBase.h"
#include "MainPC.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UAfterJoinWidgetBase::NativeConstruct()
{
	if (GotoLoginButton)
	{
		GotoLoginButton->OnClicked.AddDynamic(this, &UAfterJoinWidgetBase::OnGotoLoginButton);
		GotoLoginButton->OnHovered.AddDynamic(this, &UAfterJoinWidgetBase::OnButtonHovered);
	}
}

void UAfterJoinWidgetBase::OnGotoLoginButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);
	AMainPC* PC = GetOwningPlayer<AMainPC>();
	PC->ShowLoginWidget(true);
}

void UAfterJoinWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}
