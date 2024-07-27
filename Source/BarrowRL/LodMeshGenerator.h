// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MaterialGenerator.h"
#include "MeshGenerator.h"
#include "LodMeshGenerator.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BARROWRL_API ULodMeshGenerator : public UActorComponent, public IMeshGenerator
{
	GENERATED_BODY()
public:	
	// Sets default values for this component's properties
	ULodMeshGenerator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override {}

	virtual void generate(
		TArray<uint8> &cells,
		URealtimeMeshSimple *mesh,
		TScriptInterface<IMaterialGenerator> mat_generator,
		int32 lod = 0,
		int32 chunk_shift = 5
	);

private:
	UPROPERTY()
	TScriptInterface<IMeshGenerator> Mesher;
};
