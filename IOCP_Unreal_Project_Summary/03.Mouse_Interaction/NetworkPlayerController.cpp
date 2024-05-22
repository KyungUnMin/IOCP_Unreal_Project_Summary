// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Controllers/NetworkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/Network/NetworkPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Interface/LClickInterface.h"
#include "MabiGameInstance.h"
#include "Levels/BaseLevel.h"
#include "Levels/BattleLevel.h"
#include "Actors/Network/NetworkOthers.h"
#include "UI/ObjectInfoWidget.h"
#include "UI/MouseCursorWidget.h"
#include "Levels/BattleLevel.h"
#include "UI/HUD/HUDWidget.h"
#include "UI/HUD/SkillInfoWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"

#include "FSM/IdleState.h"
#include "FSM/MoveState.h"
#include "FSM/AttackState.h"
#include "FSM/Skill/MageSkillState.h"
#include "FSM/Skill/WarriorSkillState.h"
#include "FSM/Skill/RogSkillState.h"


void ANetworkPlayerController::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (true == GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, OUT MouseWorldHit))
	{
		CheckHover(MouseWorldHit.GetActor());
	}
	else
	{
		CheckHover(nullptr);
	}
}

void ANetworkPlayerController::CheckHover(AActor* _HoverActor)
{
	ILClickInterface* IHover = Cast<ILClickInterface>(_HoverActor);

	//기존 타겟이 없는데 새로운 타겟이 들어온 경우
	if (nullptr != IHover && nullptr == HoverTarget)
	{
		IHover->OnHoverLMouse(this);
	}

	//기존 타겟이 있는데 새로운 타겟이 없는경우
	else if (nullptr == IHover && nullptr != HoverTarget)
	{
		HoverTarget->OnUnHoverLMouse(this);
	}

	//기존 타겟이 있는데 새로운 타겟이 들어온 경우
	else if (nullptr != IHover && nullptr != HoverTarget)
	{
		HoverTarget->OnUnHoverLMouse(this);
		IHover->OnHoverLMouse(this);
	}

	HoverTarget = IHover;
}


//마우스 왼쪽 클릭을 하는 순간 호출되는 콜백
void ANetworkPlayerController::LeftClickCallBack()
{
	SetTarget(nullptr);
	
	AActor* HitActor = MouseWorldHit.GetActor();

	//인터페이스로 검출
	ILClickInterface* Excuter = Cast<ILClickInterface>(HitActor);
	if (nullptr != Excuter)
	{
		Excuter->ExcuteLClick(this);
	}
}


