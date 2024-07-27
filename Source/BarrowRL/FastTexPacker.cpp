// Fill out your copyright notice in the Description page of Project Settings.


#include "FastTexPacker.h"
#include "MaterialGenerator.h"

// Sets default values for this component's properties
UFastTexPacker::UFastTexPacker() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UMaterial> MatAsset(TEXT("/Game/StaticTileMaterial.StaticTileMaterial"));
	
	if (MatAsset.Succeeded()) {
		Material = MatAsset.Object;
	}
	fgs.Add(FColor(255, 255, 255, 255));
	fgs.Add(FColor(100, 100, 100, 255));
	fgs.Add(FColor(192, 164, 132, 255));
	fgs.Add(FColor(255, 255, 255, 255));

	bgs.Add(FColor(0, 0, 0, 255));
	bgs.Add(FColor(50, 50, 50, 255));
	bgs.Add(FColor(92, 64, 51, 255));
	bgs.Add(FColor(240, 240, 240, 255));

	uvs.Add(FColor(0, 0, 0, 0));
	uvs.Add(FColor(0, 0, 0, 255));
	uvs.Add(FColor(1, 0, 0, 100));
	uvs.Add(FColor(2, 0, 255, 0));
}


// Called when the game starts
void UFastTexPacker::BeginPlay()
{
	Super::BeginPlay();
}

UMaterialInterface* UFastTexPacker::generate(
	TArray<Quad> quads,
	TStaticArray<uint8, CHUNK_VOLUME> cells,
	TArray<FVector2f> &uv0,
	TArray<FVector2f> &uv1,
	UObject *parent
) {
	quads.Sort();
	fit(quads);
	UTexture2D *uv_tex = UTexture2D::CreateTransient(width, height, EPixelFormat::PF_B8G8R8A8);
	UTexture2D *fg_tex = UTexture2D::CreateTransient(width, height, EPixelFormat::PF_B8G8R8A8);
	UTexture2D *bg_tex = UTexture2D::CreateTransient(width, height, EPixelFormat::PF_B8G8R8A8);
	uv_tex->SRGB = 0;
	uv_tex->Filter = TextureFilter::TF_Nearest;
	fg_tex->Filter = TextureFilter::TF_Nearest;
	bg_tex->Filter = TextureFilter::TF_Nearest;
	void * uv_tex_data = uv_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void * fg_tex_data = fg_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void * bg_tex_data = bg_tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memzero(uv_tex_data, 4 * width * height);
	FMemory::Memzero(fg_tex_data, 4 * width * height);
	FMemory::Memzero(bg_tex_data, 4 * width * height);
	update_tex(
		quads,
		cells,
		(FColor *) fg_tex_data,
		(FColor *) bg_tex_data,
		(FColor *) uv_tex_data,
		uv0,
		uv1
	);
	bg_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	fg_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	uv_tex->GetPlatformData()->Mips[0].BulkData.Unlock();
	uv_tex->UpdateResource();
	fg_tex->UpdateResource();
	bg_tex->UpdateResource();
	UMaterialInstanceDynamic *mat = UMaterialInstanceDynamic::Create(Material, parent);
	mat->SetTextureParameterValue(TEXT("Foreground"), fg_tex);
	mat->SetTextureParameterValue(TEXT("Background"), bg_tex);
	mat->SetTextureParameterValue(TEXT("TexMap"), uv_tex);
	return mat;
}

void UFastTexPacker::fit(TArray<Quad> &quads) {
	int32 count = 0;
	int32 x = 0;
	int32 y = 0;
	int32 prev_height = 0;
	for (auto &quad : quads) {
		if (quad.height > prev_height || x + quad.width > CHUNK_SIZE) {
			x = 0;
			y += prev_height;
			prev_height = quad.height;
		}
		if (y + prev_height > CHUNK_SIZE) {
			x = 0;
			y = 0;
			prev_height = quad.height;
			count++;
		}
		quad.tex_x = x;
		quad.tex_y = y;
		quad.tile_id = count;
		x += quad.width;
	}
	count += 1;
	rows = floor(sqrt(count)); 
	cols = ceil(((float) count) / rows);
	height = CHUNK_SIZE * rows;
	width = CHUNK_SIZE * cols;
}

void UFastTexPacker::update_tex(
	TArray<Quad> &quads,
	TStaticArray<uint8, CHUNK_VOLUME> &cells,
	FColor *fg_tex,
	FColor *bg_tex,
	FColor *uv_tex,
	TArray<FVector2f> &uv0,
	TArray<FVector2f> &uv1
) {
	for (auto quad : quads) {
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
