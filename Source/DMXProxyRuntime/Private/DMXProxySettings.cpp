// Copyright Epic Games, Inc. All Rights Reserved.

#include "DMXProxySettings.h"
#include "Common/UdpSocketBuilder.h"
#include "Common/UdpSocketReceiver.h"
#include "Common/UdpSocketSender.h"

#include "Serialization/ArrayWriter.h"
#include "Serialization/ArrayReader.h"

DECLARE_LOG_CATEGORY_EXTERN(DMXProxySettingsProxy, Log, All);
DEFINE_LOG_CATEGORY(DMXProxySettingsProxy);

FDMXProxySettingsProxy::FDMXProxySettingsProxy()
	: bIsEnabledCached(false)
{
	ListeningSocket = nullptr;
	SendingSocket = nullptr;
	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	InputInterfacePort = 0;
	ProxyInterfacePort = 0;
	SendingPort = 0;
	bIsEnabled = false;
	bIsLog = false;
}

FDMXProxySettingsProxy::~FDMXProxySettingsProxy()
{
	StopListener();
	StopSender();
}

void FDMXProxySettingsProxy::Tick(float DeltaTime)
{
	if (bIsEnabledCached != bIsEnabled)
	{
		if (bIsEnabled)
		{
			StartListener();
			StartSender();
		}
		else
		{
			StopListener();
			StopSender();
		}

		bIsEnabledCached = bIsEnabled;
	}
}

void FDMXProxySettingsProxy::StartListener()
{
	// Create Listening socket
	bool bIsValid = false;
	TSharedPtr<FInternetAddr> ListeningAddr = SocketSubsystem->CreateInternetAddr();
	ListeningAddr->SetIp(*InputInterfaceIP, bIsValid);
	ListeningAddr->SetPort(InputInterfacePort);
	FIPv4Endpoint ListenerEndpoint = FIPv4Endpoint(ListeningAddr);

	FSocket* NewListeningSocket = FUdpSocketBuilder(TEXT("DMXProxyListener"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(ListenerEndpoint);

	if (NewListeningSocket == nullptr)
	{
		UE_LOG(DMXProxySettingsProxy, Error, TEXT("Error create NewListeningSocket: for InputInterfaceIP %s"), *InputInterfaceIP);
		return;
	}
	ListeningSocket = NewListeningSocket;

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);

	SocketReceive = MakeShared<FUdpSocketReceiver>(ListeningSocket, ThreadWaitTime, TEXT("DMXProxyListenerReceiver"));
	SocketReceive->OnDataReceived().BindRaw(this, &FDMXProxySettingsProxy::HandleListenSocketDataReceived);
	SocketReceive->SetMaxReadBufferSize(2048);
	SocketReceive->Start();
}

void FDMXProxySettingsProxy::StopListener()
{
	SocketReceive.Reset();

	// destroy sockets
	if (ListeningSocket != nullptr)
	{
		SocketSubsystem->DestroySocket(ListeningSocket);
		ListeningSocket = nullptr;
	}
}

void FDMXProxySettingsProxy::StartSender()
{
	bool bIsValid = false;
	TSharedPtr<FInternetAddr> ListeningAddr = SocketSubsystem->CreateInternetAddr();
	ListeningAddr->SetIp(*ProxyInterfaceIP, bIsValid);
	ListeningAddr->SetPort(ProxyInterfacePort);
	FIPv4Endpoint ListenerEndpoint = FIPv4Endpoint(ListeningAddr);

	FSocket* NewSenderSocket = FUdpSocketBuilder(TEXT("DMXProxySettingsProxySender"))
		.AsNonBlocking()
		.AsReusable()
		.WithBroadcast()
		.BoundToEndpoint(ListeningAddr);

	if (NewSenderSocket == nullptr)
	{
		UE_LOG(DMXProxySettingsProxy, Error, TEXT("Error create NewSenderSocket: for ProxyInterfaceIP %s"), *ProxyInterfaceIP);
		return;
	}
	SendingSocket = NewSenderSocket;

	SocketSender = MakeShared<FUdpSocketSender>(SendingSocket, TEXT("FUdpMessageProcessor.Sender"));
}

void FDMXProxySettingsProxy::StopSender()
{
	SocketSender.Reset();

	if (SendingSocket != nullptr)
	{
		SocketSubsystem->DestroySocket(SendingSocket);
		SendingSocket = nullptr;
	}
}

void FDMXProxySettingsProxy::HandleListenSocketDataReceived(const TSharedPtr<FArrayReader, ESPMode::ThreadSafe>& Data, const FIPv4Endpoint& Sender)
{
	if (bIsLog)
	{
		UE_LOG(DMXProxySettingsProxy, Log, TEXT("ListenSocketDataReceived, [NUM] : %d"), Data->Num());
	}

	if (!SocketSender)
	{
		UE_LOG(DMXProxySettingsProxy, Error, TEXT("[ListenSocketDataReceived] SocketSender is not valid"));
		return;
	}

	TSharedRef<FInternetAddr> AddrToSend = SocketSubsystem->CreateInternetAddr();
	AddrToSend->SetPort(SendingPort);
	bool bIsValid = false;
	AddrToSend->SetIp(*SendingIP, bIsValid);
	FIPv4Endpoint SendEndpoint = FIPv4Endpoint(AddrToSend);
	if (!bIsValid)
	{
		UE_LOG(DMXProxySettingsProxy, Error, TEXT("Error Set IP %s"), *SendEndpoint.ToString());
		return;
	}

	TSharedRef<FArrayWriter, ESPMode::ThreadSafe> Writer = MakeShared<FArrayWriter, ESPMode::ThreadSafe>();
	Writer->SetNumZeroed(Data->Num());
	FMemory::Memcpy(Writer->GetData(), Data.Get()->GetData(), Data->Num());
	if (!SocketSender->Send(Writer, SendEndpoint))
	{
		UE_LOG(DMXProxySettingsProxy, Error, TEXT("Error Proxy Send"));
	}
}



UDMXProxySettings::UDMXProxySettings()
{
}

TStatId UDMXProxySettings::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FNiagaraScriptToolkit, STATGROUP_Tickables);
}

void UDMXProxySettings::Tick(float DeltaTime)
{
	for (FDMXProxySettingsProxy& Proxy : ProxyInstances)
	{
		Proxy.Tick(DeltaTime);
	}
}
