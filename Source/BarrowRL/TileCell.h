// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileCell.generated.h"

UCLASS()
class BARROWRL_API ATileCell : public AActor
{
	GENERATED_BODY()
	
protected:
	TCHAR* MaterialAsset;

public:	
	// Sets default values for this actor's properties
	ATileCell();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* TileMaterialInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
