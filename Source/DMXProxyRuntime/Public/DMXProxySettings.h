// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tickable.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

#include "DMXProxySettings.generated.h"

class FSocket;
class ISocketSubsystem;
class FUdpSocketReceiver;
class FUdpSocketSender;
class FArrayReader;

USTRUCT()
struct DMXPROXYRUNTIME_API FDMXProxySettingsProxy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (editcondition = "!bIsEnabled"))
	FString InputInterfaceIP;

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (ClampMin = "0", ClampMax = "65535", editcondition = "!bIsEnabled"))
	int32 InputInterfacePort;

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (editcondition = "!bIsEnabled"))
	FString ProxyInterfaceIP;

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (ClampMin = "0", ClampMax = "65535", editcondition = "!bIsEnabled"))
	int32 ProxyInterfacePort;

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (editcondition = "!bIsEnabled"))
	FString SendingIP;

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (ClampMin = "0", ClampMax = "65535", editcondition = "!bIsEnabled"))
	int32 SendingPort;

	UPROPERTY(EditAnywhere, Category = "ProxySettings")
	bool bIsEnabled;

	UPROPERTY(EditAnywhere, Category = "ProxySettings", meta = (editcondition = "!bIsEnabled"))
	bool bIsLog;

private:
	bool bIsEnabledCached;

	FSocket* ListeningSocket;
	FSocket* SendingSocket;

	TSharedPtr<FUdpSocketReceiver> SocketReceive;
	TSharedPtr<FUdpSocketSender> SocketSender;

	FIPv4Endpoint BroadcastEndpoint;

	ISocketSubsystem* SocketSubsystem;

public:
	FDMXProxySettingsProxy();
	~FDMXProxySettingsProxy();

	void Tick(float DeltaTime);

private:
	void StartListener();
	void StopListener();
	void StartSender();
	void StopSender();

	void HandleListenSocketDataReceived(const TSharedPtr<FArrayReader, ESPMode::ThreadSafe>& Data, const FIPv4Endpoint& Sender);
};


/**  User defined protocol settings that apply to a whole protocol module */
UCLASS(config = Engine, notplaceable)
class DMXPROXYRUNTIME_API UDMXProxySettings
	: public UObject
	, public FTickableGameObject
{
public:
	GENERATED_BODY()

public:
	UDMXProxySettings();


	virtual bool IsTickableInEditor() const override { return true; }
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(Config, EditAnywhere, Category = "ProxySettings")
	TArray<FDMXProxySettingsProxy> ProxyInstances;

};
