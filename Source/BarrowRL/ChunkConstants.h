#pragma once

const uint32_t CHUNK_SIZE = 32;
constexpr uint32_t CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
const int32 CHUNK_SHIFT = 5;
const float EPSILON = 0.01;


inline int32 index(int32 x, int32 y, int32 z) {
	// return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
	return x + ((y + (z << CHUNK_SHIFT)) << CHUNK_SHIFT);
}
