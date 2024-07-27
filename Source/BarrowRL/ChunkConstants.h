#pragma once

const float EPSILON = 0.01;
const int32 CHUNK_SIZE = 32;
constexpr int32 CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

inline int32 index(int32 x, int32 y, int32 z, int32 chunk_shift) {
	// return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
	return x + ((y + (z << chunk_shift)) << chunk_shift);
}
