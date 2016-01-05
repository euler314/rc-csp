// common.cpp
#include "common.hpp"

index_t nchoosek(index_t n, index_t k)
{
	if (k == 0)
	{
		return 1;
	}

	return (n * nchoosek(n - 1, k - 1)) / k;
}

void next_pair(index_t& u, index_t& v, index_t n)
{
	if (v < n - 1)
	{
		++v;
		return;
	}

	++u;
	v = u + 1;
}