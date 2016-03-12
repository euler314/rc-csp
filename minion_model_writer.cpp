// minion_model_writer.cpp
#include "minion_model_writer.hpp"

#include "graph.hpp"
#include <string>

namespace
{
	const std::string VAR_PREFIX = "x";

	void add_minion_variables(const graph& g, index_t k, std::ostream& os)
	{
		const index_t n = g.num_vertices();

		for (index_t i = 0; i < n; ++i)
		{
			index_t adj = g.adj_[i];
			for (index_t j = i; j < n; ++j)
			{
				if (adj & (1 << j))
					os << "DISCRETE " << VAR_PREFIX << i << "_" << j << " {1.." << k << "}\n";
			}
		}
	}

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
	list_paths(get_graph(), u, v, paths, get_solution_size());

	os << "watched-or({";

	for (index_t j = 0; j < paths.size(); ++j)
	{
		add_minion_alldiff(paths[j], os);

		if (j != paths.size() - 1)
			os << ", ";
	}

	os << "})\n";
}