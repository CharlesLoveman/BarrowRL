// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MaterialGenerator.h"
#include "FastTexPacker.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BARROWRL_API UFastTexPacker : public UActorComponent, public IMaterialGenerator
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFastTexPacker();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override {}

	virtual UMaterialInterface* generate(
		TArray<Quad> &quads,
		TArray<uint8> &cells,
		TArray<FVector2f> &uv0,
		TArray<FVector2f> &uv1,
		UObject *parent,
		int32 lod,
		int32 chunk_shift
	) override;

protected:
	UPROPERTY()
	UMaterialInterface *Material;

	UPROPERTY()
	TArray<FColor> fgs;

	UPROPERTY()
	TArray<FColor> bgs;

	UPROPERTY()
	TArray<FColor> uvs;

private:
	int32 rows;
	int32 cols;
	int32 width;
	int32 height;


	void fit(TArray<Quad> &blocks, int32 chunk_shift);
	void update_tex(
		TArray<Quad> &blocks,
		TArray<uint8> &cells,
		FColor *fg_tex,
		FColor *bg_tex,
		FColor *uv_tex,
		TArray<FVector2f> &uv0,
		TArray<FVector2f> &uv1,
		int32 chunk_shift
	);
};
