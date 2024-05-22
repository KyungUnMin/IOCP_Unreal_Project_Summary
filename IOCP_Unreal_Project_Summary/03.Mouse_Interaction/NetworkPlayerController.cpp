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

	//���� Ÿ���� ���µ� ���ο� Ÿ���� ���� ���
	if (nullptr != IHover && nullptr == HoverTarget)
	{
		IHover->OnHoverLMouse(this);
	}

	//���� Ÿ���� �ִµ� ���ο� Ÿ���� ���°��
	else if (nullptr == IHover && nullptr != HoverTarget)
	{
		HoverTarget->OnUnHoverLMouse(this);
	}

	//���� Ÿ���� �ִµ� ���ο� Ÿ���� ���� ���
	else if (nullptr != IHover && nullptr != HoverTarget)
	{
		HoverTarget->OnUnHoverLMouse(this);
		IHover->OnHoverLMouse(this);
	}

	HoverTarget = IHover;
}


//���콺 ���� Ŭ���� �ϴ� ���� ȣ��Ǵ� �ݹ�
void ANetworkPlayerController::LeftClickCallBack()
{
	SetTarget(nullptr);
	
	AActor* HitActor = MouseWorldHit.GetActor();

	//�������̽��� ����
	ILClickInterface* Excuter = Cast<ILClickInterface>(HitActor);
	if (nullptr != Excuter)
	{
		Excuter->ExcuteLClick(this);
	}
}


