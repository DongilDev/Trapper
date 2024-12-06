// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestManager.h"
#include "Quest/QuestEffect.h"
#include "Particles/ParticleSystemComponent.h"
#include "Framework/TrapperGameState.h"

#include "Net/UnrealNetwork.h"

#include "Data/QuestDataTables.h"
#include "Data/QuestActorTables.h"

#include "Level/TrapperScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "Framework/TrapperPlayerController.h"
#include "Framework/TrapperGameMode.h"

#include "Monster/TutorialMonster.h"
#include "Data/TutorialMonsterDataTables.h"

#include "Trap/BearTrap.h"
#include "Ping/QuestPing.h"
#include "Quest/Interact.h"
#include "TrapperProject.h"

// Sets default values
AQuestManager::AQuestManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> QuestTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_QuestData.DT_QuestData'"));
	if (QuestTable.Succeeded() && QuestTable.Object)
	{
		QuestData = QuestTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> QuestActorTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_QuestActorData.DT_QuestActorData'"));
	if (QuestActorTable.Succeeded() && QuestActorTable.Object)
	{
		QuestActorData = QuestActorTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> TutorialMonsterTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_TutorialMonster.DT_TutorialMonster'"));
	if (TutorialMonsterTable.Succeeded() && TutorialMonsterTable.Object)
	{
		TutorialMonsterData = TutorialMonsterTable.Object;
	}
}

// Called when the game starts or when spawned
void AQuestManager::BeginPlay()
{
	Super::BeginPlay();

	ATrapperGameState* GameState = GetWorld()->GetGameState<ATrapperGameState>();
	GameState->OnQuestExecute.AddUObject(this, &AQuestManager::QuestCheck);

	if (QuestData)
	{
		// 리스트에 퀘스트 추가
		AddQuest();
	}

	// 이펙트 생성
	QuestEffect = GetWorld()->SpawnActor<AQuestEffect>();

	// 튜토리얼 몬스터 세팅
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AQuestManager::TutorialMonsterSetting, 1.f, false);
	}

	for (AInteract* BlockBox : TActorRange<AInteract>(GetWorld()))
	{
		QuestActorBox.Add(BlockBox);
		UE_LOG(LogTemp, Warning, TEXT("Add Block Box"));
	}
}

// Called every frame
void AQuestManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuestManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQuestManager, CurrentQuestIndex);
	DOREPLIFETIME(AQuestManager, bIsQuestWaiting);
}

void AQuestManager::AddQuest()
{
	LastQuestIndex = QuestData->GetRowMap().Num() - 1;

	for (int i = 1; i <= QuestData->GetRowMap().Num(); i++)
	{
		FQuestInfo* Data = QuestData->FindRow<FQuestInfo>(*FString::FromInt(i), FString());

		FQuest Quest;
		Quest.Initialize(Data->QuestCode, Data->Title, Data->Description,
			Data->GoalCount1P, Data->GoalCount2P, Data->EffectPosition, 
			Data->QuestActorCode, Data->TutorialText);

		if (Data->bIsAlwaysChecking)
		{
			Quest.bIsAlwaysChecking = true;
		}

		if (Data->bChangeMainQuest)
		{
			Quest.bChangeMainQuest = true;
		}

		if (Data->ExceptionCode != 0)
		{
			Quest.ExceptionCode = Data->ExceptionCode;
		}

		if (Data->SendEventCode.Num() != 0)
		{
			Quest.SendEventCode = Data->SendEventCode;
		}

		if (Data->bTutorialEnd)
		{
			TutorialEndIndex = i;
		}

		QuestList.Add(Quest);
	}
}

void AQuestManager::OnRep_ChangeCurrentIndex()
{
	if (!QuestList.IsValidIndex(CurrentQuestIndex))
	{
		return;
	}

	SetQuestUI();

	if (!bIsQuestWaiting)
	{
		SetQuestEffect();
	}
}

FQuest& AQuestManager::GetCurrentQuest()
{
	if (!QuestList.IsValidIndex(CurrentQuestIndex))
	{
		checkf(false, TEXT("Quest Index Error %d"), CurrentQuestIndex);
	}

	return QuestList[CurrentQuestIndex];
}

void AQuestManager::QuestStart()
{
	bIsQuestWaiting = false;
	CurrentQuestIndex++;

	if (QuestActorData)
	{
		// 퀘스트 액터 생성
		CreateQuestActor();
	}

	SetQuestEffect();
	SetQuestUI();
}

void AQuestManager::QuestCheck(int32 InQuestCode, bool bIs1P)
{
	//UE_LOG(LogQuest, Warning, TEXT("Quest Code : %d"), InQuestCode);

	if (CurrentQuestIndex < 0)
	{
		return;
	}

	// 항상 체크하는 퀘스트 확인
	AlwaysCheckQuestCheck(InQuestCode, bIs1P);

	FQuest& CurrentQuest = GetCurrentQuest();

	// 퀘스트 코드 안맞으면 Exit
	if (CurrentQuest.QuestCode != InQuestCode)
	{
		return;
	}

	// Count 증가
	if (bIs1P && (CurrentQuest.Count1P < CurrentQuest.GoalCount1P))
	{
		CurrentQuest.Count1P++;
	}
	else if (!bIs1P && (CurrentQuest.Count2P < CurrentQuest.GoalCount2P))
	{
		CurrentQuest.Count2P++;
	}

	if (IsQuestClear())
	{
		// 퀘스트 완료 처리
		if (HasAuthority())
		{
			QuestComplete();
		}
		else
		{
			ServerRPCQuestComplete();
		}
	}
	else
	{
		if (HasAuthority())
		{
			ClientRPCAddCount(CurrentQuestIndex, CurrentQuest.Count1P);
		}
		else
		{
			ServerRPCAddCount(CurrentQuestIndex, CurrentQuest.Count2P);
		}
	}

	// 이펙트 설정
	if (HasAuthority())
	{
		if (GetCurrentQuest().Count1P >= GetCurrentQuest().GoalCount1P)
		{
			QuestEffect->QuestPingEffect->Deactivate();

			if (QuestPing) QuestPing->Destroy();
		}
	}
	else
	{
		if (GetCurrentQuest().Count2P >= GetCurrentQuest().GoalCount2P)
		{
			QuestEffect->QuestPingEffect->Deactivate();

			if (QuestPing) QuestPing->Destroy();
		}
	}

	// UI 변경
	SetQuestUI();

	UE_LOG(LogQuest, Warning, TEXT("[%s] Count 1P : %d / 2P : %d - Current Index %d"),
		*GetCurrentQuest().Description, GetCurrentQuest().Count1P, GetCurrentQuest().Count2P, CurrentQuestIndex);
}

void AQuestManager::AlwaysCheckQuestCheck(int32 InQuestCode, bool bIs1P)
{
	for (int i = 0; i < QuestList.Num(); i++)
	{
		checkf(QuestList.IsValidIndex(i), TEXT("Always Check List Index Error"));
		FQuest& AlwaysCheckQuest = QuestList[i];

		if (!AlwaysCheckQuest.bIsAlwaysChecking ||
			AlwaysCheckQuest.QuestCode != InQuestCode ||
			i == CurrentQuestIndex)
		{
			continue;
		}

		//---------------------------------------------------------------

		// Count 증가
		if (bIs1P)
		{
			AlwaysCheckQuest.Count1P++;
			ClientRPCAddCount(i, AlwaysCheckQuest.Count1P);
		}
		else if (!bIs1P)
		{
			AlwaysCheckQuest.Count2P++;
			ServerRPCAddCount(i, AlwaysCheckQuest.Count2P);
		}

		//UE_LOG(LogQuest, Warning, TEXT("-- Always Check Quest -- Count 1P : %d / 2P : %d"),
		//	AlwaysCheckQuest.Count1P, AlwaysCheckQuest.Count2P);
	}
}

bool AQuestManager::IsQuestClear()
{
	int32 GoalProgress = GetCurrentQuest().Count1P + GetCurrentQuest().Count2P;
	if (GoalProgress >= GetCurrentQuest().TotalGoalCount)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AQuestManager::QuestComplete()
{
	//UE_LOG(LogQuest, Warning, TEXT("[%s] Quest Complete"), *GetCurrentQuest().Description);

	PlayQuestCompleteSound();

	if (GetCurrentQuest().SendEventCode.Num() != 0)
	{
		ATrapperGameState* GameState = GetWorld()->GetGameState<ATrapperGameState>();
		for (auto Code : GetCurrentQuest().SendEventCode)
		{
			// 이벤트 코드 브로드캐스트
			GameState->OnEventExecute.Broadcast(Code);
		}
	}
	
	// 마지막 퀘스트 클리어일 경우, 게임 클리어
	if (CurrentQuestIndex == LastQuestIndex)
	{
		UE_LOG(LogQuest, Warning, TEXT("Last Quest"));
		return;
	}

	if (GetCurrentQuest().bIsAlwaysChecking)
	{
		MulticastRPCAlwaysQuestNotCheck(CurrentQuestIndex);
	}

	// 메인 퀘스트 종료시
	if (GetCurrentQuest().bChangeMainQuest == true)
	{
		//UE_LOG(LogQuest, Warning, TEXT("Clear Main Quest"));

		for (ATrapperPlayerController* PlayerController : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			PlayerController->ShowQuestLayout(false);
		}

		// 현재 퀘스트 액터 정리
		DestroyQuestActor();

		// 이펙트 끄기
		MulticastRPCQuestEffectOff();

		// 퀘스트 대기 상태로 전환
		bIsQuestWaiting = true;

		return;
	}

	CurrentQuestIndex++;

	// 이미 클리어 했을 시 5초 뒤에 QuestComplete 호출
	if (IsQuestClear())
	{
		GetWorld()->GetTimerManager().SetTimer(QuestCompleteHandle, this,
			&AQuestManager::QuestComplete, 1.0f, false, 7.0f);
	}

	// 현재 퀘스트 액터 정리
	DestroyQuestActor();

	// 다음 퀘스트 액터 준비
	CreateQuestActor();

	// 둘다 서버만 변경함
	// 클라이언트 이펙트는 CurrentIndex의 OnRep 함수에서 변경
	SetQuestUI();
	SetQuestEffect();
}

void AQuestManager::MulticastRPCQuestEffectOff_Implementation()
{
	for (ATrapperPlayerController* PlayerController : TActorRange<ATrapperPlayerController>(GetWorld()))
	{
		PlayerController->ShowQuestLayout(false);
		PlayerController->ShowGodDialog(false);
	}

	if (IsValid(QuestPing)) QuestPing->Destroy();

	//UE_LOG(LogQuest, Warning,TEXT("Effect Off"));
	if (IsValid(QuestEffect)) QuestEffect->SetQuestEffect(false, FVector::ZeroVector);
}

void AQuestManager::MulticastRPCAlwaysQuestNotCheck_Implementation(int32 Index)
{
	QuestList[Index].bIsAlwaysChecking = false;
}

void AQuestManager::ServerRPCQuestComplete_Implementation()
{
	QuestComplete();
}

void AQuestManager::ServerRPCAddCount_Implementation(int32 Index, int32 Count)
{
	QuestList[Index].Count2P = Count;
	SetQuestUI();
}

void AQuestManager::ClientRPCAddCount_Implementation(int32 Index, int32 Count)
{
	QuestList[Index].Count1P = Count;
	SetQuestUI();
}

void AQuestManager::PlayQuestCompleteSound()
{
	ALevelScriptActor* LevelScriptActor = GetWorld()->GetLevelScriptActor();
	ATrapperScriptActor* MyLevelScriptActor = Cast<ATrapperScriptActor>(LevelScriptActor);
	if (MyLevelScriptActor)
	{
		MyLevelScriptActor->MulticastPlaySystemSound(ESystemSound::QuestClear);
	}
}

void AQuestManager::CreateQuestActor()
{
	// 서버가 아니면 return
	if (!HasAuthority())
	{
		return;
	}

	// 현재 액터에서 생성해야 할 액터 코드
	TArray<int32> CurrentQuestActorCode = GetCurrentQuest().QuestActorCode;

	// 액터 생성
	for (auto Code : CurrentQuestActorCode)
	{
		FQuestActorInfo* Data = QuestActorData->FindRow<FQuestActorInfo>(*FString::FromInt(Code), FString());

		FTransform QuestActorTransform;
		QuestActorTransform.SetLocation(Data->Position);
		QuestActorTransform.SetRotation(FQuat::MakeFromRotator(Data->Rotation));
		QuestActorTransform.SetScale3D(Data->Scale);

		FActorSpawnParameters Param;
		Param.Owner = this->Owner;
		QuestActorBox.Add(GetWorld()->SpawnActor(Data->QuestActor, &QuestActorTransform, Param));
	}

	// 퀘스트별 예외처리
	HandleQuestExceptions();
}

void AQuestManager::DestroyQuestActor()
{
	for (auto Actor : QuestActorBox)
	{
		Actor->Destroy();
	}

	QuestActorBox.Empty();

	//UE_LOG(LogQuest, Warning, TEXT("Quest Actor Destroy"));
}

void AQuestManager::TutorialMonsterSetting()
{
	int Index = 1;

	for (ATutorialMonster* TutorialMonster : TActorRange<ATutorialMonster>(GetWorld()))
	{
		FTutorialMonsterInfo* Data = TutorialMonsterData->FindRow<FTutorialMonsterInfo>(*FString::FromInt(Index), FString());
		if(!Data) return;
		TutorialMonster->Teleport(Data->MonsterPosition);
		TutorialMonster->SetActorRotation(Data->MonsterRotation);
		Index++;
	}
}

void AQuestManager::SkipTutorial()
{
	bIsQuestWaiting = true;
	CurrentQuestIndex = TutorialEndIndex-1;

	for (ATrapperPlayerController* PlayerController : TActorRange<ATrapperPlayerController>(GetWorld()))
	{
		PlayerController->ShowQuestLayout(false);
	}

	DestroyQuestActor();
	TutorialEndSetting();

	MulticastRPCQuestEffectOff();
}

void AQuestManager::TutorialEndSetting()
{
	for (ATutorialMonster* TutorialMonster : TActorRange<ATutorialMonster>(GetWorld()))
	{
		TutorialMonster->Teleport(TutorialMonster->StartPoint);
	}

	for (ABearTrap* BearTrap : TActorRange<ABearTrap>(GetWorld()))
	{
		BearTrap->DestroyHandle();
	}
}

void AQuestManager::SkipBeforeBoss()
{
	CurrentQuestIndex = 24;
	TutorialEndSetting();
	MulticastRPCQuestEffectOff();
}

void AQuestManager::SetQuest(int Index)
{
	CurrentQuestIndex = Index;
}

void AQuestManager::HandleQuestExceptions()
{
	switch (GetCurrentQuest().ExceptionCode)
	{
		// 찰코함정 활성화
		case 1 :
		{
			for (ABearTrap* BearTrap : TActorRange<ABearTrap>(GetWorld()))
			{
				BearTrap->MagneticTriggerControl(true);
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
}

void AQuestManager::SetQuestEffect()
{
	if (IsValid(QuestEffect))
	{
		bool IsActive = false;
		if (GetCurrentQuest().EffectPosition != FVector::Zero())
		{
			IsActive = true;

			// 있던건 지우고
			if (IsValid(QuestPing)) 
			{
				QuestPing->Destroy();
			}

			// 퀘스트 핑 생성
			QuestPing = GetWorld()->SpawnActor<AQuestPing>(QuestPingClass, GetCurrentQuest().EffectPosition, FRotator::ZeroRotator);
		}

		QuestEffect->SetQuestEffect(IsActive, GetCurrentQuest().EffectPosition);
	}


}

void AQuestManager::SetQuestUI()
{
	FQuest CurrentQuest = GetCurrentQuest();
	FString Description = CurrentQuest.Description;

	if (CurrentQuest.TotalGoalCount > 1)
	{
		uint8 TotalGoal = CurrentQuest.Count1P + CurrentQuest.Count2P;
		Description += TEXT("(") + FString::FromInt(TotalGoal) + TEXT(" / ") +
			FString::FromInt(CurrentQuest.TotalGoalCount) + TEXT(")");
	}

	for (ATrapperPlayerController* PlayerController : TActorRange<ATrapperPlayerController>(GetWorld()))
	{
		PlayerController->SetQuestInfo(CurrentQuest.Title, Description);

		if (bIsQuestWaiting)
		{
			PlayerController->ShowGodDialog(false);
			continue;
		}

		if (!CurrentQuest.TutorialText.IsEmpty())
		{
			PlayerController->SetGodDialog(2, CurrentQuest.TutorialText);
			PlayerController->ShowGodDialog(true);
		}
		else
		{
			PlayerController->ShowGodDialog(false);
		}
	}
}







