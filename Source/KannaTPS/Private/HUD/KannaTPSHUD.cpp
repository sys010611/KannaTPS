// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/KannaTPSHUD.h"
#include "HUD/KannaTPSOverlay.h"

void AKannaTPSHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && KannaTPSOverlayClass)
		{
			KannaTPSOverlay = CreateWidget<UKannaTPSOverlay>(Controller, KannaTPSOverlayClass);
			KannaTPSOverlay->AddToViewport();
		}
	}

}
