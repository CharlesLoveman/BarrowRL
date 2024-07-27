// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshGenerator.h"
#include "MaterialGenerator.h"
#include "RealtimeMeshComponent.h"
#include "Chunk.generated.h"

UCLASS()
class BARROWRL_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URealtimeMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IMeshGenerator> Mesher;

	UPROPERTY(VisibleAnywhere)
	TScriptInterface<IMaterialGenerator> MatGenerator;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<uint8> cells;
};
