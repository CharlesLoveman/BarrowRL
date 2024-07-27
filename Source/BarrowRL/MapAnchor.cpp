// Fill out your copyright notice in the Description page of Project Settings.


#include "MapAnchor.h"
#include "WallCell.h"

// Sets default values
AMapAnchor::AMapAnchor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	//
	//if (CubeVisualAsset.Succeeded()) {
	//	VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
	//	VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	//}

	//static ConstructorHelpers::FObjectFinder<UMaterialInstance> WallMatAsset(TEXT("/Game/Wall.Wall"));
	//
	//if (WallMatAsset.Succeeded()) {
	//	UE_LOG(LogTemp, Display, TEXT("SUCCEEDED"));
	//	TileMaterialInstance = WallMatAsset.Object;
	//	VisualMesh->SetMaterial(0, TileMaterialInstance);
	//}
}

// Called when the game starts or when spawned
void AMapAnchor::BeginPlay()
{
	Super::BeginPlay();
		//mesher.generate(cells, *VisualMesh);

	//using namespace UE::Geometry;
	//bounds.Min = FVector3i(0, 0, 0);
	//bounds.Max = FVector3i(10, 10, 10);
	//
	//map = FDenseGrid3i(bounds.Max.X - bounds.Min.X, bounds.Max.Y - bounds.Min.Y, bounds.Max.Z - bounds.Min.Z, 0);

	//for (int i = 0; i < map.Bounds().Max.X; i++) {
	//	for (int j = 0; j < map.Bounds().Max.Y; j++) {
	//		for (int k = 0; k < map.Bounds().Max.Z; k++) {
	//			FVector pos = FVector(100 * i, 100 * j, 100 * k);
	//			GetWorld()->SpawnActor(AWallCell::StaticClass(), &pos);
	//		}
	//	}
	//}
}

// Called every frame
void AMapAnchor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

