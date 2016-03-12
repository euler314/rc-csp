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

	void add_alldiff(const std::vector<index_t>& edges, std::ostream& os)
	{
		os << "alldifferent([";

		for (auto it = edges.cbegin(); it != edges.cend(); it += 2)
		{
			os << VAR_PREFIX << *(it) << "_" << *(it + 1);

			if (it != (edges.cend() - 2))
				os << ",";
		}

		os << "]) ";
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

		os << "]) ";
	}

	void add_alldiff(const edge_path& p, std::ostream& os)
	{
		add_alldiff(to_edge_list(p), os);
	}

	void add_minion_alldiff(const edge_path& p, std::ostream& os)
	{
		add_minion_alldiff(to_edge_list(p), os);
	}

	void add_diametral_constraints(const edge_path& p, std::ostream& os)
	{
		auto edges = to_edge_list(p);

		index_t color = 1;
		for (index_t i = 0; i < edges.size(); i += 2)
		{
			os << "constraint " << VAR_PREFIX << edges[i] << "_" << edges[i + 1] << " = " << color << ";\n";
			++color;
		}
	}
}

/*
void build_rc_model(const graph& g, index_t k, std::ostream& os)
{
	prepare_model(k, os);
	add_variables(g, os);

	// Enforce a diametral path to be rainbow.
	auto dpair = get_diametral_pair(g);
	auto dpath = get_shortest_path<edge_path>(g, dpair.first, dpair.second);

	//os << "% Diametral path between " << dpair.first << " " << dpair.second << "\n";
	//add_diametral_constraints(dpath, os);

	index_t u = 0;
	index_t v = 1;
	const index_t n = g.num_vertices();
	const index_t pairs = nchoosek(n, 2);

	os << "% Paths between vertex pairs\n";

	for (index_t i = 0; i < pairs; ++i)
	{
		if (!is_adjacent(g, u, v))
		{
			os << "% Vertex pair " << u << " " << v << "\n";
			std::vector<edge_path> paths;
			list_paths(g, u, v, paths, k);

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

	// Bridges must get distinct colors.
	auto bridges = get_bridges(g);

	// At least 2 bridges?
	if (bridges.size() >= 4)
	{
		os << "% Bridges\n";
		os << "constraint ( ";
		add_alldiff(bridges, os);
		os << ");\n";
	}

	os << "solve satisfy;\n";
}
*/

/*
void build_src_model(const graph& g, index_t k, std::ostream& os)
{
	prepare_model(k, os);
	add_variables(g, os);

	index_t u = 0;
	index_t v = 1;
	const index_t n = g.num_vertices();
	const index_t pairs = nchoosek(n, 2);

	os << "% Paths between vertex pairs\n";

	for (index_t i = 0; i < pairs; ++i)
	{
		if (!is_adjacent(g, u, v))
		{
			os << "% Vertex pair " << u << " " << v << "\n";
			std::vector<edge_path> paths;
			list_shortest_paths<edge_path>(g, u, v, paths);

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

	// Bridges must get distinct colors.
	auto bridges = get_bridges(g);

	// At least 2 bridges?
	if (bridges.size() >= 4)
	{
		os << "% Bridges\n";
		os << "constraint ( ";
		add_alldiff(bridges, os);
		os << ");\n";
	}

	os << "solve satisfy;\n";
}
*/

void build_src_model_forest(const graph& g, index_t k, index_t p, std::ostream& os)
{
	prepare_model(k, os);
	add_variables(g, os);

	index_t u = 0;
	index_t v = 1;
	const index_t n = g.num_vertices();
	const index_t pairs = nchoosek(n, 2);

	for (index_t i = 0; i < (p - 1); ++i)
	{
		os << "constraint x" << i << "_" << (i + 1) << " = " << (i + 1) << ";\n";
	}
	os << "\n";

	os << "% Paths between vertex pairs\n";

	for (index_t i = 0; i < pairs; ++i)
	{
		if (!is_adjacent(g, u, v))
		{
			os << "% Vertex pair " << u << " " << v << "\n";
			std::vector<edge_path> paths;
			list_shortest_paths<edge_path>(g, u, v, paths);

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

	// Bridges must get distinct colors.
	auto bridges = get_bridges(g);

	// At least 2 bridges?
	if (bridges.size() >= 4)
	{
		os << "% Bridges\n";
		os << "constraint ( ";
		add_alldiff(bridges, os);
		os << ");\n";
	}	

	os << "solve satisfy;\n";
}

void build_minion_rc_model(const graph& g, index_t k, std::ostream& os)
{
	os << "MINION 3\n\n";
	os << "**VARIABLES**\n\n";

	add_minion_variables(g, k, os);

	os << "\n**CONSTRAINTS**\n\n";

	index_t u = 0;
	index_t v = 1;
	const index_t n = g.num_vertices();
	const index_t pairs = nchoosek(n, 2);

	os << "# Paths between vertex pairs\n";

	for (index_t i = 0; i < pairs; ++i)
	{
		if (!is_adjacent(g, u, v))
		{
			os << "# Vertex pair " << u << " " << v << "\n";
			std::vector<edge_path> paths;
			list_paths(g, u, v, paths, k);

			os << "watched-or({ ";

			for (index_t j = 0; j < paths.size(); ++j)
			{
				add_minion_alldiff(paths[j], os);

				if (j != paths.size() - 1)
					os << ", ";
			}

			os << " })\n";
		}

		next_pair(u, v, n);
	}

	os << "\n**EOF**\n";
}

/*
MINION 3

* *VARIABLES**

DISCRETE a{ 0..2 }
DISCRETE b{ 0..4 }
DISCRETE c{ 0..4 }

**CONSTRAINTS**

watched - or({ alldiff([a, b, c]), alldiff([a, c]) })
eq(a, b)

**EOF**
*/