// minion_model_writer.cpp
#include "minion_model_writer.hpp"

#include "graph.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>

namespace
{
	void add_minion_alldiff(const std::vector<index_t>& edges, std::ostream& os)
	{
		os << "alldiff([";

		for (auto it = edges.cbegin(); it != edges.cend(); it += 2)
		{
			os << VAR_PREFIX << *(it) << "_" << *(it + 1);

			if (it != (edges.cend() - 2))
				os << ",";
		}

		os << "])";
	}

	void add_minion_alldiff(const edge_path& p, std::ostream& os)
	{
		add_minion_alldiff(to_edge_list(p), os);
	}
}

void minion_model_writer::impl_preprocess()
{
	auto& os = get_output_stream();

	os << "MINION 3\n\n";
	os << "**VARIABLES**\n\n";

	add_minion_variables(get_graph(), get_solution_size(), os);

	os << "\n**CONSTRAINTS**\n\n";
}

void minion_model_writer::impl_postprocess()
{
	auto& os = get_output_stream();
	os << "\n**EOF**\n";
}

void minion_model_writer::impl_process_vertex_pair(index_t u, index_t v)
{
	auto& os = get_output_stream();

	os << get_comment() << u << " " << v << "\n";
	std::vector<edge_path> paths;
	// No path pruning: generate paths of all lengths.
	// It is up to the user to make sure this is sensible.
	// For example, if k < diam(G), the instance is trivially UNSAT.
	list_paths(get_graph(), u, v, paths, std::numeric_limits<index_t>::max());

	// MINION can't handle empty constraints such as "watched-or({ })"
	//if (paths.size() < 2)
	//	return;

	os << "watched-or({";

	for (index_t j = 0; j < paths.size(); ++j)
	{
		//if (paths[j].size())

		add_minion_alldiff(paths[j], os);

		if (j != paths.size() - 1)
			os << ", ";
	}

	os << "})\n";
}

void strong_minion_model_writer::impl_process_vertex_pair(index_t u, index_t v)
{
	auto& os = get_output_stream();

	os << get_comment() << u << " " << v << "\n";
	std::vector<edge_path> paths;
	list_shortest_paths(get_graph(), u, v, paths);

	os << "watched-or({";

	for (index_t j = 0; j < paths.size(); ++j)
	{
		add_minion_alldiff(paths[j], os);

		if (j != paths.size() - 1)
			os << ", ";
	}

	os << "})\n";
}