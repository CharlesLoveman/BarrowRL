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
		TArray<Quad> quads,
		TStaticArray<uint8, CHUNK_VOLUME> cells,
		TArray<FVector2f> &uv0,
		TArray<FVector2f> &uv1,
		UObject *parent
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


	void fit(TArray<Quad> &blocks);
	void update_tex(
		TArray<Quad> &blocks,
		TStaticArray<uint8, CHUNK_VOLUME> &cells,
		FColor *fg_tex,
		FColor *bg_tex,
		FColor *uv_tex,
		TArray<FVector2f> &uv0,
		TArray<FVector2f> &uv1
	);
};
