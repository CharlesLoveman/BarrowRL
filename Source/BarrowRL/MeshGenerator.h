// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "MeshGenerator.generated.h"

const uint32_t CHUNK_SIZE = 32;
constexpr uint32_t CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BARROWRL_API UMeshGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeshGenerator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void generate(TArray<uint8> cells, UProceduralMeshComponent &mesh, TArray<FColor> &fgs, TArray<FColor> &bgs, TArray<FColor> &uvs);

private:
    UPROPERTY()
    UMaterial *Material;

    UPROPERTY()
    UTexture2D *uv_tex;

    UPROPERTY()
    UTexture2D *fg_tex;

    UPROPERTY()
    UTexture2D *bg_tex;
};
