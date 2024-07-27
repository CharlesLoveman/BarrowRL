// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChunkConstants.h"
#include "MaterialGenerator.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMaterialGenerator : public UInterface
{
	GENERATED_BODY()
};


struct Quad {
	int32 x;
	int32 y;
	int32 z;
	int32 width;
	int32 height;
	int32 face;
	int32 size;
	int32 v1;
	int32 v2;
	int32 v3;
	int32 v4;
	int32 tex_x;
	int32 tex_y;
	int32 tile_id;
	Quad(int32 _v1, int32 _v2, int32 _v3, int32 _v4, int32 _x, int32 _y, int32 _z, int32 w, int32 h, int32 f) : x(_x), y(_y), z(_z), width(w), height(h), face(f), size(fmax(w, h)), v1(_v1), v2(_v2), v3(_v3), v4(_v4), tex_x(0), tex_y(0), tile_id(0) {}

	friend bool operator<(const Quad& q1, const Quad& q2) {
		return q1.size > q2.size || (q1.size == q2.size && q1.width > q2.width) || (q1.size == q2.size && q1.width == q2.width && q1.height > q2.height);
	}
};

/**
 * 
 */
class BARROWRL_API IMaterialGenerator
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UMaterialInterface* generate(
		TArray<Quad> &quads,
		TArray<uint8> &cells,
		TArray<FVector2f> &uv0,
		TArray<FVector2f> &uv1,
		UObject *parent,
		int32 lod,
		int32 chunk_shift
	);
};
