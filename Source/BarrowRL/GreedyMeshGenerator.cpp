// Fill out your copyright notice in the Description page of Project Settings.


#include "GreedyMeshGenerator.h"
#include "Containers/StaticArray.h"
#include "Containers/StaticBitArray.h"
#include "MaterialGenerator.h"
#include "RealtimeMeshSimple.h"

// Sets default values for this component's properties
UGreedyMeshGenerator::UGreedyMeshGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

inline void quad(
	const FVector3f &v1,
	const FVector3f &v2,
	const FVector3f &v3,
	const FVector3f &v4,
	const FRealtimeMeshTangentsHighPrecision &tangent,
	const FColor &v1_color,
	const FColor &v2_color,
	const FColor &v3_color,
	const FColor &v4_color,
	TRealtimeMeshStreamBuilder<FVector3f> &PositionBuilder,
	TRealtimeMeshStreamBuilder<TIndex3<uint32>, TIndex3<uint16>> &TrianglesBuilder,
	TRealtimeMeshStreamBuilder<FColor> &ColorBuilder,
	TRealtimeMeshStreamBuilder<FRealtimeMeshTangentsHighPrecision, FRealtimeMeshTangentsNormalPrecision> &TangentBuilder,
	TRealtimeMeshStreamBuilder<uint16> &PolyGroupBuilder
) {
	int32 v1_index = PositionBuilder.Add(v1);
	int32 v2_index = PositionBuilder.Add(v2);
	int32 v3_index = PositionBuilder.Add(v3);
	int32 v4_index = PositionBuilder.Add(v4);
	TrianglesBuilder.Add(TIndex3<uint32>(v1_index, v2_index, v3_index));
	PolyGroupBuilder.Add(0);
	TrianglesBuilder.Add(TIndex3<uint32>(v1_index, v3_index, v4_index));
	PolyGroupBuilder.Add(0);
	ColorBuilder.Add(v1_color);
	ColorBuilder.Add(v2_color);
	ColorBuilder.Add(v3_color);
	ColorBuilder.Add(v4_color);
	TangentBuilder.Add(tangent);
	TangentBuilder.Add(tangent);
	TangentBuilder.Add(tangent);
	TangentBuilder.Add(tangent);
}


inline void create_quad(
	const FVector3f &v1, 
	const FVector3f &v2, 
	const FVector3f &v3, 
	const FVector3f &v4, 
	const FRealtimeMeshTangentsHighPrecision &tangent,
	TRealtimeMeshStreamBuilder<FVector3f> &PositionBuilder,
	TRealtimeMeshStreamBuilder<TIndex3<uint32>, TIndex3<uint16>> &TrianglesBuilder,
	TRealtimeMeshStreamBuilder<FColor> &ColorBuilder,
	TRealtimeMeshStreamBuilder<FRealtimeMeshTangentsHighPrecision, FRealtimeMeshTangentsNormalPrecision> &TangentBuilder,
	TRealtimeMeshStreamBuilder<uint16> &PolyGroupBuilder
) {
	quad(
		100.0 * v1,
		100.0 * v2,
		100.0 * v3,
		100.0 * v4,
		tangent,
		FColor(),
		FColor(),
		FColor(),
		FColor(),
		PositionBuilder,
		TrianglesBuilder,
		ColorBuilder,
		TangentBuilder,
		PolyGroupBuilder
	);
}

void UGreedyMeshGenerator::generate(TStaticArray<uint8, CHUNK_VOLUME> cells, URealtimeMeshComponent *mesh_component, TScriptInterface<IMaterialGenerator> mat_generator) {
	URealtimeMeshSimple *mesh = mesh_component->InitializeRealtimeMesh<URealtimeMeshSimple>();
	FRealtimeMeshStreamSet StreamSet;
	TRealtimeMeshStreamBuilder<FVector3f> PositionBuilder(StreamSet.AddStream(FRealtimeMeshStreams::Position, GetRealtimeMeshBufferLayout<FVector3f>()));
	TRealtimeMeshStreamBuilder<FRealtimeMeshTangentsHighPrecision, FRealtimeMeshTangentsNormalPrecision> TangentBuilder(
		StreamSet.AddStream(FRealtimeMeshStreams::Tangents, GetRealtimeMeshBufferLayout<FRealtimeMeshTangentsNormalPrecision>()
	));
	//auto TexCoordsStream = StreamSet.AddStream(FRealtimeMeshStreams::TexCoords, GetRealtimeMeshBufferLayout<TRealtimeMeshTexCoords<FVector2f, 2>>());
	TRealtimeMeshStreamBuilder<TRealtimeMeshTexCoords<FVector2f, 2>> UVBuilder(StreamSet.AddStream(FRealtimeMeshStreams::TexCoords, GetRealtimeMeshBufferLayout<TRealtimeMeshTexCoords<FVector2f, 2>>()));
	
	TRealtimeMeshStreamBuilder<FColor> ColorBuilder(StreamSet.AddStream(FRealtimeMeshStreams::Color, GetRealtimeMeshBufferLayout<FColor>()));
	TRealtimeMeshStreamBuilder<TIndex3<uint32>, TIndex3<uint16>> TrianglesBuilder(StreamSet.AddStream(FRealtimeMeshStreams::Triangles, GetRealtimeMeshBufferLayout<TIndex3<uint16>>()));
	TRealtimeMeshStreamBuilder<uint16> PolyGroupBuilder(StreamSet.AddStream(FRealtimeMeshStreams::PolyGroups, GetRealtimeMeshBufferLayout<uint16>()));

	TStaticArray<TStaticBitArray<CHUNK_VOLUME>, 6> faces_array;
	TStaticBitArray<CHUNK_SIZE * CHUNK_SIZE> visited;
	TArray<Quad> quads;
	
	const double start = FPlatformTime::Seconds();
	int32 pos = 0;
	//int32 old_count = 0;
	uint8 tex_count = 0;
	for (int32 z = 0; z < CHUNK_SIZE; ++z) {
		for (int32 y = 0; y < CHUNK_SIZE; ++y) {
			for (int32 x = 0; x < CHUNK_SIZE; ++x) {
				if (cells[index(x, y, z)]) {
					if (z == 0 || cells[pos - CHUNK_SIZE * CHUNK_SIZE] == 0) {
						faces_array[0][index(x, y, z)] = 1;
					}
					if (z == CHUNK_SIZE - 1 || cells[pos + CHUNK_SIZE * CHUNK_SIZE] == 0) {
						faces_array[1][index(x, y, z)] = 1;
					}
					if (y == 0 || cells[pos - CHUNK_SIZE] == 0) {
						faces_array[2][index(x, z, y)] = 1;
					}
					if (y == CHUNK_SIZE - 1 || cells[pos + CHUNK_SIZE] == 0) {
						faces_array[3][index(x, z, y)] = 1;
					}
					if (x == 0 || cells[pos - 1] == 0) {
						faces_array[4][index(y, z, x)] = 1;
					}
					if (x == CHUNK_SIZE - 1 || cells[pos + 1] == 0) {
						faces_array[5][index(y, z, x)] = 1;
					}
				}
				pos++;
			}
		}
	}
	pos = 0;
	double end = FPlatformTime::Seconds();
	uint32 num_vertices = 0;
	for (int face = 0; face < 6; face++) {
		TStaticBitArray<CHUNK_VOLUME> &faces = faces_array[face];
		for (int32 z = 0; z < CHUNK_SIZE; z++) {
			visited = visited ^ visited;
			int x = 0;
			int y = 0;
			int32 vPos = 0;
			while (y < CHUNK_SIZE) {
				if (visited[vPos] || !faces[index(x, y, z)]) {
					vPos++;
					if (x < CHUNK_SIZE - 1) {
						x++;
					} else {
						x = 0;
						y++;
					}
					continue;
				}
				visited[vPos] = 1;
				int width = 1;
				while (x + width < CHUNK_SIZE && !visited[x + width + CHUNK_SIZE * y] && faces[index(x + width, y, z)]) {
					visited[vPos + width] = 1;
					width++;
				}
				int height = 1;
				while (y + height < CHUNK_SIZE && faces[index(x, y + height, z)]) {
					bool flag = false;
					for (int i = 0; i < width; i++) {
						if (!faces[index(x + i, y + height, z)]) {
							flag = true;
							break;
						}
					}
					if (flag) {
						break;
					}
					for (int i = 0; i < width; i++) {
						visited[x + i + CHUNK_SIZE * (y + height)] = 1;
					}
					height++;
				}
				switch (face) {
				case 0:
					quads.Add(Quad(num_vertices, num_vertices + 1, num_vertices + 2, num_vertices + 3, x, y, z, width, height, face));
					create_quad(
						FVector3f(x, y, z),
						FVector3f(x + width, y, z),
						FVector3f(x + width, y + height, z),
						FVector3f(x, y + height, z),
						FRealtimeMeshTangentsHighPrecision(FVector3f(0.0f, 0.0f, -1.0f), FVector3f(-1.0, 0.0, 0.0)),
						PositionBuilder,
						TrianglesBuilder,
						ColorBuilder,
						TangentBuilder,
						PolyGroupBuilder
					);
					break;
				case 1:
					quads.Add(Quad(num_vertices, num_vertices + 3, num_vertices + 2, num_vertices + 1, x, y, z, width, height, face));
					create_quad(
						FVector3f(x, y, z + 1),
						FVector3f(x, y + height, z + 1),
						FVector3f(x + width, y + height, z + 1),
						FVector3f(x + width, y, z + 1),
						FRealtimeMeshTangentsHighPrecision(FVector3f(0.0f, 0.0f, 1.0f), FVector3f(-1.0, 0.0, 0.0)),
						PositionBuilder,
						TrianglesBuilder,
						ColorBuilder,
						TangentBuilder,
						PolyGroupBuilder
					);
					break;
				case 2:
					quads.Add(Quad(num_vertices, num_vertices + 3, num_vertices + 2, num_vertices + 1, x, z, y, width, height, face));
					create_quad(
						FVector3f(x, z, y),
						FVector3f(x, z, y + height),
						FVector3f(x + width, z, y + height),
						FVector3f(x + width, z, y),
						FRealtimeMeshTangentsHighPrecision(FVector3f(0.0, -1.0, 0.0), FVector3f(1.0, 0.0, 0.0)),
						PositionBuilder,
						TrianglesBuilder,
						ColorBuilder,
						TangentBuilder,
						PolyGroupBuilder
					);
					break;
				case 3:
					quads.Add(Quad(num_vertices, num_vertices + 1, num_vertices + 2, num_vertices + 3, x, z, y, width, height, face));
					create_quad(
						FVector3f(x, z + 1, y),
						FVector3f(x + width, z + 1, y),
						FVector3f(x + width, z + 1, y + height),
						FVector3f(x, z + 1, y + height),
						FRealtimeMeshTangentsHighPrecision(FVector3f(0.0, 1.0, 0.0), FVector3f(1.0, 0.0, 0.0)),
						PositionBuilder,
						TrianglesBuilder,
						ColorBuilder,
						TangentBuilder,
						PolyGroupBuilder
					);
					break;
				case 4:
					quads.Add(Quad(num_vertices, num_vertices + 1, num_vertices + 2, num_vertices + 3, z, x, y, width, height, face));
					create_quad(
						FVector3f(z, x, y),
						FVector3f(z, x + width, y),
						FVector3f(z, x + width, y + height),
						FVector3f(z, x, y + height),
						FRealtimeMeshTangentsHighPrecision(FVector3f(-1.0, 0.0, 0.0), FVector3f(0.0, 1.0, 0.0)),
						PositionBuilder,
						TrianglesBuilder,
						ColorBuilder,
						TangentBuilder,
						PolyGroupBuilder
					);
					break;
				case 5:
					quads.Add(Quad(num_vertices, num_vertices + 3, num_vertices + 2, num_vertices + 1, z, x, y, width, height, face));
					create_quad(
						FVector3f(z + 1, x, y),
						FVector3f(z + 1, x, y + height),
						FVector3f(z + 1, x + width, y + height),
						FVector3f(z + 1, x + width, y),
						FRealtimeMeshTangentsHighPrecision(FVector3f(1.0, 0.0, 0.0), FVector3f(0.0, 1.0, 0.0)),
						PositionBuilder,
						TrianglesBuilder,
						ColorBuilder,
						TangentBuilder,
						PolyGroupBuilder
					);
					break;
				default:
					break;
				}
				num_vertices += 4;
				x += width;
				vPos += width;
				if (x >= CHUNK_SIZE) {
					x = 0;
					y++;
				}
			}
		}
	}
	TArray<FVector2f> uv0;
	TArray<FVector2f> uv1;
	for (int i = 0; i < num_vertices; i++) {
		uv0.Add(FVector2f());
		uv1.Add(FVector2f());
	}
	mesh->SetupMaterialSlot(0, "Material", mat_generator->generate(quads, cells, uv0, uv1, mesh));

	for (int i = 0; i < num_vertices; i++) {
		//UE_LOG(LogTemp, Display, TEXT("uv1 i: %d, u: %f, v: %f"), i, UV1Builder[i].X, UV1Builder[i].Y);
		UVBuilder.Add(TRealtimeMeshTexCoords<FVector2f, 2>(uv0[i], uv1[i]));
	}
	//UV1Builder.Append(uv1);

	const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk"));
	const FRealtimeMeshSectionKey PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, 0);
	mesh->CreateSectionGroup(GroupKey, StreamSet);
	mesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0));
	mesh_component->SetRealtimeMesh(mesh);
}
