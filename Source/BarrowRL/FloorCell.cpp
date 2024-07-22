// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorCell.h"

AFloorCell::AFloorCell() {
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> FloorMatAsset(TEXT("/Game/Floor.Floor"));
	
	if (FloorMatAsset.Succeeded()) {
		UE_LOG(LogTemp, Display, TEXT("SUCCEEDED"));
		TileMaterialInstance = FloorMatAsset.Object;
		VisualMesh->SetMaterial(0, TileMaterialInstance);
	}
}
