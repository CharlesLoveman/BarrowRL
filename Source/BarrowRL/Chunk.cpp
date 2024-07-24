// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "MeshGenerator.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	VisualMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MeshComponent"));
	mesher = MeshGenerator();
}

float random(FVector2D st) {
	float x = sin(st.Dot(FVector2D(12.9898, 78.233))) * 43758.5453123;
	return x - floor(x);
}

float noise(FVector2D st) {
	FVector2D i = FVector2D(floor(st.X), floor(st.Y));
	FVector2D f = FVector2D(st.X - i.X, st.Y - i.Y);

	float a = random(i);
	float b = random(i + FVector2D(1.0, 0.0));
	float c = random(i + FVector2D(0.0, 1.0));
	float d = random(i + FVector2D(1.0, 1.0));

	FVector2D u = f * f * (FVector2D(3.0, 3.0) - 2.0 * f);

	return (1.0 - u.X) * a + u.X * b + (c - a) * u.Y * (1.0 - u.X) + (d - b) * u.X * u.Y;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	TArray<uint8_t> cells;
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				//cells.Add(FMath::RandRange(0, 1));
				if (((float) z) / CHUNK_SIZE < noise(2.0 * FVector2D(((float) x) / CHUNK_SIZE, ((float) y) / CHUNK_SIZE) + FVector2D(FPlatformTime::Seconds(), (FPlatformTime::Seconds())))) {
					cells.Add(1);
				} else {
					cells.Add(0);
				}
			}
		}
	}
	TArray<FColor> fgs;
	fgs.Add(FColor(1.0, 1.0, 1.0, 1.0));
	fgs.Add(FColor(1.0, 1.0, 1.0, 1.0));
	TArray<FColor> bgs;
	bgs.Add(FColor(0.0, 0.0, 0.0, 1.0));
	bgs.Add(FColor(0.0, 0.0, 0.0, 1.0));
	TArray<FColor> uvs;
	uvs.Add(FColor(0.0, 0.0, 1.0, 1.0));
	uvs.Add(FColor(0.0, 0.0, 1.0, 1.0));
	const double start = FPlatformTime::Seconds();
	mesher.generate(cells, *VisualMesh, fgs, bgs, uvs);
	const double end = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Display, TEXT("generated in %f seconds"), end - start);
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<uint8_t> cells;
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				//cells.Add(FMath::RandRange(0, 1));
				if (((float) z) / CHUNK_SIZE < noise(2.0 * FVector2D(((float) x) / CHUNK_SIZE, ((float) y) / CHUNK_SIZE) + FVector2D(FPlatformTime::Seconds(), (FPlatformTime::Seconds())))) {
					cells.Add(1);
				} else {
					cells.Add(0);
				}
			}
		}
	}
	TArray<FColor> fgs;
	fgs.Add(FColor(1.0, 1.0, 1.0, 1.0));
	fgs.Add(FColor(1.0, 1.0, 1.0, 1.0));
	TArray<FColor> bgs;
	bgs.Add(FColor(0.0, 0.0, 0.0, 1.0));
	bgs.Add(FColor(0.0, 0.0, 0.0, 1.0));
	TArray<FColor> uvs;
	uvs.Add(FColor(0.0, 0.0, 1.0, 1.0));
	uvs.Add(FColor(0.0, 0.0, 1.0, 1.0));
	const double start = FPlatformTime::Seconds();
	mesher.generate(cells, *VisualMesh, fgs, bgs, uvs);
	const double end = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Display, TEXT("generated in %f seconds"), end - start);

}

