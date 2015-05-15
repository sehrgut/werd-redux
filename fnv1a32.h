#ifndef WERD_FNV1A32_H
#define WERD_FNV1A32_H

#define FNV1_PRIME_32 0x01000193

uint32_t fnv1a32(uint32_t d, const uint8_t* buf, long len);

#endif
