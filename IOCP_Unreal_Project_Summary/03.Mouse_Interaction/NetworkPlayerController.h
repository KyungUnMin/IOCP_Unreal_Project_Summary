// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Protocol.pb.h"
#include "FSM/PlayerFSM.h"
#include "Define.h"
#include "NetworkPlayerController.generated.h"

class UStateBase;
class ANetworkPlayer;
class ANetworkOthers;
class UMouseCursorWidget;
class ILClickInterface;
class UCameraShakeBase;

/**
 * 
 */
UCLASS()
class MABINOGI_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANetworkPlayerController();

protected:
	virtual void Tick(float _DeltaTime) override;

private:
	void LeftClickCallBack();
	
private:
	FHitResult MouseWorldHit;
	ANetworkPlayer* Player = nullptr;

private:
	void CheckHover(AActor* _HoverActor);

private:
	ILClickInterface* HoverTarget = nullptr;
};
