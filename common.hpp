// common.hpp
#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

#if defined (_MSC_VER)
#include <intrin.h> 
#endif

typedef std::int64_t index_t;

inline int popcount64(index_t x)
{
#if defined(_MSC_VER)
	return __popcnt64(x);
#elif defined(__GNUC__)
	return __builtin_popcountll(x);
#endif
}

inline int ctz64(index_t x)
{
#if defined(_MSC_VER)
	unsigned long v;
	_BitScanForward64(&v, x);
	return v;
#elif defined(__GNUC__)
	return __builtin_ctzll(x);
#endif
}

inline bool bittest64(index_t x, int idx)
{
#if defined(_MSC_VER)
	return _bittest64(&x, idx);
#elif defined(__GNUC__)
	return x & (1 << idx);
#endif
}

index_t nchoosek(index_t n, index_t k);

void next_pair(index_t& u, index_t& v, index_t n);

#endif