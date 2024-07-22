// Fill out your copyright notice in the Description page of Project Settings.


#include "WallCell.h"

AWallCell::AWallCell() {
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> WallMatAsset(TEXT("/Game/Wall.Wall"));
	
	if (WallMatAsset.Succeeded()) {
		UE_LOG(LogTemp, Display, TEXT("SUCCEEDED"));
		TileMaterialInstance = WallMatAsset.Object;
		VisualMesh->SetMaterial(0, TileMaterialInstance);
	}
}
