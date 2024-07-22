// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FluidCell.generated.h"

UCLASS()
class BARROWRL_API AFluidCell : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AFluidCell();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* FluidMaterialInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
