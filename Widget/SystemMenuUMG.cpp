// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SystemMenuUMG.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Widget/Options/OptionsUMG.h"
#include "Framework/TrapperPlayerController.h"
#include "TrapperProject.h"
#include "Lobby/LobbyPC.h"
#include "TrapperGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TrapperPlayer.h"
#include "Components/Border.h"
#include "Components/EditableTextBox.h"


void USystemMenuUMG::NativeConstruct()
{
	Super::NativeConstruct();

	BackButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickBackButton);
	BackButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverBackButton);
	BackButton->OnUnhovered.AddDynamic(this, &USystemMenuUMG::OnUnHorverBackButton);

	OptionsButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickOptionButton);
	OptionsButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverOptionButton);
	OptionsButton->OnUnhovered.AddDynamic(this, &USystemMenuUMG::OnUnHorverOptionButton);

	GameExitButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickExitButton);
	GameExitButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverExitButton);
	GameExitButton->OnUnhovered.AddDynamic(this, &USystemMenuUMG::OnUnHorverExitButton);

	LobbyExitButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickExitButton);
	LobbyExitButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverExitButton);
	LobbyExitButton->OnUnhovered.AddDynamic(this, &USystemMenuUMG::OnUnHorverExitButton);

	RealExitButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickRealExitButton);
	RealExitButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverRealExitButton);

	CancelButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickCancelButton);
	CancelButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverCancelButton);

	/*GameReStartButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickReStartButton);
	GameReStartButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverReStartButton);
	GameReStartButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnUnHorverReStartButton);

	RealReStartButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickRealReStartButton);
	RealReStartButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverRealExitButton);

	ReStartCancelButton->OnClicked.AddDynamic(this, &USystemMenuUMG::OnClickReStartCancelButton);
	ReStartCancelButton->OnHovered.AddDynamic(this, &USystemMenuUMG::OnHorverRealExitButton);*/

	OnVisibilityChanged.AddDynamic(this, &USystemMenuUMG::OnVisibilityChangedFunction);

	FString LevelName = GetWorld()->GetMapName();
	//UE_LOG(LogTrap, Warning, TEXT("Current Level name: %s"), *LevelName);
	LevelName.RemoveFromStart(FString("UEDPIE_0_"));
	LevelName.RemoveFromStart(FString("UEDPIE_1_"));
	//UE_LOG(LogTrap, Warning, TEXT("Current Level name: %s"), *LevelName);

	//UE_LOG(LogTrap, Warning, TEXT("USystemMenuUMG::NativeConstruct"));
	//UE_LOG(LogTrap, Warning, TEXT("Current Level name: %s"), *LevelName);

	if (LevelName == "Lobby")
	{
		bInLobby = true;
		GameExitButton->SetVisibility(ESlateVisibility::Collapsed);
		//GameReStartButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (LevelName == "Trapper")
	{
		bInLobby = false;
		LobbyExitButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USystemMenuUMG::OnBack()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (!OptionsUMG)
	{
		return;
	}

	if (OptionsUMG->IsVisible())
	{
		OptionsUMG->SetVisibility(ESlateVisibility::Collapsed);
		OptionText->SetOpacity(0.5);
	}
	else if (GameExitPanel->IsVisible())
	{
		GameExitPanel->SetVisibility(ESlateVisibility::Collapsed);
		if (bInLobby)
		{
			LobbyExitText->SetOpacity(0.5);
		}
		else
		{
			GameExitText->SetOpacity(0.5);
		}
	}
	/*else if (GameReStartPanel->IsVisible())
	{
		GameReStartPanel->SetVisibility(ESlateVisibility::Collapsed);
		GameReStartText->SetOpacity(0.5);
	}*/
	else
	{
		bOnSound = false;
		OnClickBackButton();
	}
}

void USystemMenuUMG::OnMouseClickSound()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);
}

void USystemMenuUMG::OnClickBackButton()
{
	if (bOnSound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	else
	{
		bOnSound = true;
	}

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		ATrapperPlayerController* TrapperPlayerController = Cast<ATrapperPlayerController>(PlayerController);
		if (TrapperPlayerController)
		{
			TrapperPlayerController->HideSystemMenu();
		}
	}


	if (ChatBoxBorder.Get(), ChatInput.Get())
	{
		ChatBoxBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ChatInput->SetVisibility(ESlateVisibility::Visible);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void USystemMenuUMG::OnClickOptionButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);


	if (OptionsUMG->IsVisible())
	{
		OptionsUMG->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		GameExitPanel->SetVisibility(ESlateVisibility::Collapsed);
		//GameReStartPanel->SetVisibility(ESlateVisibility::Collapsed);
		OptionsUMG->SetVisibility(ESlateVisibility::Visible);

		if (bInLobby)
		{
			LobbyExitText->SetOpacity(0.5);
		}
		else
		{
			GameExitText->SetOpacity(0.5);
			//GameReStartText->SetOpacity(0.5);
		}
	}
}

void USystemMenuUMG::OnClickExitButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (GameExitPanel->IsVisible())
	{
		GameExitPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		OptionsUMG->SetVisibility(ESlateVisibility::Collapsed);
		//GameReStartPanel->SetVisibility(ESlateVisibility::Collapsed);
		GameExitPanel->SetVisibility(ESlateVisibility::Visible);

		OptionText->SetOpacity(0.5);
		//GameReStartText->SetOpacity(0.5);
	}
}

void USystemMenuUMG::OnClickRealExitButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (bInLobby)
		{
			ALobbyPC* MyController = Cast<ALobbyPC>(PlayerController);
			if (MyController)
			{
				MyController->ServerRPCTravelTitle();

				if (PlayerController->GetLocalRole() == ROLE_AutonomousProxy)
				{
					MyController->ServerRPCFriendExitLobbyOrGame();
				}
				else
				{
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
					{
						APlayerController* PC = It->Get();
						if (PC && PC != MyController)
						{
							ALobbyPC* LobbyPC = Cast<ALobbyPC>(PC);
							LobbyPC->ClientRPCFriendExitLobbyOrGame();
						}
					}
				}
			}
		}
		else
		{
			ATrapperPlayerController* MyController = Cast<ATrapperPlayerController>(PlayerController);
			if (MyController)
			{
				MyController->ServerRPCTravelTitle();

				if (PlayerController->GetLocalRole() == ROLE_AutonomousProxy)
				{
					MyController->ServerRPCFriendExitLobbyOrGame();
				}
				else
				{
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
					{
						APlayerController* PC = It->Get();
						if (PC && PC != MyController)
						{
							ATrapperPlayerController* TrapperPC = Cast<ATrapperPlayerController>(PC);
							TrapperPC->ClientRPCFriendExitLobbyOrGame();
						}
					}
				}
			}
		}
	}
}

void USystemMenuUMG::OnClickCancelButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);
	GameExitPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void USystemMenuUMG::OnHorverBackButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	BackText->SetOpacity(1);
}

void USystemMenuUMG::OnHorverOptionButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	OptionText->SetOpacity(1);
}

void USystemMenuUMG::OnHorverExitButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	if (bInLobby)
	{
		LobbyExitText->SetOpacity(1);
	}
	else
	{
		GameExitText->SetOpacity(1);
	}
}

void USystemMenuUMG::OnUnHorverBackButton()
{
	BackText->SetOpacity(0.5);
}

void USystemMenuUMG::OnUnHorverOptionButton()
{
	if (!OptionsUMG->IsVisible())
	{
		OptionText->SetOpacity(0.5);
	}
}

void USystemMenuUMG::OnUnHorverExitButton()
{
	if (!GameExitPanel->IsVisible())
	{
		if (bInLobby)
		{
			LobbyExitText->SetOpacity(0.5);
		}
		else
		{
			GameExitText->SetOpacity(0.5);
		}
	}
}

void USystemMenuUMG::OnHorverRealExitButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void USystemMenuUMG::OnHorverCancelButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void USystemMenuUMG::OnVisibilityChangedFunction(ESlateVisibility InVisibility)
{
	if (!bInLobby)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
		if (PlayerController)
		{
			if (InVisibility == ESlateVisibility::Visible)
			{
				if (GEngine && GEngine->GameViewport)
				{
					FVector2D ViewportSize;
					GEngine->GameViewport->GetViewportSize(ViewportSize);

					FVector2D CenterOfViewport = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
					FViewport* Viewport = GEngine->GameViewport->Viewport;
					if (Viewport)
					{
						Viewport->SetMouse(CenterOfViewport.X, CenterOfViewport.Y);
					}
				}

				PlayerController->SetInputModeGameAndUI();
			}
			else if (InVisibility == ESlateVisibility::Collapsed)
			{
				PlayerController->SetInputModeGame();

				ATrapperPlayer* TP = Cast<ATrapperPlayer>(PlayerController->GetPawn());
				if (IsValid(TP))
				{
					TP->UnblockBow();
				}
			}
		}
	}
}

//void USystemMenuUMG::OnClickReStartButton()
//{
//	UGameplayStatics::PlaySound2D(this, MouseClickSound);
//
//	if (GameReStartPanel->IsVisible())
//	{
//		GameReStartPanel->SetVisibility(ESlateVisibility::Collapsed);
//	}
//	else
//	{
//		OptionsUMG->SetVisibility(ESlateVisibility::Collapsed);
//		GameExitPanel->SetVisibility(ESlateVisibility::Collapsed);
//		GameReStartPanel->SetVisibility(ESlateVisibility::Visible);
//
//		OptionText->SetOpacity(0.5);
//		GameExitText->SetOpacity(0.5);
//	}
//}

//void USystemMenuUMG::OnHorverReStartButton()
//{
//	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
//	GameReStartText->SetOpacity(1);
//}
//
//void USystemMenuUMG::OnUnHorverReStartButton()
//{
//	GameReStartText->SetOpacity(0.5);
//}
//
//void USystemMenuUMG::OnClickRealReStartButton()
//{
//	UGameplayStatics::PlaySound2D(this, MouseClickSound);
//
//
//}
//
//void USystemMenuUMG::OnClickReStartCancelButton()
//{
//	UGameplayStatics::PlaySound2D(this, MouseClickSound);
//	GameReStartPanel->SetVisibility(ESlateVisibility::Collapsed);
//}

void USystemMenuUMG::SetLobbyProperty(class UBorder* Border, class UEditableTextBox* TextBox)
{
	ChatBoxBorder = Border;
	ChatInput = TextBox;
}
