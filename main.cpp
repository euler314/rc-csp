// main.cpp
#include "graph.hpp"
#include "path.hpp"
#include "model.hpp"
#include "test.hpp"

#include "model_writer.hpp"
#include "strong_model_writer.hpp"

#include <iostream>
#include <bitset>
#include <fstream>
#include <chrono>

#include <random>
#include <functional>
#include <ctime>

/*
graph forest_example(index_t n, index_t p, double prob)
{
	graph g(n + p);

	for (index_t i = 0; i < (n - 1); ++i)
	{
		g.add_edge(i, i + 1);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution d(prob);

	for (index_t i = n; i < (n + p); ++i)
	{
		for (index_t j = 0; j < n; ++j)
		{
			if (d(gen))
			{
				g.add_edge(i, j);
				std::cout << "added " << i << "," << j << "\n";
			}
		}
	}

	return g;
}
*/

int main()
{
	std::ios_base::sync_with_stdio(false);

	//const index_t n = 14;
	//graph g = forest_example(n, 4, 0.4);

	const index_t n = 4;
	graph g = build_cycle(n);

	if (!is_connected(g))
	{
		std::cerr << "The graph G is not connected\n";
		return 0;
	}

	const int k = 4; //get_diameter(g);
	
	std::ofstream file("foo.txt");
	strong_model_writer writer(g, k, std::cout);

	auto start = std::chrono::high_resolution_clock::now();
	writer.write();
	writer.add_line("% foo");

	//build_minion_rc_model(g, k, file);

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

	std::cerr << "\nElapsed time: " << (duration.count() / 1000.0) << " s\n";

	std::cin.get();
}