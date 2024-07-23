// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "MeshGenerator.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	VisualMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MeshComponent"));
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	MeshGenerator mesher = MeshGenerator();
	TArray<uint8_t> cells;
	for (int i = 0; i < 32 * 32 * 32; i++) {
		//cells.Add(FMath::RandRange(0, 1));
		cells.Add(1);
	}
	UE_LOG(LogTemp, Display, TEXT("%d"), cells.Num());
	mesher.generate(cells, *VisualMesh);
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

