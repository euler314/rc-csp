// model.cpp

#include "model.hpp"
#include "common.hpp"
#include "graph.hpp"
#include "path.hpp"

#include <string>

namespace
{
	const std::string VAR_PREFIX = "x";
	const std::string VAR_DECL = "var 1..k: " + VAR_PREFIX;

	void prepare_model(index_t k, std::ostream& os)
	{
		os << "include \"alldifferent.mzn\";\n";
		os << "int: k=" << k << ";\n";
	}

	void add_variables(const graph& g, std::ostream& os)
	{
		const index_t n = g.num_vertices();

		for (index_t i = 0; i < n; ++i)
		{
			index_t adj = g.adj_[i];
			for (index_t j = i; j < n; ++j)
			{
				if (adj & (1 << j))
					os << VAR_DECL << i << "_" << j << ";\n";
			}
		}
	}

	void add_alldiff(const edge_path& p, std::ostream& os)
	{
		os << "alldifferent([";

		auto edges = to_edge_list(p);

		for (auto it = edges.cbegin(); it != edges.cend(); it += 2)
		{
			os << VAR_PREFIX << *(it) << "_" << *(it + 1);

			if (it != (edges.cend() - 2))
				os << ",";
		}

		os << "]) ";
	}
}

void build_rc_model(const graph& g, index_t k, std::ostream& os)
{
	prepare_model(k, os);
	add_variables(g, os);

	index_t u = 0;
	index_t v = 1;
	const index_t n = g.num_vertices();
	const index_t pairs = nchoosek(n, 2);

	for (index_t i = 0; i < pairs; ++i)
	{
		if (!is_adjacent(g, u, v))
		{
			std::vector<edge_path> paths;
			list_paths(g, u, v, paths);

			os << "constraint ( ";

			for (index_t j = 0; j < paths.size(); ++j)
			{
				add_alldiff(paths[j], os);

				if (j != paths.size() - 1)
					os << "\\/ ";
			}

			os << ");\n";
		}

		next_pair(u, v, n);
	}

	os << "solve satisfy;\n";
}