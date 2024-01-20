// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DamageIndicator.h"

void UDamageIndicator::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EndDelegate.Clear();
	EndDelegate.BindDynamic(this, &UDamageIndicator::HideWidget);
	BindToAnimationFinished(FadeAnim, EndDelegate);

	KannaCharacter = GetOwningPlayerPawn();
}

void UDamageIndicator::PlayFadeAnim()
{
	PlayAnimation(FadeAnim);

}


void UDamageIndicator::HideWidget()
{
	SetRenderOpacity(0.f);

	UE_LOG(LogTemp, Log, TEXT("위젯 삭제 완료"));
}
