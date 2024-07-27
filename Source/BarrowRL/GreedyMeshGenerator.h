// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeshGenerator.h"
#include "MaterialGenerator.h"
#include "Components/ActorComponent.h"
#include "GreedyMeshGenerator.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BARROWRL_API UGreedyMeshGenerator : public UActorComponent, public IMeshGenerator
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGreedyMeshGenerator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override {}

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override {}

	virtual void generate(
		TStaticArray<uint8, CHUNK_VOLUME> cells,
		URealtimeMeshComponent *mesh_component,
		TScriptInterface<IMaterialGenerator> mat_generator
	) override;
};
