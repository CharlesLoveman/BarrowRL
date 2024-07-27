// Fill out your copyright notice in the Description page of Project Settings.


#include "LodMeshGenerator.h"
#include "GreedyMeshGenerator.h"

// Sets default values for this component's properties
ULodMeshGenerator::ULodMeshGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Mesher = CreateDefaultSubobject<UGreedyMeshGenerator>(TEXT("Mesh Generator"));
}


// Called when the game starts
void ULodMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void update_mode(TArray<uint8> &cells, int32 pos, TMap<uint8, uint8> &map, uint8 &mode, uint8 &max_count) {
	uint8 c = cells[pos];
	uint8 count = map.FindOrAdd(c) + 1;
	map[c] = count;
	if (count > max_count) {
		mode = c;
		max_count = count;
	}
}

void ULodMeshGenerator::generate(
	TArray<uint8> &cells,
	URealtimeMeshSimple *mesh,
	TScriptInterface<IMaterialGenerator> mat_generator,
	int32 lod,
	int32 chunk_shift
) {
	mesh->UpdateLODConfig(0, FRealtimeMeshLODConfig(0.75));
	Mesher->generate(cells, mesh, mat_generator, 0, chunk_shift);
	TArray<uint8> lod_cells1 = TArray<uint8>(cells);
	TArray<uint8> lod_cells2;
	TArray<uint8> *prev_cells = &lod_cells1;
	TArray<uint8> *next_cells = &lod_cells2;
	TMap<uint8, uint8> map;
	for (int32 level = 1; level < chunk_shift; level++) {
		next_cells->Empty();
		int32 chunk_size = 1 << (chunk_shift - level + 1);
		for (int32 z = 0; z < chunk_size; z += 2) {
			for (int32 y = 0; y < chunk_size; y += 2) {
				for (int32 x = 0; x < chunk_size; x += 2) {
					uint8 mode = 0;
					uint8 max_count = 0;
					map.Empty();
					for (int32 i = 0; i < 1; i++) {
						for (int32 j = 0; j < 1; j++) {
							for (int32 k = 0; k < 1; k++) {
								update_mode(*prev_cells, index(x + k, y + j, z + i, chunk_shift - level + 1), map, mode, max_count);
							}
						}
					}
					next_cells->Add(mode);
				}
			}
		}
		mesh->AddLOD(FRealtimeMeshLODConfig(FMath::Pow(0.5f, level)));
		Mesher->generate(*next_cells, mesh, mat_generator, level, chunk_shift - level);
		std::swap(prev_cells, next_cells);
	}
}
