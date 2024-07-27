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
	TRealtimeMeshStreamBuilder<uint16> &PolyGroupBuilder,
	int32 lod
) {
	quad(
		(100 << lod) * v1,
		(100 << lod) * v2,
		(100 << lod) * v3,
		(100 << lod) * v4,
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

void UGreedyMeshGenerator::generate(TArray<uint8> &cells, URealtimeMeshSimple *mesh, TScriptInterface<IMaterialGenerator> mat_generator, int32 lod, int32 chunk_shift) {
	//URealtimeMeshSimple *mesh = mesh_component->InitializeRealtimeMesh<URealtimeMeshSimple>();
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

	int32 chunk_size = 1 << chunk_shift;
	TStaticArray<TBitArray<>, 6> faces_array;
	for (int i = 0; i < 6; i++) {
		faces_array[i] = TBitArray(false, chunk_size * chunk_size * chunk_size);
	}
	TBitArray<> visited;
	TArray<Quad> quads;
	
	const double start = FPlatformTime::Seconds();
	int32 pos = 0;
	//int32 old_count = 0;
	uint8 tex_count = 0;
	for (int32 z = 0; z < chunk_size; ++z) {
		for (int32 y = 0; y < chunk_size; ++y) {
			for (int32 x = 0; x < chunk_size; ++x) {
				if (cells[index(x, y, z, chunk_shift)]) {
					if (z == 0 || cells[pos - chunk_size * chunk_size] == 0) {
						faces_array[0][index(x, y, z, chunk_shift)] = 1;
					}
					if (z == chunk_size - 1 || cells[pos + chunk_size * chunk_size] == 0) {
						faces_array[1][index(x, y, z, chunk_shift)] = 1;
					}
					if (y == 0 || cells[pos - chunk_size] == 0) {
						faces_array[2][index(x, z, y, chunk_shift)] = 1;
					}
					if (y == chunk_size - 1 || cells[pos + chunk_size] == 0) {
						faces_array[3][index(x, z, y, chunk_shift)] = 1;
					}
					if (x == 0 || cells[pos - 1] == 0) {
						faces_array[4][index(y, z, x, chunk_shift)] = 1;
					}
					if (x == chunk_size - 1 || cells[pos + 1] == 0) {
						faces_array[5][index(y, z, x, chunk_shift)] = 1;
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
		TBitArray<> &faces = faces_array[face];
		for (int32 z = 0; z < chunk_size; z++) {
			visited.Init(false, chunk_size * chunk_size);
			int x = 0;
			int y = 0;
			int32 vPos = 0;
			while (y < chunk_size) {
				if (visited[vPos] || !faces[index(x, y, z, chunk_shift)]) {
					vPos++;
					if (x < chunk_size - 1) {
						x++;
					} else {
						x = 0;
						y++;
					}
					continue;
				}
				visited[vPos] = 1;
				int width = 1;
				while (x + width < chunk_size && !visited[x + width + chunk_size * y] && faces[index(x + width, y, z, chunk_shift)]) {
					visited[vPos + width] = 1;
					width++;
				}
				int height = 1;
				while (y + height < chunk_size && faces[index(x, y + height, z, chunk_shift)]) {
					bool flag = false;
					for (int i = 0; i < width; i++) {
						if (!faces[index(x + i, y + height, z, chunk_shift)]) {
							flag = true;
							break;
						}
					}
					if (flag) {
						break;
					}
					for (int i = 0; i < width; i++) {
						visited[x + i + chunk_size * (y + height)] = 1;
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
						PolyGroupBuilder,
						lod
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
						PolyGroupBuilder,
						lod
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
						PolyGroupBuilder,
						lod
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
						PolyGroupBuilder,
						lod
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
						PolyGroupBuilder,
						lod
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
						PolyGroupBuilder,
						lod
					);
					break;
				default:
					break;
				}
				num_vertices += 4;
				x += width;
				vPos += width;
				if (x >= chunk_size) {
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
	mesh->SetupMaterialSlot(lod, "Material", mat_generator->generate(quads, cells, uv0, uv1, mesh, lod, chunk_shift));

	for (int i = 0; i < num_vertices; i++) {
		//UE_LOG(LogTemp, Display, TEXT("uv1 i: %d, u: %f, v: %f"), i, UV1Builder[i].X, UV1Builder[i].Y);
		UVBuilder.Add(TRealtimeMeshTexCoords<FVector2f, 2>(uv0[i], uv1[i]));
	}
	//UV1Builder.Append(uv1);

	const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(lod, FName("Chunk"));
	const FRealtimeMeshSectionKey PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, 0);
	mesh->CreateSectionGroup(GroupKey, StreamSet);
	mesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, lod));
}
