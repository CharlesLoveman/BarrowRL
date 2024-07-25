// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "Containers/StaticArray.h"
#include "Containers/StaticBitArray.h"
#include <cstdint>

// Sets default values for this component's properties
UMeshGenerator::UMeshGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Game/StaticTileMaterial.StaticTileMaterial"));
	
	if (MatAsset.Succeeded()) {
		Material = MatAsset.Object;
	}
}


// Called when the game starts
void UMeshGenerator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMeshGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
	TArray<FProcMeshTangent> &tangents
) {
	int32 vertex_index = vertices.Num();
	vertices.Emplace(v);
	normals.Emplace(FVector(0.0, 0.0, 0.0));
	UV0.Emplace(uv0);
	UV1.Emplace(uv1);
	UV2.Emplace(uv2);
	UV3.Emplace(uv3);
	vertex_colors.Emplace(color);
	tangents.Emplace(FProcMeshTangent());
	return vertex_index;
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
	TArray<FProcMeshTangent> &tangents
) {
	const int32 v1_index = get_vertex_index(v1, v1_UV0, v1_UV1, v1_UV2, v1_UV3, v1_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents);
	const int32 v2_index = get_vertex_index(v2, v2_UV0, v2_UV1, v2_UV2, v2_UV3, v2_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents);
	const int32 v3_index = get_vertex_index(v3, v3_UV0, v3_UV1, v3_UV2, v3_UV3, v3_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents);
	const int32 v4_index = get_vertex_index(v4, v4_UV0, v4_UV1, v4_UV2, v4_UV3, v4_color, vertices, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents);
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
	const FVector2D &uv01,
	const FVector2D &uv02,
	const FVector2D &uv03,
	const FVector2D &uv04,
	TArray<FVector> &vertices,
	TArray<int32> &triangles,
	TArray<FVector> &normals,
	TArray<FVector2D> &UV0,
	TArray<FVector2D> &UV1,
	TArray<FVector2D> &UV2,
	TArray<FVector2D> &UV3,
	TArray<FColor> &vertex_colors,
	TArray<FProcMeshTangent> &tangents
) {
	quad(
		100.0 * v1,
		100.0 * v2,
		100.0 * v3,
		100.0 * v4,
		normal,
		tangent,
		uv01,
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		uv02,
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		uv03,
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		FVector2D(0.0, 0.0),
		uv04,
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
		tangents
	);
}

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
};

struct less_than_size
{
    inline bool operator() (const Quad& q1, const Quad& q2)
    {
        return q1.size > q2.size || (q1.size == q2.size && q1.width > q2.width) || (q1.size == q2.size && q1.width == q2.width && q1.height > q2.height);
    }
};

struct QuadTree {
    int32 rows;
    int32 cols;
    int32 width;
    int32 height;

public:
	void fit(std::vector<Quad> &blocks) {
		int32 count = 0;
		int32 x = 0;
		int32 y = 0;
		int32 prev_height = 0;
		for (int32 i = 0; i < blocks.size(); i++) {
			if (blocks[i].height > prev_height || x + blocks[i].width > CHUNK_SIZE) {
				x = 0;
				y += prev_height;
				prev_height = blocks[i].height;
			}
			if (y + prev_height > CHUNK_SIZE) {
				x = 0;
				y = 0;
				prev_height = blocks[i].height;
				count++;
			}
			blocks[i].tex_x = x;
			blocks[i].tex_y = y;
			blocks[i].tile_id = count;
			x += blocks[i].width;
		}
		count += 1;
		rows = floor(sqrt(count)); 
		cols = ceil(((float) count) / rows);
		height = CHUNK_SIZE * rows;
		width = CHUNK_SIZE * cols;
	}

	void update_tex(
		const std::vector<Quad> &blocks,
		FColor *fg_tex,
		FColor *bg_tex,
		FColor *uv_tex,
		TArray<FVector2D> uv,
		const TArray<uint8> &cells,
		TArray<FColor> &fgs,
		TArray<FColor> &bgs,
		TArray<FColor> &uvs
	) {
		for (auto quad : blocks) {
			int32 x_offset = (quad.tile_id % cols) * CHUNK_SIZE;
			int32 y_offset = (quad.tile_id / cols) * CHUNK_SIZE;
			uv[quad.v1] = FVector2D(x_offset + quad.tex_x, y_offset + quad.tex_y) / FVector2D(width, height);
			uv[quad.v2] = FVector2D(x_offset + quad.tex_x + quad.width, y_offset + quad.tex_y) / FVector2D(width, height);
			uv[quad.v3] = FVector2D(x_offset + quad.tex_x + quad.width, y_offset + quad.tex_y + quad.height) / FVector2D(width, height);
			uv[quad.v4] = FVector2D(x_offset + quad.tex_x, y_offset + quad.tex_y + quad.height) / FVector2D(width, height);
			switch (quad.face) {
			case 0:
			case 1:
				for (int i = 0; i < quad.height; i++) {
					for (int j = 0; j < quad.width; j++) {
						fg_tex[(x_offset + quad.tex_x + j) + (y_offset + quad.tex_y + i) * width] = fgs[cells[index(quad.x + j, quad.y + i, quad.z)]];
						bg_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = bgs[cells[index(quad.x + j, quad.y + i, quad.z)]];
						uv_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = uvs[cells[index(quad.x + j, quad.y + i, quad.z)]];
					}
				}
				break;
			case 2:
			case 3:
				for (int i = 0; i < quad.height; i++) {
					for (int j = 0; j < quad.width; j++) {
						fg_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = fgs[cells[index(quad.x + j, quad.y, quad.z + i)]];
						bg_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = bgs[cells[index(quad.x + j, quad.y, quad.z + i)]];
						uv_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = uvs[cells[index(quad.x + j, quad.y, quad.z + i)]];
					}
				}
				break;
			default:
				for (int i = 0; i < quad.height; i++) {
					for (int j = 0; j < quad.width; j++) {
						fg_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = fgs[cells[index(quad.x, quad.y + j, quad.z + i)]];
						bg_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = bgs[cells[index(quad.x, quad.y + j, quad.z + i)]];
						uv_tex[x_offset + quad.tex_x + j + (y_offset + quad.tex_y + i) * width] = uvs[cells[index(quad.x, quad.y + j, quad.z + i)]];
					}
				}
				break;
			}
		}
		
	}
};

void UMeshGenerator::generate(TArray<uint8> cells, UProceduralMeshComponent &mesh, TArray<FColor> &fgs, TArray<FColor> &bgs, TArray<FColor> &texUV) {
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FVector2D> UV1;
	TArray<FVector2D> UV2;
	TArray<FVector2D> UV3;
	TArray<FColor> vertex_colors;
	TArray<FProcMeshTangent> tangents;
	TStaticArray<TStaticBitArray<CHUNK_VOLUME>, 6> faces_array;
	TStaticBitArray<CHUNK_SIZE * CHUNK_SIZE> visited;
	std::vector<Quad> quads;
	
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
					quads.push_back(Quad(vertices.Num(), vertices.Num() + 1, vertices.Num() + 2, vertices.Num() + 3, x, y, z, width, height, face));
					create_quad(
						FVector(x, y, z),
						FVector(x + width, y, z),
						FVector(x + width, y + height, z),
						FVector(x, y + height, z),
						FVector(0.0f, 0.0f, -1.0f),
						FProcMeshTangent(-1.0, 0.0, 0.0),
						FVector2D(0.0, 0.0),
						FVector2D(width, 0.0),
						FVector2D(width, height),
						FVector2D(0.0, height),
						vertices,
						triangles,
						normals,
						UV0,
						UV1,
						UV2,
						UV3,
						vertex_colors,
						tangents
					);
					break;
				case 1:
					quads.push_back(Quad(vertices.Num(), vertices.Num() + 3, vertices.Num() + 2, vertices.Num() + 1, x, y, z, width, height, face));
					create_quad(
						FVector(x, y, z + 1),
						FVector(x, y + height, z + 1),
						FVector(x + width, y + height, z + 1),
						FVector(x + width, y, z + 1),
						FVector(0.0f, 0.0f, 1.0f),
						FProcMeshTangent(-1.0, 0.0, 0.0),
						FVector2D(0.0, 0.0),
						FVector2D(0.0, height),
						FVector2D(width, height),
						FVector2D(width, 0.0),
						vertices,
						triangles,
						normals,
						UV0,
						UV1,
						UV2,
						UV3,
						vertex_colors,
						tangents
					);
					break;
				case 2:
					quads.push_back(Quad(vertices.Num(), vertices.Num() + 1, vertices.Num() + 2, vertices.Num() + 3, x, z, y, width, height, face));
					create_quad(
						FVector(x, z, y),
						FVector(x, z, y + height),
						FVector(x + width, z, y + height),
						FVector(x + width, z, y),
						FVector(0.0f, -1.0f, 0.0f),
						FProcMeshTangent(1.0, 0.0, 0.0),
						FVector2D(0.0, 0.0),
						FVector2D(0.0, height),
						FVector2D(width, height),
						FVector2D(width, 0.0),
						vertices,
						triangles,
						normals,
						UV0,
						UV1,
						UV2,
						UV3,
						vertex_colors,
						tangents
					);
					break;
				case 3:
					quads.push_back(Quad(vertices.Num(), vertices.Num() + 3, vertices.Num() + 2, vertices.Num() + 1, x, z, y, width, height, face));
					create_quad(
						FVector(x, z + 1, y),
						FVector(x + width, z + 1, y),
						FVector(x + width, z + 1, y + height),
						FVector(x, z + 1, y + height),
						FVector(0.0f, 1.0f, 0.0f),
						FProcMeshTangent(1.0, 0.0, 0.0),
						FVector2D(0.0, 0.0),
						FVector2D(width, 0.0),
						FVector2D(width, height),
						FVector2D(0.0, height),
						vertices,
						triangles,
						normals,
						UV0,
						UV1,
						UV2,
						UV3,
						vertex_colors,
						tangents
					);
					break;
				case 4:
					quads.push_back(Quad(vertices.Num(), vertices.Num() + 1, vertices.Num() + 2, vertices.Num() + 3, z, x, y, width, height, face));
					create_quad(
						FVector(z, x, y),
						FVector(z, x + width, y),
						FVector(z, x + width, y + height),
						FVector(z, x, y + height),
						FVector(-1.0f, 0.0f, 0.0f),
						FProcMeshTangent(0.0, 0.0, 0.0),
						FVector2D(0.0, 0.0),
						FVector2D(width, 0.0),
						FVector2D(width, height),
						FVector2D(0.0, height),
						vertices,
						triangles,
						normals,
						UV0,
						UV1,
						UV2,
						UV3,
						vertex_colors,
						tangents
					);
					break;
				case 5:
					quads.push_back(Quad(vertices.Num(), vertices.Num() + 3, vertices.Num() + 2, vertices.Num() + 1, z, x, y, width, height, face));
					create_quad(
						FVector(z + 1, x, y),
						FVector(z + 1, x, y + height),
						FVector(z + 1, x + width, y + height),
						FVector(z + 1, x + width, y),
						FVector(1.0f, 0.0f, 0.0f),
						FProcMeshTangent(0.0, 0.0, 0.0),
						FVector2D(0.0, 0.0),
						FVector2D(0.0, height),
						FVector2D(width, height),
						FVector2D(width, 0.0),
						vertices,
						triangles,
						normals,
						UV0,
						UV1,
						UV2,
						UV3,
						vertex_colors,
						tangents
					);
					break;
				default:
					break;
				}
				x += width;
				vPos += width;
				if (x >= CHUNK_SIZE) {
					x = 0;
					y++;
				}
			}
		}
	}
	
	for (int i = 0; i < vertices.Num(); i++) {
		normals[i].Normalize(EPSILON);
	}
	std::sort(quads.begin(), quads.end(), less_than_size());
	QuadTree tree = QuadTree();
	tree.fit(quads);
	uv_tex = UTexture2D::CreateTransient(tree.width, tree.height, EPixelFormat::PF_B8G8R8A8);
	fg_tex = UTexture2D::CreateTransient(tree.width, tree.height, EPixelFormat::PF_B8G8R8A8);
	bg_tex = UTexture2D::CreateTransient(tree.width, tree.height, EPixelFormat::PF_B8G8R8A8);
	uv_tex->SRGB = 0;
	uv_tex->Filter = TextureFilter::TF_Nearest;

	void * uv_tex_data = uv_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void * fg_tex_data = fg_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void * bg_tex_data = bg_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memzero(uv_tex_data, 4 * tree.width * tree.height);
	FMemory::Memzero(fg_tex_data, 4 * tree.width * tree.height);
	FMemory::Memzero(bg_tex_data, 4 * tree.width * tree.height);
	tree.update_tex(quads, (FColor*) fg_tex_data, (FColor*) bg_tex_data, (FColor*) uv_tex_data, UV1, cells, fgs, bgs, texUV);
	bg_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	fg_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	uv_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	uv_tex->UpdateResource();
	fg_tex->UpdateResource();
	bg_tex->UpdateResource();
	mesh.ClearMeshSection(0);
	mesh.CreateMeshSection(0, vertices, triangles, normals, UV0, UV1, UV2, UV3, vertex_colors, tangents, false);
	UMaterialInstanceDynamic *mat = UMaterialInstanceDynamic::Create(Material, &mesh);
	mat->SetTextureParameterValue(TEXT("Foreground"), fg_tex);
	mat->SetTextureParameterValue(TEXT("Background"), bg_tex);
	mat->SetTextureParameterValue(TEXT("TexMap"), uv_tex);
	mesh.SetMaterial(0, mat);
}
