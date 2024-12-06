// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapperGameInstance.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "PacketSession.h"
#include "TimerManager.h"
#include "Framework/TrapperGameState.h"
#include "Blueprint/UserWidget.h"
#include "Widget/Options/OptionsUMG.h"
#include "GameFramework/GameUserSettings.h"
#include "RHIGlobals.h"
#include "Windows/WindowsWindow.h"
#include "RHI.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"

//#define LOGIN

void UTrapperGameInstance::Init()
{
	Super::Init();

	FSlateApplication::Get().OnApplicationActivationStateChanged()
		.AddUObject(this, &UTrapperGameInstance::OnWindowFocusChanged);

	bool bLoadSuccess = LoadSettings();

	if (!bLoadSuccess)
	{
		UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
		if (GameUserSettings && GEngine)
		{
			// 디스플레이 모드
			//GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			//GraphicSettings.DisplayMode = EDisplayMode::FullScreen;
			GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			GraphicSettings.DisplayMode = EDisplayMode::WindowedFullScreen;
			DefaultDisplayModeString = "Windowed Full Screen";

			// 해상도
			//FIntPoint ScreenResolution = GEngine->GameUserSettings->GetDesktopResolution();
			FIntPoint ScreenResolution = FIntPoint(1920, 1080);
			GameUserSettings->SetScreenResolution(ScreenResolution);
			GraphicSettings.ScreenWidth = ScreenResolution.X;
			GraphicSettings.ScreenHeight = ScreenResolution.Y;
			DefaultResolutionString = FString::Printf(TEXT("%d x %d"), ScreenResolution.X, ScreenResolution.Y);

//#if !WITH_EDITOR
			// 동적 해상도
			FString GPUModel = FPlatformMisc::GetPrimaryGPUBrand();
			if (GPUModel.Contains(TEXT("NVIDIA")) || GPUModel.Contains(TEXT("AMD")))
			{
				GameUserSettings->SetDynamicResolutionEnabled(true);
			}

			// 하드웨어 벤치마크 실행 및 벤치마크 결과를 바탕으로 설정 조정
			GameUserSettings->RunHardwareBenchmark();
			GameUserSettings->ApplyHardwareBenchmarkResults();
//#endif
			// Texture
			int32 TextureQuality = GameUserSettings->GetTextureQuality();
			GraphicSettings.TextureQuality = TextureQuality;
			DefaultTextureQuality = TextureQuality;

			// Shadow
			int32 ShadowQuality = GameUserSettings->GetShadowQuality();
			GraphicSettings.ShadowQuality = ShadowQuality;
			DefaultShadowQuality = ShadowQuality;

			// Effect
			int32 EffectQuality = GameUserSettings->GetVisualEffectQuality();
			GraphicSettings.EffectQuality = EffectQuality;
			DefaultEffectQuality = EffectQuality;

			// PostProcessing
			int32 PostProcessingQuality = GameUserSettings->GetPostProcessingQuality();
			GraphicSettings.PostProcessingQuality = PostProcessingQuality;
			DefaultPostProcessingQuality = PostProcessingQuality;

			// VSyncEnabled
			bool bVSyncEnabled = GameUserSettings->IsVSyncEnabled();
			GraphicSettings.bVSyncEnabled = bVSyncEnabled;
			DefaultbVSyncEnabled = bVSyncEnabled;

			// ScreenShake
			GraphicSettings.bScreenShake = true;
			DefaultbVSyncEnabled = true;

			// AntiAliasing
			int32 AntialiasingType = 0;
			IConsoleVariable* AntiAliasingMethod = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
			if (AntiAliasingMethod)
			{
				AntialiasingType = AntiAliasingMethod->GetInt();
			}
			GraphicSettings.AntialiasingType = AntialiasingType;
			DefaultAntialiasingType = AntialiasingType;

			GameUserSettings->ApplySettings(false);

			SaveSettings();
		}
	}
	else
	{
		//UE_LOG(LogTrap, Warning, TEXT("Load true"));
		UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
		if (GameUserSettings)
		{
			if (GraphicSettings.DisplayMode == EDisplayMode::FullScreen)
			{
				//UE_LOG(LogTrap, Warning, TEXT("FullScreen"));
				GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			}
			else if (GraphicSettings.DisplayMode == EDisplayMode::WindowedFullScreen)
			{
				//UE_LOG(LogTrap, Warning, TEXT("WindowedFullScreen"));
				GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			}
			else if (GraphicSettings.DisplayMode == EDisplayMode::Window)
			{
				//UE_LOG(LogTrap, Warning, TEXT("Window"));
				GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);

				//UE_LOG(LogTrap, Warning, TEXT("ScreenWidth : %d, ScreenHeight : %d"), GraphicSettings.ScreenWidth, GraphicSettings.ScreenHeight);
				FIntPoint ScreenResolution = FIntPoint(GraphicSettings.ScreenWidth, GraphicSettings.ScreenHeight);
				GameUserSettings->SetScreenResolution(ScreenResolution);
			}
		} 
	}

#ifdef LOGIN
	LoginVer = true;
#else
	LoginVer = false;
#endif
}

void UTrapperGameInstance::Shutdown()
{
	Super::Shutdown();

	SaveSettings();
}

void UTrapperGameInstance::OnWindowFocusChanged(bool bIsFocused)
{
	if (GEngine && GEngine->GameViewport)
	{
		EWindowMode::Type WindowMode = GEngine->GameViewport->GetWindow()->GetWindowMode();
		void* WindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();

		if (WindowHandle)
		{
			if (bIsFocused)
			{
				::ShowWindow((HWND)WindowHandle, SW_SHOW);
			}
			else
			{
				if (WindowMode == EWindowMode::Fullscreen)
				{
					::ShowWindow((HWND)WindowHandle, SW_SHOWMINIMIZED);
				}
			}
		}
	}
}

void UTrapperGameInstance::ConnectToGameServer()
{
	// 이미 게임 세션이 있으면 return
	if (GameServerSession != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("ServerSession Already Exist"));
		return;
	}
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	
	UE_LOG(LogTemp, Log, TEXT("Server IP Address: %s"), *ServerIPAddress);

	FIPv4Address Ip;
	FIPv4Address::Parse(ServerIPAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTrapperGameInstance::TimerTick, 0.01f, true);

	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		// Session

		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UTrapperGameInstance::DisconnectFromGameServer()
{
	if (Socket)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UTrapperGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

FString UTrapperGameInstance::GetLocalIPAddress()
{
	bool bCanBind = false;
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);

	if (LocalIp->IsValid())
	{
		return LocalIp->ToString(false);
	}

	return FString("Unable to determine local IP address");
}

void UTrapperGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UTrapperGameInstance::TimerTick()
{
	HandleRecvPackets();
}

void UTrapperGameInstance::SaveControlsData(FControlSettings InControlSettings)
{
	for (TFieldIterator<FProperty> PropIt(FControlSettings::StaticStruct()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		void* SourceValue = Property->ContainerPtrToValuePtr<void>(&InControlSettings);
		void* TargetValue = Property->ContainerPtrToValuePtr<void>(&ControlSettings);

		Property->CopyCompleteValue(TargetValue, SourceValue);
	}
}

void UTrapperGameInstance::SaveSoundsData(FSoundSettings InSoundSettings)
{
	for (TFieldIterator<FProperty> PropIt(FSoundSettings::StaticStruct()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		void* SourceValue = Property->ContainerPtrToValuePtr<void>(&InSoundSettings);
		void* TargetValue = Property->ContainerPtrToValuePtr<void>(&SoundSettings);

		Property->CopyCompleteValue(TargetValue, SourceValue);
	}
}

void UTrapperGameInstance::SaveGraphicsData(FGraphicSettings InGraphicSettings)
{
	for (TFieldIterator<FProperty> PropIt(FGraphicSettings::StaticStruct()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		void* SourceValue = Property->ContainerPtrToValuePtr<void>(&InGraphicSettings);
		void* TargetValue = Property->ContainerPtrToValuePtr<void>(&GraphicSettings);

		Property->CopyCompleteValue(TargetValue, SourceValue);
	}
}

void UTrapperGameInstance::SaveSettings()
{
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("Settings.json");
	FString JsonString;

	TSharedPtr<FJsonObject> ControlJsonObject = MakeShareable(new FJsonObject());
	ControlJsonObject->SetNumberField(TEXT("GeneralSensitivitySliderValue"), ControlSettings.GeneralSensitivitySliderValue);
	ControlJsonObject->SetNumberField(TEXT("AimSensitivitySliderValue"), ControlSettings.AimSensitivitySliderValue);

	TSharedPtr<FJsonObject> SoundJsonObject = MakeShareable(new FJsonObject());
	SoundJsonObject->SetNumberField(TEXT("MasterSoundsSliderValue"), SoundSettings.MasterSoundsSliderValue);
	SoundJsonObject->SetNumberField(TEXT("EffectSoundsSliderValue"), SoundSettings.EffectSoundsSliderValue);
	SoundJsonObject->SetNumberField(TEXT("BackgroundSoundsSliderValue"), SoundSettings.BackgroundSoundsSliderValue);
	SoundJsonObject->SetNumberField(TEXT("VoiceDialogSoundsSliderValue"), SoundSettings.VoiceDialogSoundsSliderValue);

	TSharedPtr<FJsonObject> GraphicJsonObject = MakeShareable(new FJsonObject());
	GraphicJsonObject->SetNumberField(TEXT("DisplayMode"), static_cast<uint8>(GraphicSettings.DisplayMode));
	GraphicJsonObject->SetNumberField(TEXT("ScreenWidth"), GraphicSettings.ScreenWidth);
	GraphicJsonObject->SetNumberField(TEXT("ScreenHeight"), GraphicSettings.ScreenHeight);
	GraphicJsonObject->SetNumberField(TEXT("TextureQuality"), GraphicSettings.TextureQuality);
	GraphicJsonObject->SetNumberField(TEXT("ShadowQuality"), GraphicSettings.ShadowQuality);
	GraphicJsonObject->SetNumberField(TEXT("EffectQuality"), GraphicSettings.EffectQuality);
	GraphicJsonObject->SetNumberField(TEXT("PostProcessingQuality"), GraphicSettings.PostProcessingQuality);
	GraphicJsonObject->SetBoolField(TEXT("bVSyncEnabled"), GraphicSettings.bVSyncEnabled);
	GraphicJsonObject->SetBoolField(TEXT("bScreenShake"), GraphicSettings.bScreenShake);
	GraphicJsonObject->SetNumberField(TEXT("AntialiasingType"), GraphicSettings.AntialiasingType);
	GraphicJsonObject->SetNumberField(TEXT("FrameRateLimit"), GraphicSettings.FrameRateLimit);
	GraphicJsonObject->SetNumberField(TEXT("ScreenBrightness"), GraphicSettings.ScreenBrightness);

	TSharedPtr<FJsonObject> DefaultValuesObject = MakeShareable(new FJsonObject());
	DefaultValuesObject->SetStringField(TEXT("DefaultDisplayModeString"), DefaultDisplayModeString);
	DefaultValuesObject->SetStringField(TEXT("DefaultResolutionString"), DefaultResolutionString);
	DefaultValuesObject->SetNumberField(TEXT("DefaultTextureQuality"), DefaultTextureQuality);
	DefaultValuesObject->SetNumberField(TEXT("DefaultShadowQuality"), DefaultShadowQuality);
	DefaultValuesObject->SetNumberField(TEXT("DefaultEffectQuality"), DefaultEffectQuality);
	DefaultValuesObject->SetNumberField(TEXT("DefaultPostProcessingQuality"), DefaultPostProcessingQuality);
	DefaultValuesObject->SetBoolField(TEXT("DefaultbVSyncEnabled"), DefaultbVSyncEnabled);
	DefaultValuesObject->SetBoolField(TEXT("DefaultbScreenShake"), DefaultbScreenShake);
	DefaultValuesObject->SetNumberField(TEXT("DefaultAntialiasingType"), DefaultAntialiasingType);

	TSharedPtr<FJsonObject> FinalJsonObject = MakeShareable(new FJsonObject());
	FinalJsonObject->SetObjectField(TEXT("ControlSettings"), ControlJsonObject);
	FinalJsonObject->SetObjectField(TEXT("SoundSettings"), SoundJsonObject);
	FinalJsonObject->SetObjectField(TEXT("GraphicSettings"), GraphicJsonObject);
	FinalJsonObject->SetObjectField(TEXT("DefaultOptimization"), DefaultValuesObject);

	FString PrettyJsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&PrettyJsonString, 0);
	FJsonSerializer::Serialize(FinalJsonObject.ToSharedRef(), JsonWriter);

	FFileHelper::SaveStringToFile(PrettyJsonString, *FilePath);
}

bool UTrapperGameInstance::LoadSettings()
{
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("Settings.json");
	FString JsonString;

	if (FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			if (JsonObject->HasField(TEXT("ControlSettings")))
			{
				TSharedPtr<FJsonObject> ControlSettingsObject = JsonObject->GetObjectField(TEXT("ControlSettings"));
				ControlSettings.GeneralSensitivitySliderValue = ControlSettingsObject->GetNumberField(TEXT("GeneralSensitivitySliderValue"));
				ControlSettings.AimSensitivitySliderValue = ControlSettingsObject->GetNumberField(TEXT("AimSensitivitySliderValue"));
			}

			if (JsonObject->HasField(TEXT("SoundSettings")))
			{
				TSharedPtr<FJsonObject> SoundSettingsObject = JsonObject->GetObjectField(TEXT("SoundSettings"));
				SoundSettings.MasterSoundsSliderValue = SoundSettingsObject->GetNumberField(TEXT("MasterSoundsSliderValue"));
				SoundSettings.EffectSoundsSliderValue = SoundSettingsObject->GetNumberField(TEXT("EffectSoundsSliderValue"));
				SoundSettings.BackgroundSoundsSliderValue = SoundSettingsObject->GetNumberField(TEXT("BackgroundSoundsSliderValue"));
				SoundSettings.VoiceDialogSoundsSliderValue = SoundSettingsObject->GetNumberField(TEXT("VoiceDialogSoundsSliderValue"));
			}

			if (JsonObject->HasField(TEXT("GraphicSettings")))
			{
				TSharedPtr<FJsonObject> GraphicSettingsObject = JsonObject->GetObjectField(TEXT("GraphicSettings"));
				GraphicSettings.DisplayMode = static_cast<EDisplayMode>(GraphicSettingsObject->GetNumberField(TEXT("DisplayMode")));
				GraphicSettings.ScreenWidth = GraphicSettingsObject->GetNumberField(TEXT("ScreenWidth"));
				GraphicSettings.ScreenHeight = GraphicSettingsObject->GetNumberField(TEXT("ScreenHeight"));
				GraphicSettings.TextureQuality = GraphicSettingsObject->GetNumberField(TEXT("TextureQuality"));
				GraphicSettings.ShadowQuality = GraphicSettingsObject->GetNumberField(TEXT("ShadowQuality"));
				GraphicSettings.EffectQuality = GraphicSettingsObject->GetNumberField(TEXT("EffectQuality"));
				GraphicSettings.PostProcessingQuality = GraphicSettingsObject->GetNumberField(TEXT("PostProcessingQuality"));
				GraphicSettings.bVSyncEnabled = GraphicSettingsObject->GetBoolField(TEXT("bVSyncEnabled"));
				GraphicSettings.bScreenShake = GraphicSettingsObject->GetBoolField(TEXT("bScreenShake"));
				GraphicSettings.AntialiasingType = GraphicSettingsObject->GetNumberField(TEXT("AntialiasingType"));
				GraphicSettings.FrameRateLimit = GraphicSettingsObject->GetNumberField(TEXT("FrameRateLimit"));
				GraphicSettings.ScreenBrightness = GraphicSettingsObject->GetNumberField(TEXT("ScreenBrightness"));
			}
			if (JsonObject->HasField(TEXT("DefaultOptimization")))
			{
				TSharedPtr<FJsonObject> DefaultValuesObject = JsonObject->GetObjectField(TEXT("DefaultOptimization"));
				DefaultDisplayModeString = DefaultValuesObject->GetStringField(TEXT("DefaultDisplayModeString"));
				DefaultResolutionString = DefaultValuesObject->GetStringField(TEXT("DefaultResolutionString"));
				DefaultTextureQuality = DefaultValuesObject->GetNumberField(TEXT("DefaultTextureQuality"));
				DefaultShadowQuality = DefaultValuesObject->GetNumberField(TEXT("DefaultShadowQuality"));
				DefaultEffectQuality = DefaultValuesObject->GetNumberField(TEXT("DefaultEffectQuality"));
				DefaultPostProcessingQuality = DefaultValuesObject->GetNumberField(TEXT("DefaultPostProcessingQuality"));
				DefaultbVSyncEnabled = DefaultValuesObject->GetBoolField(TEXT("DefaultbVSyncEnabled"));
				DefaultbScreenShake = DefaultValuesObject->GetBoolField(TEXT("DefaultbScreenShake"));
				DefaultAntialiasingType = DefaultValuesObject->GetNumberField(TEXT("DefaultAntialiasingType"));
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void UTrapperGameInstance::LoadNewLevel(const FString& LevelName, FString Option)
{
	if (!GetWorld())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("World is not valid in LoadNewLevel.")));
		return;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), true, Option);
}

const FString& UTrapperGameInstance::GetPlayerID()
{
	if (PlayerID.IsEmpty())
	{
		FString Temp = FString::Printf(TEXT("Noname%d"), FMath::RandRange(1, 100));
		PlayerID = Temp;
	}

	return PlayerID;
}

void UTrapperGameInstance::SetPlayerID(FString NewUserID)
{
	PlayerID = NewUserID;
}

const FString& UTrapperGameInstance::GetPlayerNickname()
{
	return PlayerNickname;
}

void UTrapperGameInstance::SetPlayerNickname(FString NewUserID)
{
	PlayerNickname = NewUserID;
}

uint32 UTrapperGameInstance::GetRoomNumber()
{
	return RoomNumber;
}

void UTrapperGameInstance::SetRoomNumber(uint32 NewRoomNumber)
{
	RoomNumber = NewRoomNumber;
}

FString UTrapperGameInstance::GetRoomIp()
{
	return RoomIpAddress;
}

void UTrapperGameInstance::SetRoomIp(FString NewRoomIpAddress)
{
	RoomIpAddress = NewRoomIpAddress;
}

void UTrapperGameInstance::AddFriends(FString Friends, bool IsActive)
{
	MyFriends.Emplace(Friends, IsActive);
}

void UTrapperGameInstance::AddFriendRequests(FString FriendsId, FString FriendNickname)
{
	MyFriendRequests.Emplace(FriendsId, FriendNickname);
}

TMap<FString, FString>& UTrapperGameInstance::GetFriendRequests()
{
	return MyFriendRequests;
}

TMap<FString, bool>& UTrapperGameInstance::GetFriends()
{
	return MyFriends;
}

void UTrapperGameInstance::InitPlayerInfo()
{
	// 로그아웃시 초기화
	RoomNumber = 0;
	RoomIpAddress = "";
	MyFriends.Empty();
	MyFriendRequests.Empty();
	PlayerNickname = "";
	PlayerID = "";
}