// test.cpp
#include "test.hpp"
#include "graph.hpp"
#include "common.hpp"

#include <cassert>

void run_tests()
{
	std::cout << "Running tests ...\n";

	// Test if all edges are present and found using is_adjacent.
	{
		std::cout << "Adjacency test ... ";

		const index_t n = 64;
		graph g = build_clique(n);

		index_t u = 0;
		index_t v = 1;

		for (index_t i = 0; i < nchoosek(n, 2); ++i)
		{
			assert(is_adjacent(g, u, v));
			next_pair(u, v, n);
		}

		std::cout << "OK!\n";
	}
}