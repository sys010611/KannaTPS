// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if(HealthBarWidget == nullptr) //null일 시에만 캐스팅해서 할당
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	
	if (HealthBarWidget && HealthBarWidget->HealthBar) //null check
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
