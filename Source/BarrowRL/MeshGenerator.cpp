// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "Containers/StaticArray.h"
#include "Containers/StaticBitArray.h"
#include <vector>
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
		TArray<FVector2f> &uv0,
		TArray<FVector2f> &uv1,
		const TStaticArray<uint8, CHUNK_VOLUME> &cells,
		TArray<FColor> &fgs,
		TArray<FColor> &bgs,
		TArray<FColor> &uvs
	) {
		for (auto quad : blocks) {
			int32 x_offset = (quad.tile_id % cols) * CHUNK_SIZE;
			int32 y_offset = (quad.tile_id / cols) * CHUNK_SIZE;
			uv0[quad.v1] = FVector2f(0.0, 0.0);
			uv0[quad.v2] = FVector2f(quad.width, 0.0);
			uv0[quad.v3] = FVector2f(quad.width, quad.height);
			uv0[quad.v4] = FVector2f(0.0, quad.height);
			uv1[quad.v1] = FVector2f(x_offset + quad.tex_x, y_offset + quad.tex_y) / FVector2f(width, height);
			uv1[quad.v2] = FVector2f(x_offset + quad.tex_x + quad.width, y_offset + quad.tex_y) / FVector2f(width, height);
			uv1[quad.v3] = FVector2f(x_offset + quad.tex_x + quad.width, y_offset + quad.tex_y + quad.height) / FVector2f(width, height);
			uv1[quad.v4] = FVector2f(x_offset + quad.tex_x, y_offset + quad.tex_y + quad.height) / FVector2f(width, height);
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

void UMeshGenerator::generate(TStaticArray<uint8, CHUNK_VOLUME> cells, URealtimeMeshComponent *mesh_component, TArray<FColor> &fgs, TArray<FColor> &bgs, TArray<FColor> &texUV) {
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
					quads.push_back(Quad(num_vertices, num_vertices + 1, num_vertices + 2, num_vertices + 3, x, y, z, width, height, face));
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
					quads.push_back(Quad(num_vertices, num_vertices + 3, num_vertices + 2, num_vertices + 1, x, y, z, width, height, face));
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
					quads.push_back(Quad(num_vertices, num_vertices + 3, num_vertices + 2, num_vertices + 1, x, z, y, width, height, face));
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
					quads.push_back(Quad(num_vertices, num_vertices + 1, num_vertices + 2, num_vertices + 3, x, z, y, width, height, face));
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
					quads.push_back(Quad(num_vertices, num_vertices + 1, num_vertices + 2, num_vertices + 3, z, x, y, width, height, face));
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
					quads.push_back(Quad(num_vertices, num_vertices + 3, num_vertices + 2, num_vertices + 1, z, x, y, width, height, face));
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
	std::sort(quads.begin(), quads.end(), less_than_size());
	QuadTree tree = QuadTree();
	tree.fit(quads);
	uv_tex = UTexture2D::CreateTransient(tree.width, tree.height, EPixelFormat::PF_B8G8R8A8);
	fg_tex = UTexture2D::CreateTransient(tree.width, tree.height, EPixelFormat::PF_B8G8R8A8);
	bg_tex = UTexture2D::CreateTransient(tree.width, tree.height, EPixelFormat::PF_B8G8R8A8);
	uv_tex->SRGB = 0;
	uv_tex->Filter = TextureFilter::TF_Nearest;
	fg_tex->Filter = TextureFilter::TF_Nearest;
	bg_tex->Filter = TextureFilter::TF_Nearest;

	void * uv_tex_data = uv_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void * fg_tex_data = fg_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void * bg_tex_data = bg_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memzero(uv_tex_data, 4 * tree.width * tree.height);
	FMemory::Memzero(fg_tex_data, 4 * tree.width * tree.height);
	FMemory::Memzero(bg_tex_data, 4 * tree.width * tree.height);
	tree.update_tex(quads, (FColor*) fg_tex_data, (FColor*) bg_tex_data, (FColor*) uv_tex_data, uv0, uv1, cells, fgs, bgs, texUV);
	bg_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	fg_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	uv_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	uv_tex->UpdateResource();
	fg_tex->UpdateResource();
	bg_tex->UpdateResource();

	UMaterialInstanceDynamic *mat = UMaterialInstanceDynamic::Create(Material, mesh);
	mat->SetTextureParameterValue(TEXT("Foreground"), fg_tex);
	mat->SetTextureParameterValue(TEXT("Background"), bg_tex);
	mat->SetTextureParameterValue(TEXT("TexMap"), uv_tex);
	mesh->SetupMaterialSlot(0, "Material", mat);

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
