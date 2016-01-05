// main.cpp
#include "graph.hpp"
#include "path.hpp"
#include "model.hpp"

#include <iostream>
#include <bitset>

int main()
{
	graph g = build_cycle(6);
	print_graph(g);

	std::cout << "\n";
	build_rc_model(g, 2);

	/*
	std::vector<edge_path> paths;
	list_paths(g, 0, 2, paths);

	for (auto p : paths)
	{
		for (auto it = p.cbegin(); it != p.cend(); ++it)
			std::cout << *it << " ";
		std::cout << "\n";
		auto vec = to_edge_list(p);
		for (auto it = vec.cbegin(); it != vec.cend(); ++it)
			std::cout << *it << " ";

		std::cout << "\n";
	}
	*/
	

	std::cin.get();
}