// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "stdint.h"

const uint32_t CHUNK_SIZE = 32;
constexpr uint32_t CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

/**
 * 
 */
class BARROWRL_API MeshGenerator
{
public:
	MeshGenerator();
	~MeshGenerator();

	void generate(TArray<uint8_t> cells, UProceduralMeshComponent &mesh);
};
