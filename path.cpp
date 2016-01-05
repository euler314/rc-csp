// path.cpp
#include "path.hpp"

#include <iterator>

void drop_endpoints(vertex_path& p)
{
	
}

std::vector<index_t> to_edge_list(const edge_path& p)
{
	std::vector<index_t> edge_list;
	edge_list.reserve(2 * std::distance(p.cbegin(), p.cend() - 1));

	for (auto it = p.cbegin(), end = (p.cend() - 1); it != end; ++it)
	{
		auto edge = std::minmax(*it, *(it + 1));
		edge_list.emplace_back(edge.first);
		edge_list.emplace_back(edge.second);
	}

	return edge_list;
}