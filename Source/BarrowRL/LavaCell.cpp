// Fill out your copyright notice in the Description page of Project Settings.


#include "LavaCell.h"

ALavaCell::ALavaCell() {
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> LavaMatAsset(TEXT("/Game/Lava.Lava"));
	
	if (LavaMatAsset.Succeeded()) {
		UE_LOG(LogTemp, Display, TEXT("SUCCEEDED"));
		FluidMaterialInstance = LavaMatAsset.Object;
		VisualMesh->SetMaterial(0, FluidMaterialInstance);
	}
}
