// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TrapperProject.h"
#include "Options.h"
#include "TrapperGameInstance.generated.h"


/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UTrapperGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	virtual void Init() override;
	virtual void Shutdown() override;
	void OnWindowFocusChanged(bool bIsFocused);

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	UFUNCTION(BlueprintCallable, Category = "Network")
	FString GetLocalIPAddress();

	void SendPacket(SendBufferRef SendBuffer);

	void LoadNewLevel(const FString& LevelName, FString Option = "");

	const FString& GetPlayerID();
	void SetPlayerID(FString NewUserID);

	const FString& GetPlayerNickname();
	void SetPlayerNickname(FString NewUserID);

	uint32 GetRoomNumber();
	void SetRoomNumber(uint32 RoomNumber);

	FString GetRoomIp();
	void SetRoomIp(FString NewRoomIpAddress);

	void AddFriends(FString Friends, bool IsActive);
	TMap<FString, bool>& GetFriends();

	void AddFriendRequests(FString FriendId, FString FriendNickname);
	TMap<FString, FString>& GetFriendRequests();
	
	bool GetLoginVer() { return LoginVer; }

	void InitPlayerInfo();

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Room Data")
	int32 RoomNumber = 0;
	FString RoomIpAddress = "";

	class FSocket* Socket;
	FString ServerIPAddress = "3.36.168.218";
	int16 Port = 7777;
	TSharedPtr<class PacketSession> GameServerSession = nullptr;

private:
	FTimerHandle TimerHandle;
	void TimerTick();
	TMap<FString, bool> MyFriends;
	TMap<FString, FString> MyFriendRequests;

	bool LoginVer = false;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PlayerNickname;
	FString PlayerID;

#pragma region Option

public:
	void SaveControlsData(FControlSettings InControlSettings);
	void SaveSoundsData(FSoundSettings InSoundSettings);
	void SaveGraphicsData(FGraphicSettings InGraphicSettings);

	FORCEINLINE FControlSettings GetControlSettigs() const { return ControlSettings; }
	FORCEINLINE FSoundSettings GetSoundSettings() const { return SoundSettings; }
	FORCEINLINE FGraphicSettings GetGraphicSettings() const { return GraphicSettings; }

	FORCEINLINE USoundMix* GetSoundClassMix() const { return SoundMix; }
	FORCEINLINE USoundClass* GetMasterSoundClass() const { return MasterSoundClass; }
	FORCEINLINE USoundClass* GetEffectSoundClass() const { return EffectSoundClass; }
	FORCEINLINE USoundClass* GetBackgroundSoundClass() const { return BackgroundSoundClass; }
	FORCEINLINE USoundClass* GetVoiceDialogSoundClass() const { return VoiceDialogSoundClass; }

	FORCEINLINE FString GetDefaultDisplayModeString() const { return DefaultDisplayModeString; }
	FORCEINLINE FString GetDefaultDefaultResolutionString() const { return DefaultResolutionString; }
	FORCEINLINE int32 GetDefaultTexture() const { return DefaultTextureQuality; }
	FORCEINLINE int32 GetDefaultShadow() const { return DefaultShadowQuality; }
	FORCEINLINE int32 GetDefaultEffect() const { return DefaultEffectQuality; }
	FORCEINLINE int32 GetDefaultPostProcessing() const { return DefaultPostProcessingQuality; }
	FORCEINLINE bool GetDefaultbVSyncEnabled() const { return DefaultbVSyncEnabled; }
	FORCEINLINE bool GetDefaultScreenShake() const { return DefaultbScreenShake; }
	FORCEINLINE int32 GetDefaultAntialiasing() const { return DefaultAntialiasingType; }

private:
	// Controls Data
	FControlSettings ControlSettings;

	// Sounds Data
	FSoundSettings SoundSettings;

	// Graphic Data
	FGraphicSettings GraphicSettings;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundMix > SoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundClass> MasterSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundClass> EffectSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundClass> BackgroundSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundClass> VoiceDialogSoundClass;

	FString DefaultDisplayModeString = "";
	FString DefaultResolutionString = "";
	int32 DefaultTextureQuality = 0;
	int32 DefaultShadowQuality = 0;
	int32 DefaultEffectQuality = 0;
	int32 DefaultPostProcessingQuality = 0;
	uint8 DefaultbVSyncEnabled : 1 = false;
	uint8 DefaultbScreenShake : 1 = false;
	int32 DefaultAntialiasingType = 0;

#pragma endregion Option

#pragma region SystemMenu

public:
	bool IsFriendExitLobbyOrGame() const { return bFriendExitLobbyOrGame; }
	FORCEINLINE void SetFriendExitLobbyOrGame(bool Value) { bFriendExitLobbyOrGame = Value; }
	void SetExitFriendName(FString Name) { ExitFriendName = Name; }
	FORCEINLINE FString GetExitFriendName() const { return ExitFriendName; }

private:
	uint8 bFriendExitLobbyOrGame : 1 = false;
	FString ExitFriendName;

#pragma endregion SystemMenu

#pragma region SaveLoad

public:
	void SaveSettings();

private:
	bool LoadSettings();

#pragma endregion SaveLoad
};
