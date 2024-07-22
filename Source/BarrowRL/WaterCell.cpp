// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterCell.h"


AWaterCell::AWaterCell() {
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> WaterMatAsset(TEXT("/Game/Water.Water"));
	
	if (WaterMatAsset.Succeeded()) {
		UE_LOG(LogTemp, Display, TEXT("SUCCEEDED"));
		FluidMaterialInstance = WaterMatAsset.Object;
		VisualMesh->SetMaterial(0, FluidMaterialInstance);
	}
}
