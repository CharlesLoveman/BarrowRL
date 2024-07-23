// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "Containers/StaticArray.h"
#include "Containers/StaticBitArray.h"

MeshGenerator::MeshGenerator()
{
}

MeshGenerator::~MeshGenerator()
{
}

const int32 CHUNK_SHIFT = 5;
const float EPSILON = 0.01;

inline int32 index(int32 x, int32 y, int32 z) {
	// return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
	return x + ((y + (z << CHUNK_SHIFT)) << CHUNK_SHIFT);
}

inline int32 get_vertex_index(
	const FVector &v,
	const FVector2D &uv0,
	const FVector2D &uv1,
	const FVector2D &uv2,
	const FVector2D &uv3,
	const FColor &color,
	TArray<FVector> &vertices,
	TArray<FVector> &normals,
	TArray<FVector2D> &UV0,
	TArray<FVector2D> &UV1,
	TArray<FVector2D> &UV2,
	TArray<FVector2D> &UV3,
	TArray<FColor> &vertex_colors,
	TArray<FProcMeshTangent> &tangents,
	TMap<FVector, int32> &vertex_map
) {
	if (!vertex_map.Contains(v)) {
		int32 vertex_index = vertices.Num();
		vertex_map.Add(v, vertex_index);
		vertices.Emplace(v);
		normals.Emplace(FVector(0.0, 0.0, 0.0));
		UV0.Emplace(uv0);
		UV1.Emplace(uv1);
		UV2.Emplace(uv2);
		UV3.Emplace(uv3);
		vertex_colors.Emplace(color);
		tangents.Emplace(FProcMeshTangent());
		return vertex_index;
	} else {
		return vertex_map[v];
	}
}


inline void quad(
	const FVector &v1,
	const FVector &v2,
	const FVector &v3,
	const FVector &v4,
	const FVector &normal,
	const FProcMeshTangent tangent,
	const FVector2D &v1_UV0,
	const FVector2D &v1_UV1,
	const FVector2D &v1_UV2,
	const FVector2D &v1_UV3,
	const FVector2D &v2_UV0,
	const FVector2D &v2_UV1,
	const FVector2D &v2_UV2,
	const FVector2D &v2_UV3,
	const FVector2D &v3_UV0,
	const FVector2D &v3_UV1,
	const FVector2D &v3_UV2,
	const FVector2D &v3_UV3,
	const FVector2D &v4_UV0,
	const FVector2D &v4_UV1,
	const FVector2D &v4_UV2,
	const FVector2D &v4_UV3,
	const FColor &v1_color,
	const FColor &v2_color,
	const FColor &v3_color,
	const FColor &v4_color,
	TArray<FVector> &vertices,
	TArray<int32> &triangles,
	TArray<FVector> &normals,
	TArray<FVector2D> &UV0,
	TArray<FVector2D> &UV1,
	TArray<FVector2D> &UV2,
	TArray<FVector2D> &UV3,
	TArray<FColor> &vertex_colors,
	TArray<FProcMeshTangent> &tangents,
	TMap<FVector, int32> &vertex_map
) {
	const int32 v1_index = get_vertex_index(v1, v1_UV0, v1_UV1, v1_UV2, v1_UV3, v1_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
	const int32 v2_index = get_vertex_index(v2, v2_UV0, v2_UV1, v2_UV2, v2_UV3, v2_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
	const int32 v3_index = get_vertex_index(v3, v3_UV0, v3_UV1, v3_UV2, v3_UV3, v3_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
	const int32 v4_index = get_vertex_index(v4, v4_UV0, v4_UV1, v4_UV2, v4_UV3, v4_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
	triangles.Add(v1_index);
	triangles.Add(v2_index);
	triangles.Add(v3_index);
	triangles.Add(v1_index);
	triangles.Add(v3_index);
	triangles.Add(v4_index);
	normals[v1_index] += normal; 
	tangents[v1_index] = tangent;
	normals[v2_index] += normal; 
	tangents[v2_index] = tangent;
	normals[v3_index] += normal; 
	tangents[v3_index] = tangent;
	normals[v4_index] += normal; 
	tangents[v4_index] = tangent;
}


inline void create_quad(
	const FVector &v1, 
	const FVector &v2, 
	const FVector &v3, 
	const FVector &v4, 
	const FVector &normal,
	const FProcMeshTangent tangent,
	TArray<FVector> &vertices,
	TArray<int32> &triangles,
	TArray<FVector> &normals,
	TArray<FVector2D> &UV0,
	TArray<FVector2D> &UV1,
	TArray<FVector2D> &UV2,
	TArray<FVector2D> &UV3,
	TArray<FColor> &vertex_colors,
	TArray<FProcMeshTangent> &tangents,
	TMap<FVector, int32> &vertex_map
) {
	quad(
		100.0 * v1,
		100.0 * v2,
		100.0 * v3,
		100.0 * v4,
		normal,
		tangent,
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 1.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(1.0, 1.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(1.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FColor(),
		FColor(),
		FColor(),
		FColor(),
		vertices,
		triangles,
		normals,
		UV0,
		UV1,
		UV2,
		UV3,
		vertex_colors,
		tangents,
		vertex_map
	);
}


void MeshGenerator::generate(TArray<uint8_t> cells, UProceduralMeshComponent &mesh) {
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FVector2D> UV1;
	TArray<FVector2D> UV2;
	TArray<FVector2D> UV3;
	TArray<FColor> vertex_colors;
	TArray<FProcMeshTangent> tangents;
	TMap<FVector, int32> vertex_map;
	
	TStaticBitArray<CHUNK_VOLUME> candidates;
	TStaticArray<uint8_t, CHUNK_VOLUME> faces;
	TStaticBitArray<CHUNK_SIZE * CHUNK_SIZE> visited;
	
	int32 pos = 0;
	for (int32 z = 0; z < CHUNK_SIZE; ++z) {
		for (int32 y = 0; y < CHUNK_SIZE; ++y) {
			for (int32 x = 0; x < CHUNK_SIZE; ++x) {
				if (cells[pos]) {
					if (z == 0 || cells[pos - CHUNK_SIZE * CHUNK_SIZE] == 0) {
						candidates[pos] = 1;
						faces[pos] |= 1;
					}
					if (z == CHUNK_SIZE - 1 || cells[pos + CHUNK_SIZE * CHUNK_SIZE] == 0) {
						candidates[pos] = 1;
						faces[pos] |= 1 << 1;
					}
					if (y == 0 || cells[pos - CHUNK_SIZE] == 0) {
						candidates[pos] = 1;
						faces[pos] |= 1 << 2;
					}
					if (y == CHUNK_SIZE - 1 || cells[pos + CHUNK_SIZE] == 0) {
						candidates[pos] = 1;
						faces[pos] |= 1 << 3;
					}
					if (x == 0 || cells[pos - 1] == 0) {
						candidates[pos] = 1;
						faces[pos] |= 1 << 4;
					}
					if (x == CHUNK_SIZE - 1 || cells[pos + 1] == 0) {
						candidates[pos] = 1;
						faces[pos] |= 1 << 5;
					}
				}
				pos++;
			}
		}
	}
	pos = 0;
	for (int32 z = CHUNK_SIZE - 1; z < CHUNK_SIZE; z++) {
		for (int32 face = 0; face <= 1; face++) {
			visited = visited ^ visited;
			int x = 0;
			int y = 0;
			int32 vPos = 0;
			while (y < CHUNK_SIZE) {
				if (visited[vPos] || !candidates[index(x, y, z)]) {
					vPos++;
					if (x < CHUNK_SIZE - 1) {
						x++;
					} else {
						x = 0;
						y += 1;
					}
					continue;
				}
				visited[vPos] = 1;
				int width = 1;
				while (x + width < CHUNK_SIZE && (faces[index(x + width, y, z)] & (1 << face))) {
					visited[vPos + width] = 1;
					width++;
				}
				int height = 1;
				while (y + height < CHUNK_SIZE && (faces[index(x + width, y + height, z)] & (1 << face))) {
					bool flag = false;
					for (int i = 0; i < width; i++) {
						if (!(faces[index(x + i, y + height, z)] & (1 << face))) {
							flag = true;
							break;
						}
						visited[x + i + CHUNK_SIZE * (y + height)] = 1;
					}
					if (flag) {
						break;
					}
					height++;
				}
				if (face % 2) {
					create_quad(FVector(x, y, z + 1), FVector(x, y + height, z + 1), FVector(x + width, y + height, z + 1), FVector(x + width, y, z + 1), FVector(0.0f, 0.0f, 1.0f), FProcMeshTangent(-1.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				} else {
					create_quad(FVector(x, y, z), FVector(x + width, y, z), FVector(x + width, y + height, z), FVector(x, y + height, z), FVector(0.0f, 0.0f, -1.0f), FProcMeshTangent(1.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				}
				vPos++;
				if (x < CHUNK_SIZE - 1) {
					x++;
				} else {
					x = 0;
					y += 1;
				}
			}
		}
	}
				
				
				//if (candidates[pos]) {
				//	if (faces[pos] & 1) {
				//		create_quad(FVector(x, y, z), FVector(x + 1, y, z), FVector(x + 1, y + 1, z), FVector(x, y + 1, z), FVector(0.0f, 0.0f, -1.0f), FProcMeshTangent(1.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				//	}
				//	if (faces[pos] & (1 << 1)) {
				//		create_quad(FVector(x, y, z + 1), FVector(x, y + 1, z + 1), FVector(x + 1, y + 1, z + 1), FVector(x + 1, y, z + 1), FVector(0.0f, 0.0f, 1.0f), FProcMeshTangent(-1.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				//	}
				//	if (faces[pos] & (1 << 2)) {
				//		create_quad(FVector(x, y, z), FVector(x, y, z + 1), FVector(x + 1, y, z + 1), FVector(x + 1, y, z), FVector(0.0f, -1.0f, 0.0f), FProcMeshTangent(1.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				//	}
				//	if (faces[pos] & (1 << 3)) {
				//		create_quad(FVector(x, y + 1, z), FVector(x + 1, y + 1, z), FVector(x + 1, y + 1, z + 1), FVector(x, y + 1, z + 1), FVector(0.0f, 1.0f, 0.0f), FProcMeshTangent(1.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				//	}
				//	if (faces[pos] & (1 << 4)) {
				//		create_quad(FVector(x, y, z), FVector(x, y + 1, z), FVector(x, y + 1, z + 1), FVector(x, y, z + 1), FVector(-1.0f, 0.0f, 0.0f), FProcMeshTangent(0.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				//	}
				//	if (faces[pos] & (1 << 5)) {
				//		create_quad(FVector(x + 1, y, z), FVector(x + 1, y, z + 1), FVector(x + 1, y + 1, z + 1), FVector(x + 1, y + 1, z), FVector(1.0f, 0.0f, 0.0f), FProcMeshTangent(0.0, 0.0, 0.0), vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, vertex_map);
				//	}
				//	
				//}
			//}
		//}
	//}
	for (int i = 0; i < vertices.Num(); i++) {
		normals[i].Normalize(EPSILON);
	}
	mesh.CreateMeshSection(0, vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, false);
}
