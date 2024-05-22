// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LClickInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULClickInterface : public UInterface
{
	GENERATED_BODY()
};

class ANetworkPlayerController;

/**
 * 
 */
class MABINOGI_API ILClickInterface
{
	GENERATED_BODY()
	
public:
	virtual void ExcuteLClick(ANetworkPlayerController* _PlayerCtrl) = 0;
	virtual void OnHoverLMouse(ANetworkPlayerController* _PlayerCtrl){}
	virtual void OnUnHoverLMouse(ANetworkPlayerController* _PlayerCtrl){}
};
