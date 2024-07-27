// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RealtimeMeshComponent.h"
#include "ChunkConstants.h"
#include "MaterialGenerator.h"
#include "MeshGenerator.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMeshGenerator : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BARROWRL_API IMeshGenerator
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void generate(
		TStaticArray<uint8, CHUNK_VOLUME> cells,
		URealtimeMeshComponent *mesh_component,
		TScriptInterface<IMaterialGenerator> mat_generator
	) {}
};
