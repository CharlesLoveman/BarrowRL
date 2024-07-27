// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "ChunkConstants.h"
#include "FastTexPacker.h"
#include "LodMeshGenerator.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesher = CreateDefaultSubobject<ULodMeshGenerator>(TEXT("Lod Mesher"));
	MatGenerator = CreateDefaultSubobject<UFastTexPacker>(TEXT("Fast Packer"));
	VisualMesh = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(VisualMesh);
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

void generate_cells(TArray<uint8> &cells) {
	int count = 0;
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				//cells.Add(FMath::RandRange(0, 1));
				float noise_val = noise(2.0 * FVector2D(((float) x) / CHUNK_SIZE, ((float) y) / CHUNK_SIZE) + FVector2D(FPlatformTime::Seconds(), (FPlatformTime::Seconds())));
				if (((float) z) / CHUNK_SIZE > noise_val) {
					cells[count] = 0;
				} else if (z > 28){
					cells[count] = 3;
				} else if (z > 10) {
					cells[count] = 2;
				} else {
					cells[count] = 1;
				}
				count++;
			}
		}
	}
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	//VisualMesh = NewObject<URealtimeMeshComponent>();
	//VisualMesh->SetupAttachment(RootComponent);
	for (int i = 0; i < CHUNK_VOLUME; i++) {
		cells.Add(0);
	}
	generate_cells(cells);
	URealtimeMeshSimple *mesh = VisualMesh->InitializeRealtimeMesh<URealtimeMeshSimple>();
	const double start = FPlatformTime::Seconds();
	Mesher->generate(cells, mesh, MatGenerator);
	const double end = FPlatformTime::Seconds();
	VisualMesh->SetRealtimeMesh(mesh);
	UE_LOG(LogTemp, Display, TEXT("generated in %f seconds"), end - start);
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	generate_cells(cells);
	URealtimeMeshSimple *mesh = VisualMesh->InitializeRealtimeMesh<URealtimeMeshSimple>();
	const double start = FPlatformTime::Seconds();
	Mesher->generate(cells, mesh, MatGenerator);
	const double end = FPlatformTime::Seconds();
	VisualMesh->SetRealtimeMesh(mesh);
	UE_LOG(LogTemp, Display, TEXT("generated in %f seconds"), end - start);

}

