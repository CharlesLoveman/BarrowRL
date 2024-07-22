// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IntBoxTypes.h"
#include "Spatial/DenseGrid3.h"
#include "GameFramework/Actor.h"
#include "TileCell.h"
#include "MeshGenerator.h"
#include "ProceduralMeshComponent.h"
#include "MapAnchor.generated.h"


UCLASS()
class BARROWRL_API AMapAnchor : public AActor
{
	GENERATED_BODY()
	
protected:
	//FDenseGrid3i map;
	UE::Geometry::FDenseGrid3i map;

	//FAxisAlignedBox3d bounds;
	UE::Geometry::FAxisAlignedBox3i bounds;

	UProceduralMeshComponent* VisualMesh;
	UMaterialInstance* TileMaterialInstance;
public:
	// Sets default values for this actor's properties
	AMapAnchor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
